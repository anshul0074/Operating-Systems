#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

typedef struct __myarg_t{
    int a;
    char *b;
} myarg_t;
typedef struct __myret_t{
    int x;
    char *y;
} myret_t;
void *mythread(void *arg){
    myarg_t *m = (myarg_t *)arg;
    int print_index = 0;
    for (print_index = 0; print_index < m->a; print_index++){
        printf("Printing %d th character %c\n", print_index, *(m->b + print_index));
    }
    myret_t *r=malloc(sizeof(myret_t));
    r->x=1;
    r->y=m->b;
    return (void *)r;
}
int main(int argc, char *argv[]){
    if (argc != 2){
        printf("number of arguments incorrect");
        _exit(0);
    }
    pthread_t p;
    myret_t *m;
    myarg_t args;
    args.a = strlen(argv[1]);
    args.b = (char *)malloc(strlen(argv[1]) + 1);
    strcpy(args.b, argv[1]);
    pthread_create(&p, NULL, mythread, &args);
    pthread_join(p, (void **)&m);
    printf("returned %d \"%s\"\n\n", m->x, m->y);
}