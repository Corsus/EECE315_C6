#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/wait.h>

#define MAX_INPUT_LEN 100
#define MAX_DIR_LEN 200
#define MAX_ARGS 20

typedef void (*function)(char *curDir, char ** args);	//For internal functions

int stringParser(char* line, char*** args, char *delimiter);
void executeExternalCommandSync(char * command, char ** args);
void executeExternalCommandAsync(char * command, char ** args);
int findFile(char *filename, char **pathName);
char* readFromPipe (int file);
void writeToPipe (int file, char* input);
void cd(char *curDir, char **args);

int main(int argc, char *argv[]){
	char ** args;	//args list
	char *command, *curDir;
	char *internalCommands[] = {"cd",(char *)0};	//List of internal commands
	function functions[] = {cd};	//The list of functions (internal)
	char inputBuffer[MAX_INPUT_LEN];
	int i, argcount, isInternal;
	int commandCount = 0;

	while(1){
		//Get Current Working Directory (CWD)
		curDir = getcwd(NULL, MAX_DIR_LEN);	

		//Read input
		printf("\n%s$%d: ", curDir, commandCount);
		gets(inputBuffer);
		
		//Parse String
		argcount = stringParser(inputBuffer, &args, " ");
		command = args[0];	//First argument is the command

		//Check for quit
		if(strcmp(command,"quit") == 0){
			break;
		}

		//Check for Internal command
		isInternal = 0;
		for(i=0;internalCommands[i]!=(char *)0;i++){	
			if(strcmp(command,internalCommands[i]) == 0){	//compare to the list
				isInternal = 1;	//is an internal command
				commandCount++;
				functions[i](curDir,args);	//Execute the internal function
				break;
			}
		}
		
		if(isInternal == 0){
			//Execute the External command
			if(args[argcount-1][strlen(args[argcount-1])-1] == '&'){//Execute Asynchronously
				commandCount++;
				args[argcount-1][strlen(args[argcount-1])-1] = '\0';//Remove the &
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

int findFile(char *fileName, char **pathName){
	char **fraggedPath;
	char *PATH, *dirBuffer;
	struct stat fileStat;
	int pathNum, i;
	
	//Get PATH environmental variable
	PATH = getenv("PATH");

	//Parse the PATH string
	pathNum = stringParser(PATH, &fraggedPath, ":");

	for(i=0;i<pathNum;i++){
		dirBuffer = (char *) calloc(MAX_DIR_LEN,sizeof(char*));//Allocate memory for buffer
		
		//Create the potential directory
		strcat(dirBuffer, fraggedPath[i]);
		strcat(dirBuffer,"/");
		strcat(dirBuffer,fileName);

		//Check if the directory exists
		if(stat(dirBuffer, &fileStat) >= 0){
			(*pathName) = dirBuffer;
			free(fraggedPath);
			return 1;
		}
		free(dirBuffer);
	}
	return 0;
}

void cd(char *curDir, char **args){
	if(chdir(args[1]) != 0){	//Change the current directory
		printf("\ncd command failed");
	}else{
		printf("\ncd command complete");
	}
	return;	
}

void executeExternalCommandSync(char * command, char ** args){
	int status;
	char *pathName;
	pid_t pid = fork();	//Create new process
	
	//Execute the command in a child process
	if(pid == 0){ //Child Process
		if(findFile(command,&pathName) == 0){	//Convert filename to pathname
			printf("\nCould not find file");
			exit(0);
		}
		execv(pathName,args);
		perror("Failed to execute command");
		exit(0);
	}
	else if(pid > 0){ //Parent Process
		waitpid(pid,&status,0);
		//printf("\nCommand: %s complete", command);
	}
	else{ //Error
		printf("\nError creating the child process");
	}
	return;
}

void executeExternalCommandAsync(char * command, char ** args){
	char *pathName;	
	pid_t pid = fork();	//Create new process
	//Execute the command in a child process

	if(pid == 0){ //Child Process
		if(findFile(command,&pathName) == 0){	//Convert filename to pathname
			printf("\nCould not find file");
			return;
		}
		execv(pathName,args);
		perror("Failed to execute command");
		exit(0);
	}
	else if(pid > 0){ //Parent Process
		//Dont wait for the child to finish
	}
	else{ //Error
		printf("\nError creating the child process");
	}
	return;
}
