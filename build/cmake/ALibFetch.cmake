# ALibFetch.cmake
#
# Helper to ensure ALib sources are available locally.
# Usage:
#   include("${CMAKE_CURRENT_LIST_DIR}/cmake/ALibFetch.cmake")
#   ALibEnsureSourceAvailable(
#     VERSION_TAG V2605R1
#     # optional:
#     # FETCH_ROOT "/path/to/cache"
#     # DOWNLOAD_TIMEOUT 60
#     # SHA256 "<hex>"
#     # QUIET
#     # OUT_BASE_DIR ALIB_BASE_DIR_LOCAL
#   )
#   include("${ALIB_BASE_DIR}/build/cmake/ALib.cmake")

# \file

include_guard(GLOBAL)

function(ALibEnsureSourceAvailable)
  set(_opts QUIET)
  set(_one_value VERSION_TAG FETCH_ROOT DOWNLOAD_TIMEOUT SHA256 OUT_BASE_DIR)
  cmake_parse_arguments(ALIB_FETCH "${_opts}" "${_one_value}" "" ${ARGN})

  # Respect a pre-set ALIB_BASE_DIR if it points to a valid ALib source tree.
  if(DEFINED ALIB_BASE_DIR
     AND EXISTS "${ALIB_BASE_DIR}/src/ALib.Lang.H"
     AND EXISTS "${ALIB_BASE_DIR}/src/alib")
    if(ALIB_FETCH_OUT_BASE_DIR)
      set(${ALIB_FETCH_OUT_BASE_DIR} "${ALIB_BASE_DIR}" PARENT_SCOPE)
    endif()
    return()
  endif()

  # Resolve inputs from function arguments first, then from cache variables.
  if(NOT ALIB_FETCH_VERSION_TAG)
    if(DEFINED ALIB_FETCH_VERSION_TAG AND NOT "${ALIB_FETCH_VERSION_TAG}" STREQUAL "")
      set(ALIB_FETCH_VERSION_TAG "${ALIB_FETCH_VERSION_TAG}")
    endif()
  endif()
  if(NOT ALIB_FETCH_VERSION_TAG)
    message(FATAL_ERROR "ALibEnsureSourceAvailable: missing VERSION_TAG (example: V2605R1).")
  endif()

  if(NOT ALIB_FETCH_FETCH_ROOT)
    if(DEFINED ALIB_FETCH_ROOT AND NOT "${ALIB_FETCH_ROOT}" STREQUAL "")
      set(ALIB_FETCH_FETCH_ROOT "${ALIB_FETCH_ROOT}")
    endif()
  endif()
  if(NOT ALIB_FETCH_FETCH_ROOT)
    if(WIN32)
      if(DEFINED ENV{LocalAppData} AND NOT "$ENV{LocalAppData}" STREQUAL "")
        set(ALIB_FETCH_FETCH_ROOT "$ENV{LocalAppData}/ALib")
      else()
        set(ALIB_FETCH_FETCH_ROOT "$ENV{USERPROFILE}/.alib")
      endif()
    elseif(APPLE)
      set(ALIB_FETCH_FETCH_ROOT "$ENV{HOME}/Library/ALib")
    else()
      set(ALIB_FETCH_FETCH_ROOT "$ENV{HOME}/.cache/ALib")
    endif()
  endif()

  if(NOT ALIB_FETCH_DOWNLOAD_TIMEOUT)
    if(DEFINED ALIB_DOWNLOAD_TIMEOUT AND NOT "${ALIB_DOWNLOAD_TIMEOUT}" STREQUAL "")
      set(ALIB_FETCH_DOWNLOAD_TIMEOUT "${ALIB_DOWNLOAD_TIMEOUT}")
    else()
      set(ALIB_FETCH_DOWNLOAD_TIMEOUT 60)
    endif()
  endif()

  if(NOT ALIB_FETCH_SHA256)
    if(DEFINED ALIB_SHA256 AND NOT "${ALIB_SHA256}" STREQUAL "")
      set(ALIB_FETCH_SHA256 "${ALIB_SHA256}")
    endif()
  endif()

  # Support either QUIET argument or ALIB_FETCH_QUIET cache variable.
  set(_alib_quiet "${ALIB_FETCH_QUIET}")
  if(NOT _alib_quiet AND DEFINED ALIB_FETCH_QUIET AND ALIB_FETCH_QUIET)
    set(_alib_quiet ON)
  endif()

  set(_alib_tag_with_v "${ALIB_FETCH_VERSION_TAG}")
  if(NOT _alib_tag_with_v MATCHES "^V")
    set(_alib_tag_with_v "V${_alib_tag_with_v}")
  endif()
  set(_alib_tag_without_v "${_alib_tag_with_v}")
  string(REGEX REPLACE "^V" "" _alib_tag_without_v "${_alib_tag_without_v}")

  # GitHub archive extraction currently yields a top-level folder without leading 'V'
  # (for tags like V2510R0 -> ALib-Class-Library-2510R0).
  set(_alib_src_dir "${ALIB_FETCH_FETCH_ROOT}/ALib-Class-Library-${_alib_tag_without_v}")

  if(EXISTS "${_alib_src_dir}/src/ALib.Lang.H" AND EXISTS "${_alib_src_dir}/src/alib")
    set(ALIB_BASE_DIR "${_alib_src_dir}" CACHE PATH "Path to ALib sources" FORCE)
    if(ALIB_FETCH_OUT_BASE_DIR)
      set(${ALIB_FETCH_OUT_BASE_DIR} "${ALIB_BASE_DIR}" PARENT_SCOPE)
    endif()
    return()
  endif()

  file(MAKE_DIRECTORY "${ALIB_FETCH_FETCH_ROOT}")
  set(_zip "${ALIB_FETCH_FETCH_ROOT}/ALib-${ALIB_FETCH_VERSION_TAG}.zip")
  set(_url "https://github.com/AlexWorx/ALib-Class-Library/archive/refs/tags/${ALIB_FETCH_VERSION_TAG}.zip")

  set(_dl_args SHOW_PROGRESS)
  if(_alib_quiet)
    set(_dl_args)
  endif()

  if(NOT _alib_quiet)
    message(STATUS "Downloading ALib ${ALIB_FETCH_VERSION_TAG} from ${_url}")
  endif()
  file(DOWNLOAD "${_url}" "${_zip}"
       TLS_VERIFY ON
       TIMEOUT "${ALIB_FETCH_DOWNLOAD_TIMEOUT}"
       STATUS _dl_status
       ${_dl_args})
  list(GET _dl_status 0 _dl_code)
  list(GET _dl_status 1 _dl_text)
  if(NOT _dl_code EQUAL 0)
    message(FATAL_ERROR "ALib download failed (${_dl_code}): ${_dl_text}")
  endif()

  if(ALIB_FETCH_SHA256)
    file(SHA256 "${_zip}" _zip_sha)
    if(NOT _zip_sha STREQUAL "${ALIB_FETCH_SHA256}")
      message(FATAL_ERROR "ALib zip checksum mismatch. Expected ${ALIB_FETCH_SHA256} got ${_zip_sha}")
    endif()
  endif()

  file(ARCHIVE_EXTRACT INPUT "${_zip}" DESTINATION "${ALIB_FETCH_FETCH_ROOT}")

  if(NOT (EXISTS "${_alib_src_dir}/src/ALib.Lang.H" AND EXISTS "${_alib_src_dir}/src/alib"))
    message(FATAL_ERROR
      "Extracted ALib sources not found under ${ALIB_FETCH_FETCH_ROOT} "
      "(expected path: ALib-Class-Library-${_alib_tag_without_v}).")
  endif()

  set(ALIB_BASE_DIR "${_alib_src_dir}" CACHE PATH "Path to ALib sources" FORCE)
  if(ALIB_FETCH_OUT_BASE_DIR)
    set(${ALIB_FETCH_OUT_BASE_DIR} "${ALIB_BASE_DIR}" PARENT_SCOPE)
  endif()
endfunction()
