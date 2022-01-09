#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <termios.h>
#include <fcntl.h>

#define MAXTOKENS 256 // maximum number of tokens in a command
#define MAXINP 1024 // maximum number of characters from keyboard
int drp; // to prevent the printing of the shell in some cases

//shell pid, pgid, terminal modes
static pid_t GBSH_PID;
static pid_t GBSH_PGID;
static int GBSH_IS_INTERACTIVE;
static struct termios GBSH_TMODES;
static char* curr_directory;
extern char** environ;
struct sigaction act_child;
struct sigaction act_int;
pid_t pid;


//******************************** linked list functions *****************************************

// node struct
struct Node { 
    char data[1024];
    int pid; 
    struct Node* next; 
};

struct Node* head = NULL; 
struct Node* curr = NULL;

//append
void append(struct Node** head, char *new_data,int child_pid) {
	struct Node* new_node = (struct Node*) malloc(sizeof(struct Node)); 
	struct Node *tail = *head;
  strcpy(new_node-> data , new_data);  
  new_node->pid=child_pid;
	new_node->next = NULL; 
	if (*head == NULL) 
	{ 
	*head = new_node; 
	return; 
	} 
	while (tail->next != NULL){
		tail = tail->next; 
	}
	tail->next = new_node; 
	return;	 
}

//number of nodes
int numnodes(struct Node* head){ 
    if (head == NULL){ 
        return 0; 
	}
    return 1 + numnodes(head->next); 
} 

//delete a node
void delete_node(struct Node** head_ptr, int key){
    struct Node *tmp = *head_ptr, *prev;
    if (tmp != NULL && tmp->pid == key) {
        *head_ptr = tmp->next; 
        free(tmp); 
        return;
    }
    while (tmp != NULL && tmp->pid != key) {
        prev = tmp;
        tmp = tmp->next;
    }
    if (tmp == NULL){
        return;
	}
    prev->next = tmp->next;
 	free(tmp); 
}

//delete full list
void delete_list(struct Node** head_ptr){
   struct Node* curr = *head_ptr;
   struct Node* next;
   while (curr != NULL) {
       next = curr->next;
       free(curr);
       curr = next;
   }
   *head_ptr = NULL;
}

//******************************************** shell display functions *******************************

//welcome screen
void welcome(){
        printf("\n\t******************************************\n");
        printf("\t        	Shell_Prompt\n");
        printf("\t********************************************\n");
        printf("\n\n");
}

//prompt display
void shellPrompt(){
	char host[101];
	strcpy(host,"anshul@iitjammu");
	char cwd[1024];
	char hostn[1204] = "";
	gethostname(hostn, sizeof(hostn));
	getcwd(cwd, sizeof(cwd));
	if (strncmp(cwd,hostn,strlen(hostn))==0){
		char *b = cwd +strlen(hostn);
		printf("<%s:~%s>",host,b);
	}
	else{
		printf("<%s:%s> ",host,cwd);
	}	
}

//********************************** command executing functions **************************

//program runner
void runner(struct Node** head,struct Node** curr,char **args, int bkg){
	 int a=0;
	char line[1024]="";
	if(bkg==1){
		char token[256];
		strncpy(args[0],args[0]+1,strlen(args[0]));
	}
	while (args[a]){
		strcat(line,args[a]);
		strcat(line," ");
		a++;
	}
	 if((pid=fork())==-1){
		 printf("Unable to create child process\n");
		 return;
	 }
	if(pid==0){
		signal(SIGINT, SIG_IGN); // setting child to ignore SIGINT signals
		setenv("parent",getcwd(curr_directory, 1024),1);	
		if (execvp(args[0],args)==-1){
			printf("Command not found\n");
		}
	 }
	 else{
		if(bkg==0){
		 	waitpid(pid,NULL,0);
	 	}
		else{
		 	append(curr,line,pid);
		}
		append(head,line,pid);
	 }	 
}

//parser
void parse(char *line, char **args){
    if (strcmp(line, "exit\n") == 0){
        exit(EXIT_SUCCESS);
	}
    char **next = args;
    char *tmp = strtok(line, " \n");
    while (tmp != NULL){
        *next++ = tmp;
        tmp = strtok(NULL, " \n");
    }
    *next = NULL;
    for (next = args; *next != 0; next++){
        puts(*next);
	}
}

// directory change function
int change_directory(char* args[]){
	// if nothing is written after cd then move to home directory
	if (args[1] == NULL) {
		chdir(getenv("HOME")); 
		return 1;
	}
	// else change directory to the name provided
	else{ 
		if (chdir(args[1]) == -1) {
			printf(" %s: no directory of given name\n", args[1]);
            return -1;
		}
	}
	return 0;
}

int digits(char *s){
    for (int i = 0; i < strlen(s); i++) {
        if ((s[i]<'0' || s[i]>'9')){
			 return 0;
		}
    }
    return 1;
}
//printer
void print(struct Node** head_ptr,char* value) {
  struct Node* ptr = *head_ptr;
  if (strcmp(value,"FULL")==0){
  	printf("Processes spawned from this shell: (if no output then no background processes)\n");
  	while(ptr) {
      printf("command name: %s  process id: %d\n",ptr->data,ptr->pid);
      ptr = ptr->next;
  	}
  }
  else if (digits(value)==1){
    int num=atoi(value);
    int tail=0; 
    int cnt=1;
    int com=numnodes(ptr);
    while(ptr) {
      tail=numnodes(ptr);
      if(tail<=num){
        while(ptr) {
      		printf("%d %s\n",cnt,ptr->data);
      		cnt=cnt+1;
      		ptr = ptr->next;
    	} 
        break;
      }
      ptr = ptr->next;
  	}
    if(tail!=num){
        printf(" %d commands were executed previously.\n",com);
    	}
	}
}

// command processor and executer
void executer(int num,int cnt,struct Node* tmp);
int command_processor(char * args[]){
	int i=0,j=0,bkg=0,a=0;
	int fileDescriptor,standardOut, aux;
	char line[1024]="";
	int child_pid;
	while (args[a]){
		strcat(line,args[a]);
		strcat(line," ");
		a++;
	}
	char *args_tmp[256];
	while ( args[j] != NULL){
		if ( (strcmp(args[j],">") == 0) || (strcmp(args[j],"<") == 0) || (strcmp(args[j],"&") == 0)){
			break;
		}
		args_tmp[j] = args[j];
		j++;
	}
	//"pwd" command 
 	if(strcmp(args[0],"pwd") == 0){
		 append(&head, "pwd",getpid());
		if (args[j] != NULL){
			if ( (strcmp(args[j],">") == 0) && (args[j+1] != NULL) ){
				fileDescriptor = open(args[j+1], O_CREAT | O_TRUNC | O_WRONLY, 0600); 
				standardOut = dup(STDOUT_FILENO); 	
				dup2(fileDescriptor, STDOUT_FILENO); 
				close(fileDescriptor);
				printf("%s\n", getcwd(curr_directory, 1024));
				dup2(standardOut, STDOUT_FILENO);
			}
		}else{
			printf("%s\n", getcwd(curr_directory, 1024));
		}
	} 
	// type "quit" to exit the shell
	else if(strcmp(args[0],"quit") == 0) {
		delete_list(&head);
		printf("Memory freed and exiting\n");
		exit(0);
	}
	// type "clear" to clear screen
	else if (strcmp(args[0],"clear") == 0) {
		system("clear");
	}
	// "cd" command
	else if (strcmp(args[0],"cd") == 0) {
	   append(&head, line ,getpid());
		change_directory(args);
	}
	//"pid all" and "pid current" commands
	else if (strcmp("pid",args[0])==0 && args[1]){
		if (strcmp("all",args[1])==0){
		print(&head,"FULL");
        }
		else if(strcmp("curr",args[1])==0){
		print(&curr,"FULL");
        }
		append(&head,line,getpid()); 
	  }
	// if no argument is given after "pid"
	else if (strcmp("pid",args[0])==0 ){
		int child_pid;
		printf("command name: ./a.out  process id: %d\n",getpid());
        child_pid=getpid();
		append(&head,line,getpid());
        return child_pid;   
	  }
	//"HISTN" command
    else if (strncmp("HIST",args[0],4)==0){
          char num_check[1024];
          strncpy(num_check,args[0]+4,strlen(args[0]));
          if (digits(num_check)==1 && strcmp(num_check,"")!=0){
              print(&head,num_check);   
          }
          else{
              printf("HIST is to be used with number n, where the n commands executed previously will be shown\n");
          }
          child_pid=getpid();
            append(&head,line,child_pid);
            return child_pid;

      }
	//"!HISTN" and "!HIST(N1,N2,.....)" commands
	else if (strncmp(args[0],"!HIST",5)==0){
        if(args[0][5]=='('){
            int arr[101];
            int idx=6,cnter=0,value=0;
            while(idx<strlen(args[0])){
                if(args[0][idx]==',' || args[0][idx]==')'){
                    arr[cnter]=value;
                    cnter++;
                    idx++;
                    value=0;
                    continue;
                }
                value=10*value+(args[0][idx]-'0');
                idx++;
            }
            for(int i=0;i<cnter;i++){
                int num=arr[i];
                struct Node *tmp=head;
		        int cnt=1;
		        executer(num,cnt,tmp);
            }
        }
        else{
		    strncpy(args[0],args[0]+5,strlen(args[0]));
		    int num=atoi(args[0]);
		    struct Node *tmp=head;
		    int cnt=1;
            executer(num,cnt,tmp);
        }

	}
	//all other commands
	else{
		while (args[i] != NULL && bkg == 0){
			if (strncmp(args[i],"&",1) == 0){
				bkg = 1;
			}
			i++;
		}
		args_tmp[i] = NULL;
		runner(&head,&curr,args_tmp,bkg);
	}
	return 1;
}
void executer(int num,int cnt,struct Node* tmp){
     while(tmp){
		if (cnt==num){
			char new_args[MAXINP]; 
			char * tokens[MAXTOKENS];
			memset ( new_args, '\0', MAXINP );
			strcpy(new_args,tmp->data);
			if((tokens[0] = strtok(new_args," \n\t")) == NULL){
				 continue;
			}
			int numtkn = 1;
			while((tokens[numtkn] = strtok(NULL, " \n\t")) != NULL) numtkn++;
				command_processor(tokens);
				break;
			}
			cnt++;
		tmp=tmp->next;
		}
}

//************************************************ signal handling functions *************************************************8

//signal handler for SIGINT
void signalHandler_int(int p){
	// Sending SIGTERM signal to child process;
	if (kill(pid,SIGTERM) == 0){
		printf("\nprocess %d SIGINT signal received\n",pid);
		drp = 1;			
	}
	else{
		printf("\n");
	}
}

// signal handler for child
void signalHandler_child(int p){
	int child;
	int status;
	struct Node *check = curr;
	child=waitpid(-1, &status, WNOHANG);
	while(check){
		if (check->pid==child){
		printf("command %s with process id %d has exited\n",check->data,check->pid);
		delete_node(&curr,child);
		break;	
		}
		check=check->next;
	}	
}

//initializer
void init(){
    GBSH_PID = getpid();  
    GBSH_IS_INTERACTIVE =isatty(STDIN_FILENO);  
	if (GBSH_IS_INTERACTIVE) {
		while (tcgetpgrp(STDIN_FILENO) != (GBSH_PGID = getpgrp())){
				kill(GBSH_PID, SIGTTIN);             
		}     
	    //signal handlers for SIGINT and SIGCHILD
		act_child.sa_handler = signalHandler_child;
		act_int.sa_handler = signalHandler_int;			
		sigaction(SIGCHLD, &act_child, 0);
		sigaction(SIGINT, &act_int, 0);
		//put in process group
		setpgid(GBSH_PID, GBSH_PID); 
		GBSH_PGID = getpgrp();
		if (GBSH_PID != GBSH_PGID) {
				printf("Error, the shell is not process group leader");
				exit(EXIT_FAILURE);
		}
		//control of the terminal is taken
		tcsetpgrp(STDIN_FILENO, GBSH_PGID);  
		//default terminal attributes for shell are saved
		tcgetattr(STDIN_FILENO, &GBSH_TMODES);
		//get curr directory
		curr_directory = (char*) calloc(1024, sizeof(char));
    } 
	else{
        printf("Error, the shell could not be made interactive.\n");
        exit(EXIT_FAILURE);
    }
}

//********************************************* main function **********************************************************

int main(int argc, char *argv[], char ** envp) {
	// keyboard command 
	char line[MAXINP]; 
	//tokens in the command
	char * tokens[MAXTOKENS]; 
	int numtkn;
	drp = 0; 	
	pid =-1;
	init();
	welcome();
	environ = envp;
	setenv("shell",getcwd(curr_directory, 1024),1);
	while(1){
		if (drp == 0) {
			shellPrompt();
		}
		drp=0;
		memset ( line, '\0', MAXINP );
		fgets(line, MAXINP, stdin);
		//if nothing is given in shell
		if((tokens[0] = strtok(line," \n\t")) == NULL){
			 continue;
		}
		//read the command and extract tokens
		numtkn=1;
		while((tokens[numtkn] = strtok(NULL, " \n\t")) != NULL){
			numtkn++;
		} 
		//execute the command
		command_processor(tokens);
	}          
	exit(0);
}
