//##################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
//========================================= Global Fragment ========================================
#include "alib/bitbuffer/bitbuffer.prepro.hpp"

//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.BitBuffer;
    import   ALib.Containers.FixedCapacityVector;
#else
#   include "ALib.Containers.FixedCapacityVector.H"
#   include "ALib.BitBuffer.H"
#endif

namespace alib {  namespace bitbuffer { namespace ac_v1 {

#if !DOXYGEN
//#define TEMP_PT(...)    __VA_ARGS__
#   define TEMP_PT(...)
#endif

#if !DOXYGEN
namespace
{
/// Internal struct representing nodes of the huffman code tree.
class Node
{
    using Symbol= HuffmanEncoder::Symbol;

  protected:
    /// Either a pointer to a left subtree or to a symbol.
    /// The latter makes the node a leaf.
    union LeftPointer
    {
        Symbol*         symbol;      ///< The symbol represented by this node (if not \c nullptr).
        Node*           left;        ///< The left subtree.

        LeftPointer()  = default;    ///< Non-initializing default constructor.

        /// Constructs this union with a symbol pointer.
        /// @param s The symbol to set.
        LeftPointer( Symbol* s )
        : symbol( s )                                                                             {}

        /// Constructs this union with a cursor.
        /// @param l The node to set.
        LeftPointer( Node* l )
        : left ( l )                                                                              {}
    };

    LeftPointer left;   ///< If #right set, then this is a pointer to the left subtree,
                        ///< otherwise a pointer to a symbol.
    Node*       right;  ///< The right subtree.


  public:
    size_t     frequency;    ///< The frequency of the symbol or the sum of the two subtrees.

    /// Default constructor used when defining arrays of nodes on stack memory.
    Node()                                                                                 =default;

    /// Constructs a node representing a symbol (leaf).
    /// Left and right pointers are set to nullptr
    /// @param s Pointer to the symbol in #symbols, that this node represents.
    Node(Symbol* s)
    : left      (s)
    , right     (nullptr)
    , frequency (s->frequency)                                                                    {}

    /// Constructs a node as an internal non-leaf node.
    /// Left and right pointers are set to as given.
    /// @param l Pointer to the left node.
    /// @param r Pointer to the right node.
    Node(Node* l, Node* r)
    : left      (l)
    , right     (r)
    , frequency (l->frequency + r->frequency)                                                     {}

    /// Determines if this is a leaf node.
    /// @returns \c true if this is a leaf node.
    bool    isLeaf()                                              const { return right == nullptr; }

    /// Must be called only for leaf nodes.
    /// @returns The pointer to the symbol.
    Symbol* getSymbol()                                                const { return left.symbol; }

    /// Must be called only for non-leaf nodes.
    /// @returns The left node.
    Node*   getLeft()                                                    const { return left.left; }

    /// @returns The right node.
    Node*   getRight()                                                       const { return right; }
}; //struct Node
} // anonymous namespace

#endif


void HuffmanEncoder::Generate() {
    constexpr int maxNodesNeeded= 256 + 255;
    Node nodePool[maxNodesNeeded];

    // buildTree
    Node* tree;
    {
        struct cmpHN { bool operator()(Node* l, Node* r)  { return (l->frequency > r->frequency); } };

        // create a list of nodes, sorted by frequency (least in front)
        // Note: for this, we first create fixed array on the stack, where we fill the
        //       nodes unsorted. Then we move this into the priority queue, what sorts them
        //       once. (unfortunately the move copies the array, but still it is faster than
        //       if the heap was sorted with each insert)
        int  npNext= 0;
        FixedCapacityVector<Node*, 256> underlyingNodeVector;
        for (std::size_t i = 0; i < 256 ; ++i)
            if( (symbols + i)->frequency > 0 )
                underlyingNodeVector.push_back( new (nodePool + npNext++) Node(symbols + i) );
        FixedSizePriorityQueue<Node*, 256, cmpHN> sortedNodes(cmpHN(), std::move(underlyingNodeVector));

ALIB_DBG( dbgAllValuesAreSame= (sortedNodes.size() == 1);  )
        // Merge two front nodes into one, until one node remains
        while (sortedNodes.size() > 1) {
            // Extract two least freq nodes
            Node* left = sortedNodes.top();   sortedNodes.pop();
            Node* right= sortedNodes.top();   sortedNodes.pop();

            // New internal node with frequency equal to the sum of the two nodes frequencies.
            // The two extracted nodes become left and right children
            sortedNodes.push(new ( nodePool + npNext++ ) Node(left, right) );
        }

        tree= sortedNodes.top();
        ALIB_ASSERT_ERROR( npNext <= maxNodesNeeded , "BITBUFFER/AC/HFMN", "This can never happen" )
    }

    // generate codes and write tree information to bit buffer
    {
        struct Stack
        {
            Node* node;
            int   walked;
        };

        int         depth                     = 0;
        Stack       stack[MAX_CODE_LENGTH];
                    stack[0]                  = Stack{ tree, 0 };
        uint32_t    words[MAX_WORDS]          = {};

TEMP_PT(Log_Warning("------ Huffman Encoding Table ----------")    )

        // 'recursion loop'
        while(depth>=0) {
            auto* node= stack[depth].node;

            auto wordNo= depth / WORD_SIZE;
            auto bitNo = depth % WORD_SIZE;

            // leaf?
            if( node->isLeaf() ) {
                // write '1' for leaf and symbol value
                bw.Write<9>( 1 | static_cast<unsigned>(node->getSymbol() - symbols) << 1  );

                // store word length and words to symbol's data
                node->getSymbol()->wordLength= depth;
                for( int i= 0 ; i <= wordNo   ; ++i )
                    node->getSymbol()->words[i]= words[i];

TEMP_PT(        NString512 bits; bits << Bin(node->symbol->words[0], node->symbol->wordLength);
                bits.Reverse();
                Lox_Warning("HM I: {:3}: {:<15}  (len={!ATAB:2}, freq={:>5})",
                            (node->symbol - symbols),
                             bits,
                             node->symbol->wordLength,
                             node->symbol->frequency )              )

                // clear last bit and step up
                words[wordNo]&= ~(uint32_t(1) << ( bitNo ) );
                --depth;
                continue;
            }

            // write '0' for not being a leave
            bw.Write<1>( 0u );

            // process left child
            if( stack[depth].walked == 0) {
                stack[depth].walked++;
                stack[depth+1]= Stack{ node->getLeft()  , 0};
                depth++;
                continue;
            }

            // process right child
            if( stack[depth].walked == 1) {
                stack[depth].walked++;
                words[wordNo] |= (1 << ( bitNo ) );
                stack[depth+1]= Stack{ node->getRight()  , 0};
                depth++;
                continue;
            }

            // clear last bit and step up
            words[wordNo]&= ~(uint32_t(1) << ( bitNo ) );
            --depth;
        }
TEMP_PT(  Log_Warning("------End of Huffman Encoding Table ----------")  )
}   }

void HuffmanDecoder::ReadTree() {
TEMP_PT(  Log_Warning("------ Huffman Decoding Table ----------")    )
TEMP_PT( String512 dbgWord; )
    struct Stack
    {
        Node* node;
        int   read; // 0: none, 1: left, 2: both
    };

    int     depth;
    Stack   stack[HuffmanEncoder::MAX_CODE_LENGTH];
    stack[depth= 0]= Stack{ &tree, 0 };
    while(depth>=0) {
        auto* node= stack[depth].node;
        // leaf?
        if( br.Read<1>() ) {
            node->symbol= uint8_t(br.Read<8>());
TEMP_PT(        Log_Warning("HM D: {:3}: {:5} (len={})",
                        (node->symbol),
                         dbgWord,
                         dbgWord.Length()   )                    )

            --depth;
TEMP_PT(        dbgWord.DeleteEnd(1);   )
            continue;
        }

        // left 'recursion'
        if( stack[depth].read == 0) {
            ++stack[depth].read;
            node->left= new ( nodePool + npNext++ ) Node();
TEMP_PT(        dbgWord << '0';          )
            stack[depth+1]= Stack{ node->left  , 0 };
            ++depth;
            continue;
        }

        // right 'recursion'
        if( stack[depth].read == 1) {
            ++stack[depth].read;
TEMP_PT(        dbgWord << '1';    )
            node->right= new ( nodePool + npNext++ ) Node();
            stack[depth+1]= Stack{ node->right  , 0 };
            ++depth;
            continue;
        }

TEMP_PT(    dbgWord.DeleteEnd(1);     )
        --depth;
    }

ALIB_ASSERT_ERROR( npNext <= MAX_NODES, "BITBUFFER/AC/HFMN", "This can never happen" )

TEMP_PT(  Log_Warning("------End of Huffman Decoding Table ----------")  )
}




#undef TEMP_PT

}}} // namespace [alib::bitbuffer::ac_v1]
