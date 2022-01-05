#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<assert.h>
#define MAX 11

int idx=0;
int arr[MAX]={0};
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER , lock2=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;

typedef struct __myarg_t {
    int a;
    int b;
 } myarg_t;
void * producerthread(void * arg) {
    pthread_mutex_lock(&lock1);
    int val=rand()%101;
    arr[idx]=val;
    idx++;
    printf("produced array index is %d and value is  %d\n\n",idx-1 ,val);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock1);
    return NULL;
}
void * consumerthread( void * arg){
    myarg_t * m=(myarg_t*)arg;
    pthread_mutex_lock(&lock2);
    for(int i=m->a;i<m->b;i++){
        while(arr[i]==-7){
            pthread_cond_wait(&cond, &lock2);
        }
        int u=arr[i];
        arr[i]=-1;
        printf("consumed array index is %d and value is %d\n\n",i,u);
    }
    pthread_mutex_unlock(&lock2);
    return NULL;
}
int main(){
    pthread_t p[10],c[3];
    myarg_t args[3];
    args[0].a=0;
    args[0].b=5;
    args[1].a=5;
    args[1].b=8;
    args[2].a=8;
    args[2].b=11;
    for(int i=0;i<MAX;i++){
        arr[i]=-7;
    }
    for(int i =0;i<MAX;i++){
        pthread_create(&(p[i]),NULL,producerthread,NULL);
    }
    for(int i=0;i<3;i++){
        pthread_create(&(c[i]),NULL,consumerthread,&(args[i]));
    }
    for(int i=0;i<MAX;i++){
        pthread_join(p[i],NULL);
    }
    for(int i=0;i<3;i++){
        pthread_join(c[i],NULL);
    }
}