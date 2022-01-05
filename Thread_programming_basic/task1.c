#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAX 11

int cnt=0;
int arr[MAX]={0};
pthread_t th[MAX];
pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;

void *mythread(void *arg){
    pthread_mutex_lock(&lock);
    arr[cnt]=rand()%101;
    printf("cnt = %d , integer = %d\n",cnt,arr[cnt]);
    cnt+=1;
    pthread_mutex_unlock(&lock);
    return NULL;
}
int main(int argc, char *argv[]){
    srand(time(NULL));
    for (int i=0;i<MAX;i++){
        pthread_create(&(th[i]),NULL,mythread,NULL);
    }
    for (int i=0;i<MAX;i++){
        pthread_join((th[i]), NULL);
    }
    printf("array values->\n");
    for (int i=0;i<MAX;i++){
        printf("%d ",arr[i]);
    };
    printf("\n");
}