#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

char **pathlist;
int numpath = 1;

int access_path(char* path_command, char* directory ){
   
   if(strcmp(path_command, "add") == 0){
   	char** temp = malloc(++numpath*sizeof(char*));
	*temp = strdup(directory);

	for(int i = 1; i < numpath; i++){
	  * (temp + i) = strdup(*(pathlist + i - 1));
	}
	
	pathlist = temp;
	return 0;	
   }else if(strcmp(path_command, "remove") == 0){
   	int access = 0;
	char **rm_pathlist = malloc(--numpath*sizeof(char*));
	for(int i = 0; i < numpath+1; i++ ){
	    if(strcmp(pathlist[i], directory)==0){
		access = 1;
		printf("accessed");
	        for(int j = 0; j < numpath; j++){
	            if(i>j){
		        rm_pathlist[j] = pathlist[j];
		    }else{
		        rm_pathlist[j] = pathlist[j+1];
		    }	    
	        }
	    }
	}
	if(!access){
	    printf("%s want to remove %s", pathlist[1], directory);
	    return -1;
	}else{
       	    pathlist = rm_pathlist;
	    return 0;
	}	   

   }else if(strcmp(path_command, "clear") == 0 && directory == NULL){
   	numpath = 0;
        pathlist = realloc(pathlist, numpath*sizeof(char*));
	return 0;
   }
   return -1;

}

void exe_command(char *commands[], int size){
   //printf("executing");
  
   //for(int i = 0; i < size - 1; i ++){
   //    if(commands[i]!=NULL){
   //      printf("%s\n", commands[i] );
   //    }
   //}
  
   int rc = fork();
   if(rc ==0){
      // printf("%d", getpid());
       for(int i = 0; i < numpath; i++){
 	   if(access(*(pathlist+i), X_OK) == 0 ){
               char* path = malloc(strlen(pathlist[i]) + strlen(commands[0]));
	       path = strcat(path,(*(pathlist+i)));
	       path = strcat(path, "/");
	       path = strcat(path, *commands);
	       execv(path, commands);
	       break;
	   }
       }
      // int j = execv("/bin/ls", arg);

   }else{
       wait(NULL);
   }


}

int handle_command(char *user_input){
     char* args[10];
     int stop = 0;
     int length = strlen(user_input);
             user_input[length-1] = 0;
             char *sep_input = strsep(&user_input, " ");


             if(strcmp(sep_input, "exit")== 0){//built-in call exit
                 stop = 1;
             } else if(strcmp(sep_input, "path") == 0 ){//built in call path
                 sep_input = strsep(&user_input, " " );
                 if(sep_input != NULL){
                     char* dir_name = strsep(&user_input, " ");

                     int i = access_path(sep_input, dir_name );
                     if(i < 0 ){
                        fprintf(stderr, "Error locating  path");
                     }
                 }
             } else if(strcmp(sep_input, "cd") == 0) {//built-in call cd
                 char* directory = strsep(&user_input, " ");
                 if(directory == NULL|| strsep(&user_input, " ") != NULL){
                     fprintf(stderr, "Needs one argument");
                 }else{
                     int j = chdir(directory);
                     if(j != 0){
                         fprintf(stderr, "No such directory");
                     }
                 }

             }else{//execute command
	//	if(strstr(sep_input, "&")!= NULL || strstr(user_input,"&")!=NULL ){
	//	    printf("Parallel Command !");

	//	}
 
  	        int count = 0;
   	        args[0] = sep_input;
   	        while(args[count] != NULL){
               	   count++;
           	   args[count] = strsep(&user_input, " ");
   	        }
   	        char *command[count + 1];
   	        for(int i = 0; i < count; i++){
           	   command[i] = args[i];
                   args[i]= NULL;
    	         }
  	      command[count] = NULL;
  	      exe_command(command, count + 1);
	}
    return stop;
}


int main(int argc, char *argv[]){
   // char *path = "/usr/bin";
   pathlist  = malloc(1*sizeof(char*));
    *pathlist = "/bin";   
    if(argc > 2){
    	fprintf(stderr, "Usage: %s <file> or NULL \n", argv[0]);
	exit(1);
    }
    int stop = 0;
    char *user_input;
    size_t input_size = 0;
    if(argc == 2){
        FILE *f_exe = fopen(argv[1], "r");
        if(f_exe == NULL){
	    printf("cannot open file");
	    exit(1);
	}	
	while(!feof(f_exe)&&!stop){
           getline(&user_input, &input_size, f_exe);
	   stop = handle_command(user_input);
	   user_input = NULL;
	}
	fclose(f_exe);
    }else{// if no file is specified, open user input
   	
   	 while(!stop){//loop until type exit
             printf("smash> ");
	//     for(int i = 0; i < numpath; i++){
	//         printf("%s", pathlist[i] );
	//     }
	     getline(&user_input, &input_size, stdin);
	     stop = handle_command(user_input);
	     printf("%d", stop);
	     user_input = NULL;
       	 }
    }

    free(user_input);
    exit(0);

}



