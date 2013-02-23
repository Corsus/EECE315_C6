#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>

#define MAX_INPUT_LEN 100
#define MAX_DIR_LEN 200
#define MAX_ARGS 20

typedef void (*function)(char *curDir, char ** args);

int stringParser(char* line, char*** args, char *delimiter);
void executeExternalCommandSync(char * command, char ** args);
void executeExternalCommandAsync(char * command, char ** args);
char* readFromPipe (int file);
void writeToPipe (int file, char* input);
void cd(char *curDir, char **args);

int main(int argc, char *argv[]){
	char ** args;
	char *command, *curDir;
	char *internalCommands[] = {"cd",(char *)0};
	function functions[] = {cd};
	char inputBuffer[MAX_INPUT_LEN];
	int i, argcount, isInternal;
	int commandCount = 0;

	while(1){
		curDir = getcwd(NULL, MAX_DIR_LEN);	

		//Read input
		printf("\n%s$%d: ", curDir, commandCount);
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
				commandCount++;
				functions[i](curDir,args);
				break;
			}
		}
		
		if(isInternal == 0){
			//Execute the External command
			if(args[argcount-1][strlen(args[argcount-1])-1] == '&'){//Execute Asynchronously
				commandCount++;
				args[argcount-1][strlen(args[argcount-1])-1] = '\0';
				executeExternalCommandAsync(args[0],args);
			}else{	//Execute Synchronously
				commandCount++;
				executeExternalCommandSync(command,args);
			}			
		}
		free(args);
	}

	return;
}

char* readFromPipe (int file){
	FILE *stream;
	char *buffer;
	int c,i;
	buffer = (char*) malloc(MAX_DIR_LEN * sizeof(char*));
	stream = fdopen (file, "r");
	for(i=0; ((c = fgetc (stream)) != EOF); i++)
		buffer[i] = c;
	fclose (stream);
	return buffer;
}

void writeToPipe (int file, char* input){
	int i;
	FILE *stream;
	stream = fdopen (file, "w");
	for(i=0;i<strlen(input);i++){
		if(fputc(input[i],stream) == EOF)
			printf("\nPrinting to pipe fail");
	}
	fclose (stream);
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
	char newCurDir[MAX_DIR_LEN];
	char * parentCurDir;
	int dirLen,i,status;
	int myPipe[2];
	pid_t pid;

	if(pipe(myPipe)){
		printf("\nError creating Pipe");
		return;
	}

	pid = fork();

	if(pid == 0){ //Child Process
		dirLen = stringParser(curDir, &fraggedDir, "/");
		if(strcmp("..",args[1]) == 0){	//Going down one folder
			curDir[0] = '\0';
			for(i=0;i<dirLen-1;i++){
				strcat(curDir,"/");
				strcat(curDir,fraggedDir[i]);
			}
			strcat(newCurDir,curDir);
		}
		else if(args[1][0] == '/'){	//absolute path
			strcat(newCurDir,args[1]);
		}
		else{	//relative path
			strcat(newCurDir,curDir);
			strcat(newCurDir,"/");
			strcat(newCurDir,args[1]);
		}
		close(myPipe[0]);
		writeToPipe(myPipe[1],newCurDir);
		exit(0);
	}
	else if(pid > 0){ //Parent Process
		waitpid(pid,&status,0);
		close(myPipe[1]);
		parentCurDir = readFromPipe(myPipe[0]);	
		if(chdir(parentCurDir) != 0){
			printf("\ncd command failed");
		}else{
			printf("\ncd command complete");
		}
	}
	else{ //Error
		printf("\nError creating the child process");
	}
	return;	
}

void executeExternalCommandSync(char * command, char ** args){
	int status;
	pid_t pid = fork();	//Create new process
	//Execute the command in a child process

	if(pid == 0){ //Child Process
		execvp(command,args);
		perror("Failed to execute command");
		exit(0);
	}
	else if(pid > 0){ //Parent Process
		waitpid(pid,&status,0);
		printf("\nCommand: %s complete", command);
	}
	else{ //Error
		printf("\nError creating the child process");
	}
	return;
}

void executeExternalCommandAsync(char * command, char ** args){
	pid_t pid = fork();	//Create new process
	//Execute the command in a child process

	if(pid == 0){ //Child Process
		execvp(command,args);
		perror("Failed to execute command");
		exit(0);
	}
	else if(pid > 0){ //Parent Process
	}
	else{ //Error
		printf("\nError creating the child process");
	}
	return;
}
