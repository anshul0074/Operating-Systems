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
typedef struct __concat_t{
    char *s1;
    char *s2;
} concat_t;

void *mythread(void *arg){
    myarg_t *m = (myarg_t *)arg;
    int print_index = 0;
    for (print_index = 0; print_index < m->a; print_index++){
        printf("Printing %d th character %c\n", print_index, *(m->b + print_index));
    }
    myret_t *r = malloc(sizeof(myret_t));
    r->x = 10;
    r->y = m->b;
    return (void *)r;
}
void *concat_func_t(void *arg){
    concat_t *m = (concat_t *)arg;
    char *s3 = (char *)malloc(strlen(m->s1) + 1 + strlen(m->s2) + 1);
    strcpy(s3, m->s1);
    strcat(s3, " ");
    strcat(s3, m->s2);
    int print_index = 0;
    for (print_index = 0; print_index < strlen(s3); print_index++){
        printf("Printing %d th character %c\n", print_index, *(s3 + print_index));
    }
    myret_t *r = malloc(sizeof(myret_t));
    r->x = 1;
    r->y = s3;
    return (void *)r;
}

int main(int argc, char *argv[]){
    if (argc != 3){
        printf("number of arguments incorrect");
        _exit(0);
    }
    pthread_t p,p2,p3;
    myret_t *m,*m2, *m3;
    myarg_t args,args2;

    //1st string
    args.a = strlen(argv[1]);
    args.b = (char *)malloc(strlen(argv[1]) + 1);
    strcpy(args.b, argv[1]);
    pthread_create(&p, NULL, mythread, &args);
    pthread_join(p, (void **)&m);
    printf("1st thread\n");
    printf("returned %d \"%s\"\n\n", m->x, m->y);

    //2nd string
    args2.a = strlen(argv[2]);
    args2.b = (char *)malloc(strlen(argv[2]) + 1);
    strcpy(args2.b, argv[2]);
    pthread_create(&p2, NULL, mythread, &args2);
    pthread_join(p2, (void **)&m2);
    printf("2nd thread\n");
    printf("returned %d \"%s\"\n\n", m2->x, m2->y);

    //3rd thread
    concat_t args3;
    args3.s1 = (char *)malloc(strlen(m->y) + 1);
    strcpy(args3.s1, m->y);
    args3.s2 = (char *)malloc(strlen(m2->y) + 1);
    strcpy(args3.s2, m2->y);
    pthread_create(&p3, NULL, concat_func_t, &args3);
    pthread_join(p3, (void **)&m3);
    printf("3rd thread\n");
    printf("returned %d \"%s\"\n", m3->x, m3->y);
}