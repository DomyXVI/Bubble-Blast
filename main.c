#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define COLUMNS 7
#define ROWS 6
typedef struct {
    int x;
    int y;
}Versor;

typedef struct{
    int value;
}Bubble;


void initialize_matrix(Bubble [ROWS][COLUMNS]);

void print_matrix(char [ROWS + 2][COLUMNS + 2]);

void print_table(Bubble [ROWS][COLUMNS]);

void merge(Bubble [ROWS][COLUMNS], char [ROWS + 2][COLUMNS + 2]);

void esplosione(Bubble griglia[ROWS][COLUMNS], int rig, int col);

Versor newVersor();

Versor rotateVersor(Versor vs);

bool isInsideMatrix (int rig, int col);

bool isWin(Bubble griglia[ROWS][COLUMNS]);


int main() {

    //inizializzazione versore
    Versor vs = newVersor();


    int choice1, choice2;
    Bubble game_matrix[ROWS][COLUMNS];
    system("cls");

    // Menu
    printf(""
           "\033[1;31m"
           "██████   ██    ██  ██████   ██████   ██       ███████  ██████   ██        █████   ███████  ████████    \n"
           "██   ██  ██    ██  ██   ██  ██   ██  ██       ██       ██   ██  ██       ██   ██  ██          ██       \n"
           "██████   ██    ██  ██████   ██████   ██       █████    ██████   ██       ███████  ███████     ██       \n"
           "██   ██  ██    ██  ██   ██  ██   ██  ██       ██       ██   ██  ██       ██   ██       ██     ██       \n"
           "██████    ██████   ██████   ██████   ███████  ███████  ██████   ███████  ██   ██  ███████     ██       \n\n"
           "");


    printf(""
           "\033[0;34m"
           "██    ██      ██     ██████  \n"
           "██    ██     ███    ██  ████ \n"
           "██    ██      ██    ██ ██ ██ \n"
           " ██  ██       ██    ████  ██ \n"
           "  ████        ██ ██  ██████  \033[0m\n"
           "                            ");
    printf("\nIn this game you have to destroy all bubbles of the game table.\nComputer will calculate the number of moves needed to win the game.\nBubbles have 3 states:\n");
    printf("\x1b[1;36m -3 represents the quiet state of bubble\n");
    printf("\033[1;34m -2 represents the half-exploded state of bubble\n");
    printf("\033[1;33m -1 represents the bubble that is about to explode\n");
    printf("\033[1;31m -0 represents the bubble that is exploded (empty cell)\n");
    printf("\033[0m\n");
    printf("\033[32mRemember, only state '1' can propagate the explosion.\033[0m\n");
    printf("\nPress Enter to Continue: \n");

    while (getchar() != '\n');

    initialize_matrix(game_matrix);
    do {
        print_table(game_matrix);
        printf("select x: ");
        scanf("%d", &choice1);
        printf("select y: ");
        scanf("%d", &choice2);
        printf("you have chosen number: %d\n", game_matrix[choice2 - 1][choice1 - 1]);
        esplosione(game_matrix, choice2 - 1, choice1 - 1);
    }while(!isWin(game_matrix));
}


//populating the 2D array with random integers between 1 and 3
void initialize_matrix(Bubble matrix[ROWS][COLUMNS]) {
    srand(time(NULL));
    int r;
    for (int i = 0; i < ROWS; i++) {
        for (int w = 0; w < COLUMNS; w++) {
            Bubble bubble;
            // random numbers between 1 and 3
            bubble.value = rand() % 3 + 1;
            matrix[i][w] = bubble;
        }
    }
}

Versor newVersor(){
    Versor vs;
    vs.x = 0;
    vs.y = -1;
    return vs;
}


//funzione per ruotare il versore counter-clockwise
Versor rotateVersor(Versor vs){
    Versor temp = newVersor();
    temp.x =  vs.y;
    temp.y = vs.x*-1;
    return temp;
}

//merging the 2D array with the index rows and columns for better clarity
void merge(Bubble game_matrix[ROWS][COLUMNS], char ind[ROWS + 2][COLUMNS + 2]) {
    int r2 = 0;
    for (int r1 = 1; r1 < ROWS + 1; r1++) {
        for (int c1 = 1, c2 = 0; c1 < COLUMNS + 1; c1++) {
            // implicit casting from int to char
            ind[r1][c1] = '0' + game_matrix[r2][c2].value;
            c2++;
        }
        r2++;
    }
}

//printing the matrix with colors
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
                    printf("\033[1;31m");
                    break;
                case '1':
                    // SELECT BOLD Yellow Colour
                    printf("\033[1;33m");
                    break;
                case '2':
                    // SELECT BOLD Yellow Colour
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
            printf("%c\t", ind[row][columns]);
        }
        printf("\n\n\n");
    }
}

//printing the entire game table
void print_table(Bubble game_matrix[ROWS][COLUMNS]) {
    //this is a matrix that I made to print indexes of the game_matrix, it is for making the game more comfortable

    char ind[ROWS + 2][COLUMNS + 2] = {{'o', '1', '2', '3', '4', '5', '6', 'X'},
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

bool isInsideMatrix (int rig, int col){
    if(rig<ROWS && rig>=0 && col<COLUMNS && col>=0 ) return true;
    return false;
}

bool isWin(Bubble griglia[ROWS][COLUMNS]){
    for(int i =0;i<ROWS;i++){
        for(int j=0;j<COLUMNS;j++){
            if(griglia[i][j].value!= 0) return false;
        }
    }
    return true;
}




