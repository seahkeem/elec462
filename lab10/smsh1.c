#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "smsh.h"

#define DFL_PROMPT  "> "

int main(){
    char *cmdline, *prompt, **arglist;
    int result;
    void setup();

    prompt=DFL_PROMPT;
    setup();

    while ((cmdline=next_cmd(prompt, stdin))!=NULL){
        if((arglist=splitline(cmdline))!=NULL){
            result=excute(arglist);
            freelist(arglist);
        }
        free(cmdline);
    }
    return 0;
}

void setup(){
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
}

void fatal(char *s1, char *s2, int n){
    fprintf(stderr, "Error: %s, %s\n", s1, s2);
    exit(n);
}
