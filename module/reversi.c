#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/uaccess.h>

static char some_kernel_buffer[67];
const int ALLDIRECTIONS[8]={-9, -8, -7, -1, 1, 7, 8, 9};
char board[67];
char player1Color='-';
char player2Color='-';
int userMove=0;
int legalMoves[65];

const char EMPTY='-';
const char BLACK='X';
const char WHITE='O';

const char ok[3]="OK\n";
const char win[4]="WIN\n";
const char lose[5]="LOSE\n";
const char illmove[8]="ILLMOVE\n";
const char unkcmd[7]="UNKCMD\n";
const char invfmt[7]="INVFMT\n";
const char oot[4]="oot\n";
const char nogame[7]="nogame\n";


char opponent (void) {
    if (board[65] == BLACK){return WHITE;}
    else if (board[65] == WHITE){return BLACK;}
    else{
        pr_info("illegal player\n");
        return 0;
    }
}

void initialboard (void) {
    int i;

    for (i = 0; i<=63; i++) {
        board[i]=EMPTY;
    }
    board[27]=WHITE; board[28]=BLACK; board[35]=BLACK; board[36]=WHITE; board[64]='\t'; board[65]=BLACK; board[66]='\n';
}

void printboard (void) {
    int i=0;
    for(i=0;i<67;i++){
        some_kernel_buffer[i] = board[i];
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

int anylegalmove (void) {
    int move;
    move = 0;
    while (move <= 63 && !legalp(move)) move++;
    if (move <= 63) return 1; else return 0;
}

char nexttoplay (void) {
    char opp;
    opp = opponent();
    if (anylegalmove()) return opp;
    if (anylegalmove()) {
        pr_info("%c has no moves and must pass.\n", opp);
        return board[65];
    }
    return 0;
}

void legalmoves (void) {
    int move=0;
    int i = 0;
    legalMoves[i]=0;
    for (move=0; move<=63; move++)
        if (legalp(move)) {
            i++;
            legalMoves[i]=move;
        }
    legalMoves[0]=i;
}

int reversi (void) {
    if(some_kernel_buffer[1]=='0'){ //setup game
        initialboard();
        player1Color = some_kernel_buffer[2];
        pr_info("player1Color %c", player1Color);
        if(player1Color == BLACK){player2Color= WHITE;}
        else if(player1Color == WHITE){player2Color= BLACK;}
	memset(some_kernel_buffer, 0, sizeof some_kernel_buffer);
	memset(some_kernel_buffer, ok, sizeof ok -1);
    }
    else if(some_kernel_buffer[1]=='1'){ //printBoard
        printboard();
    }
    else if(some_kernel_buffer[1]=='2'){ //player make move
        if(player1Color == board[65]){
	    userMove = (int)(some_kernel_buffer[2]-'0')*8+(int)(some_kernel_buffer[3]-'0');
	
	    if (legalp(userMove)) {
                pr_info("%c moves to %d\n", board[65], userMove);
                makemove(userMove);
            }
            else {
                pr_info("Illegal move %d\n", userMove);
            }    	
            board[65] = nexttoplay();
        }
        else {pr_info("not your turn");}
    }
    else if(some_kernel_buffer[1]=='3') { //computer make move
        if (player2Color == board[65]) {
	    legalmoves();
	    makemove(legalMoves[1]);
            board[65] = nexttoplay();
        } else { pr_info("not computer turn"); }
    }
}

/*
** This function will be called when we open the Misc device file
*/
static int misc_open(struct inode *inode, struct file *file)
{
    pr_info("misc device open\n");
    return 0;
}
/*
** This function will be called when we close the Misc Device file
*/
static int misc_close(struct inode *inodep, struct file *filp)
{
    pr_info("misc device close\n");
    return 0;
}
/*
** This function will be called when we write the Misc Device file
*/
static ssize_t misc_write(struct file *filp, const char __user *ubuf,
size_t count, loff_t *ppos){
    int retval;
    pr_info("misc_write called");
    if (count > sizeof(some_kernel_buffer))
        count = sizeof(some_kernel_buffer);
    retval = copy_from_user(some_kernel_buffer, ubuf, count);
    if (retval < 0)
        return -EINVAL;
    reversi();
    return count;

}

/*
** This function will be called when we read the Misc Device file
*/
static ssize_t misc_read(struct file *filp, char __user *ubuf,
size_t count, loff_t *f_pos){
    int retval;
    if (count > sizeof(some_kernel_buffer))
        count = sizeof(some_kernel_buffer);
    retval = copy_to_user(ubuf, some_kernel_buffer, count);
    if (retval < 0)
        return -EINVAL;
    return count;
}

//File operation structure
static const struct file_operations fops = {
        .owner          = THIS_MODULE,
        .write          = misc_write,
        .read           = misc_read,
        .open           = misc_open,
        .release        = misc_close,
        .llseek         = no_llseek,
};
//Misc device structure
struct miscdevice etx_misc_device = {
        .minor = MISC_DYNAMIC_MINOR,
        .name = "reversi",
        .fops = &fops,
};
/*
** Misc Init function
*/
static int __init misc_init(void)
{
    int error;

    error = misc_register(&etx_misc_device);
    if (error) {
        pr_err("misc_register failed!!!\n");
        return error;
    }

    pr_info("misc_register init done!!!\n");
    return 0;
}
/*
** Misc exit function
*/
static void __exit misc_exit(void)
{
    misc_deregister(&etx_misc_device);
    pr_info("misc_register exit done!!!\n");
}

module_init(misc_init)
module_exit(misc_exit)
