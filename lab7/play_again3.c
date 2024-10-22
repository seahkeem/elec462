/* play_again2.c
 *     purpose : ask if user wants another transaction
 *      method : set tty into char-by-char, no-echo mode
 *               set tty into no-delay mode
 *               read char, return result
 *      return : 0=>yes, 1=>no, 2=>timeout
 *      better : timeout if user walks away
 */
#include    <stdio.h>
#include    <termios.h>
#include    <fcntl.h>
#include    <string.h>
#include    <unistd.h>
#include    <ctype.h>

#define ASK         "Do you want another transaction"
#define TRIES       3               /* max tries    */
#define SLEEPTIME   2               /* time per tty */
#define BEEP        putchar('\a')   /* alert user   */

int get_response(char *, int);
int get_ok_char();
void set_cr_noecho_mode();
void set_nodelay_mode();
void tty_mode(int);

int main(){
    int     response;

    tty_mode(0);                         /* save tty mode       */
    set_cr_noecho_mode();                     /* set -icanon, -echo  */
    set_nodelay_mode();                       /* noinput => EOF      */
    response= get_response(ASK, TRIES);   /* get some answer      */
    tty_mode(1);                         /* restore orig mode   */

    return response;
}

int get_response(char *question, int maxtries){
    /*
     * purpose : ask a question and wait for a y/n answer or maxtries
     * method : use getchar and complain about non-y/n input
     * return : 0=>yes, 1=>no
     */
    int input;
    printf("%s (y/n)?", question);  /* ask */
    fflush(stdout);
    while (1){
        sleep(SLEEPTIME);
        input= tolower(get_ok_char());
        if(input=='y')
            return 0;
        if(input=='n')
            return 1;
        if(maxtries--==0)
            return 2;
        BEEP;
    }
}

int get_ok_char(){
	int c;
	while((c=getchar())!=EOF&&strchr("yYnN", c)==NULL)
		;

	return c;
}

void set_cr_noecho_mode(){

    struct  termios ttystate;

    tcgetattr(0, &ttystate);            /* read curr, setting   */
    ttystate.c_lflag        &=~ICANON;  /* no buffering         */
    ttystate.c_lflag        &=~ECHO;    /* no echo either       */
    ttystate.c_cc[VMIN]     =1;         /* get 1 char at a time */
    tcsetattr(0, TCSANOW, &ttystate);   /* install settings     */
}

void set_nodelay_mode(){
    int     termflags;

    termflags= fcntl(0, F_GETFL);
    termflags|=O_NDELAY;
    fcntl(0, F_SETFL, termflags);
}

void tty_mode(int how){
    static struct termios   original_mode;
    static int              original_flags;
    if(how==0) {
        tcgetattr(0, &original_mode);
        original_flags = fcntl(0, F_GETFL);
    }
    else {
        tcsetattr(0, TCSANOW, &original_mode);
        fcntl(0, F_SETFL, original_flags);
    }
}
