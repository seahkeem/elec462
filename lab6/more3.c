#include    <stdio.h>
#include    <stdlib.h>
#include    <sys/ioctl.h>
#include    <termios.h>

//#define     PAGELEN 24  //row
//#define     LINELEN 512 //col

void do_more(FILE *);
int  see_more();

int main(int ac, char *av[]){
    FILE    *fp;

    if(ac==1)
        do_more(stdin);
    else
        while(-ac)
            if((fp=fopen(*++av, "r"))!=NULL) {
                do_more(fp);
                fclose(fp);
            }
            else
                exit(1);

    return 0;
}

void do_more(FILE *fp){
    struct winsize  wbuf;

    int row=0;
    int col=0;

    if(ioctl(0, TIOCGWINSZ, &wbuf)!=-1){
        row=wbuf.ws_row;
        col=wbuf.ws_col;
    }

    char    line[row];
    int     num_of_lines=0;
    int     see_more(), reply;

    while(fgets(line, col, fp)){
        if(num_of_lines==row){
            reply=see_more();
            if(reply==0)
                break;
            num_of_lines-=reply;
        }
        if(fputs(line, stdout)==EOF)
            exit(1);
            num_of_lines++;
    }
}

int see_more(){
    struct winsize  wbuf;
    int row=0;
    int     c;

    if(ioctl(0, TIOCGWINSZ, &wbuf)!=-1){
        row=wbuf.ws_row;
    }

    printf("\033[7m more? \033[m");
    while ((c=getchar())!=EOF){
        if(c=='q')
            return 0;
        if(c==' ')
            return row;
        if(c=='\n')
            return 1;
    }
    return 0;
}

