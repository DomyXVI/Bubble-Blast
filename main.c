#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#define COLUMNS 5
#define ROWS 6

/* This struct is used to change the explosion's direction according to the up,left,down,right scheme*/
typedef struct {
    int x;
    int y;
}Versor;

/* Simply making the game more understandable - instead of ints the bubbles are stored as Bubble Structs*/
typedef struct{
    int value;
}Bubble;

/* Struct used to represent the anatomy of the best move with its coordinates and relative score*/
typedef struct{
    int x;
    int y;
    int score;
}Best_move;

/* Used to keep track of user statistics*/
typedef struct{
    int totalMoves;
    int points;
    int blunders;
}User_stats;

/* Function protoypes */
void initialize_matrix(Bubble [ROWS][COLUMNS]);

void print_matrix(char [ROWS + 2][COLUMNS + 2]);

void print_table(Bubble [ROWS][COLUMNS]);

Best_move getBestMove(Bubble game_matrix[ROWS][COLUMNS]);

void merge(Bubble [ROWS][COLUMNS], char [ROWS + 2][COLUMNS + 2]);

void esplosione(Bubble griglia[ROWS][COLUMNS], int rig, int col);

int calculateScore(Bubble griglia[ROWS][COLUMNS], int rig, int col);

void printWinScreen(User_stats *userStats);

Versor newVersor();

Versor rotateVersor(Versor vs);

bool isInsideMatrix (int rig, int col);

bool isWin(Bubble griglia[ROWS][COLUMNS]);

void printMenu();


int main() {
    /* initializing variables and struct members */
    int x, y;
    User_stats userStats;
    userStats.totalMoves=0;
    userStats.points=0;
    userStats.blunders=0;

    /* this boolean is a flag. True -> show hint for the best next move, False -> hide best move hint*/
    bool assistedRun = false;
    Bubble game_matrix[ROWS][COLUMNS];
    initialize_matrix(game_matrix);
    Bubble matrixcopy [ROWS][COLUMNS];

    /*creating an exact copy of the matrix that will be used to calculate the best moves*/
    memcpy(matrixcopy, game_matrix, sizeof matrixcopy);

    system("cls");
    /*Printing Menu and prompt to enable best move predictions*/
    printMenu();

    printf("\033[32;1mInsert 'h' to enable best move predictions or press Enter twice to continue: \033[0m");

    if(getchar() == 'h'){
        assistedRun = true;
    }

    while (getchar() != '\n');     /*Insert Enter twice to continue*/


    /*Game logic*/
    do {
        printf("\n");
        print_table(game_matrix);
        Best_move bm = getBestMove(game_matrix);
        if(assistedRun) {
            printf("----------------------BEST MOVE: %d/%d----------------------\n",bm.x+1,bm.y+1);
        }
        printf("insert x: ");
        scanf("%d", &x);
        printf("insert y: ");
        scanf("%d", &y);
        printf("you have chosen number: %d\n", game_matrix[y - 1][x - 1]);

        /*Calculating User Statistics for each Move*/
        userStats.totalMoves++;

        if(bm.x!=x-1 || bm.y!=y-1) userStats.blunders++;

        userStats.points += calculateScore(matrixcopy,y-1,x-1);

        /*Invoking the explosion DFS Algorithm*/
        esplosione(game_matrix, y - 1, x - 1);

    }while(!isWin(game_matrix));
    printWinScreen(&userStats);

}


/*populating the 2D array with random integers between 1 and 3*/
void initialize_matrix(Bubble matrix[ROWS][COLUMNS]) {
    srand(time(NULL));
    for (int i = 0; i < ROWS; i++) {
        for (int w = 0; w < COLUMNS; w++) {
            Bubble bubble;
            // random numbers between 1 and 3
            bubble.value = rand() % 3 + 1;
            matrix[i][w] = bubble;
        }
    }
}

/*Instantiating a new Versor. It's values (0,-1) represent the first direction, which will be upwards */
Versor newVersor(){
    Versor vs;
    vs.x = 0;
    vs.y = -1;
    return vs;
}


/* Rotates the versor counter-clockwise, in order to propagate the explosion in all the four directions; check https://it.wikipedia.org/wiki/Versore */
Versor rotateVersor(Versor vs){
    Versor temp = newVersor();
    temp.x =  vs.y;
    temp.y = vs.x*-1;
    return temp;
}

/*Merging the 2D array with the index rows and columns for better clarity*/
void merge(Bubble game_matrix[ROWS][COLUMNS], char ind[ROWS + 2][COLUMNS + 2]) {
    int r2 = 0;
    for (int r1 = 1; r1 < ROWS + 1; r1++) {
        for (int c1 = 1, c2 = 0; c1 < COLUMNS + 1; c1++) {
            /*implicit casting from int to char*/
            ind[r1][c1] = '0' + game_matrix[r2][c2].value;
            c2++;
        }
        r2++;
    }
}

/*printing the matrix with colors*/
void print_matrix(char ind[ROWS + 2][COLUMNS + 2]) {
    char colour;
    for (int row = 0; row < ROWS + 2; row++) {
        for (int columns = 0; columns < COLUMNS + 2; columns++) {

            if (row == 0 || columns == 0) {
                colour = '0';
            } else {
                colour = ind[row][columns];
            }

            switch (colour) {
                case '0':
                    // SELECT BOLD RED Colour
                    printf("\033[31;1m");
                    break;
                case '1':
                    // SELECT BOLD Yellow Colour
                    printf("\033[1;33m");
                    break;
                case '2':
                    // SELECT BOLD Green Colour
                    printf("\033[1;34m");
                    break;
                case '3':
                    // SELECT BOLD Cyan Colour
                    printf("\x1b[1;36m");
                    break;
                default:
                    // SELECT Normal font
                    printf("\033[0m");
                    break;
            }
            printf("%c\t", ind[row][columns]); /* Printing the Matrix as a table */
        }
        printf("\n\n");
    }
}

/* this function prints the entire merged-gametable*/
void print_table(Bubble game_matrix[ROWS][COLUMNS]) {
    /*indexes*/
    char ind[ROWS + 2][COLUMNS + 2] = {{'0', '1', '2', '3', '4', '5', '6', 'X'},
                                       {'1'},
                                       {'2'},
                                       {'3'},
                                       {'4'},
                                       {'5'},
                                       {'Y'}};

    //merge game matrix with the game table
    merge(game_matrix, ind);
    print_matrix(ind);
}

/*This is the Core of the Game. the explosion is recursive and happens in all the four directions for each bubble, traversing the matrix radially.*/
void esplosione(Bubble griglia[ROWS][COLUMNS], int rig, int col) {
    //printf("Considerazione di coordinate: (%d,%d)\n", col+1, rig+1);
    //print_table(griglia);
    printf("");

    if(!isInsideMatrix(rig,col) || griglia[rig][col].value < 1){
        //printf("Nulla da fare o flagged\n");
        return;
    }

    if(griglia[rig][col].value > 1){
        //printf("valore maggiore di uno, sottraggo e termino iterazione\n");
        griglia[rig][col].value--;
        return;
    }
    Versor vs = newVersor();
    griglia[rig][col].value = -1;
    for(int i =0;i<4;i++){
        //printf("Propagazione di (%d,%d) in direzione: (%d,%d)\n" ,col+1, rig+1, vs.x, vs.y);
        esplosione(griglia,rig+vs.y,col+vs.x);
        vs = rotateVersor(vs);
    }
    //printf("fine propagazione\n");
    griglia[rig][col].value = 0;
}

/*checks if a bubble is inside or outside the matrix; used to check for borders; returns true if the bubble is within the constraints*/
bool isInsideMatrix (int rig, int col){
    if(rig<ROWS && rig>=0 && col<COLUMNS && col>=0 ) return true;
    return false;
}

/*Checks if each bubble in the game table is equal to 0. returns true if there aren't any unexploded bubbles; terminates the game loop*/
bool isWin(Bubble griglia[ROWS][COLUMNS]){
    for(int i =0;i<ROWS;i++){
        for(int j=0;j<COLUMNS;j++){
            if(griglia[i][j].value!= 0) return false;
        }
    }
    return true;
}

/*Prints the Menu*/
void printMenu(){
    printf(""
           "\033[1;31m"
           "███████████████████████████████████████████████████████████████████████████████████████████████████████████\n"
           "█                                                                                                         █\n"
           "█   ██████   ██    ██  ██████   ██████   ██       ███████  ██████   ██        █████   ███████  ████████   █\n"
           "█   ██   ██  ██    ██  ██   ██  ██   ██  ██       ██       ██   ██  ██       ██   ██  ██          ██      █\n"
           "█   ██████   ██    ██  ██████   ██████   ██       █████    ██████   ██       ███████  ███████     ██      █\n"
           "█   ██   ██  ██    ██  ██   ██  ██   ██  ██       ██       ██   ██  ██       ██   ██       ██     ██      █\n"
           "█   ██████    ██████   ██████   ██████   ███████  ███████  ██████   ███████  ██   ██  ███████     ██      █\n"
           "█                                                                                                         █\n"
           "███████████████████████████████████████████████████████████████████████████████████████████████████████████\033[0m\n"
           "");

    printf("\nIn this game you have to destroy all bubbles of the game table.\nComputer will calculate the number of moves needed to win the game.\nBubbles have 3 states:\n");
    printf("\x1b[1;36m -3 represents the quiet state of bubble\n");
    printf("\033[1;34m -2 represents the half-exploded state of bubble\n");
    printf("\033[1;33m -1 represents the bubble that is about to explode\n");
    printf("\033[1;31m -0 represents the bubble that is exploded (empty cell)\n");
    printf("\033[0m\n");
    printf("\033[1;33mRemember, only state '1' can propagate the explosion.\033[0m\n");
}

/*Gets the best move for each bubble in the game table. Returns the one with the highest score, which is calculated
 * based on the amount of explosions and how many bubbles are touched and decremented.
*/

Best_move getBestMove(Bubble game_matrix[ROWS][COLUMNS]){
    Best_move bestMove;
    Bubble matrixCopy [ROWS][COLUMNS];

    for(int i =0;i<ROWS;i++){
        for(int j=0;j<COLUMNS;j++){
            memcpy(matrixCopy, game_matrix, sizeof matrixCopy);
            int score = calculateScore(matrixCopy,i,j);
            if(score > bestMove.score){
                bestMove.x= j;
                bestMove.y = i;
                bestMove.score = score;
            }
        }
    }
    return bestMove;
}

/*This function calculates the best score for said move. returns an integer, and takes in input the coordinates of a bubble and a copy of the game matrix */
int calculateScore(Bubble griglia[ROWS][COLUMNS], int rig, int col) {
    int score = 0;
    printf("");

    /*If the bubble is outside of matrix or already exploded, return 0 */

    if(!isInsideMatrix(rig,col) || griglia[rig][col].value < 1){
        return score;
    }

    /*If the bubble's value is greater than one, decrement the bubble and increment the score for the move */
    if(griglia[rig][col].value > 1){
        griglia[rig][col].value--;
        score++;
        return score;
    }
    Versor vs = newVersor();
    griglia[rig][col].value = -1; /* flags the bubble to avoid checking it again*/

    /*Expands the explosions radially, and calculates score based on how many bubbles are affected by them */
    for(int i =0;i<4;i++){
        score = score + calculateScore(griglia,rig+vs.y,col+vs.x);
        vs = rotateVersor(vs);
    }
    score++;

    /*explodes the bubble*/
    griglia[rig][col].value = 0;
    return score;
}

/*Prints win screen and user statistics*/
void printWinScreen(User_stats *userStats){
    printf("\n"
           "--------------------------------------------------------------\n\n"
           "\033[32;1m"
           "██████████████████████████████████████████████████████████████\n"
           "█                                                            █\n"
           "█    ██  ██   ██████  ██    ██     ██     ██ ██ ███    ██    █\n"
           "█    ██  ██  ██    ██ ██    ██     ██     ██ ██ ████   ██    █\n"
           "█     ████   ██    ██ ██    ██     ██  █  ██ ██ ██ ██  ██    █\n"
           "█      ██    ██    ██ ██    ██     ██ ███ ██ ██ ██  ██ ██    █\n"
           "█      ██     ██████   ██████       ███ ███  ██ ██   ████    █\n"
           "█                                                            █\n"
           "██████████████████████████████████████████████████████████████\n"
           "                                                      ");

    printf("\n\033[1;33mYour statistics: \033[0m\n");
    printf("Total moves: %d\n",userStats->totalMoves);
    printf("Points: %d\n",userStats->points);
    printf("Blunders: %d\n",userStats->blunders);

    while (getchar() != '\n');
}



