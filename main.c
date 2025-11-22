// Header files:
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>
#include<unistd.h>
#include<time.h>

#ifndef PATH_MAX
#define PATH_MAX 4096 
#endif

// function declarations: 
void lsh_loop();
char* lsh_readLine();
char** lsh_parse(char* line);
int lsh_execute(char** args);
int lsh_launch(char** args);

// function definitions:
int main(int argc, char** argv)
{
    // load any config files
    // Run the main loop
    lsh_loop();

    // Perform any shutdown/cleanup
    return EXIT_SUCCESS;
}

void lsh_loop()
{
    char cwd[PATH_MAX];

    // the line we read
    char* line;
    
    // after tokenisation:
    char** args;

    // status of the process:
    int status;

    do
    {
        getcwd(cwd, sizeof(cwd));
        printf("%s>> ",cwd);

        // read line, parse it, and execute
        line = lsh_readLine();
        args = lsh_parse(line);
        status = lsh_execute(args);
    } while (status);

    return;   
}


char* lsh_readLine()
{
    // declare the line we are reading and the buffer size
    char* line = NULL;
    ssize_t buffersize = 0;

    // getline() takes care of most of it
    if(getline(&line, &buffersize, stdin) == -1){
        if(feof(stdin)){
            // if stdin detects an eof
            exit(EXIT_SUCCESS);
        } else {
            perror("readline");
            exit(EXIT_FAILURE);
        }
    }

    return line;
}

// String parser function : 
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
char** lsh_parse(char* line)
{
    // Allocate a buffer size, array of strings, and a string
    int bufsize = LSH_TOK_BUFSIZE;
    char** tokens = malloc(bufsize * sizeof(char*));
    char* token;
    int position = 0; // Current index of args we are writing to 

    if(!tokens){
        fprintf(stderr, "Token array error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIM);

    // Read string into token and pass to args till we reach the end (NULL pointer)
    while(token != NULL){
        tokens[position] = token;
        position++;

        if(position >= bufsize){
            bufsize *= 2;
            tokens = realloc(tokens, bufsize * sizeof(char*));

            if(!tokens){
                fprintf(stderr, "Token array error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, LSH_TOK_DELIM);
    }

    tokens[position] = NULL;
    return tokens;
}

int lsh_launch(char** args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    if(pid == 0){
        // child process runs the command and checks for an error
        if(execvp(args[0], args) == -1){
            perror("lsh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // If there was an error in making the child process
        perror("lsh");
    } else {
        // Parent waits till child processes exit 
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}


/*
    Function declarations for builtin shell commands:
*/

int lsh_cd(char **args);
int lsh_help(char** args);
int lsh_exit(char** args);
int lsh_time(char** args);
int lsh_filestruct(char** args);

char* builtin_str[] = {
    "call",
    "help",
    "gt",
    "filestruct",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_time,
    &lsh_filestruct,
    &lsh_exit
};

int lsh_num_builtins()
{
    return sizeof(builtin_str) / sizeof(char *);
}

/*
    Builtin function implementations
*/
int lsh_cd(char** args){
    if(args[1] == NULL){
        fprintf(stderr, "lsh: expected argument to \"call\"\n");
    } else {
        if(chdir(args[1]) != 0){
            perror("lsh");
        }
    }
    return 1;
}

int lsh_help(char **args)
{
  int i;
  printf("Manel Om Nayak's LSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int lsh_exit(char **args)
{
  return 0;
}


int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

int lsh_time(char** args)
{
    time_t currentTime;
    time(&currentTime);

    currentTime += 19800;
    char** tokens = lsh_parse(ctime(&currentTime));

    printf("Today is: %s, %s %s %s, Time: %s\n",tokens[0], tokens[1], tokens[2], tokens[4], tokens[3]);
    return 1;
}

#define DELIM "/"
int lsh_filestruct(char** args)
{
    char cwd[PATH_MAX];

    getcwd(cwd, sizeof(cwd));

    char* token = strtok(cwd, DELIM);
    char** tokens = malloc(sizeof(char*) * 64);
    int index = 0;
    while(token){
        tokens[index++] = token;
        token = strtok(NULL, DELIM);
    }

    tokens[index] = NULL;



    printf("%s|\n",tokens[0]);
    int count  = 1;
    while(tokens[count] != NULL){
        for(size_t j = 0; j < count; j++) printf("\t");
        printf("--->%s|\n",tokens[count++]);
    }

    return 1;
}
