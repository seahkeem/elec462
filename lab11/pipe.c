#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

#define define oops(m, x) { perror(m); exit(x); }

int main(int ac, char **av){
    int thepipe[2], newfd, pid;
    
    if(ac!=3){
        fprintf(stderr, "usage: pipe cmd1 cmd2\n");
        exit(1);
    }
    if(pipe(thepipe)==-1){
        perror("Cannot get a pipe");
        exit(1);
    }
    if((pid=fork())==-1) {
        perror("Cannot fork");
        exit(2);
    }
    if(pid>0){
        close(thepipe[1]);
        
        if(dup2(thepipe[0], 0)==-1) {
            perror("could not redirect stdin");
            exit(3);
        }
        close(thepipe[0]);
        execlp(av[2], av[2], NULL);
        
        perror(av[2]);
        exit(4);
        }
    close(thepipe[0]);
    
    if(dup2(thepipe[1], 1)==-1){
        perror("could not redirect stdout");
        exit(4);
    }
    close(thepipe[1]);
    execlp(av[1], av[1], NULL);
    
    perror(av[1]);
    exit(5);
    
    return 0;
}
