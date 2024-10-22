#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define CLOSE_DUP
//#define USE_DUP2 

int main(){
    int fd;
    int newfd;
    int line[100];

    fgets(line, 100, stdin);
    printf("%s", line);
    fgets(line, 100, stdin);
    printf("%s", line);
    fgets(line, 100, stdin);
    printf("%s", line);

    fd= open("/etc/passwd", O_RDONLY);

    close(0);
    
    newfd=dup(fd);
    newfd= dup2(fd, 0);
    
    if(newfd!=0){
        fprintf(stderr, "Could not open data as fd to 0\n");
        exit(1);
    }
    close(fd);
    
    fgets(line, 100, stdin);
    printf("%s", line);
    fgets(line, 100, stdin);
    printf("%s", line);
    fgets(line, 100, stdin);
    printf("%s", line);

    return 0;
}
