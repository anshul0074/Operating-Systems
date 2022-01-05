#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<time.h>
#include<pthread.h>
#include<assert.h>
#include<errno.h>
#define MAX 11

int idx=0;
int tmp_idx=0;
int arr[MAX]={0};
int tmp_arr[MAX];
pthread_mutex_t lock1=PTHREAD_MUTEX_INITIALIZER,lock2=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond=PTHREAD_COND_INITIALIZER;

void swap(int *x,int *y){
    int tmp=*x;
    *x=*y;
    *y=tmp;
}
void * producerthread(void *arg){
    sleep(1);
    pthread_mutex_lock(&lock1);
    int val=rand()%101;
    arr[idx]=val;
    idx++;
    printf("produced array index is %d and val is %d\n\n",idx-1,val);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock1);
    return NULL;
}
void * consumerthread( void * arg){
    pthread_mutex_lock(&lock2);
    while(tmp_idx<MAX){
        int local_idx=tmp_arr[tmp_idx];
        tmp_idx++;
        while(arr[local_idx]==-7){
            pthread_cond_wait(&cond, &lock2);
        }
        int u=arr[local_idx];
        arr[local_idx]=-1;
        printf("consumed array index is %d and val is %d\n\n",local_idx ,u);
    }
    pthread_mutex_unlock(&lock2);
    return NULL;
}
int main(){
    srand(time(NULL));
    pthread_t p[MAX],c[2];
    for(int i=0;i<MAX;i++){
        arr[i]=-7;
        tmp_arr[i]=i;
    }
    for(int i=MAX-1;i>0;i--){ 
        int j=rand()%(i+1); 
        swap(&tmp_arr[i],&tmp_arr[j]); 
    } 
    for(int i=0;i<MAX;i++){
        pthread_create(&(p[i]),NULL,producerthread,NULL);
    }
    for(int i=0;i<2;i++){
        pthread_create(&(c[i]),NULL,consumerthread,NULL);
    }
    for(int i=0;i<MAX;i++){
        pthread_join(p[i],NULL);
    }
    for(int i=0;i<2;i++){
        pthread_join(c[i], NULL);
    }
    exit(0);
}