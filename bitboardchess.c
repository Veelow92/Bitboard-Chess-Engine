

/*************************************************\
===================================================
                System Headers
===================================================
\*************************************************/

#include <stdio.h>
#include <string.h>

// define bitboard data type
#define U64 unsigned long long

// FEN dedug positions
#define empty_board "8/8/8/8/8/8/8/8 w - - "
#define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
#define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define cmk_position "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "

//  enumerate boards
enum {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1, no_sq
};

// encode pieces
enum { P, N, B, R, Q, K, p, n, b, r, q, k };

// enum sides to moves (colors)
enum { white, black, both};

// enum rook and bishop
enum { rook, bishop};

// castling rights binary encoding
/*

    bin  dec
    
   0001    1  white king can castle to the king side
   0010    2  white king can castle to the queen side
   0100    4  black king can castle to the king side
   1000    8  black king can castle to the queen side

   examples

   1111       both sides an castle both directions
   1001       black king => queen side
              white king => king side

*/
// castling rights binary encoding
enum { wk = 1, wq = 2, bk = 4, bq = 8};

// convert squares to coordinates
const char *square_to_coordinates[] = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
};

// ASCII pieces
char ascii_pieces[12] = "PNBRQKpnbrqk";

// unicode pieces
// char *unicode_pieces[12] = {"♙", "♘", "♗", "♖", "♕", "♔", "♟", "♞", "♝", "♜", "♛", "♚"};
char *unicode_pieces[12] = {"♙", "♘", "♗", "♖", "♕", "♔", "♟", "♞", "♝", "♜", "♛", "♚"};


// convert ASCII character pieces to encoded constants
int char_pieces[] = {
    ['P'] = P,
    ['N'] = N,
    ['B'] = B,
    ['R'] = R,
    ['Q'] = Q,
    ['K'] = K,
    ['p'] = p,
    ['n'] = n,
    ['b'] = b,
    ['r'] = r,
    ['q'] = q,
    ['k'] = k
};

// promoted pieces
char promoted_pieces[] = {
    [Q] = 'q',
    [R] = 'r',
    [B] = 'b',
    [N] = 'n',
    [q] = 'q',
    [r] = 'r',
    [b] = 'b',
    [n] = 'n'
};

/**********************************\
 ==================================
 
            Chess board
 
 ==================================
\**********************************/

/*
                            WHITE PIECES


        Pawns                  Knights              Bishops
        
  8  0 0 0 0 0 0 0 0    8  0 0 0 0 0 0 0 0    8  0 0 0 0 0 0 0 0
  7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0
  6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0
  5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0
  4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0
  3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0
  2  1 1 1 1 1 1 1 1    2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0
  1  0 0 0 0 0 0 0 0    1  0 1 0 0 0 0 1 0    1  0 0 1 0 0 1 0 0

     a b c d e f g h       a b c d e f g h       a b c d e f g h


         Rooks                 Queens                 King

  8  0 0 0 0 0 0 0 0    8  0 0 0 0 0 0 0 0    8  0 0 0 0 0 0 0 0
  7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0
  6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0
  5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0
  4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0
  3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0
  2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0
  1  1 0 0 0 0 0 0 1    1  0 0 0 1 0 0 0 0    1  0 0 0 0 1 0 0 0

     a b c d e f g h       a b c d e f g h       a b c d e f g h


                            BLACK PIECES


        Pawns                  Knights              Bishops
        
  8  0 0 0 0 0 0 0 0    8  0 1 0 0 0 0 1 0    8  0 0 1 0 0 1 0 0
  7  1 1 1 1 1 1 1 1    7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0
  6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0
  5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0
  4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0
  3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0
  2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0
  1  0 0 0 0 0 0 0 0    1  0 0 0 0 0 0 0 0    1  0 0 0 0 0 0 0 0

     a b c d e f g h       a b c d e f g h       a b c d e f g h


         Rooks                 Queens                 King

  8  1 0 0 0 0 0 0 1    8  0 0 0 1 0 0 0 0    8  0 0 0 0 1 0 0 0
  7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0
  6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0
  5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0
  4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0
  3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0
  2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0
  1  0 0 0 0 0 0 0 0    1  0 0 0 0 0 0 0 0    1  0 0 0 0 0 0 0 0

     a b c d e f g h       a b c d e f g h       a b c d e f g h



                             OCCUPANCIES


     White occupancy       Black occupancy       All occupancies

  8  0 0 0 0 0 0 0 0    8  1 1 1 1 1 1 1 1    8  1 1 1 1 1 1 1 1
  7  0 0 0 0 0 0 0 0    7  1 1 1 1 1 1 1 1    7  1 1 1 1 1 1 1 1
  6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0
  5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0
  4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0
  3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0
  2  1 1 1 1 1 1 1 1    2  0 0 0 0 0 0 0 0    2  1 1 1 1 1 1 1 1
  1  1 1 1 1 1 1 1 1    1  0 0 0 0 0 0 0 0    1  1 1 1 1 1 1 1 1



                            ALL TOGETHER

                        8  ♜ ♞ ♝ ♛ ♚ ♝ ♞ ♜
                        7  ♟︎ ♟︎ ♟︎ ♟︎ ♟︎ ♟︎ ♟︎ ♟︎
                        6  . . . . . . . .
                        5  . . . . . . . .
                        4  . . . . . . . .
                        3  . . . . . . . .
                        2  ♙ ♙ ♙ ♙ ♙ ♙ ♙ ♙
                        1  ♖ ♘ ♗ ♕ ♔ ♗ ♘ ♖

                           a b c d e f g h

*/

// piece bitboards
U64 bitboards[12];

// occupancy bitboards
U64 occupancies[3];

// side to move
int side;

// enpassant square
int enpassant = no_sq;

// castling rights
int castle;

/*************************************************\
===================================================
                PRNG / Magic Numbers
===================================================
\*************************************************/

//  pseudo random number state
unsigned int random_state = 1804289383;

// generate 32-bit pseudo legal numbers
unsigned int get_random_U32_number() {

    // get current state
    unsigned int number = random_state;

    // XOR shift algorithm
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;

    // update random number sate
    random_state = number;

    // return random number
    return number;
}

// generate 64-bit pseudo legal numbers
U64 get_random_U64_numbers() {

    // define 4 random numbers
    U64 n1, n2, n3, n4;

    // init random numbers slicing 16bits from MS1B side
    n1 = (U64)(get_random_U32_number()) & 0xFFFF;
    n2 = (U64)(get_random_U32_number()) & 0xFFFF;
    n3 = (U64)(get_random_U32_number()) & 0xFFFF;
    n4 = (U64)(get_random_U32_number()) & 0xFFFF;

    // return random number
    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

// generate magic number candidate
U64 generate_magic_number () {
    // return magic numbers
    return get_random_U64_numbers() & get_random_U64_numbers() & get_random_U64_numbers();
}

/*************************************************\
===================================================
                Bit Manipulations
===================================================
\*************************************************/

// set/get/pop macros  *****need to add brackets on bitboard and square and update pop bit double check
#define set_bit(bitboard, square) (bitboard |= (1ULL << square))
#define get_bit(bitboard, square) (bitboard & (1ULL << square))
#define pop_bit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))
// #define pop_bit(bitboard, square) (get_bit(bitboard, square) ? bitboard ^= (1ULL << square) : 0)

// count active bits within a bitboard
static inline int count_bits(U64 bitboard) {
    
    // bit counter
    int count = 0;

    // consecutively reset least significant 1st bit
    while (bitboard)
    {
        // increment count
        count++;

        // reset least significant 1st bit
        bitboard &= bitboard - 1;
    }
    
    // return bit count
    return count;

}

// get least significant 1st bit index
static inline int get_ls1b_index(U64 bitboard) {

    // make sure bitboard is not 0!!
    if (bitboard)
    {
        // count trailing bits before LS1B
        return count_bits((bitboard & -bitboard) - 1);
    } else
    {
        // return illegal index
        return -1;
    }
    
    
}


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

// print board
void print_board() {

    // loop over board ranks
    for (int rank = 0; rank < 8; rank++)
    {
        // loop over board file
        for (int file = 0; file < 8; file++)
        {
            // init square
            int square = rank * 8 + file;

            // print ranks/row
            if (!file)
            {
                printf("  %d  ", 8 - rank);
            }

            // define piece variable
            int piece = -1;

            // loop over all pieces bitboards
            for (int bb_piece = P; bb_piece <= k; bb_piece++)
            {
                // if there is a piece on current square
                if (get_bit(bitboards[bb_piece], square))
                {
                    // get piece code
                    piece = bb_piece;
                }
                
            }
            // print different piece set depending on OS
            #ifdef WIN64
                printf(" %c", (piece == -1) ? '.' : ascii_pieces[piece]);
            #else
                printf(" %s", (piece == -1) ? "." : unicode_pieces[piece]);
            #endif
        }
        // print new line every rank
        printf("\n");

        
    }

    // print board files/column
    printf("\n      a b c d e f g h\n\n");

    // print side to move
    printf("      Side:     %s\n", !side ? "White" : "Black");

    // print enpassant square
    printf("     Enpassant:   %s\n", (enpassant != no_sq) ? square_to_coordinates[enpassant] : "no");

    // print castling rights
    printf("  Castling:      %c%c%c%c\n\n", (castle & wk) ? 'K' : '-', (castle & wq) ? 'Q' : '-', (castle & bk) ? 'k' : '-', (castle & bq) ? 'q' : '-');
}

// parse FEN string
void parse_fen(char *fen) {
    // reset board position (bitboards)
    memset(bitboards, 0ULL, sizeof(bitboards));

    // reset occupancies (bitboard)
    memset(occupancies, 0ULL, sizeof(occupancies));

    // reset game state state variables
    side = 0;
    enpassant = no_sq;
    castle = 0;

    // loop over board ranks
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            // init current square
            int square = rank * 8 + file;

            // match ascii pieces within FEN string
            if ((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z'))
            {
                // init piece type
                int piece = char_pieces[*fen];

                // set piece on corresponding bitboard
                set_bit(bitboards[piece], square);

                // increment pointer to FEN string
                fen++;
            }

            // match empty square numbers within FEN string
            if (*fen >= '0' && *fen <= '9')
            {
                // init offset (convert char 0 to int 0)
                int offset = *fen - '0';
                /* This line converts the character digit to an integer value.
                 The character '0' has an ASCII value of 48. 
                 By subtracting '0' from the character, 
                 we get the integer value it represents:

                '1' (ASCII 49) - '0' (ASCII 48) = 1
                '2' (ASCII 50) - '0' (ASCII 48) = 2
                ...
                '9' (ASCII 57) - '0' (ASCII 48) = 9 
                
                */

                // define piece variable
                int piece = -1;

                // loop over all pieces bitboards
                for (int bb_piece = P; bb_piece <= k; bb_piece++)
                {
                    // if there is a piece on current square
                    if (get_bit(bitboards[bb_piece], square))
                    {
                        // get piece code
                        piece = bb_piece;
                    }
                    
                }

                // if there are no previous pieces, 
                if (piece == -1)
                {
                    // decrement file to not exceed 7 and move to new rows/rank
                    file--;
                }

                // adjust by current file counter added by the number of empty bits on offset
                file += offset;

                // increment pointer to FEN string
                fen++;
            }

            // match rank separator
            if (*fen == '/')
            {
                // increment pointer to FEN string
                fen++;
            }
            
            
            
        }
        
    }
    
    // go to parsing side to move (increment pointer to FEN string)
    fen++;

    // parse side to move
    (*fen == 'w') ? (side = white) : (side = black);

    // go to parsing castling rights
    fen += 2;

    // parse castling rights
    while (*fen != ' ')
    {
        switch (*fen)
        {

        case 'K': castle |= wk; break;
        case 'Q': castle |= wq; break;
        case 'k': castle |= bk; break;
        case 'q': castle |= bq; break;
        case '-': break;

        }

        // increment pointer to FEN string
        fen++;
    }
    
    // go to parsing enpassant square (increment pointer to FEN string)
    fen++;

    // parse enpassant square
    if (*fen != '-')
    {
        // parse enpassant file & rank into squares /* cant this be directly use into square_to_coordinates?
        int file = fen[0] - 'a';
        int rank = 8 - (fen[1] - '0');

        // int enpassant square
        enpassant = rank * 8 + file;
    } 
    // no enpassant square
    else
    {
        enpassant = no_sq;
    }
    
    // loop over white pieces bitboards
    for (int piece = P; piece <= K; piece++)
    {
        // populate white occupancies bitboard
        occupancies[white] |= bitboards[piece];
    }

    // loop over black pieces bitboards
    for (int piece = p; piece <= k; piece++)
    {
        // populate white occupancies bitboard
        occupancies[black] |= bitboards[piece];
    }

    // init all occupancies
    occupancies[both] |= occupancies[white];
    occupancies[both] |= occupancies[black];

    // debug FEN
    //printf("fen: %s\n", fen);
    
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

// bishop attack masks
U64 bishop_masks[64];

// rook attack masks
U64 rook_masks[64];

// bishop attacks table [square][occupancies]
U64 bishop_attacks[64][512];

// rook attacks table [square][occupancies]
U64 rook_attacks[64][4096];

// bishop relevant occupancy bit count for every square on board
const int bishop_relevant_bits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6, 
    5, 5, 5, 5, 5, 5, 5, 5, 
    5, 5, 7, 7, 7, 7, 5, 5, 
    5, 5, 7, 9, 9, 7, 5, 5, 
    5, 5, 7, 9, 9, 7, 5, 5, 
    5, 5, 7, 7, 7, 7, 5, 5, 
    5, 5, 5, 5, 5, 5, 5, 5, 
    6, 5, 5, 5, 5, 5, 5, 6
};

// rook relevant occupancy bit count for every square on board
const int rook_relevant_bits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    12, 11, 11, 11, 11, 11, 11, 12
};

// rook magic numbers
U64 rook_magic_numbers[64] = {
    0x8a80104000800020ULL,
    0x140002000100040ULL,
    0x2801880a0017001ULL,
    0x100081001000420ULL,
    0x200020010080420ULL,
    0x3001c0002010008ULL,
    0x8480008002000100ULL,
    0x2080088004402900ULL,
    0x800098204000ULL,
    0x2024401000200040ULL,
    0x100802000801000ULL,
    0x120800800801000ULL,
    0x208808088000400ULL,
    0x2802200800400ULL,
    0x2200800100020080ULL,
    0x801000060821100ULL,
    0x80044006422000ULL,
    0x100808020004000ULL,
    0x12108a0010204200ULL,
    0x140848010000802ULL,
    0x481828014002800ULL,
    0x8094004002004100ULL,
    0x4010040010010802ULL,
    0x20008806104ULL,
    0x100400080208000ULL,
    0x2040002120081000ULL,
    0x21200680100081ULL,
    0x20100080080080ULL,
    0x2000a00200410ULL,
    0x20080800400ULL,
    0x80088400100102ULL,
    0x80004600042881ULL,
    0x4040008040800020ULL,
    0x440003000200801ULL,
    0x4200011004500ULL,
    0x188020010100100ULL,
    0x14800401802800ULL,
    0x2080040080800200ULL,
    0x124080204001001ULL,
    0x200046502000484ULL,
    0x480400080088020ULL,
    0x1000422010034000ULL,
    0x30200100110040ULL,
    0x100021010009ULL,
    0x2002080100110004ULL,
    0x202008004008002ULL,
    0x20020004010100ULL,
    0x2048440040820001ULL,
    0x101002200408200ULL,
    0x40802000401080ULL,
    0x4008142004410100ULL,
    0x2060820c0120200ULL,
    0x1001004080100ULL,
    0x20c020080040080ULL,
    0x2935610830022400ULL,
    0x44440041009200ULL,
    0x280001040802101ULL,
    0x2100190040002085ULL,
    0x80c0084100102001ULL,
    0x4024081001000421ULL,
    0x20030a0244872ULL,
    0x12001008414402ULL,
    0x2006104900a0804ULL,
    0x1004081002402ULL
};

// bishop magic numbers
U64 bishop_magic_numbers[64] = {
    0x40040844404084ULL,
    0x2004208a004208ULL,
    0x10190041080202ULL,
    0x108060845042010ULL,
    0x581104180800210ULL,
    0x2112080446200010ULL,
    0x1080820820060210ULL,
    0x3c0808410220200ULL,
    0x4050404440404ULL,
    0x21001420088ULL,
    0x24d0080801082102ULL,
    0x1020a0a020400ULL,
    0x40308200402ULL,
    0x4011002100800ULL,
    0x401484104104005ULL,
    0x801010402020200ULL,
    0x400210c3880100ULL,
    0x404022024108200ULL,
    0x810018200204102ULL,
    0x4002801a02003ULL,
    0x85040820080400ULL,
    0x810102c808880400ULL,
    0xe900410884800ULL,
    0x8002020480840102ULL,
    0x220200865090201ULL,
    0x2010100a02021202ULL,
    0x152048408022401ULL,
    0x20080002081110ULL,
    0x4001001021004000ULL,
    0x800040400a011002ULL,
    0xe4004081011002ULL,
    0x1c004001012080ULL,
    0x8004200962a00220ULL,
    0x8422100208500202ULL,
    0x2000402200300c08ULL,
    0x8646020080080080ULL,
    0x80020a0200100808ULL,
    0x2010004880111000ULL,
    0x623000a080011400ULL,
    0x42008c0340209202ULL,
    0x209188240001000ULL,
    0x400408a884001800ULL,
    0x110400a6080400ULL,
    0x1840060a44020800ULL,
    0x90080104000041ULL,
    0x201011000808101ULL,
    0x1a2208080504f080ULL,
    0x8012020600211212ULL,
    0x500861011240000ULL,
    0x180806108200800ULL,
    0x4000020e01040044ULL,
    0x300000261044000aULL,
    0x802241102020002ULL,
    0x20906061210001ULL,
    0x5a84841004010310ULL,
    0x4010801011c04ULL,
    0xa010109502200ULL,
    0x4a02012000ULL,
    0x500201010098b028ULL,
    0x8040002811040900ULL,
    0x28000010020204ULL,
    0x6000020202d0240ULL,
    0x8918844842082200ULL,
    0x4010011029020020ULL
};

// queen attacks
U64 queen_attacks[64];

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

// mask knight attacks
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

// mask king attacks
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

// mask bishop attacks
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

// mask rooks attacks
U64 mask_rook_attacks(int square) {

    // result attacks bitboard
    U64 attacks = 0ULL;

    // piece bitboard *1ULL for iteration
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

    // mask relevant rook occupancy bits
    for (r = tr +1; r <=6; r++) attacks |= (bitboard << (r * 8 + tf)); // bottom
    for (r = tr -1; r >=1; r--) attacks |= (bitboard << (r * 8 + tf)); // up
    for (f = tf -1; f >=1 ; f--) attacks |= (bitboard << (tr * 8 + f)); // right
    for (f = tf +1; f <=6 ; f++) attacks |= (bitboard << (tr * 8 + f)); // left

    // return attacks map
    return attacks;

}


// mask queen attacks
U64 mask_queen_attacks(int square) {

    // result attacks bitboard
    U64 attacks = 0ULL;

    // piece bitboard *1ULL for iteration
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

    // mask relevant queen occupancy bits
    for (r = tr +1; r <=6; r++) attacks |= (bitboard << (r * 8 + tf)); // bottom
    for (r = tr -1; r >=1; r--) attacks |= (bitboard << (r * 8 + tf)); // up
    for (f = tf -1; f >=1 ; f--) attacks |= (bitboard << (tr * 8 + f)); // right
    for (f = tf +1; f <=6 ; f++) attacks |= (bitboard << (tr * 8 + f)); // left
    for (r = tr +1, f = tf +1; r <=6 && f <=6 ; r++, f++) attacks |= (bitboard << (r * 8 + f)); // bottom right
    for (r = tr +1, f = tf -1; r <=6 && f >=1 ; r++, f--) attacks |= (bitboard << (r * 8 + f)); // bottom left
    for (r = tr -1, f = tf +1; r >=1 && f <=6 ; r--, f++) attacks |= (bitboard << (r * 8 + f)); // top right
    for (r = tr -1, f = tf -1; r >=1 && f >=1 ; r--, f--) attacks |= (bitboard << (r * 8 + f)); // top left

    // return attacks map
    return attacks;

}

/* ************************************************* */
/* Generate block occupancy for bishop, rook, queen */
/* ************************************************* */

// generate bishop attacks on the fly
U64 bishop_attacks_on_the_fly(int square, U64 block) {

    // result attacks bitboard
    U64 attacks = 0ULL;

    // piece bitboard *1ULL for iteration
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

    // generate block occupancy for bishop attacks
    for (r = tr +1, f = tf +1; r <=7 && f <=7 ; r++, f++) { attacks |= (bitboard << (r * 8 + f)); if((bitboard << (r * 8 + f)) & block) break;} // bottom right
    for (r = tr +1, f = tf -1; r <=7 && f >=0 ; r++, f--) { attacks |= (bitboard << (r * 8 + f)); if((bitboard << (r * 8 + f)) & block) break;}// bottom left
    for (r = tr -1, f = tf +1; r >=0 && f <=7 ; r--, f++) { attacks |= (bitboard << (r * 8 + f)); if((bitboard << (r * 8 + f)) & block) break;}// top right
    for (r = tr -1, f = tf -1; r >=0 && f >=0 ; r--, f--) { attacks |= (bitboard << (r * 8 + f)); if((bitboard << (r * 8 + f)) & block) break;}// top left

    /* Key Differences in Results with attacks to break or the conditions
        q. When the break occurs: (attacks)

        First loop: The break condition depends on the entire attacks bitboard. If any bit in attacks overlaps with block, the loop breaks. 
        This means that if an earlier position in the diagonal already set a bit in attacks that overlaps with block, the loop will break immediately in subsequent iterations.

        Second loop: The break condition depends only on the current position's bit. 
        The loop breaks only if the current shifted bitboard position overlaps with block, regardless of previous positions in the attacks bitboard.

        2. Impact of previous iterations: (bitboard << (r * 8 + f))

        First loop: Previous iterations can influence the breaking condition because attacks accumulates bits from all previous positions.
        Second loop: Only the current position is checked against block, so previous iterations do not affect the breaking condition. */

    // return attacks map
    return attacks;

}


// generate rooks attacks on the fly
U64 rook_attacks_on_the_fly(int square, U64 block) {

    // result attacks bitboard
    U64 attacks = 0ULL;

    // piece bitboard *1ULL for iteration
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

    // mask relevant rook occupancy bits
    for (r = tr +1; r <=7; r++) { attacks |= (bitboard << (r * 8 + tf)); if((bitboard << (r * 8 + tf)) & block) break;} // bottom
    for (r = tr -1; r >=0; r--) { attacks |= (bitboard << (r * 8 + tf)); if((bitboard << (r * 8 + tf)) & block) break;}// up
    for (f = tf -1; f >=0 ; f--) { attacks |= (bitboard << (tr * 8 + f)); if((bitboard << (tr * 8 + f)) & block) break;}// right
    for (f = tf +1; f <=7 ; f++) { attacks |= (bitboard << (tr * 8 + f)); if((bitboard << (tr * 8 + f)) & block) break;}// left

    // return attacks map
    return attacks;

}


// generate queen attacks on the fly
U64 queen_attacks_on_the_fly(int square, U64 block) {

    // result attacks bitboard
    U64 attacks = 0ULL;

    // piece bitboard *1ULL for iteration
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

    // mask relevant queen occupancy bits
    for (r = tr +1; r <=6; r++) { attacks |= (bitboard << (r * 8 + tf)); if((bitboard << (r * 8 + tf)) & block) break;}  // bottom
    for (r = tr -1; r >=1; r--) { attacks |= (bitboard << (r * 8 + tf)); if((bitboard << (r * 8 + tf)) & block) break;}  // up
    for (f = tf -1; f >=1 ; f--) { attacks |= (bitboard << (tr * 8 + f)); if((bitboard << (tr * 8 + f)) & block) break;}  // right
    for (f = tf +1; f <=6 ; f++) { attacks |= (bitboard << (tr * 8 + f)); if((bitboard << (tr * 8 + f)) & block) break;}  // left
    for (r = tr +1, f = tf +1; r <=6 && f <=6 ; r++, f++) { attacks |= (bitboard << (r * 8 + f)); if((bitboard << (r * 8 + f)) & block) break;}  // bottom right
    for (r = tr +1, f = tf -1; r <=6 && f >=1 ; r++, f--) { attacks |= (bitboard << (r * 8 + f)); if((bitboard << (r * 8 + f)) & block) break;}  // bottom left
    for (r = tr -1, f = tf +1; r >=1 && f <=6 ; r--, f++) { attacks |= (bitboard << (r * 8 + f)); if((bitboard << (r * 8 + f)) & block) break;}  // top right
    for (r = tr -1, f = tf -1; r >=1 && f >=1 ; r--, f--) { attacks |= (bitboard << (r * 8 + f)); if((bitboard << (r * 8 + f)) & block) break;}  // top left

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

        // // init bishop attacks
        // bishop_attacks[square] = mask_bishop_attacks(square);

        // // init rook attacks
        // rook_attacks[square] = mask_rook_attacks(square);

        // // init queen attacks
        // queen_attacks[square] = mask_queen_attacks(square);

    }
    
}

// set occupancies
U64 set_occupancy(int index, int bits_in_mask, U64 attack_mask) {

    // occupancy map
    U64 occupancy = 0ULL;

    // loop over the range of bits within attack mask
    for (int count = 0; count < bits_in_mask; count++)
    {
        // get LS1B index of attacks mask
        int square = get_ls1b_index(attack_mask);

        // pop LS1B in attack map
        pop_bit(attack_mask,square);

        // make sure occupancy is on board
        if (index & (1 << count))
        {
            // populate occupancy map
            occupancy |= (1ULL << square);
        }
        
    }
    
    // return occupancy map
    return occupancy;
}

/*************************************************\
===================================================
                Magics Numbers
===================================================
\*************************************************/

// find appropriate magic number
U64 find_magic_number(int square, int relevant_bits, int bishop) {

    // init occupancies
    U64 occupancies[4096];

    // init attack tables
    U64 attacks[4096];

    // init used attacks
    U64 used_attacks[4096];

    // init attack mask for a current piece
    U64 attack_mask = bishop ? mask_bishop_attacks(square) : mask_rook_attacks(square);

    // init occupancy indices
    int occupancy_indicies = 1 << relevant_bits;

    // loop over occupancy indices
    for (int index = 0; index < occupancy_indicies; index++)
    {
        // init occupancies
        occupancies[index] = set_occupancy(index, relevant_bits, attack_mask);

        // init attacks
        attacks[index] = bishop ? bishop_attacks_on_the_fly(square, occupancies[index]) : rook_attacks_on_the_fly(square, occupancies[index]);
    }

    // test magic numbers loop
    for (int random_count = 0; random_count < 100000000; random_count++)
    {
        // generate magic number candidate
        U64 magic_number = generate_magic_number();

        // skip inappropriate magic numbers
        if (count_bits((attack_mask * magic_number) & 0xFF00000000000000) < 6) continue;
        
        // init used attacks
        memset(used_attacks, 0ULL, sizeof(used_attacks));

        // init index & fail flag
        int index, fail;

        // test magic index
        for (index = 0, fail = 0; !fail && index < occupancy_indicies; index++)
        {
            // init magic index
            int magic_index = (int)((occupancies[index] * magic_number) >> (64 - relevant_bits));

            // if magic index works
            if (used_attacks[magic_index] == 0ULL)
            {
                // init used attacks
                used_attacks[magic_index] = attacks[index];
            } else if (used_attacks[magic_index] != attacks[index])
            {
                fail = 1;
            }
        }

        // if magic number works
        if (!fail)
        {
            // return it
            return magic_number;
        }
    }
    
    // if magic number doesnt work
    printf("  Magic Number Fails!\n");
    return 0ULL;
    
}

// init magic numbers
void init_magic_numbers() {

    // loop over 64 board squares
    for (int square = 0; square < 64; square++)
    {
        // init rook magic numbers
        // printf(" 0x%llxULL\n", find_magic_number(square, rook_relevant_bits[square], rook));
        rook_magic_numbers[square] = find_magic_number(square, rook_relevant_bits[square], rook);
    }

    // printf("\n\n");

    // loop over 64 board squares
    for (int square = 0; square < 64; square++)
    {
        // init bishop magic numbers
        // printf(" 0x%llxULL\n", find_magic_number(square, bishop_relevant_bits[square], bishop));
        bishop_magic_numbers[square] = find_magic_number(square, bishop_relevant_bits[square], bishop);
    }
    
}

// init slider piece's attack tables
void init_sliders_attacks(int bishop) {

    // loop over 64 board square
    for (int square = 0; square < 64; square++)
    {
        // init bishop & rook mask
        bishop_masks[square] = mask_bishop_attacks(square);
        rook_masks[square] = mask_rook_attacks(square);

        // init current mask
        U64 attacks_mask = bishop ? bishop_masks[square] : rook_masks[square];

        // init relevant occupancy bit count
        int relevant_bits_count = count_bits(attacks_mask);
        
        // init occupancy indicies
        int occupancy_indicies = (1 << relevant_bits_count);

        // loop over occupancy indicies
        for (int index = 0; index < occupancy_indicies; index++)
        {
            // bishop
            if (bishop)
            {
                // init current occupancy variation
                U64 occupancy = set_occupancy(index, relevant_bits_count, attacks_mask);

                // init magic index
                int magic_index = (occupancy * bishop_magic_numbers[square]) >> (64 - bishop_relevant_bits[square]);

                // init bishop attacks
                bishop_attacks[square][magic_index] = bishop_attacks_on_the_fly(square, occupancy);
            } 
            // rook
            else
            {
                // init current occupancy variation
                U64 occupancy = set_occupancy(index, relevant_bits_count, attacks_mask);

                // init magic index
                int magic_index = (occupancy * rook_magic_numbers[square]) >> (64 - rook_relevant_bits[square]);

                // init rook attacks
                rook_attacks[square][magic_index] = rook_attacks_on_the_fly(square, occupancy);
            }
            
            
        }
        
    }
    
}

// get bishop attacks
static inline U64 get_bishop_attacks(int square, U64 occupancy) {

    // get bishop attacks assuming current board occupancy
    occupancy &= bishop_masks[square];
    occupancy *= bishop_magic_numbers[square];
    occupancy >>= 64 - bishop_relevant_bits[square];

    // return bishop attacks
    return bishop_attacks[square][occupancy];
}

// get rook attacks
static inline U64 get_rook_attacks(int square, U64 occupancy) {

    // get rook attacks assuming current board occupancy
    occupancy &= rook_masks[square];
    occupancy *= rook_magic_numbers[square];
    occupancy >>= 64 - rook_relevant_bits[square];

    // return rook attacks
    return rook_attacks[square][occupancy];
}

// get queen attacks
static inline U64 get_queen_attacks(int square, U64 occupancy) {

    // init result attacks bitboard
    U64 queen_attacks = 0ULL;

    // init bishop occupancies
    U64 bishop_occupancy = occupancy;

    // init rook occupancies
    U64 rook_occupancy = occupancy;

    // get bishop attacks assuming current board occupancy
    bishop_occupancy &= bishop_masks[square];
    bishop_occupancy *= bishop_magic_numbers[square];
    bishop_occupancy >>= 64 - bishop_relevant_bits[square];

    // get bishop attacks
    queen_attacks = bishop_attacks[square][bishop_occupancy];

    // get rook attacks assuming current board occupancy
    rook_occupancy &= rook_masks[square];
    rook_occupancy *= rook_magic_numbers[square];
    rook_occupancy >>= 64 - rook_relevant_bits[square];

    // get rook attacks
    queen_attacks |= rook_attacks[square][rook_occupancy];

    // return queen attacks consist of both bishop and rooks
    return queen_attacks;
}

/*************************************************\
===================================================
                Move Generator
===================================================
\*************************************************/

// is current given square attacked by the current given sides
static inline int is_square_attacked(int square, int side) {

    // attacked by white pawn
    if ((side == white) && (pawn_attacks[black][square] & bitboards[P]))
    {
        return 1;
    }

    // attacked by black pawn
    if ((side == black) && (pawn_attacks[white][square] & bitboards[p]))
    {
        return 1;
    }

    // attacked by knights
    if (knight_attacks[square] & ((side == white) ? bitboards[N] : bitboards[n]))
    {
        return 1;
    }

    // attacked by bishops
    if (get_bishop_attacks(square, occupancies[both]) & ((side == white) ? bitboards[B] : bitboards[b]))
    {
        return 1;
    }
    
    // attacked by rooks
    if (get_rook_attacks(square, occupancies[both]) & ((side == white) ? bitboards[R] : bitboards[r]))
    {
        return 1;
    }

    // attacked by queens
    if (get_queen_attacks(square, occupancies[both]) & ((side == white) ? bitboards[Q] : bitboards[q]))
    {
        return 1;
    }

    // attacked by kings
    if (king_attacks[square] & ((side == white) ? bitboards[K] : bitboards[k]))
    {
        return 1;
    }
    
    // by default return false
    return 0;
}

// print attacked squares
void print_attacked_squares(int side) {

    printf("\n");
    // loop over board ranks
    for (int rank = 0; rank < 8; rank++)
    {
        // loop over files
        for (int file = 0; file < 8; file++)
        {
            // init squares
            int square = rank * 8 + file;

            // print ranks
            if (!file)
            {
                printf("  %d  ", 8 - rank);
            }
            
            // check wheter current square is attacked or not
            printf(" %d", is_square_attacked(square, side) ? 1 : 0);
        }

        // print newline every rank
        printf("\n");
        
    }

    // print files
    printf("\n      a b c d e f g h\n\n");
    
}


/*
          binary move bits                               hexidecimal constants
    
    0000 0000 0000 0000 0011 1111    source square       0x3f
    0000 0000 0000 1111 1100 0000    target square       0xfc0
    0000 0000 1111 0000 0000 0000    piece               0xf000
    0000 1111 0000 0000 0000 0000    promoted piece      0xf0000
    0001 0000 0000 0000 0000 0000    capture flag        0x100000
    0010 0000 0000 0000 0000 0000    double push flag    0x200000
    0100 0000 0000 0000 0000 0000    enpassant flag      0x400000
    1000 0000 0000 0000 0000 0000    castling flag       0x800000
*/

/* This step is called a preprocessor and macros, in which the compiler will check for this before compiling resulting in less to no memory uses.
    The way it works is basically a copy and paste without the need to identify the type as in function. Personal note is like LET/LAMBDA or formulas.*/

// encode move {this is called preprocessor and macros that the compiler will use before compiling if available}
#define encode_move(source, target, piece, promoted, capture, double, enpassant, castling) \
    (source) |           \
    (target << 6) |      \
    (piece << 12) |      \
    (promoted << 16) |   \
    (capture << 20) |    \
    (double << 21) |     \
    (enpassant << 22) |  \
    (castling << 23)     \

// extract source square
#define get_move_source(move) (move & 0x3f)

// extract target square
#define get_move_target(move) ((move & 0xfc0) >> 6)

// extract piece
#define get_move_piece(move) ((move & 0xf000) >> 12)

// extract promoted piece
#define get_move_promoted(move) ((move & 0xf0000) >> 16)

// extract capture flag
#define get_move_capture(move) (move & 0x100000)

// extract double pawn push flag
#define get_move_double(move) (move & 0x200000)

// extract enpassant flag
#define get_move_enpassant(move) (move & 0x400000)

// extract castling flag
#define get_move_castling(move) (move & 0x800000)

// move list structure
typedef struct {
    // moves
    int moves[256];
    
    // move count
    int count;
} moves;

// add move to the move list
static inline void add_move(moves *move_list, int move)
{
    // strore move
    move_list->moves[move_list->count] = move;
    
    // increment move count
    move_list->count++;
}

// print move (for UCI purposes)
void print_move(int move)
{
    printf("%s%s%c\n", square_to_coordinates[get_move_source(move)],
                     square_to_coordinates[get_move_target(move)],
                     promoted_pieces[get_move_promoted(move)]);
}


// print move list
void print_move_list(moves *move_list)
{
    printf("\n    move    piece   capture   double    enpass    castling\n\n");
    
    // loop over moves within a move list
    for (int move_count = 0; move_count < move_list->count; move_count++)
    {
        // init move
        int move = move_list->moves[move_count];
        
        #ifdef WIN64
            // print move
            printf("    %s%s%c   %c       %d         %d         %d         %d\n", square_to_coordinates[get_move_source(move)],
                                                                                  square_to_coordinates[get_move_target(move)],
                                                                                  promoted_pieces[get_move_promoted(move)],
                                                                                  ascii_pieces[get_move_piece(move)],
                                                                                  get_move_capture(move) ? 1 : 0,
                                                                                  get_move_double(move) ? 1 : 0,
                                                                                  get_move_enpassant(move) ? 1 : 0,
                                                                                  get_move_castling(move) ? 1 : 0);
        #else
            // print move
            printf("    %s%s%c   %s       %d         %d         %d         %d\n", square_to_coordinates[get_move_source(move)],
                                                                                  square_to_coordinates[get_move_target(move)],
                                                                                  promoted_pieces[get_move_promoted(move)],
                                                                                  unicode_pieces[get_move_piece(move)],
                                                                                  get_move_capture(move) ? 1 : 0,
                                                                                  get_move_double(move) ? 1 : 0,
                                                                                  get_move_enpassant(move) ? 1 : 0,
                                                                                  get_move_castling(move) ? 1 : 0);
        #endif
        
        // print total number of moves
        printf("\n\n    Total number of moves: %d\n\n", move_list->count);
    }
}

// generate all moves skeleton
static inline void generate_moves() {
    // init source & target squares
    int source_square, target_square;

    // define current piece's bitboard copy & it's attacks
    U64 bitboard, attacks;

    // loop over all the bitboards
    for (int piece = P; piece <= k; piece++)
    {
        // init piece bitboard copy
        bitboard = bitboards[piece];

        // generate white pawns and white king castling moves
        if (side == white)
        {
            // pick up white pawn bitboards index
            if (piece == P)
            {
                // loop over white pawn within white pawn bitboard
                while (bitboard)
                {
                    // init source square 
                    source_square = get_ls1b_index(bitboard);

                    // init target square
                    target_square = source_square - 8;

                    // generate quiet pawn moves
                    if (!(target_square < a8) && !get_bit(occupancies[both], target_square))
                    {
                        // pawn promotion
                        if (source_square >= a7 && source_square <= h7)
                        {
                            printf("%s%sq pawn promotion\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sr pawn promotion\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sb pawn promotion\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sn pawn promotion\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);

                        }

                        else
                        {
                            // one square ahead pawn move
                            printf("%s%s  pawn push\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);

                            // two squares ahead pawn move
                            if ((source_square >= a2 && source_square <= h2) && !get_bit(occupancies[both], target_square - 8))
                            {
                                printf("%s%s  double pawn push\n", square_to_coordinates[source_square], square_to_coordinates[target_square - 8]);

                            }
                            
                        }
                        
                    }

                     // init pawn attacks bitboard
                    attacks = pawn_attacks[side][source_square] & occupancies[black];
                    
                    // generate pawn captures
                    while (attacks)
                    {
                        // init target square
                        target_square = get_ls1b_index(attacks);
                        
                        // pawn promotion
                        if (source_square >= a7 && source_square <= h7)
                        {
                            printf("%s%sq pawn promotion capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sr pawn promotion capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sb pawn promotion capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sn pawn promotion capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                        }
                        
                        else
                        {
                            // one square ahead pawn move
                            printf("%s%s  pawn capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                        }
                        
                        // pop ls1b of the pawn attacks
                        pop_bit(attacks, target_square);
                    }
                    
                    // generate enpassant captures
                    if (enpassant != no_sq)
                    {
                        // lookup pawn attacks and bitwise AND with enpassant square (bit)
                        U64 enpassant_attacks = pawn_attacks[side][source_square] & (1ULL << enpassant);
                        
                        // make sure enpassant capture available
                        if (enpassant_attacks)
                        {
                            // init enpassant capture target square
                            int target_enpassant = get_ls1b_index(enpassant_attacks);
                            printf("%s%s  pawn enpassant capture\n", square_to_coordinates[source_square], square_to_coordinates[target_enpassant]);
                        }
                    }
                    
                    // pop ls1b from piece bitboard copy
                    pop_bit(bitboard, source_square);
                }
                
            }
            
            // castling moves
            if (piece == K)
            {
                // king side castling is vailable
                if (castle & wk)
                {
                    // make sure square between king and king's rook are empty
                    if (!get_bit(occupancies[both], f1) && !get_bit(occupancies[both], g1))
                    {
                        // make sure king and the f1 squares are not under attacks
                        if (!is_square_attacked(e1, black) && !is_square_attacked(f1, black))
                        {
                            printf("e1g1  castling move\n");
                        }
                        
                    }
                    
                }
                
                // queen side castling is available
                if (castle & wq)
                {
                    // make sure square between queen and queen's rook are empty
                    if (!get_bit(occupancies[both], b1) && !get_bit(occupancies[both], c1) && !get_bit(occupancies[both], d1))
                    {
                        // make sure queen and the d1 squares are not under attacks
                        if (!is_square_attacked(e1, black) && !is_square_attacked(d1, black))
                        {
                            printf("e1c1  castling move\n");
                        }
                        
                    }
                }
                
            }
            
        }
        
         // generate black pawns & black king castling moves
        else
        {
            // pick up black pawn bitboards index
            if (piece == p)
            {
                // loop over white pawns within white pawn bitboard
                while (bitboard)
                {
                    // init source square
                    source_square = get_ls1b_index(bitboard);
                    
                    // init target square
                    target_square = source_square + 8;
                    
                    // generate quiet pawn moves
                    if (!(target_square > h1) && !get_bit(occupancies[both], target_square))
                    {
                        // pawn promotion
                        if (source_square >= a2 && source_square <= h2)
                        {
                            printf("%s%sq pawn promotion\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sr pawn promotion\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sb pawn promotion\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sn pawn promotion\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                        }
                        
                        else
                        {
                            // one square ahead pawn move
                            printf("%s%s  pawn push\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            
                            // two squares ahead pawn move
                            if ((source_square >= a7 && source_square <= h7) && !get_bit(occupancies[both], target_square + 8))
                                printf("%s%s  double pawn push\n", square_to_coordinates[source_square], square_to_coordinates[target_square + 8]);
                        }
                    }
                    
                    // init pawn attacks bitboard
                    attacks = pawn_attacks[side][source_square] & occupancies[white];
                    
                    // generate pawn captures
                    while (attacks)
                    {
                        // init target square
                        target_square = get_ls1b_index(attacks);
                        
                        // pawn promotion
                        if (source_square >= a2 && source_square <= h2)
                        {
                            printf("%s%sq pawn promotion capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sr pawn promotion capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sb pawn promotion capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sn pawn promotion capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                        }
                        
                        else
                            // one square ahead pawn move
                            printf("%s%s  pawn capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                        
                        // pop ls1b of the pawn attacks
                        pop_bit(attacks, target_square);
                    }
                    
                    // generate enpassant captures
                    if (enpassant != no_sq)
                    {
                        // lookup pawn attacks and bitwise AND with enpassant square (bit)
                        U64 enpassant_attacks = pawn_attacks[side][source_square] & (1ULL << enpassant);
                        
                        // make sure enpassant capture available
                        if (enpassant_attacks)
                        {
                            // init enpassant capture target square
                            int target_enpassant = get_ls1b_index(enpassant_attacks);
                            printf("%s%s  pawn enpassant capture\n", square_to_coordinates[source_square], square_to_coordinates[target_enpassant]);
                        }
                    }
                    
                    // pop ls1b from piece bitboard copy
                    pop_bit(bitboard, source_square);
                }
            }

            // castling moves
            if (piece == k)
            {
                // king side castling is vailable
                if (castle & bk)
                {
                    // make sure square between king and king's rook are empty
                    if (!get_bit(occupancies[both], f8) && !get_bit(occupancies[both], g8))
                    {
                        // make sure king and the f8 squares are not under attacks
                        if (!is_square_attacked(e8, white) && !is_square_attacked(f8, white))
                        {
                            printf("e8g8  castling move\n");
                        }
                        
                    }
                    
                }
                
                // queen side castling is available
                if (castle & bq)
                {
                    // make sure square between queen and queen's rook are empty
                    if (!get_bit(occupancies[both], b8) && !get_bit(occupancies[both], c8) && !get_bit(occupancies[both], d8))
                    {
                        // make sure queen and the d8 squares are not under attacks
                        if (!is_square_attacked(e8, white) && !is_square_attacked(d8, white))
                        {
                            printf("e8c8  castling move\n");
                        }
                        
                    }
                }
                
            }
        }
        
        // generate knight moves
        if ((side == white) ? piece == N : piece == n)
        {
            // loop over source square of piece bitboard copy
            while (bitboard)
            {
                // init source square and ls1b base on init bitboard from the if condition
                source_square = get_ls1b_index(bitboard);

                // init piece attacks in order to get set of target square (if tertiary with bitwise ~ NOT)
                attacks = knight_attacks[source_square] & ((side == white) ? ~occupancies[white] : ~occupancies[black]);

                // loop over target squares available from generated attacks
                while (attacks)
                {
                    // init target square
                    target_square = get_ls1b_index(attacks);

                    // quiet move
                    if (!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_square))
                    {
                        printf("%s%s  piece quiet move\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                    }

                    else
                    {
                        // capture move
                        printf("%s%s  piece capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);

                    }
                    
                    // pop ls1b in current attacks set
                    pop_bit(attacks, target_square);
                }
                
                // pop ls1b of the current piece bitboard
                pop_bit(bitboard, source_square);
            }
            
        }
        
        // generate bishop moves
        if ((side == white) ? piece == B : piece == b)
        {
            // loop over source square of piece bitboard copy
            while (bitboard)
            {
                // init source square and ls1b base on init bitboard from the if condition
                source_square = get_ls1b_index(bitboard);

                // init piece attacks in order to get set of target square (if tertiary with bitwise ~ NOT)
                attacks = get_bishop_attacks(source_square, occupancies[both]) & ((side == white) ? ~occupancies[white] : ~occupancies[black]);

                // loop over target squares available from generated attacks
                while (attacks)
                {
                    // init target square
                    target_square = get_ls1b_index(attacks);

                    // quiet move
                    if (!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_square))
                    {
                        printf("%s%s  piece quiet move\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                    }

                    else
                    {
                        // capture move
                        printf("%s%s  piece capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);

                    }
                    
                    // pop ls1b in current attacks set
                    pop_bit(attacks, target_square);
                }

                // pop ls1b of the current piece bitboard
                pop_bit(bitboard, source_square);
            }
            
        }

        // generate rook moves
        if ((side == white) ? piece == R : piece == r)
        {
            // loop over source square of piece bitboard copy
            while (bitboard)
            {
                // init source square and ls1b base on init bitboard from the if condition
                source_square = get_ls1b_index(bitboard);

                // init piece attacks in order to get set of target square (if tertiary with bitwise ~ NOT)
                attacks = get_rook_attacks(source_square, occupancies[both]) & ((side == white) ? ~occupancies[white] : ~occupancies[black]);

                // loop over target squares available from generated attacks
                while (attacks)
                {
                    // init target square
                    target_square = get_ls1b_index(attacks);

                    // quiet move
                    if (!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_square))
                    {
                        printf("%s%s  piece quiet move\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                    }

                    else
                    {
                        // capture move
                        printf("%s%s  piece capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);

                    }
                    
                    // pop ls1b in current attacks set
                    pop_bit(attacks, target_square);
                }

                // pop ls1b of the current piece bitboard
                pop_bit(bitboard, source_square);
            }
            
        }

        // generate queen moves
        if ((side == white) ? piece == Q : piece == q)
        {
            // loop over source square of piece bitboard copy
            while (bitboard)
            {
                // init source square and ls1b base on init bitboard from the if condition
                source_square = get_ls1b_index(bitboard);

                // init piece attacks in order to get set of target square (if tertiary with bitwise ~ NOT)
                attacks = get_queen_attacks(source_square, occupancies[both]) & ((side == white) ? ~occupancies[white] : ~occupancies[black]);

                // loop over target squares available from generated attacks
                while (attacks)
                {
                    // init target square
                    target_square = get_ls1b_index(attacks);

                    // quiet move
                    if (!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_square))
                    {
                        printf("%s%s  piece quiet move\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                    }

                    else
                    {
                        // capture move
                        printf("%s%s  piece capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);

                    }
                    
                    // pop ls1b in current attacks set
                    pop_bit(attacks, target_square);
                }

                // pop ls1b of the current piece bitboard
                pop_bit(bitboard, source_square);
            }
            
        }

        // generate king moves
        if ((side == white) ? piece == K : piece == k)
        {
            // loop over source square of piece bitboard copy
            while (bitboard)
            {
                // init source square and ls1b base on init bitboard from the if condition
                source_square = get_ls1b_index(bitboard);

                // init piece attacks in order to get set of target square (if tertiary with bitwise ~ NOT)
                attacks = king_attacks[source_square] & ((side == white) ? ~occupancies[white] : ~occupancies[black]);

                // loop over target squares available from generated attacks
                while (attacks)
                {
                    // init target square
                    target_square = get_ls1b_index(attacks);

                    // quiet move
                    if (!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_square))
                    {
                        printf("%s%s  piece quiet move\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                    }

                    else
                    {
                        // capture move
                        printf("%s%s  piece capture\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);

                    }
                    
                    // pop ls1b in current attacks set
                    pop_bit(attacks, target_square);
                }

                // pop ls1b of the current piece bitboard
                pop_bit(bitboard, source_square);
            }
            
        }
    }
    
}

/*************************************************\
===================================================
                Init All
===================================================
\*************************************************/

// init all variables
void init_all() {
    // Init leaper pieces attacks
    init_leapers_attacks();

    // Init slider piece's attacks
    init_sliders_attacks(bishop);
    init_sliders_attacks(rook);

    // init magic numbers
    // init_magic_numbers();
}

/*************************************************\
===================================================
                Main Driver
===================================================
\*************************************************/

int main() {


    // Init all
    init_all();

    // parse_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ");
    // print_board();

     // create move list
    moves move_list[1];
    
    // init move count
    move_list->count = 0;
    
    // add move
    add_move(move_list, encode_move(d7, e8, B, Q, 0, 0, 0, 1));
    
    // print move list
    print_move_list(move_list);

    // create move
    // int move = encode_move(d7, e8, P, Q, 1, 0, 0, 0);
    
    // // exract move items
    // int source_square = get_move_source(move);
    // int target_square = get_move_target(move);
    // int piece = get_move_piece(move);
    // int promoted_piece = get_move_promoted(move);
    
    // // print move items
    // printf("source square: %s\n", square_to_coordinates[source_square]);
    // printf("target square: %s\n", square_to_coordinates[target_square]);
    // printf("piece: %s\n", unicode_pieces[piece]);
    // printf("piece: %s\n", unicode_pieces[promoted_piece]);
    // printf("capture flag: %d\n", get_move_capture(move) ? 1 : 0);
    // printf("double pawn push flag: %d\n", get_move_double(move) ? 1 : 0);
    // printf("enpassant flag: %d\n", get_move_enpassant(move) ? 1 : 0);
    // printf("castling flag: %d\n", get_move_castling(move) ? 1 : 0);

    // generate_moves();

    // parse FEN
    // parse_fen(start_position);

    // print_board();

    // // parse FEN
    // parse_fen(tricky_position);

    // print_board();

    // parse FEN
    // parse_fen(cmk_position);

    // print_board();

    // print_bitboard(occupancies[black]);
    // print_bitboard(occupancies[white]);
    // print_bitboard(occupancies[both]);




    // // set white pawns
    // set_bit(bitboards[P], a2);
    // set_bit(bitboards[P], b2);
    // set_bit(bitboards[P], c2);
    // set_bit(bitboards[P], d2);
    // set_bit(bitboards[P], e2);
    // set_bit(bitboards[P], f2);
    // set_bit(bitboards[P], g2);
    // set_bit(bitboards[P], h2);

    // // set white knight
    // set_bit(bitboards[N], b1);
    // set_bit(bitboards[N], g1);

    // // set white bishop
    // set_bit(bitboards[B], c1);
    // set_bit(bitboards[B], f1);

    // // set white rook
    // set_bit(bitboards[R], a1);
    // set_bit(bitboards[R], h1);

    // // set queen and king
    // set_bit(bitboards[Q], d1);
    // set_bit(bitboards[K], e1);

    // // set black pawns
    // set_bit(bitboards[P], a7);
    // set_bit(bitboards[P], b7);
    // set_bit(bitboards[P], c7);
    // set_bit(bitboards[P], d7);
    // set_bit(bitboards[P], e7);
    // set_bit(bitboards[P], f7);
    // set_bit(bitboards[P], g7);
    // set_bit(bitboards[P], h7);

    // // set black knight
    // set_bit(bitboards[n], b8);
    // set_bit(bitboards[n], g8);

    // // set black bishop
    // set_bit(bitboards[b], c8);
    // set_bit(bitboards[b], f8);

    // // set black rook
    // set_bit(bitboards[r], a8);
    // set_bit(bitboards[r], h8);

    // // set queen and king
    // set_bit(bitboards[k], d8);
    // set_bit(bitboards[q], e8);

    // side = black;
    // enpassant = e3;
    // castle |= wk;
    // castle |= wq;
    // castle |= bk;
    // castle |= bq;

    // print_board();




    // // define test bitboard
    // U64 occupancy = 0ULL;
    // set_bit(occupancy, c5);
    // set_bit(occupancy, f2);
    // set_bit(occupancy, g7);
    // set_bit(occupancy, b2);
    // set_bit(occupancy, g5);
    // set_bit(occupancy, e2);
    // set_bit(occupancy, e7);
    // set_bit(occupancy, e4);

    // print_bitboard(occupancy);

  
    // print_bitboard(get_rook_attacks(e5, occupancy));

    
    

    // printf("\n %d \n", bishop_relevant_bits[62]);

    // print_bitboard((U64)get_random_U32_number());
    // print_bitboard((U64)(get_random_U32_number() & 0xFFFF));
    // print_bitboard(get_random_U64_numbers());
    // print_bitboard(generate_magic_number());

    // // relevance occupancy bits count om each square
    // for (int i = 0; i < 8; i++)
    // {
    //     for (int j = 0; j < 8; j++)
    //     {
    //         int square = i * 8 + j;
    //         printf("%d, ", count_bits(mask_rook_attacks(square)));
    //     }
    //     printf("\n");
    // }
    
    // // mask piece attacks at give square
    // U64 atkmask = mask_rook_attacks(a1);

    // // loop over occupancy indicese
    // for (int i = 0; i < 4096; i++)
    // {
    //     print_bitboard(set_occupancy(i, count_bits(atkmask), atkmask));
    //     getchar();
    // };
    
    // // Init block occupancy
    // U64 block = 0ULL;
    // set_bit(block, f3);
    // set_bit(block, c6);

    // print_bitboard(block);


    // U64 bitboard = 0ULL;
    // set_bit(bitboard, h2);
    // print_bitboard(bishop_attacks_on_the_fly(e4, block));

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
    


    return 0;

}