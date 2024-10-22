#include    <stdio.h>
#include    <curses.h>
#include    <signal.h>
#include    <string.h>

#define MESSAGE "hello"
#define BLANK   "     "

void move_msg(int);
int set_ticker(int);

int     row;
int     col;
int     dir;

int main(){
    int     delay;
    int     ndelay;
    int     c;
    void    move_msg(int);

    initscr();
    crmode();
    noecho();
    clear();

    row     =10;
    col     =0;
    dir     =1;
    delay   =200;

    move(row, col);
    addstr(MESSAGE);
    signal(SIGALRM, move_msg);
    set_ticker(delay);

    while (1){
        ndelay=0;
        c=getch();
        if(c=='Q') break;
        if(c==' ') dir=-dir;
        if(c=='f' && delay>2) ndelay=delay/2;
        if(c=='s') ndelay=delay*2;
        if(ndelay>0)
            set_ticker(delay=ndelay);
    }
    endwin();
    return 0;
}

void move_msg(int signum){
    signal(SIGALRM, move_msg);
    move(row, col);
    addstr(BLANK);
    col+=dir;
    move(row, col);
    addstr(MESSAGE);
    refresh();

    if(dir==-1 && col<=0)
        dir=1;
    else if(dir==1 && col+strlen(MESSAGE)>=COLS)
        dir=-1;
}

