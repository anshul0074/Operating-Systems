#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int cmp(const void *a,const void *b){
    return (*(int *)a - *(int *)b);
}
int main(){
    FILE *ptr,*tmp;
    ptr=fopen("./file2.txt", "r");
    tmp=fopen("./file2.txt", "r+");
    int arr[10];
    int hash[100];
    for(int i=0;i<100;i++){
        hash[i]=0;
    }
    srand(time(NULL));
    for (int i=0;i<10;i++){
        arr[i]=(rand()+7)%100;
        while(hash[arr[i]]==1){
            arr[i]=(rand()+7)%100;
        }
        hash[arr[i]]=1;
    }
    qsort(arr,10,sizeof(int),cmp);
    int cnt=0,idx=0,num;
    fscanf(ptr,"%d ",&num);
    while (!feof(ptr)){
        if (idx == arr[cnt]){
            fprintf(tmp,"%d ",0);
            cnt++;
        }
        else{
            fprintf(tmp,"%d ",num);
        }
        idx++;
        fscanf(ptr,"%d ",&num);
    }
    fclose(ptr);
    fclose(tmp);
}