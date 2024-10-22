#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#define SIZE    10000
#define FILE    255

char    path[SIZE][FILE];
int     len=0;

int compare(const void *x, const void *y){
    return strcmp(x, y);
}

void DFS(char *dirname){
    DIR             *dir_ptr;
    char            directory[SIZE][FILE];
    struct dirent   *dirent_ptr;
    struct stat     buf;

    int             size=0;
    int             i=0;

    if((dir_ptr=opendir(dirname))<0){
        len--;
        printf("\n");
        return ;
    }

    chdir(dirname);

    if(strcmp(".", dirname)!=0){
        strcpy(path[len], dirname);
        printf("./");
        len++;

        for(i=0; i<len-1; i++){
            printf("%s/", path[i]);
        }
        printf("%s:\n", path[len-1]);
    }

    while((dirent_ptr=readdir(dir_ptr))!=NULL){
        stat(dirent_ptr->d_name, &buf);

        if(strcmp(dirent_ptr->d_name, ".")==0 || strcmp(dirent_ptr->d_name, "..")==0)
            continue;

        strcpy(directory[size], dirent_ptr->d_name);
        size++;
    }

    qsort((void *)directory, size, sizeof(directory[0]), compare);

    for(i=0; i<size; i++){
        printf("%s\t", directory[i]);
    }
    printf("\n");

    for(i=0; i<size; i++){
        stat(directory[i], &buf);
        if (S_ISDIR(buf.st_mode)){
            DFS(directory[i]);
        }
    }

    len--;
    closedir(dir_ptr);
    chdir("..");
}

int main(){
    DFS(".");

    return 0;
}
