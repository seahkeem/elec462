#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAXARGS 20
#define ARGLEN  100

int excute(char **);
char *makestring(char *);

int main(){
    char    *arglist[MAXARGS+1];
    int     numargs;
    char    argbuf[ARGLEN];
    char    *makestring();
    int *test;

    numargs=0;
    
    while (numargs<MAXARGS){
        printf("Arg[%d]? ", numargs);
	test=fgets(argbuf, ARGLEN, stdin);
        if(test&&*argbuf!='\n'){
            arglist[numargs++]= makestring(argbuf);
            if(strcmp(argbuf, "exit")==0)
                exit(1);
	}
	else{

            if(numargs>0){
                arglist[numargs]=NULL;
                excute(arglist);
                numargs=0;
            }
	    if(test==NULL)
                exit(1);
    }
    }
    return 0;
}

int excute(char *arglist[]){
    int pid, exitstatus;

    pid=fork();
    switch (pid) {
        case -1:
            perror("fork failed");
            exit(1);
        case 0:
            execvp(arglist[0], arglist);
            perror("execvp failed");
            exit(1);
        default:
            while (wait(&exitstatus)!=pid)
                ;
            printf("child exited with status %d, %d\n", exitstatus>>8, exitstatus&0377);

    }
}

char *makestring(char *buf){
    char    *cp;

    buf[strlen(buf)-1]='\0';
    cp= malloc(strlen(buf)+1);
    if(cp==NULL){
        fprintf(stderr, "no memory\n");
        exit(1);
    }
    strcpy(cp, buf);
    return cp;
}
