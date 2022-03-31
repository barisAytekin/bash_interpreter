 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <string.h>
 #include <fcntl.h>
 #include <sys/wait.h>

 char* escape(char* str){
   int len = strlen(str);
   char* strfinal = (char*)malloc(sizeof(char)*(len-1));
   int i;
   for(i = 1; i < len - 1; i++) {
       strfinal[i - 1] = str[i];
   }
   strfinal[len - 2] = '\0';
   return strfinal;
  } 


int main(int argc, char *argv[]) {
    FILE* filename;
    int storage[100];
    int s_size = 0;
    filename = fopen("commands.txt", "r");
    char temp[100];
    int i = 0;
    while(fgets(temp, 100, filename) != NULL) {
        int j = 0;
        char* command = "";
        char* input = "";
        char* options = "-";
        char* redirection = "-";
        char* back = "n";
        char* token;
        char* arglist [10];
        char* newstr;
        temp[strcspn(temp, "\r\n")] = 0;
        token = strtok(temp, " ");
        command = strdup(token);
        do {
            token[strcspn(token, "\r\n")] = 0; 
            if (token[0] == '\"') {
                newstr = escape(token);
                arglist[j] = strdup(newstr);
                free(newstr);
            }
            else {
                arglist[j] = strdup(token);
            }
            j++;
            if (j == 2 && token[0] != '&' && token[0] != '-') {
                input = strdup(token);
            }
            if (token[0] == '-') {
                options = strdup(token);
            }
            if (token[0] == '>' || token[0] == '<') {
                redirection = strdup(token);
            }
            if (token[0] == '&') {
                back = "y";
            }
            token = strtok(NULL, " ");
        }while(token != NULL );
        printf("----------\nCommand: %s\nInput: %s\nOptions: %s\nRedirection: %s\nBackground job: %s\n----------\n", command, input, options, redirection, back);
        fflush(stdout);
        if (strcmp(command, "wait")) {
            int fork_state = fork();
            if (fork_state == 0) {
                int args_size = 0;
                if (strcmp(redirection, "-")) {
                    args_size = j - 2;
                }
                else {
                    args_size = j;
                }
                if(!strcmp(back, "y")) {
                    args_size--;
                }
                char** args = (char**) malloc((args_size+1)*sizeof(char*));
                for (i = 0; i < args_size; i++) {
                    args[i] = arglist[i];
                }
                args[args_size] = NULL;
                if (!strcmp(redirection, "<")) {
                    close(STDIN_FILENO);
                    char bar[] = "./";
                    if (!strcmp(back, "y")) {
                        strcat(bar, arglist[j-2]);
                    }
                    else {
                        strcat(bar, arglist[j-1]);
                    }
                    open(bar, O_RDONLY);
                }
                if (!strcmp(redirection, ">")) {
                    close(STDOUT_FILENO);
                    char bar[] = "./";
                    if (!strcmp(back, "y")) {
                        strcat(bar, arglist[j-2]);
                    }
                    else {
                        strcat(bar, arglist[j-1]);
                    }
                    open(bar, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
                }
                execvp(args[0], args); 
                i = 0;
                for (i; i < j; i++) {
                    free(arglist[i]);
                    arglist[i] = NULL;
                }
                free(args);
                if (strcmp(command, "")) {
                    free(command);
                }
                if (strcmp(input, "")) {
                    free(input);
                }
                if (strcmp(options, "-")) {
                    free(options);
                }
                if (strcmp(redirection, "-")) {
                    free(redirection);
                }
            } 
            else {
                if (!strcmp(back, "n")) {
                    wait(NULL);
                }
                else {
                    storage[s_size]= fork_state;
                    s_size++;
                }
            }
        }
        else {
            int k;
            for (k = 0; k < s_size; k++) {
                waitpid(storage[k],NULL, 0);
            }
            s_size = 0;
        }
    }
    fclose(filename);
    return 0;
}
