/*
 * System Programming Mid Term Project : Make own shell
 * Name : INYEONG KIM
 * Student ID : 2016015878
 * Major : Division of Computer Science (Softerware major)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <grp.h>

#define BUFFER_SIZE 1024
#define MAX_COMMAND_CNT 10
#define MAX_HISTORY_SIZE 10

#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

static void printManual(){
    puts(ANSI_COLOR_MAGENTA "\n\t\t##################################################################");
    puts("\t\t#                         " ANSI_COLOR_YELLOW "INYEONG SHELL" ANSI_COLOR_MAGENTA "                          #");
    puts("\t\t#   ls : Show list of now directory                              #");
    puts("\t\t#   cd : Change directory                                        #");
    puts("\t\t#   ps : Show process state                                      #");
    puts("\t\t#   cat : Print file                                             #");
    puts("\t\t#   chmod : Change mode (chmod [mode] [file])                    #");
    puts("\t\t#   env : Show all enviroments                                   #");
    puts("\t\t#   echo : eeeee~c~h~oooooooo~ (To show env is also allowable)   #");
    puts("\t\t#   mkdir : Make directory                                       #");
    puts("\t\t#   mv : Move File                                               #");
    puts("\t\t#   rm : Remove File                                             #");
    puts("\t\t#   ln : Make Symbolic Link                                      #");
    puts("\t\t#   grep : Find pattern in file                                  #");
    puts("\t\t#   rmdir : Remove directory                                     #");
    puts("\t\t#   history : Show command log (MAX_SIZE = 10)                   #");
    puts("\t\t#   pwd : Print Working directory                                #");
    puts("\t\t#   cls : Claer window                                           #");
    puts("\t\t#   menu(help): Show command menu                                #");
    puts("\t\t#   date : Show date                                             #");
    puts("\t\t#   vi(vim) : Open vim editor                                    #");
    puts("\t\t#   make : Build makefile                                        #");
    puts("\t\t#   man : Show function manual                                   #");
    puts("\t\t#   exit : Terminate shell                                       #");
    puts("\t\t##################################################################\n" ANSI_COLOR_RESET);
}

char history[MAX_HISTORY_SIZE][100];
int historyIndex = -1;

static void die(char *s);
static void make_path(const char *path);

void do_cmd(char *getCommand, char **envp);
void signal_easter_egg(int signum);

int main(int argc, char *argv[], char **envp){
    char getCommand[100];   // raw command
    char *commmandLine;
    char *cwd;

    char *homeDIR = getenv("HOME");

    if (argc>1){
        perror("Argument error");
        exit(1);
    }

    uid_t user_id;
    struct passwd *user_pw;
    char hostname[1024];
    user_id  = getuid();
    user_pw  = getpwuid( user_id );
    gethostname(hostname, 1024);

    // signal(20, (void *)signal_easter_egg); //Please set in here in Ubuntu
    // signal(SIGTSTP, (void *)signal_easter_egg); //set singal easter egg (SIGTSTP)

    system("clear");
    printManual();
    while(1){
        cwd = getcwd(NULL, BUFFER_SIZE);

        printf(ANSI_COLOR_GREEN "%s@%s" ANSI_COLOR_RESET ":", user_pw->pw_name, hostname);
        printf(ANSI_COLOR_CYAN "INYEONG_SHELL %s $ " ANSI_COLOR_RESET, (strcmp(homeDIR, cwd)==0) ? "~" : strrchr(cwd, '/')); // show now DIR
        gets(getCommand);

        if(strcmp(getCommand, "\0")!=0){ //Add on history 
            strcpy(history[(++historyIndex) % MAX_HISTORY_SIZE], getCommand);
            if(historyIndex>9){
                historyIndex %= historyIndex;
            }
        }

        if(strstr(getCommand, "|")!=NULL || strstr(getCommand, ">")!=NULL || strstr(getCommand, "<")!=NULL ){

            /* pipe, redirection found */

            system(getCommand);
        }
        else {   

            /* Do singel command */

            do_cmd(getCommand, envp);
        } 
    }
    exit(0);
}

void do_cmd(char *getCommand, char **envp){
    char *commandArr[MAX_COMMAND_CNT] = { NULL, };
    char *commmandLine = strtok(getCommand, " ");
    int cmdLength = 0; //total command length        

    while (commmandLine != NULL){
        commandArr[cmdLength] = commmandLine;
        cmdLength++;
        commmandLine = strtok(NULL, " ");
    }
    
    // have a command line
    if(commandArr[0]!=NULL){

        if (strcmp(commandArr[0], "exit")==0){ /* exit */
            puts(ANSI_COLOR_YELLOW "Bye!" ANSI_COLOR_RESET);
            exit(0);
        }
        
        else if(strcmp(commandArr[0], "cd")==0){ /* cd */
            if(cmdLength==1){
                chdir(getenv("HOME"));
            }
            else if(cmdLength==2){
                if(chdir(commandArr[1])){
                    printf(ANSI_COLOR_YELLOW "cd: %s: No such file or directory\n" ANSI_COLOR_YELLOW, commandArr[1]);
                }
            }
            else{
                printf(ANSI_COLOR_YELLOW "ERROR: Put correct form!\n");
                puts("cd [dir]" ANSI_COLOR_YELLOW);
            }
        }

        else if(strcmp(commandArr[0], "ls")==0){ /* ls */
            
            pid_t pid = fork();
            int status;
            switch (pid){
                case -1:
                    perror("FORK ERROR");
                    exit(1);
                    break;
                case 0:
                    for(int i=0; commandArr[i]!=NULL; i++) ;

                    char **newcommandArr = (char **)malloc(sizeof(char *)*(cmdLength));

                    for(int idx=0; idx<cmdLength; idx++){
                        newcommandArr[idx] = commandArr[idx];
                    }
                    newcommandArr[cmdLength] = NULL;
                    if(execvp("ls", newcommandArr)==-1){
                        printf("ERROR");
                        exit(1);
                    }
                    exit(3);  
                    break;

                default:
                    wait(&status);
                    break;
            }

        }

        else if(strcmp(commandArr[0], "history")==0){ /* history */    
            int historyStart = (historyIndex%MAX_HISTORY_SIZE);

            for(int i=historyStart+1; i<=historyStart+MAX_HISTORY_SIZE; i++){
                if(strcmp(history[i%MAX_HISTORY_SIZE], "") != 0){
                    printf("%s\n", history[i%MAX_HISTORY_SIZE]);
                }
            }
        }
        
        else if(strcmp(commandArr[0], "man")==0){ /* man */
            if(cmdLength!=2){
                puts(ANSI_COLOR_YELLOW "ERROR : make [func]" ANSI_COLOR_RESET);
            }
            else{
                pid_t pid = fork();
                int status;
                switch (pid){
                    case -1:
                        perror("FORK ERROR");
                        exit(1);
                        break;
                    case 0:
                        for(int i=0; commandArr[i]!=NULL; i++) ;
                        char **newcommandArr = (char **)malloc(sizeof(char *)*(cmdLength));

                        for(int idx=0; idx<cmdLength; idx++){
                            newcommandArr[idx] = commandArr[idx];
                        }
                        newcommandArr[cmdLength] = NULL;
                        if(execvp("man", newcommandArr)==-1){
                            printf("ERROR");
                            exit(1);
                        }
                        exit(3);
                        break;

                    default:
                        wait(&status);
                        break;
                }
            }
        }

        else if(strcmp(commandArr[0], "pwd")==0){ /* pwd */
            puts(getcwd(NULL, BUFFER_SIZE));
        }

        else if(strcmp(commandArr[0], "chmod")==0){ /* chmod */
            if(cmdLength!=2){
                puts(ANSI_COLOR_YELLOW "ERROR : chmod [mode] [file]" ANSI_COLOR_RESET);
            }else{
                int mode = strtol(commandArr[1], NULL, 8);

                for(int i=2; i<cmdLength; i++){
                    if(chmod(commandArr[i], mode) < 0){
                        printf("%s : chmod ERROR\n", commandArr[i]);   
                    }
                }
            }
        }

        else if(strcmp(commandArr[0], "rm")==0){ /* rm */
            pid_t pid = fork();
            int status;
            switch (pid){
                case -1:
                    perror("FORK ERROR");
                    exit(1);
                    break;
                case 0:
                    for(int i=0; commandArr[i]!=NULL; i++){

                    }
                    char **newcommandArr = (char **)malloc(sizeof(char *)*(cmdLength));

                    for(int idx=0; idx<cmdLength; idx++){
                        newcommandArr[idx] = commandArr[idx];
                    }
                    newcommandArr[cmdLength] = NULL;
                    if(execvp("rm", newcommandArr)==-1){
                        printf("ERROR");
                        exit(1);
                    }
                    exit(3);  
                    break;

                default:
                    wait(&status); 
                    break;
            }
        }

        else if(strcmp(commandArr[0], "echo")==0){ /* echo */
            if(cmdLength>1){
                if(commandArr[1][0]=='$'){ //show env
                    char *echoMsg;
                    echoMsg = (char *)malloc(strlen(commandArr[0])+strlen(commandArr[1])+2);
                    strcat(echoMsg, "echo ");
                    strcat(echoMsg, commandArr[1]);
                    system(echoMsg);
                }
                else{
                    for(int i=1; commandArr[i]!=NULL; i++){
                        printf("%s ", commandArr[i]);
                    }
                    printf("\n");
                }
            }
            else if(cmdLength==1){
                puts(ANSI_COLOR_YELLOW "eeeee~c~h~oooooooo~" ANSI_COLOR_RESET);
            }
        }

        else if(strcmp(commandArr[0], "ps")==0){ /* ps */
            
            pid_t pid = fork();
            int status;
            switch (pid){
                case -1:
                    perror("FORK ERROR");
                    exit(1);
                    break;
                case 0:
                    for(int i=0; commandArr[i]!=NULL; i++) ;

                    char **newcommandArr = (char **)malloc(sizeof(char *)*(cmdLength));

                    for(int idx=0; idx<cmdLength; idx++){
                        newcommandArr[idx] = commandArr[idx];
                    }
                    newcommandArr[cmdLength] = NULL;
                    if(execvp("ps", newcommandArr)==-1){
                        printf("ERROR");
                        exit(1);
                    }
                    exit(3);  
                    break;

                default:
                    wait(&status); 
                    break;
            }

        }
        
        else if(strcmp(commandArr[0], "grep")==0){ /* grep */
            pid_t pid = fork();
            int status;
            switch (pid){
                case -1:
                    perror("FORK ERROR");
                    exit(1);
                    break;
                case 0:
                    for(int i=0; commandArr[i]!=NULL; i++) ;

                    char **newcommandArr = (char **)malloc(sizeof(char *)*(cmdLength));

                    for(int idx=0; idx<cmdLength; idx++){
                        newcommandArr[idx] = commandArr[idx];
                    }
                    newcommandArr[cmdLength] = NULL;
                    if(execvp("grep", newcommandArr)==-1){
                        printf("ERROR");
                        exit(1);
                    }
                    exit(3);  
                    break;

                default:
                    wait(&status); 
                    break;
            }
        }

        else if(strcmp(commandArr[0], "env")==0){ /* env */
            if(cmdLength!=1){
                puts(ANSI_COLOR_YELLOW "Please put just command : env" ANSI_COLOR_RESET);
            }
            else{
                char **env = envp;
                while(*env){
                    printf("%s\n", *env);
                    env++;
                }
            }
        }

        else if(strcmp(commandArr[0], "cat")==0){ /* cat */
            FILE *f;
            int c;
            for(int i=1; commandArr[i]!=NULL; i++){
                f = fopen(commandArr[i], "r");
                if(!f){
                    printf(ANSI_COLOR_YELLOW "%s: No such file or directory\n" ANSI_COLOR_RESET, commandArr[i]);
                    break;
                }
                while((c=fgetc(f)) != EOF){
                    if(putchar(c) < 0 ){
                        puts("EOF ERROR!");
                        break;
                    }
                }
            }
        }

        else if(strcmp(commandArr[0], "mkdir")==0){ /* mkdir */
            for(int i=1; commandArr[i]!=NULL; i++){
                make_path(commandArr[i]);
            } 
        }

        else if(strcmp(commandArr[0], "rmdir")==0){ /* rmdir */
            for(int i=1; commandArr[i]!=NULL; i++){
                if(rmdir(commandArr[i]) < 0){
                    printf(ANSI_COLOR_YELLOW "%s: rmdir ERROR\n" ANSI_COLOR_RESET, commandArr[i]);
                    break;
                }
            }
        }

        else if(strcmp(commandArr[0], "cls")==0){ /* cls */
            system("clear");
        }

        else if(strcmp(commandArr[0], "menu")==0 || strcmp(commandArr[0], "help")==0){ /* menu */
            printManual();
        }

        else if(strcmp(commandArr[0], "mv")==0){ /* mv */
            if(cmdLength!=3){
                puts(ANSI_COLOR_YELLOW "Put correct form!\nmv [file] [path/file]" ANSI_COLOR_RESET);
            }

            if(rename(commandArr[1], commandArr[2])<0){
                printf("ERROR[%s] : mv [file] [path/file]\n", commandArr[2]);
            }
        }

        else if(strcmp(commandArr[0], "ln")==0){ /* ln */
            if(cmdLength!=3){
                puts(ANSI_COLOR_YELLOW "Put correct form!\nln [file] [path/file]" ANSI_COLOR_RESET);
            }

            if(link(commandArr[1], commandArr[2])<0){
                printf("ERROR[%s] : ln [file] [path/file]\n", commandArr[2]);
            }
        }

        else if(strcmp(commandArr[0], "vi")==0 || strcmp(commandArr[0], "vim")==0){ /* vi */
            if(cmdLength!=2){
                puts(ANSI_COLOR_YELLOW "Put correct form!\nvi [file]" ANSI_COLOR_RESET);
            }
            else{
                pid_t pid = fork();
                int status;
                switch (pid){
                case -1:
                    perror("FORK ERROR");
                    exit(1);
                    break;
                case 0:
                    for(int i=0; commandArr[i]!=NULL; i++) ;
                    
                    char **newcommandArr = (char **)malloc(sizeof(char *)*(cmdLength));

                    for(int idx=0; idx<cmdLength; idx++){
                        newcommandArr[idx] = commandArr[idx];
                    }
                    newcommandArr[cmdLength] = NULL;
                    if(execvp("vi", newcommandArr)==-1){
                        printf("ERROR");
                        exit(1);
                    }
                    exit(3);  
                    break;

                default:
                    wait(&status); 
                    break;
                }
            }
        }

        else if(strcmp(commandArr[0], "make")==0){ /* make */
            pid_t pid = fork();
                int status;
                switch (pid){
                case -1:
                    perror("FORK ERROR");
                    exit(1);
                    break;
                case 0:
                    for(int i=0; commandArr[i]!=NULL; i++) ;
                    
                    char **newcommandArr = (char **)malloc(sizeof(char *)*(cmdLength));

                    for(int idx=0; idx<cmdLength; idx++){
                        newcommandArr[idx] = commandArr[idx];
                    }
                    newcommandArr[cmdLength] = NULL;
                    if(execvp("make", newcommandArr)==-1){
                        printf("ERROR");
                        exit(1);
                    }
                    exit(3);
                    break;

                default:
                    wait(&status); 
                    break;
                }
        }

        else if(strcmp(commandArr[0], "date")==0){ /* date */
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            
            printf("now: %d-%d-%d %d:%d:%d\n",tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,tm.tm_hour, tm.tm_min, tm.tm_sec);
        }

        else{ /* Wrong Command */
            puts(ANSI_COLOR_YELLOW "Please put correct command!" ANSI_COLOR_RESET);
        }
        
    } 
}

static void die(char *s){
    perror(s);
    exit(1);
}

static void make_path(const char *path){

    if(mkdir(path, 0777)==0){
	    return;
    }
    
    if (errno == EEXIST) {
        struct stat st;
        if (stat(path, &st) < 0){
            perror("stat");
            exit(1);
        }
        
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "file exists but is not a directory: %s\n", path);
            exit(1);
        }
        return;
    }
    else if (errno == ENOENT) {
        char *parent_path = strdup(path);
        if (!parent_path){
            perror("stat");
            exit(1);
        }

        char *last = parent_path + strlen(parent_path) - 1;
        while (*last == '/' && last != parent_path) {
            *last-- = '\0';
        }

        if (strcmp(parent_path, "/") == 0) {
            fprintf(stderr, "error: root directory is not a directory???\n");
            exit(1);
        }

        char *sep = strrchr(parent_path, '/');
        if (!sep) {
            fprintf(stderr, "error: current directory is not a directory???\n");
            exit(1);
        }
        else if (sep == parent_path) {
            fprintf(stderr, "error: root directory is not a directory???\n");
            exit(1);
        }
        *sep = '\0';

        make_path(parent_path);
        mkdir(path, 0777);
        return;
    }

    else {
        perror(path);
        exit(1);
    }
}

void signal_easter_egg(int signum){
    puts(ANSI_COLOR_YELLOW "\nHummm... There is no break in here... (Press Enter)" ANSI_COLOR_RESET);
}
