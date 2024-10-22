#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(){
    int pid;
    int fd;

    printf("About to run sort with input from sample.txt\n");

    if((pid=fork())==-1){
        perror("fork");
        exit(1);
    }

    if(pid==0){
        close(0);
        fd=open("sample.txt", O_RDONLY);
        execlp("sort", "sort", NULL);
        //execlp("O_CREATE", "S_IRUSR", NULL);
        //execlp("O_CREATE", "S_IWUSR", NULL);
        perror("execlp");
        exit(1);
    }

    if(pid!=0){
        wait(NULL);
        printf("Done running sort. <sample.txt\n");
    }

    return 0;
}
