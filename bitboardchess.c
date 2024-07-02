

/*************************************************\
===================================================
                System Headers
===================================================
\*************************************************/

#include <stdio.h>

// define bitboard data type
#define U64 unsigned long long

//  enumerate boards
enum {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1
};

// enum sides to moves (colors)
enum { white, black};


// for future uses
// "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
//  "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7"
//  "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6"
//  "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5"
//  "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4"
//  "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3"
//  "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2"
//  "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"


/*************************************************\
===================================================
                Bit Manipulations
===================================================
\*************************************************/

// set/get/pop macros
#define set_bit(bitboard, square) (bitboard |= (1ULL << square))
#define get_bit(bitboard, square) (bitboard & (1ULL << square))
#define pop_bit(bitboard, square) (get_bit(bitboard, square) ? bitboard ^= (1ULL << square) : 0)


// print bitboard
void print_bitboard(U64 bitboard) {

    printf("\n");

    // loop over board ranks or rows
    for (int rank = 0; rank < 8; rank++)
    {
        // loop over board files or column
        for (int file = 0; file < 8; file++)
        {
            // convert file & rank into square index
            int square = rank * 8 + file;

            // print ranks/row
            if (!file)
            {
                printf("  %d  ", 8 - rank);
            }
            

            // print bit sate (either 1 or 0)
            printf(" %d", get_bit(bitboard, square) ? 1 : 0);
        }
      
        // print new line every rank
        printf("\n");
    }

    // print board files/column
    printf("\n      a b c d e f g h\n\n");

    // print bitboard as unsigned decimal number
    printf("      Bitboard: %llud\n\n", bitboard);
   
}

/*************************************************\
===================================================
                Attacks Moves
===================================================
\*************************************************/

/**************************************************
        Not A File/Row

    8   0 1 1 1 1 1 1 1
    7   0 1 1 1 1 1 1 1
    6   0 1 1 1 1 1 1 1
    5   0 1 1 1 1 1 1 1
    4   0 1 1 1 1 1 1 1
    3   0 1 1 1 1 1 1 1
    2   0 1 1 1 1 1 1 1
    1   0 1 1 1 1 1 1 1

        a b c d e f g h

===================================================

        Not AB File/Row

    8   0 0 1 1 1 1 1 1
    7   0 0 1 1 1 1 1 1
    6   0 0 1 1 1 1 1 1
    5   0 0 1 1 1 1 1 1
    4   0 0 1 1 1 1 1 1
    3   0 0 1 1 1 1 1 1
    2   0 0 1 1 1 1 1 1
    1   0 0 1 1 1 1 1 1

        a b c d e f g h

===================================================

        Not GH File/Row

    8   1 1 1 1 1 1 0 0
    7   1 1 1 1 1 1 0 0
    6   1 1 1 1 1 1 0 0
    5   1 1 1 1 1 1 0 0
    4   1 1 1 1 1 1 0 0
    3   1 1 1 1 1 1 0 0
    2   1 1 1 1 1 1 0 0
    1   1 1 1 1 1 1 0 0

        a b c d e f g h

===================================================

        Not H File/Row

    8   1 1 1 1 1 1 1 0
    7   1 1 1 1 1 1 1 0
    6   1 1 1 1 1 1 1 0
    5   1 1 1 1 1 1 1 0
    4   1 1 1 1 1 1 1 0
    3   1 1 1 1 1 1 1 0
    2   1 1 1 1 1 1 1 0
    1   1 1 1 1 1 1 1 0

        a b c d e f g h

***************************************************/

// constant not A file/row
const U64 not_a_file = 18374403900871474942ULL;

// constant not AB file/row
const U64 not_ab_file = 18229723555195321596ULL; 

// constant not GH file/row
const U64 not_gh_file = 4557430888798830399ULL;

// constant not H file/row
const U64 not_h_file = 9187201950435737471ULL;


// pawn attacks table [side][square]
U64 pawn_attacks[2][64];

//generate pawn attacks
U64 mask_pawn_attacks(int side, int square) {

    // result attacks bitboard
    U64 attacks = 0ULL;

    // piece bitboard
    U64 bitboard = 0ULL;

    // set piece on board
    set_bit(bitboard, square);

    // white pawns moves
    if (!side)
    {
        if ((bitboard >> 7) & not_a_file) attacks |= (bitboard >> 7);
        if ((bitboard >> 9) & not_h_file) attacks |= (bitboard >> 9);
        
    }
    // black pawns moves
    else
    {
        if ((bitboard << 7) & not_h_file) attacks |= (bitboard << 7);
        if ((bitboard << 9) & not_a_file) attacks |= (bitboard << 9);
    }

    // return attacks map
    return attacks;
    
    
}

// init leaper pieces attacks
void init_leapers_attacks() {

    // loop over 64 board squares to check for legit moves
    for (int square = 0; square < 64; square++)
    {
        // init pawn attacks
        pawn_attacks[white][square] = mask_pawn_attacks(white, square);
        pawn_attacks[black][square] = mask_pawn_attacks(black, square);
    }
    
}





/*************************************************\
===================================================
                Defense Moves
===================================================
\*************************************************/







int main() {


    // Init leaper pieces attacks
    init_leapers_attacks();






    // U64 bitboard = 0ULL;
    // set_bit(bitboard, h2);
    print_bitboard(mask_pawn_attacks(black, e4));

    
    //for constant file/row abhg overboard control.
    // for (int i = 0; i < 8; i++)
    // {
    //     for (int j = 0; j < 8; j++)
    //     {
    //         int square = i * 8 + j;
    //         if (j > 1)
    //         {
    //             set_bit(not_a_file, square);
    //         }
            
            
    //     }
        
    // }
    
    // print_bitboard(not_a_file);


    // eum board codes/.
    // for (int i = 8; i >= 1; i--)
    // {
    //     printf(" \"a%d\", \"b%d\", \"c%d\", \"d%d\", \"e%d\", \"f%d\", \"g%d\", \"h%d\"\n", i, i, i, i, i, i, i, i, i);
    // }
    




}