#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
#include<string.h>

int main(){
    FILE *ptr,*tmp;
    ptr=fopen("./file2.txt","r+");
    for (int i=1;i<=100;i++){
        fprintf(ptr,"%d ",i);
    }
    fclose(ptr);
    int pid=fork();
    if(pid==0){
        char *args[]={"./child",NULL};
        execv(args[0],args);
    }
    else{
        wait(NULL);
        int idx=0,num;
        FILE *ptr=fopen("./file2.txt","r");
        fscanf(ptr, "%d ",&num);
        while (!feof(ptr)){
            if(num==0){
                printf("%d ",idx);
            }
            fscanf(ptr,"%d ",&num);
            idx++;
        }
        fclose(ptr);
    }
    printf("\n");
}