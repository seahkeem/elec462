#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int time=0;

int main(){
    void f();	
    signal(SIGINT, f);

    printf("you can't stop me!\n");
    while(1){
        sleep(1);
	time++;
        printf("haha\n");
    }

    return 0;
}

void f(){
	printf("^CCurrently elapsed time:  %d sec(s)\n", time);
}
