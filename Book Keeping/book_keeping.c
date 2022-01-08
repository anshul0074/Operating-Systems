#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define MAX_LEN 101
int *commands_executed,*double_flag,*single_flag;
struct copy{
    int filled;
    char **command;
};
struct copy *records;
void sigint_handler(int signo){
    printf("\nCaught SIGINT --Interupting current running command.\n");
}
void sigint_handler_main(int signo){
    printf("\nCaught SIGINT -- Exiting forefully.\n");
    _exit(1);
}
int length(int num){
    int len=0;
    while(num){
        num=num/10;
        len++;
    }
    return len;
}
void executer(int arg_cnt,char **command){
    pid_t pid;
    if ((pid =fork())<(pid_t)0){
        printf("Fork failed.\n");
        _exit(1);
    }
    else if(pid==(pid_t)0){
        if (*single_flag || *double_flag){
            printf("Format invalid\n");
        }
        printf("Executing :");
        for (int i=0;i<arg_cnt;i++){
            printf(" %s", command[i]);
        }
        printf("\n");
        execvp(command[0], command);
        printf("Invalid command.\n");
        printf("\"");
        for (int i=0;i<arg_cnt-1;i++){
            printf("%s ", command[i]);
        }
        printf("%s\" ",command[arg_cnt-1]);
        printf("is not a valid command.\n");
        _exit(1);
    }
    else{
        signal(SIGINT,sigint_handler);
        wait(NULL);
        (records+*(commands_executed))->filled =arg_cnt;
        (records+*(commands_executed))->command=command;
        *(commands_executed)+=1;
        printf("Done :");
        for (int i=0;i<arg_cnt;i++){
            printf(" %s",command[i]);
        }
        printf("\n");
    }
}
int spaces(char *str){
    int numspaces=0;
    for (int i=0;i<strlen(str);i++){
        if (str[i]==' ' || str[i]=='\t'){
            numspaces++;
        }
    }
    return numspaces;
}
int tokenizer(char *private_line,char **command){
    int cnt=0;
    char *iter,*start;
    iter=private_line;
    start=private_line;
    int double_flag_private=0;
    while(*iter!='\n' && *iter!='\0'){
        if(*iter=='"' && double_flag_private==0){
            double_flag_private = !double_flag_private;
            *(double_flag)=1;
        }
        else if (*iter=='\'' && *(single_flag)==0){
            *(single_flag)=1;
        }
        else if (*iter=='"' && double_flag_private==1){
            double_flag_private=!double_flag_private;
            start++;
            *iter='\0';
            command[cnt]=start;
            cnt++;
            start=iter+1;
        }
        else if((*iter==' ' || *iter=='\t') && double_flag_private!=1){
            if((int)(iter-start)==0){
                start++;
            }
            else{
                *iter='\0';
                command[cnt] = start;
                cnt++;
                start=iter+1;
            }
        }
        iter+=1;
    }
    if((int)(iter-start)!=0){
        *iter='\0';
        command[cnt]=start;
        cnt++;
    }
    command[cnt]=NULL;
    return cnt;
}
void read_command(char *file_name){
    FILE *stream=fopen(file_name, "r");
    if(stream==NULL){
        printf("Error : %s does not exist.\n", file_name);
        return;
    }
    char *private_line;
    char **command;
    while(1){
        private_line=(char *)malloc(sizeof(char)*MAX_LEN);
        if(fgets(private_line,MAX_LEN,stream)==NULL){
            break;
        }
        int numspaces=spaces(private_line);
        command=(char **)malloc(sizeof(char *)*(numspaces+2));
        int cnt=tokenizer(private_line,command);
        executer(cnt,command);
    }
    fclose(stream);
}
int main(int argc, char *argv[]){
    if (argc<2){
        printf("Please provide atleast one file with the executable\n");
        return 0;
    }
    records=(struct copy *)malloc(sizeof(struct copy) * MAX_LEN);
    commands_executed=(int *)malloc(sizeof(int));
    *(commands_executed)=0;
    double_flag=(int *)malloc(sizeof(int));
    single_flag=(int *)malloc(sizeof(int));
    *(double_flag)=0;
    *(single_flag)=0;
    for(int i=1;i<argc;i++){
        printf("EXECUTING commands of file%d :\n", i);
        read_command(argv[i]);
        printf("file%d Done\n", i);
    }
    printf("all the files done\n");
    char *input;
    char **command;
    while(1){
        printf("2019ucs0074@iitjammu : ");
        input=(char *)malloc(sizeof(char)*MAX_LEN);
        signal(SIGINT,sigint_handler_main);
        fgets(input,MAX_LEN,stdin);
        int numspaces=spaces(input);
        command=(char **)malloc(sizeof(char *)*(numspaces+2));
        int cnt=tokenizer(input, command);
        if (!strcmp("STOP\0",command[0])){
            if(cnt==1){
                break;
            }
            else{
                printf("Invalid Command.\n");
            }
        }
        else if(!(strcmp("HISTORY\0", command[0]))){
            if(cnt==1){
                printf("Please choose a correct command\n");
                continue;
            }
            if (!(strcmp("BRIEF\0",command[1]))){
                if (*(commands_executed)==0){
                    printf("HISTORY not available.\n");
                    continue;
                }
                for(int i=0;i<*(commands_executed);i++){
                    printf(" %d. %s\n",i+1,((records+i)->command[0]));
                }
            }
            else if (!strcmp("FULL\0",command[1])){
                if (*(commands_executed)==0){
                    printf("HISTORY not available.\n");
                    continue;
                }
                for (int i=0;i<*(commands_executed);i++){
                    printf(" %d.",i+1);
                    for (int j=0;j<records[i].filled;j++){
                        printf(" %s",(records+i)->command[j]);
                    }
                    printf("\n");
                }
            }
            else{
                printf("Please choose a correct command.\n");
            }
        }
        else if (!(strcmp("EXEC\0", command[0]))){
            if (cnt==1){
                printf("Please choose a correct command.\n");
                continue;
            }
            int command_no=atoi(command[1]);
            if(command_no==1){
                if(strlen(command[1])==length(command_no)){
                    command_no=1;
                }
                else{
                    command_no=0;
                }
            }
            if (command_no){
                if (command_no<=*(commands_executed) && command_no>0){
                    executer(records[command_no-1].filled,records[command_no-1].command);
                }
                else{
                    printf("Please give  a valid Command index. .\n");
                }
            }
            else {
                executer(cnt-1,command + 1);
            }
        }
        else{
            for(int i=0;i<cnt-1;i++){
                printf("%s ", command[i]);
            }
            printf("%s",command[cnt-1]);
            printf("\" is not a valid command.\n");
        }
        *(double_flag)=0;
        *(single_flag)=0;
    }
    printf("\"Exiting normally, bye.\"\n");
}