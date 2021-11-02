#include <stdlib.h>
#include <stdio.h>

const int ALLDIRECTIONS[8]={-9, -8, -7, -1, 1, 7, 8, 9};
char board[66];

const char EMPTY='-';
const char BLACK='x';
const char WHITE='o';

int human();
int randomstrategy();

char opponent () {
    if (board[65] == 'x'){return 'o';}
    else if (board[65] == 'o'){return 'x';}
    else{
        printf("illegal player\n");
        return 0;
    }
}

void initialboard (void) {
    int i;

    for (i = 0; i<=63; i++) {
        board[i]=EMPTY;
    }
    board[27]=WHITE; board[28]=BLACK; board[35]=BLACK; board[36]=WHITE; board[64]='\t'; board[65]=WHITE;
}

void printboard () {
    int row, col;
    for (row=0; row<=7; row++) {
        for (col=0; col<=7; col++)
            printf("%c ", board[col + (8 * row)]);
        printf("\n");
    }
}

int validp (int move) {
    if ((move >= 0) && (move <= 63) && (move%8 >= 1) && (move%8 <= 8))
        return 1;
    else return 0;
}

int findbracketingpiece(int square, int dir) {

    while (board[square] == opponent()) square = square + dir;
    if (board[square] == board[65]) return square;
    else return 0;
}

int wouldflip (int move, int dir) {
    int c;
    c = move + dir;
    if (board[c] == opponent()) {
        return findbracketingpiece(c + dir, dir);
    }
    else return 0;
}

int legalp (int move) {
    int i;
    if (!validp(move)) return 0;
    if (board[move]==EMPTY) {
        i=0;
        while (i<=7 && !wouldflip(move, ALLDIRECTIONS[i])){
            i++;
        }
        if (i==8) return 0; else { return 1; }

    }
    else return 0;
}

void makeflips (int move, int dir) {
    int bracketer, c;
    bracketer = wouldflip(move, dir);
    if (bracketer) {
        c = move + dir;
        do {
            board[c] = board[65];
            c = c + dir;
        } while (c != bracketer);
    }
}

void makemove (int move) {
    int i;
    board[move] = board[65];
    for (i=0; i<=7; i++) makeflips(move, ALLDIRECTIONS[i]);
}

int anylegalmove () {
    int move;
    move = 0;
    while (move <= 63 && !legalp(move)) move++;
    if (move <= 63) return 1; else return 0;
}

char nexttoplay (int printflag) {
    char opp;
    opp = opponent();
    if (anylegalmove()) return opp;
    if (anylegalmove()) {
        if (printflag) printf("%c has no moves and must pass.\n", opp);
        return board[65];
    }
    return 0;
}

int * legalmoves () {
    int move, i, * moves;
    moves = (int *)malloc(65 * sizeof(int));
    moves[0] = 0;
    i = 0;
    for (move=0; move<=63; move++)
        if (legalp(move)) {
            i++;
            moves[i]=move;
        }
    moves[0]=i;
    return moves;
}

int human () {
    int move,x,y;
    printf("%c to move:", board[65]); scanf("%d %d", &x, &y);
    move = x*8+y;
    return move;
}

int randomstrategy() {
    int r, * moves;
    moves = legalmoves();
    r = moves[(rand() % moves[0])+1];
    free(moves);
    return(r);
}

void getmove (int (* strategy)(), int printflag) {
    int move;
    if (printflag) printboard();
    move = (* strategy)();
    if (legalp(move)) {
        if (printflag) printf("%c moves to %d\n", board[65], move);
        makemove(move);
    }
    else {
        printf("Illegal move %d\n", move);
        getmove(strategy, printflag);
    }
}

void othello (int (* blstrategy)(), int (* whstrategy)(), int printflag) {
    initialboard();
    board[65] = BLACK;
    do {
        if (board[65] == BLACK) getmove(blstrategy, printflag);
        else getmove(whstrategy, printflag);
        board[65] = nexttoplay(printflag);
    }
    while (board[65] != 0);
    if (printflag) {
        printf("game over\n");
        printboard();
    }
}

int main (void) {
    int printflag;
    int (* strfn1)();  int (* strfn2)();

    strfn1 = human; strfn2 = randomstrategy;
    printflag = 1;

    othello(strfn1, strfn2, printflag);
}
