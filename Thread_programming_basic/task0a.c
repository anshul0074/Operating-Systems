#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int *wait;
typedef struct __myarg_t{
    int a;
    char *b;
} myarg_t;
void *mythread(void *arg){
    myarg_t *m = (myarg_t *)arg;
    printf("%d %s\n", m->a, m->b);
    *wait = 0;
    return NULL;
}
int main(int argc, char *argv[]){
    pthread_t p;
    wait =(int *)malloc(sizeof(int));
    *wait=1;
    myarg_t args={10,"hello world"};
    printf("hello world without pthread_join->\n");
    pthread_create(&p, NULL, mythread, &args);
    while (*wait == 1){
        //waiting
    }
    printf("\n");
    printf("hello world with pthread_join->\n");
    pthread_t p1;
    myarg_t args1={10,"hello world"};
    pthread_create(&p1, NULL, mythread, &args1);
    pthread_join(p1, NULL);
    free(wait);
}