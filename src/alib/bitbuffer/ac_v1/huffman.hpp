/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_bitbuffer of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_BITBUFFER_AC_V1_HUFFMAN
#define HPP_ALIB_BITBUFFER_AC_V1_HUFFMAN 1

#if !defined (HPP_AWORX_ALIB_BITBUFFER)
#   include "alib/bitbuffer/bitbuffer.hpp"
#endif

namespace aworx { namespace lib { namespace bitbuffer { namespace ac_v1 {

/**
 * This class, together with sibling \alib{bitbuffer::ac_v1,HuffmanDecoder} implements the well known
 * \https{Huffman compression,en.wikipedia.org/wiki/Huffman_coding}.
 * The class uses type \alib{bitbuffer,BitWriter} to write the bit stream into an underlying
 * \alib{bitbuffer,BitBuffer}.
 *
 * The use of this class is quite straight forward:
 * - Create an instance of this class
 * - Feed the data to be compressed once to the algorithm using method
 *   \alib{bitbuffer::ac_v1::HuffmanEncoder,CountSymbol}
 * - Invoke method \alib{bitbuffer::ac_v1::HuffmanEncoder,Generate}
 * - Feed all data a second time using method
 *   \alib{bitbuffer::ac_v1::HuffmanEncoder,Write}.
 */
class HuffmanEncoder
{
    public:
        static constexpr int   WORD_SIZE      = 32; ///< Codes are stored in two words of size 32
                                                    ///< (this constant)
        static constexpr int   MAX_WORDS      = 2;  ///< Codes are stored in two (this constant) words
                                                    ///< of size 32.

        /**
         * The maximum length of a huffman code.
         * In theory, with 256 symbols, a maximum code length of 255 could occur. For this, each
         * next symbol needed to have the double frequency than the previous. Hence, setting the
         * value to 64, limits the encoded data size to 2^64 symbols, which should be more than enaugh
         * for the next decades.
         */
        static constexpr int   MAX_CODE_LENGTH   = 64;

        /**
         * Information about the encoding of symbols. The symbol's value (between \c 0  and \c 255) is
         * not included, but deducted from the objects position in the symbol array found in
         * field \alib{bitbuffer::ac_v1::HuffmanEncoder,symbols}.
         */
        struct Symbol
        {
            std::size_t         frequency        =  0;     ///< The number of occurrences of the symbol.
            aworx::ShiftOpRHS   wordLength       =  0;     ///< 0: symbol not used, otherwise between 1 and 255.
            uint32_t            words[MAX_WORDS] = {0,0};  ///< The bitcode of the symbol.
        };

    protected:

    BitWriter&      bw;             ///< The bit writer to use for encoding the data.
    Symbol          symbols[256];   ///< The symbol table.

ALIB_DBG( bool      dbgAllValuesAreSame;  )

    public:
        /**
         * Constructor.
         * @param bitWriter  The bit writer to write the encoding information as well as the encoded
         *                   symbols to. (Stored in field #bw.)
         */
        HuffmanEncoder( BitWriter& bitWriter )
        : bw(bitWriter)
        { }

        /**
         * Counts a symbol.
         * This method has to be invoked for each byte to compress later.
         * @param symbol  The symbol to count.
         */
        ALIB_FORCE_INLINE
        void CountSymbol(uint8_t symbol)
        {
            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            symbols[symbol].frequency++;
            ALIB_WARNINGS_RESTORE
        }

    /**
     * Generates the huffman encoding table and writes this information to the bit writer.
     */
    ALIB_API
    void Generate();

    /**
     * Writes the given \p symbol to the bit stream.
     * @param symbol The symbol to write.
     */
    void Write(uint8_t symbol)
    {
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        auto& rec     = symbols[symbol];
        int   bitsLeft= rec.wordLength;
        #if ALIB_STRINGS
            ALIB_ASSERT_ERROR( (bitsLeft != 0) | dbgAllValuesAreSame, "BITBUFFER/AC/HFMN",
                               NString256("Try to write symbol unknown to Huffman table: ")
                               <<  symbol << ", " << bitsLeft )
        #else
            ALIB_ASSERT_ERROR( bitsLeft | dbgAllValuesAreSame, "BITBUFFER/AC/HFMN",
                               "Try to write symbol unknown to Huffman table: ",  symbol )
        #endif


        auto* word= rec.words;
        if( bitsLeft > 32)
        {
            bw.Write<32>( *word );
            word++;
            bitsLeft-= 32;
        }
        ALIB_WARNINGS_RESTORE

        bw.Write(bitsLeft, *word );
    }

}; // HuffmanEncoder

/**
 * This class, together with sibling \alib{bitbuffer::ac_v1,HuffmanEncoder} implements the well known
 * \https{Huffman compression,en.wikipedia.org/wiki/Huffman_coding}.
 * The class uses type \alib{bitbuffer,BitReader} to read the bit stream from an underlying
 * \alib{bitbuffer,BitBuffer}.
 *
 * The use of this class is quite straight forward:
 * - Prepare a \b BitReader to point to the beginning of the bit stream generated with
 *   class \b HuffmanEncoder.
 * - Invoke method \alib{bitbuffer::ac_v1::HuffmanDecoder,ReadTree} once.
 * - For each encoded byte, invoke \alib{bitbuffer::ac_v1::HuffmanDecoder,Read}.
 *
 * Note, that the length of the stream (the number of bytes to be decompressed) have to be
 * known by the using software. This class is not responsible for storing this piece of information.
 */
class HuffmanDecoder
{
    protected:
        /** Internal struct representing nodes of the huffman code tree.   */
        struct Node
        {
            Node*    left;      ///< The left child node.
            Node*    right;     ///< The right child node.
            uint8_t  symbol;    ///< If this is a leaf node (neither #left nor #right are set, then
                                ///< then this is the symbol found.

            /** Constructor. */
            Node()
            : left      (nullptr)
            , right     (nullptr)
            {}
        };

        static constexpr int    MAX_NODES= 511;      ///< The maximum number of nodes in the tree.

        BitReader&              br;                  ///< The bit reader given in the constructor.
        Node                    tree;                ///< The root node of the symbol tree.
        Node                    nodePool[MAX_NODES]; ///< Pre-allocated node objects.
        int                     npNext= 0;           ///< The next node in #nodePool to use.

    public:
        /**
         * Constructor.
         * @param bitReader The bit reader to read the huffman encoding table and then the encoded
         *                  symbols from. (Stored in field #br.)
         */
        HuffmanDecoder( BitReader& bitReader )
        :br(bitReader)
        {}

    /** Reads the information to decode the data from the beginning of the bit stream.
     * This method has to be invoked once prior to reading the symbols with method #Read.   */
    ALIB_API
    void ReadTree();

    /** Reads a symbol from the bit stream.
     * This method has to be invoked for every symbol stored, after once reading the encoding
     * information with method #ReadTree.
     * @return The symbol read.
     */
    uint8_t  Read()
    {
        Node* node= &tree;
        while(true)
        {
            if( node->left == nullptr) // (could also test on right)
                return node->symbol;

            auto v= br.Read<1>();
            node= v ? node->right
                    : node->left;
        }
    }

}; // HuffmanDecoder

}}}} // namespace [aworx::lib::bitbuffer::ac_v1]

#endif // HPP_ALIB_BITBUFFER_AC_V1_HUFFMAN
