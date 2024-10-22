#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/vfs.h>

int main(int av, char ** ac){
    struct statfs data;

    if(av==1){
        printf("Error:Please enter a valid value\n");
        exit(0);
    }

    statfs(ac[1], &data);
    
    long total=data.f_blocks*data.f_bsize;
    long used=data.f_blocks*data.f_bsize-data.f_bsize*data.f_bavail;
    long avail=data.f_bsize*data.f_bavail;
    long myused=(used/(double)total)*100;

    if(av==2){
        printf("%9s %8s %9s %5s%%\n", "1K-blocks", "Used", "Available", "MyUse");
        printf("%9ld %8ld %9ld %5ld%%\n", total/1000, used/1000, avail/1000, myused);
    }

    if(av==3){
        if (ac[2][1]=='b' || ac[2][1]=='B'){
            printf("%12s %10s %12s %5s%%\n", "1B-blocks", "Used", "Available", "MyUse");
            printf("%12ld %10ld %12ld %5ld%%\n", total, used, avail, myused);
        }
        else if (ac[2][1]=='k' || ac[2][1]=='K'){
            printf("%12s %10s %12s %5s%%\n", "1K-blocks", "Used", "Available", "MyUse");
            printf("%12ld %10ld %12ld %5ld%%\n", total/1000, used/1000, avail/1000, myused);
        }
        else if (ac[2][1]=='m' || ac[2][1]=='M'){
            printf("%12s %10s %12s %5s%%\n", "1M-blocks", "Used", "Available", "MyUse");
            printf("%12ld %9ldM %11ldM %5ld%%\n", total/(1000*1000), used/(1000*1000), avail/(1000*1000), myused);
        }
        else if (ac[2][1]=='g' || ac[2][1]=='G'){
            printf("%12s %10s %12s %5s%%\n", "1G-blocks", "Used", "Available", "MyUse");
            printf("%12ld %9ldG %11ldG %5ld%%\n", total/(1000*1000*1000), used/(1000*1000*1000), avail/(1000*1000*1000), myused);
        }
    }

    return 0;
}
