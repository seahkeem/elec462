#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define DELAY   2

int main(){
    int newpid;
    void child_code(), parent_code();

    printf("before: my pid is %d\n", getpid());

    if((newpid=fork()==-1))
        perror("fork");
    else if(newpid==0)
        child_code(DELAY);
    else
        parent_code(newpid);

    return 0;
}

void child_code(int delay){
    printf("chile %d here. will sleep for %d seconds\n", getpid(), delay);
    sleep(delay);
    printf("child done. about to exit\n");
    exit(17);
}

void parent_code(int childpid){
    int wait_rv;
    wait_rv= wait(NULL);
    printf("doen waiting for %d. Wait returned: %d\n", childpid, wait_rv);
}
