#!/usr/bin/env python3
"""
Check ALIB warnings markers are well-nested.

Push  = ALIB_WARNINGS_<XYZ>      (any XYZ except RESTORE)
Pop   = ALIB_WARNINGS_RESTORE

Usage:
  python check_alib_warnings.py /path/to/dir
  python check_alib_warnings.py /path/to/dir --include "*.cpp" "*.hpp" "*.c" "*.h"
  python check_alib_warnings.py /path/to/dir --exclude ".git" "node_modules" --list-ok
  python check_alib_warnings.py /path/to/dir --follow-symlinks

Exit codes:
  0 = all good
  1 = issues found or an internal error occurred
"""
from __future__ import annotations

import argparse
import fnmatch
import os
from pathlib import Path
import re
import sys
from typing import Iterable, List, Tuple

# Matches ALIB_WARNINGS_<TOKEN>, capturing TOKEN (letters, digits, underscores)
MARKER_RE = re.compile(r"ALIB_WARNINGS_([A-Z0-9_]+)")

def iter_files(root: Path,
               include_globs: List[str],
               exclude_dirs: List[str],
               follow_symlinks: bool) -> Iterable[Path]:
    """
    Yield files under root honoring include patterns and excluded dir names.
    If include_globs is empty, include all files.
    """
    for dirpath, dirnames, filenames in os.walk(root, followlinks=follow_symlinks):
        # Prune excluded directories (by name match, not path)
        dirnames[:] = [d for d in dirnames if not any(fnmatch.fnmatch(d, pat) for pat in exclude_dirs)]
        for name in filenames:
            p = Path(dirpath) / name
            if include_globs:
                if any(fnmatch.fnmatch(name, pat) for pat in include_globs):
                    yield p
            else:
                yield p

def analyze_file(path: Path) -> Tuple[List[str], List[Tuple[str,int,int]]]:
    """
    Returns (errors, pushes_left)
    - errors: human-readable error strings
    - pushes_left: list of (token, line, col) still on the stack at EOF
    """
    errors: List[str] = []
    stack: List[Tuple[str,int,int]] = []  # (token, line, col)

    try:
        text = path.read_text(encoding="utf-8", errors="replace")
    except Exception as e:
        return [f"ERROR: could not read file ({e})"], []

    # Walk the file line-by-line so we can report line/column numbers
    for lineno, line in enumerate(text.splitlines(keepends=False), start=1):
        for m in MARKER_RE.finditer(line):
            token = m.group(1)
            col = m.start(0) + 1  # 1-based column
            if token == "RESTORE":
                if not stack:
                    errors.append(f"{path}: line {lineno}, col {col}: RESTORE without matching push")
                else:
                    stack.pop()
            else:
                stack.append((token, lineno, col))

    # Any remaining pushes are unmatched
    return errors, stack

def main(argv: List[str]) -> int:
    ap = argparse.ArgumentParser(description="Verify ALIB warning markers are well-nested.")
    ap.add_argument("root", type=Path, help="Root folder to scan")
    ap.add_argument("--include", nargs="*", default=[],
                    help="File glob(s) to include (match against filename). If omitted, scans all files.")
    ap.add_argument("--exclude", nargs="*", default=[".git", ".svn", ".hg", "node_modules", "build", "dist", "__pycache__"],
                    help="Directory name glob(s) to exclude")
    ap.add_argument("--follow-symlinks", action="store_true", help="Follow directory symlinks")
    ap.add_argument("--list-ok", action="store_true", help="Also list files that are OK")
    args = ap.parse_args(argv)

    root: Path = args.root
    if not root.exists():
        print(f"ERROR: path does not exist: {root}", file=sys.stderr)
        return 1
    if not root.is_dir():
        print(f"ERROR: not a directory: {root}", file=sys.stderr)
        return 1

    total_files = 0
    files_with_markers = 0
    bad_files = 0
    any_error = False

    for f in iter_files(root, args.include, args.exclude, args.follow_symlinks):
        total_files += 1
        errors, leftover = analyze_file(f)

        if errors or leftover:
            # Determine if the file had any markers at all
            had_markers = bool(errors) or bool(leftover)
            if had_markers:
                files_with_markers += 1
            # Print detailed report for problematic files
            if errors:
                any_error = True
                for e in errors:
                    print(e)
            if leftover:
                any_error = True
                for token, line, col in leftover:
                    print(f"{f}: line {line}, col {col}: push '{token}' without matching RESTORE")
            if not (errors or leftover) and args.list_ok and had_markers:
                print(f"OK: {f}")
        else:
            # No errors and stack empty; decide if we list
            if args.list_ok:
                # Determine if file had any markers (quickly re-scan by search)
                try:
                    txt = f.read_text(encoding="utf-8", errors="replace")
                except Exception:
                    txt = ""
                if "ALIB_WARNINGS_" in txt:
                    files_with_markers += 1
                    print(f"OK: {f}")

        if errors or leftover:
            bad_files += 1

    # Summary
    print("\nSummary:")
    print(f"  Scanned files        : {total_files}")
    print(f"  Files with markers   : {files_with_markers}")
    print(f"  Files with issues    : {bad_files}")
    if any_error:
        print("Result: Problems found.")
        return 1
    else:
        print("Result: All good.")
        return 0

if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
