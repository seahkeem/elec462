#include    <unistd.h>
#include    <curses.h>

#define LEFTEDGE    20
#define RIGHTEDGE   30
#define ROW         10

int main(){
    char    message[]   ="Hello";
    char    blank[]     ="     ";
    int     dir         =+1;
    int     pos         =LEFTEDGE;

    initscr();
    clear();
    while(1){
        move(ROW, pos);
        addstr(message);
	move(ROW, pos);
	refresh();
	sleep(1);
	move(ROW, pos);
	addstr(blank);
	refresh();
	sleep(1);
    }
    return 0;
}
