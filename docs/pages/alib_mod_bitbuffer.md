// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/**
\page alib_mod_bitbuffer   ALib Module BitBuffer - Programmer's Manual

\I{################################################################################################}
# 1. Introduction
Serialization of data is usually needed for storage and for communication (data transport) purposes.
Here, algorithmic overhead to perform such serialization is mostly a welcome drawback at the moment
it helps to reasonably compress the data in respect to its usual representation in random access memory.

With the types found in this module, serialization is performed on bit-level instead of fixed-size
integral types.
While this implies a tremendous amount of bit-shifting operations, the two immediate advantages
are:
1. Any integral type may be stored exactly with the number of bits needed in accordance with its
   possible value range.
2. The data stream becomes platform-independent in respect to byte-order (big- vs. little-endian)
   and length of platform-dependent data types. (Note that already C++ <c>int</c> is of undefined
   length).

Encoding data in bit buffers usually omits any redundancy caused by attribute names. At least
if a straight forward use of the classes provided here is taken. Instead of attribute naming
(like with JSon or XML encoding) or attribute indexing (like done with Google Protobuf family of APIs
and tools), attribute information is purely defined by the position of the data in the stream.

\note
  Google \https{Protobuf APIs and tools,developers.google.com/protocol-buffers/} is a solid
  compromise between highly un-efficient encodings that JSon or XML provide and the bit buffer
  implementation presented with \alib.
  Especially the guaranteed downward compatibility of \e protobuf is an important argument
  for their use. It all depends on the use case scenario. In this respect, bit buffers are
  not at all to be seen as an alternative to those. They just cover a different set of use cases.

# 2. Classes BitBuffer, BitWriter and BitReader # {#alib_bitbuffer_classes}
To encode and decode data into a stream of bits, three classes are provided.
Class \alib{bitbuffer;BitBuffer} provides the memory allocation for the bit data as well as mechanics
for writing the encoded byte streams to <c>std::ostream</c> and likewise for retrieving them from
<c>std::istream</c>.<br>
Variants of \b BitBuffer are classes \alib{bitbuffer;BitBufferMA} and \alib{bitbuffer;BitBufferLocal},
which provide alternative memory management.

Class \alib{bitbuffer;BitWriter} is used to fill a \b BitBuffer and class \alib{bitbuffer;BitReader}
reads data back (either data just written by a \b BitWriter or data loaded using \c std::istream.

The interface of the classes is rather simple and straight forward, and for that reason this manual
refers its reader to the reference documentation for further information.

# 3. Array Compression # {#alib_bitbuffer_arrays}
When writing bigger amounts of integral data, it might be reasonable to compress such data.
Several different algorithms for compression are available. While the most prominent among them
implements \https{Huffman compression,en.wikipedia.org/wiki/Huffman_coding}, real-life data is often
better compressible with different approaches. The term "real-life" data here means, arrays of values
which are not randomly generated but occur from an application and has some special attributes.
For example, if a machine provides a certain "signal" which is logged regularly, the value of
such signal often is either:
- limited to a certain lower and upper bound,
- incrementally raising and falling
- only sparsely set (many zero or default values)
- or a combination of the upper.

The algorithms incorporated with \alib address such specialties.

All mechanics in respect to array compression are interfaced by class
#alib::bitbuffer::ac_v1::ArrayCompressor, which is as usually aliased to #alib::ArrayCompressor.
The rationale for introducing inner namespace \b v1 is that future versions of \alib may incorporate
changes in the binary format of one or more of the algorithms. In this case, a new namespace
\c v2 will be introduced and the old version will still be available to support the deserialization
of existing datasets. However, it is up to the programmer of software to detect the version
of compression used with filed data and to add mechanics to use the right version of the class
for decompression.

The list of algorithms are given with bitwise enumeration \alib{bitbuffer::ac_v1;ArrayCompressor::Algorithm}.
For details about them, please consult the reference documentation of this type.

If parameter \p algorithmsToTry is set to \alib{bitbuffer::ac_v1::ArrayCompressor;Algorithm::ALL} when
invoking method \alib{bitbuffer::ac_v1;ArrayCompressor::Compress}, each built-in algorithms is executed
and finally the one with the best compression ratio is chosen. With that, an algorithm-identifier
(currently 3 bits) is written upfront to enable method \alib{bitbuffer::ac_v1;ArrayCompressor::Decompress}
to choose the right algorithm for decoding the data.

While this incorporates a certain degree of overhead in respect to execution performance, such
overhead is exclusively done for encoding the data. With the fact that usually any data
is encoded only once and decoded (very) often, such overhead should not weigh in too much.
If so, the set of algorithms may be narrowed by providing a different set of algorithms with
parameter \p algorithmsToTry.

Most of the algorithms perform best if no unusual disruptive change in the data definition
occurs. However, in real-life applications, disruptive values may occur every once in a while.
Therefore, it might be efficient to constraint the arrays to a certain length and rather store
the data in smaller packages than just compress all data available. To explain what we mean here,
consider the following sample:<br>
Some software captures every second value from a vehicle, including:
- Speed
- Engine temperature
- Remaining-Fuel

Now, if this vehicle takes a tour of five hours when the three values are captured every second, it
might be more efficient to split the data into hourly packages of 3600 values for each of the
three signals. After giving this some thought, the rationale should be quite obvious.

<br><br><br><br><br><br> */
