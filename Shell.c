#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>

#define MAX_INPUT_LEN 100
#define MAX_ARGS 20

typedef void (*function)(char *curDir, char ** args);

int stringParser(char* line, char*** args, char *delimiter);
void executeExternalCommand(char * command, char ** args);
void cd(char *command, char **args);

int main(int argc, char *argv[]){
	char ** args;
	char *command, *curDir;
	char *internalCommands[] = {"cd",(char *)0};
	function functions[] = {cd};
	char inputBuffer[MAX_INPUT_LEN];
	int i, argcount, isInternal;

	curDir = getenv("PWD");

	while(1){
		//Read input
		printf("\n%s$: ", curDir);
		gets(inputBuffer);
		
		//Parse String
		argcount = stringParser(inputBuffer, &args, " ");
		command = args[0];

		//Check for quit
		if(strcmp(command,"quit") == 0){
			break;
		}

		//printf("\nthe command is: %s", command);
		//printf("\nThe arguments are: ");
		//for(i=0;i<argcount;i++){
		//	printf("\narg%d: %s", i, args[i]);
		//}
		
		//Check for Internal command
		isInternal = 0;
		for(i=0;internalCommands[i]!=(char *)0;i++){
			if(strcmp(command,internalCommands[i]) == 0){
				isInternal = 1;
				functions[i](curDir,args);
				break;
			}
		}
		
		if(isInternal == 0){
			//Execute the External command
			executeExternalCommand(command,args);
		}
		free(args);
	}

	return;
}

int stringParser(char* line, char*** args,char *delimiter) {

	int argc = 0;
	char *buffer;

	//Use buffer so line is not ruined
	buffer = (char *) malloc(strlen(line)*sizeof(char*));
	strcpy(buffer,line);
	
	//Allocate space for arguments
	(*args) = (char**) malloc(MAX_ARGS * sizeof(char**));
 
	//Sort strings between delimiters into the args array
	(*args)[argc++] = strtok(buffer, delimiter);
	while ((((*args)[argc] = strtok(NULL, delimiter)) != NULL) && (argc < MAX_ARGS))
		++argc;

	return argc;
}

void cd(char *curDir, char **args){
	char ** fraggedDir;
	char forwardSlash[] = "/";
	int dirLen,i;
	pid_t pid = fork();

	if(pid == 0){ //Child Process
		dirLen = stringParser(curDir, &fraggedDir, "/");
		for(i=0;i<dirLen;i++){
			printf("\ndir%d: %s", i, fraggedDir[i]);
		}
		if(strcmp("..",args[1]) == 0){
			curDir[0] = '\0';
			for(i=0;i<dirLen-1;i++){
				strcat(curDir,"/");
				strcat(curDir,fraggedDir[i]);
			}
			printf("\nresult path: %s", curDir);
		}
		exit(0);
	}
	else if(pid > 0){ //Parent Process
		wait((int *)0);
		printf("\ncd command complete");
	}
	else{ //Error
		printf("\nError creating the child process");
	}
	return;	
}

void executeExternalCommand(char * command, char ** args){
	pid_t pid = fork();	//Create new process
	//Execute the command in a child process

	if(pid == 0){ //Child Process
		execvp(command,args);
		perror("Failed to execute command");
	}
	else if(pid > 0){ //Parent Process
		wait((int *)0);
		printf("\nCommand: %s complete", command);
	}
	else{ //Error
		printf("\nError creating the child process");
	}
	return;
}
