

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
            

            // print bit state (either 1 or 0)
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

// knight attacks table [square]
U64 knight_attacks[64];

// king attacks table;
U64 king_attacks[64];

// bishop attacks
U64 bishop_attacks[64];

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
        // generate pawn attacks
        if ((bitboard >> 7) & not_a_file) attacks |= (bitboard >> 7);
        if ((bitboard >> 9) & not_h_file) attacks |= (bitboard >> 9);
        
    }
    // black pawns moves
    else
    {
        // generate pawn attacks
        if ((bitboard << 7) & not_h_file) attacks |= (bitboard << 7);
        if ((bitboard << 9) & not_a_file) attacks |= (bitboard << 9);
    }

    // return attacks map
    return attacks;
    
    
}

// generate knight attacks
U64 mask_knight_attacks(int square) {

    // result attacks bitboard
    U64 attacks = 0ULL;

    // piece bitboard
    U64 bitboard = 0ULL;

    // set piece on board
    set_bit(bitboard, square);

    // knight moves (offsets 17, 15, 10, 6)
    if ((bitboard >> 17) & not_h_file) attacks |= (bitboard >> 17);
    if ((bitboard >> 15) & not_a_file) attacks |= (bitboard >> 15);
    if ((bitboard >> 10) & not_gh_file) attacks |= (bitboard >> 10);
    if ((bitboard >> 6) & not_ab_file) attacks |= (bitboard >> 6);
    if ((bitboard << 17) & not_a_file) attacks |= (bitboard << 17);
    if ((bitboard << 15) & not_h_file) attacks |= (bitboard << 15);
    if ((bitboard << 10) & not_ab_file) attacks |= (bitboard << 10);
    if ((bitboard << 6) & not_gh_file) attacks |= (bitboard << 6);

    // return attacks map
    return attacks;

}

// generate king attacks
U64 mask_king_attacks(int square) {

    // result attacks bitboard
    U64 attacks = 0ULL;

    // piece bitboard
    U64 bitboard = 0ULL;

    // set piece on board
    set_bit(bitboard, square);

    // generate king attacks (1, 7, 8, 9 )
    // up
    if ((bitboard >> 1) & not_h_file) attacks |= (bitboard >> 1);
    if ((bitboard >> 7) & not_a_file) attacks |= (bitboard >> 7);
    if ((bitboard >> 8)) attacks |= (bitboard >> 8);
    if ((bitboard >> 9) & not_h_file) attacks |= (bitboard >> 9);
    // down
    if ((bitboard << 1) & not_a_file) attacks |= (bitboard << 1);
    if ((bitboard << 7) & not_h_file) attacks |= (bitboard << 7);
    if ((bitboard << 8)) attacks |= (bitboard << 8);
    if ((bitboard << 9) & not_a_file) attacks |= (bitboard << 9);

    // return attacks map
    return attacks;
    
}

// generate bishop attacks
U64 mask_bishop_attacks(int square) {

    // result attacks bitboard
    U64 attacks = 0ULL;

    // piece bitboard *1ULL for iteration see notes below
    U64 bitboard = 1ULL;

    // set piece on board *dont need this
    //set_bit(bitboard, square);

    // init ranks & files
    int r, f;

    // init target rank & files
    int tr = square / 8;
    //printf(" %d for tr debug\n", tr);
    int tf = square % 8;
    //printf(" %d for tf debug\n", tf);

    // mask relevant bishop occupancy bits
    for (r = tr +1, f = tf +1; r <=6 && f <=6 ; r++, f++) attacks |= (bitboard << (r * 8 + f)); // bottom right
    for (r = tr +1, f = tf -1; r <=6 && f >=1 ; r++, f--) attacks |= (bitboard << (r * 8 + f)); // bottom left
    for (r = tr -1, f = tf +1; r >=1 && f <=6 ; r--, f++) attacks |= (bitboard << (r * 8 + f)); // top right
    for (r = tr -1, f = tf -1; r >=1 && f >=1 ; r--, f--) attacks |= (bitboard << (r * 8 + f)); // top left

    /* So what my understanding of the difference between king, pawn, knight and bishop, rook possibly queen aswell are the amount of possible moves it can do.
    hence why the bit iteration are different here.
    Put it this way, for king, pawns, knight. you've set where the pieces are on the board with set_bit and make the moves because their movement is fixed and
    wont exceed the 64 bits of the board while only possible to make 1 fixed set of moves out of max of 8 for king, knight and 2 for pawns.
    Meanwhile bishop, rooks, and queen can move across the board if there are no pieces interupting hence you do a iteration with the given square and
    calculate each possible moves for each column and rows from that square hence the 1ULL is TO SET where the possible moves ARE!! IDIOT. I hope this makes sense for my future self. */

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

        // init knight attacks
        knight_attacks[square] = mask_knight_attacks(square);

        // init king attacks
        king_attacks[square] = mask_king_attacks(square);

        // init bishop attacks
        bishop_attacks[square] = mask_bishop_attacks(square);

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
    print_bitboard(mask_bishop_attacks(e4));

    // for (int i = 0; i < 64; i++)
    // {
    //     int a = i / 8;
    //     int b = i % 8;
    //     printf(" a = %d \n\n", a + 1);
    //     printf(" b = %d \n\n", b + 1);
    // }
    

    
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