#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ansi-colors.h"

#define boardlength 8

// Function prototypes
int getInt(char prompt[], int min, int max);
void titleScreen();
void rulesScreen();
void menuBuffer();
void printBoard(const char *arr[], int SIZE);
void swapPlaces(const char *arr[], int start, int destination);
int gameStateCheck(const char *arr[], int attacker, int defender, char turn);
void takePiece(const char *arr[], int attacker, int defender);
int isKingInCheck(const char *board[], char kingColor);
int firstNonEmptyLeft(const char *board[], int from);
int firstNonEmptyRight(const char *board[], int from);
int indexOfKing(const char *board[], char kingColor);
int pathClear(const char *board[], int from, int to);
int isLegalPieceMove(const char *board[], int from, int to);
int hasLegalMove(const char *board[], char turn);


int main()
{   
    const char *board[boardlength] = {"WK", "WN", "WR", "__", "__", "BR", "BN", "BK"};

    int startingPosition, endingPosition;
    int rounds = 0;
    const int MAX_ROUNDS = 20;
    int gameOver = 0;
    char currentTurn = 'W';   // White moves first

    titleScreen();
    rulesScreen();
    printf("\nWhite moves first!\n\n");

    do{
        int validTurn = 0;
        if (!hasLegalMove(board, currentTurn)) 
        {
            printBoard(board, boardlength);

            if (isKingInCheck(board, currentTurn)) 
            {
                printf("%s is checkmated. %s wins!\n", (currentTurn == 'W') ? "White" : "Black", (currentTurn == 'W') ? "Black" : "White");
            } 
            else 
            {
                printf("Stalemate! Game ends in a draw!\n");
            }
            printf("Press Enter to exit...");
            while (getchar() != '\n');  // Flush the buffer
            getchar();   
            break;
        }

        do{
            printBoard(board, boardlength);

            printf(currentTurn == 'W' ? "\nWhite's turn:\n" : "\nBlack's turn:\n");

            startingPosition = getInt("Choose the start position from 1 to 8: ", 1, boardlength) - 1;
            endingPosition   = getInt("Choose the destination from 1 to 8: ", 1, boardlength) - 1;

            validTurn = gameStateCheck(board, startingPosition, endingPosition, currentTurn);

            if (!validTurn) 
            {
                printf("Invalid move. Try again.\n\n");
            }
        }while(!validTurn);

        if (validTurn == 2) 
        {      
            gameOver = 1;
            break; // end the game 
        } 
        else 
        {
            currentTurn = (currentTurn == 'W') ? 'B' : 'W';
            if (currentTurn == 'W') 
            { 
                rounds++;
                if (rounds >= MAX_ROUNDS) 
                {
                    printBoard(board, boardlength);
                    printf("Draw: 20-round limit reached.\n");
                    printf("Press Enter to exit...");
                    while (getchar() != '\n');
                    getchar();
                    break;                 
                }
            }
            system("clear");
        }
    }while(gameOver == 0);
}

int gameStateCheck(const char *arr[], int attacker, int defender, char turn)
{
    if (attacker < 0 || attacker >= boardlength || defender < 0 || defender >= boardlength) // Out of Bounds
    {
        printf("Out of bounds\n");
        return 0;
    }

    if (attacker == defender) // Cannot stand still
    {
        printf("You cannot stay in the same spot\n");
        return 0;
    }

    const char *a = arr[attacker];
    const char *d = arr[defender];

    char colorAttacker = a[0];   // Grabs first letter to tell which piece it is
    char colorDefender = d[0];

    if (colorAttacker != turn) // Cannot grab nothing and cannot grab another piece
    {
        if (colorAttacker == '_')
        {
            printf("There is no piece to move in that position\n");
        }
        else 
        {
            printf("That is not your piece\n");
        }
        return 0;
    }

    if (colorDefender != '_' && colorAttacker == colorDefender) // Cannot move onto your own piece
    {
        printf("You cannot move on a space occupied by your own piece\n");
        return 0;
    }

    if (!isLegalPieceMove(arr, attacker, defender)) 
    {
    printf("That piece cannot move like that\n");
    return 0;
    }

    const char *simulatedAttack = arr[attacker];
    const char *simulatedDefense = arr[defender];

    // perform tentative move
    arr[defender] = arr[attacker];
    arr[attacker] = "__";

    // if you captured a king, immediate win
    if (simulatedDefense[0] != '_' && simulatedDefense[1] == 'K') {
        printf("%s takes %s!\n", simulatedAttack, simulatedDefense);
        printBoard(arr, boardlength);
        printf("Checkmate. %s wins!\n", (turn == 'W') ? "White" : "Black");

        printf("Press Enter to exit...");
        while(getchar() != '\n');
        getchar();
        return 2;
    }

    // reject if your own king is now in check
    if (isKingInCheck(arr, turn)) {
        // undo
        arr[attacker] = simulatedAttack;
        arr[defender] = simulatedDefense;
        printf("You cannot leave your king in check\n");
        return 0;
    }

    // commit already done by the simulation
    if (simulatedDefense[0] == '_') {
        // move
        return 1;
    } else {
        // capture
        printf("%s takes %s!\n", simulatedAttack, simulatedDefense);
        return 1;
    }
}

void takePiece(const char *arr[], int attacker, int defender)
{
    arr[defender] = arr[attacker];
    arr[attacker] = "__";
}

void swapPlaces(const char *arr[], int start, int destination)
{
    const char* temp = arr[start];
    arr[start] = arr[destination];
    arr[destination] = temp;
}

int getInt(char prompt[], int min, int max)
{   
    int moveNumber = 0;
    do
    {
        printf("%s", prompt);
        scanf("%d", &moveNumber);
        if(moveNumber < min || moveNumber > max)
        {
            printf("Value must be in the range %d-%d\n", min, max);
        }

    }
    while(moveNumber < min || moveNumber > max);
    
    return moveNumber;
}

void titleScreen()
{
    printf("               %s1-DIMENSION CHESS%s\n               by Hamza Damlakhi\n\n", BYEL, CRESET);

    printf("%sHow to Play:%s\n\nThe game is played on a single row of 8 positions.\n"
    "Each player has three pieces: a king, a knight, and a rook.\n"
    "White's pieces are on the left; Black's pieces are on the right.\n\n", BRED, CRESET);

    printf("1. A player chooses one of their pieces to move on their turn.\n"
    "A piece may not move to a space that is occupied by a piece of its own color.\n"
    "When a piece moves to a space occupied by an enemy piece, the enemy piece is captured, i.e., removed from the board.\n\n");

    printf("2.A king is in check if either (1) there are only empty spaces between it and the enemy rook or (2) the opponent's knight is two spaces away from it.\n"
    "At the end of a player's turn, their own king must neither be in check nor next to the enemy king.\n"
    "If a player's king is in check and cannot get out of check, then it is checkmated and the player loses.\n\n");

    printf("3. Stalemate occurs if a player has no legal moves but their king is not in check.\n"
    "A draw by insufficient material occurs when all non-king pieces have been captured.\n\n");

    menuBuffer();
    system("clear");
}

void rulesScreen()
{
    printf("%sRules:%s \n\n1. A king may move one space left or right.\n2. A rook may move to any space as long as every space between the departure and destination spaces is unoccupied.\nThe rook cannot jump over other pieces.\n"
    "3. A knight moves two spaces left or right. If it jumps over another piece, that piece is not captured.\n"
    "4. There is no castling.\n\n", BHBLU, CRESET);

    menuBuffer();
    system("clear");

}

void menuBuffer()
{
    int button;
    int result;

    do {
        printf("Type 1 and press Enter to continue: ");
        result = scanf("%d", &button);

        if (result != 1) {
            // invalid input: clear it out
            while (getchar() != '\n'); 
            button = 0; // reset
        }
    } while (button != 1);
    printf("\n");
}

void printBoard(const char *arr[], int SIZE)
{
     for(int i = 0; i < SIZE; i++)
    {
        if(strcmp(arr[i], "WK") == 0 || strcmp(arr[i], "WN") == 0 || strcmp(arr[i], "WR") == 0)
        {
            printf("%s%s%s ", BHWHT, arr[i], CRESET);
        }
        else if (strcmp(arr[i], "BK") == 0 || strcmp(arr[i], "BN") == 0 || strcmp(arr[i], "BR") == 0)
        {
            printf("%s%s%s ", BHBLK, arr[i], CRESET);
        }
        else
        printf("%s ", arr[i]);
    }

    printf("\n");
}

int indexOfKing(const char *board[], char kingColor)
{
    char code[3] = { kingColor, 'K', '\0' };
    for (int i = 0; i < boardlength; ++i) {
        if (strcmp(board[i], code) == 0) return i;
    }
    return -1;
}

int firstNonEmptyLeft(const char *board[], int from)
{
    for (int i = from - 1; i >= 0; --i) {
        if (board[i][0] != '_') return i;
    }
    return -1;
}

int firstNonEmptyRight(const char *board[], int from)
{
    for (int i = from + 1; i < boardlength; ++i) {
        if (board[i][0] != '_') return i;
    }
    return -1;
}

int isKingInCheck(const char *board[], char kingColor)
{
    int kingSpace = indexOfKing(board, kingColor);
    char enemy = (kingColor == 'W') ? 'B' : 'W';

    // Enemy King is adjacent
    if (kingSpace - 1 >= 0 && board[kingSpace - 1][0] == enemy && board[kingSpace - 1][1] == 'K') return 1;
    if (kingSpace + 1 <  boardlength && board[kingSpace + 1][0] == enemy && board[kingSpace + 1][1] == 'K') return 1;

    // Knight is two spaces away
    if (kingSpace - 2 >= 0 && board[kingSpace - 2][0] == enemy && board[kingSpace - 2][1] == 'N') return 1;
    if (kingSpace + 2 <  boardlength && board[kingSpace + 2][0] == enemy && board[kingSpace + 2][1] == 'N') return 1;

    // Rook has empty spaces towards the King
    int Left = firstNonEmptyLeft(board, kingSpace);
    if (Left >= 0 && board[Left][0] == enemy && board[Left][1] == 'R') return 1;

    int Right = firstNonEmptyRight(board, kingSpace);
    if (Right >= 0 && board[Right][0] == enemy && board[Right][1] == 'R') return 1;

    return 0;
}

int pathClear(const char *board[], int start, int end) {
    if (start == end) return 0;
    int step = (end > start) ? 1 : -1;
    for (int i = start + step; i != end; i += step) {
        if (board[i][0] != '_') return 0;
    }
    return 1;
}

int isLegalPieceMove(const char *board[], int start, int end) {
    const char *p = board[start];
    char type = p[1];      // Checks for the second later to see role of piece
    int d = end - start;
    if (d < 0) d = -d;

    if (type == 'K') 
    {
    // King Movement
    return d == 1;
    }
    else if (type == 'N') 
    {
    // Knight Movement
    return d == 2;
    }
    else if (type == 'R') 
    {
    // Rook movement
    return pathClear(board, start, end);
    }
    else 
    {
    return 0; 
    }
}

int hasLegalMove(const char *board[], char turn)
{
    for (int from = 0; from < boardlength; ++from) {
        if (board[from][0] != turn) continue;     // Must be my piece

        for (int to = 0; to < boardlength; ++to) {
            if (to == from) continue;
            if (board[to][0] == turn) continue;     // can't land on own piece
            if (!isLegalPieceMove(board, from, to)) continue;

            // Simulating on a copy of the board
            const char *tmp[boardlength];
            memcpy(tmp, board, sizeof tmp);
            const char *sa = tmp[from];
            const char *sd = tmp[to];

            tmp[to]   = sa;
            tmp[from] = "__";

            // If the simulate results on a safe King, the real king is sae
            if (!isKingInCheck(tmp, turn)) {
                return 1;
            }
        }
    }
    return 0; // Otherwise, my King is never safe
}


