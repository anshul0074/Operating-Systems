#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <limits.h>
#include <time.h>
#include <semaphore.h>

#define MAX 251
pthread_mutex_t array_lock=PTHREAD_MUTEX_INITIALIZER,sort_lock=PTHREAD_MUTEX_INITIALIZER,search_lock =PTHREAD_MUTEX_INITIALIZER;
sem_t search_signal[MAX],full[MAX],empty[MAX];
int sortflag=0,searchflag=0,global=0,idx=0;
int* queries;
//for file reading threads
typedef struct __myarg_t {
    FILE *fp;
} myarg_t;
//for searching threads
struct search_arg{
    int id;
};
struct arrays{
    int* arr;
    int size;
};
// stored_array stores all available arrays
struct arrays* stored_arrays;

// extract function that extracts arrays from file
 void * extract(void * arg) {
    myarg_t * x= (myarg_t * )arg;
    FILE* fp=x->fp;
    char * line = NULL;
    size_t len = 0;
    while (getline(&line, &len, fp) != -1) {
        // sleep for 1 sec
        usleep(1);
        if(line[0]==' '){
            continue;
        }
        char* str=(char *)malloc(sizeof(char)*(strlen(line)+1));
        strcpy(str, line);
        char* token;
        int len=0;
        // extract integers 
        token=strtok(line, " ");
        while( token != NULL ) {
        token = strtok(NULL, " ");
            len++;
        }
        //new array
        stored_arrays[global].arr = (int *) malloc(sizeof(int)*len);
        token = strtok(str," ");
        // assign values 
        for(int i=0;i<len;i++){
            stored_arrays[global].arr[i] = atoi(token);
            token = strtok(NULL, " ");
        }
        stored_arrays[global].size = len;
        global++;
    }
    //set sortflag = 1 after reading arrays
    pthread_mutex_lock(&sort_lock);
    sortflag = 1;
    pthread_mutex_unlock(&sort_lock);
    return NULL;
 }

//comparartor for sort
int comp(const void *x, const void *y){ 
      return (*(int*)x-*(int*)y);
} 
// the sort function to sort arrays one at a time
void* sort(void * arg){
    while(1){
        // check stopping condition
        pthread_mutex_lock(&sort_lock);
        if(sortflag == 1 && idx >= global) {
            break;
        }
        pthread_mutex_unlock(&sort_lock);
        // spin until global index not greater sort index
        while(idx>=global);
        qsort((void*)stored_arrays[idx].arr,stored_arrays[idx].size,sizeof(stored_arrays[idx].arr[0]),comp);
        printf("array %d is sorted\n",idx);
        sem_post(&search_signal[idx]); 
        pthread_mutex_lock(&array_lock);
        idx++;
        pthread_mutex_unlock(&array_lock);
    }
    return NULL;
}

// process function for processing queries
void * process(void * arg){
    myarg_t * x = (myarg_t * ) arg;
    FILE* fp = x->fp;
    char * line = NULL;
    size_t len = 0;
    // start reading queries
    while (getline(&line, &len, fp) != -1){
        // sleep for 1 sec
        usleep(1);
        int q = atoi(line);
        // check the number of available arrays
        pthread_mutex_lock(&array_lock);
        int n = idx;
        pthread_mutex_unlock(&array_lock);
        // for each thread send a signal
        for(int i=0;i<n;i++){
            //wait for empty signal
            sem_wait(&empty[i]);
            //add the integer
            queries[i] = q;
            //post full signal
            sem_post(&full[i]);
        }
    }
    // set searchflag = 1 after all queries
    pthread_mutex_lock(&search_lock);
    searchflag = 1;
    pthread_mutex_unlock(&search_lock);
    return NULL;
}
// search funtion for searching queries one at a time
void* search(void * arg){
    struct search_arg* x= (struct search_arg*) arg;
    sem_wait(&search_signal[x->id]);
    while(1){
        sem_wait(&full[x->id]);
        // stopping condition
        pthread_mutex_lock(&search_lock);
        if((searchflag == 1 &&   queries[x->id] == -1) || queries[x->id] == -1){
            break;
        }
        pthread_mutex_unlock(&search_lock);
        int val=queries[x->id];
        // binary search for the value
        int* res=bsearch(&val,stored_arrays[x->id].arr,stored_arrays[x->id].size,sizeof(stored_arrays[x->id].arr[0]),comp);
        //if value found
        if(res!=NULL){
            printf("Query : %d  found in array %d at location %d by search thread %d\n",val,x->id,(int)(res - stored_arrays[x->id].arr)-1,x->id);
        }
        // query processing done
        // post the empty signal and consume the query integer
        queries[x->id] = -1;
        sem_post(&empty[x->id]);
    }
    return NULL;
}

int main() {
    stored_arrays=(struct arrays *) malloc(sizeof(struct arrays)* MAX);
    queries=(int *) malloc(sizeof(int)*MAX);
    FILE* af = fopen("arraydata.txt", "r");
    FILE* qf = fopen("querydata.txt", "r");
    if(af==NULL || qf==NULL){
        exit(0);
    }
    // initialize MAX semaphores variables
    for(int i=0;i<MAX;i++){
        sem_init(&search_signal[i],0,0);
        sem_init(&empty[i],0,1);
        sem_init(&full[i],0,0);
    } 
    // create thread for reading an array
    pthread_t pread;
    myarg_t args;
    args.fp = af;
    pthread_create(&(pread), NULL, extract, &args);
    // create a sort thread
    pthread_t psort;
    pthread_create(&(psort), NULL, sort, &args);
    // create MAX search threads
    pthread_t th[MAX];
    struct search_arg arg1[MAX];
    for(int i = 0; i < MAX; i++){
       arg1[i].id = i;
       pthread_create(&(th[i]), NULL, search, &(arg1[i]));
    }
    //create a thread to fetch queries from file.
    pthread_t pquery;
    myarg_t args2;
    args2.fp = qf;
    pthread_create(&(pquery), NULL, process, &args2);
    // join array array thread
    pthread_join(pread, NULL);
    // join array sorting thread
    pthread_join(psort, NULL);
    printf("sorting thread exits\n");
    // join query reading thread
    pthread_join(pquery, NULL);
    // cancel remaining search threads
    for(int i=0;i<MAX;i++){
    	pthread_cancel(th[i]);
    }
    printf("all queries processed\n");
    return 1;
 }