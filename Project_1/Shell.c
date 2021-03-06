#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/wait.h>

#define MAX_INPUT_LEN 100
#define MAX_ENV_VAR_LEN 1024
#define MAX_DIR_LEN 200
#define MAX_ARGS 20

typedef void (*function)(char *curDir, int argc, char ** args);	//For internal functions

int stringParser(char* line, char*** args, char *delimiter);
int findFile(char *filename, char **pathName);
//External functions
void executeExternalCommandSync(char * command, char ** args);
void executeExternalCommandAsync(char * command, char ** args);
//Internal functions
void cd(char *curDir, int argc, char **args);
void export(char *curDir, int argc, char **args);
void unset(char *curDir, int argc, char **args);

int main(int argc, char *argv[]){
	char ** args;	//args list
	char *command, *curDir;
	char *internalCommands[] = {"cd","export","unset",(char *)0};	//List of internal commands
	function functions[] = {cd,export,unset};	//The list of functions (internal)
	char inputBuffer[MAX_INPUT_LEN];
	int i, argcount, isInternal;
	int commandCount = 0;

	//Make current working directory HOME
	chdir(getenv("HOME"));

	while(1){
		//Get Current Working Directory (CWD)
		curDir = getcwd(NULL, MAX_DIR_LEN);	

		do{
			//Read input
			do{
				printf("%s$%d: ", curDir, commandCount);
				gets(inputBuffer);
			}while(strlen(inputBuffer) == strspn(inputBuffer, " \t\n"));
		
			//Parse String
			argcount = stringParser(inputBuffer, &args, " \n\t");
			command = args[0];	//First argument is the command
		}while(command == NULL);

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
				functions[i](curDir,argcount,args);//Execute the internal function
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

int stringParser(char* line, char*** args,char *delimiter) {

	int argc = 0;
	int i;
	char *buffer;
	char *temp;
	
	//Use buffer so line is not ruined
	buffer = (char *) malloc(strlen(line)*sizeof(char*));
	strcpy(buffer,line);
	
	temp = (char *) malloc(strlen(line)*sizeof(char*));
	
	//Allocate space for arguments
	(*args) = (char**) malloc(MAX_ARGS * sizeof(char**));
 
	//Sort strings between delimiters into the args array
	(*args)[argc] = strtok(buffer, delimiter);
	while (((*args)[argc] != NULL) && (argc < MAX_ARGS)){
		//if the end of the term contains the character '\' (ASCII value 92)
		if ((*args)[argc][strlen((*args)[argc])-1] == 92){
			//the next term is assigned to temp
			temp = strtok(NULL, delimiter);
			//if temp is not NULL, change '\' to space, add the content of temp of the end of (*args)[argc]
			if (temp != NULL){
				(*args)[argc][strlen((*args)[argc])-1] = ' ';
				strcat((*args)[argc], temp);
			}
			else {
				++argc;
				(*args)[argc] = temp;
				}
		}
		else {
			++argc;
			(*args)[argc] = strtok(NULL, delimiter);
		}
	}

	//Check for variable dereference ($)
	for(i=0;i<argc;i++){
		if((*args)[i][0] == '$'){
			if(getenv((*args)[i]+1) == NULL){
				printf("%s is Not a valid Variable\n", (*args)[i]+1);
				(*args)[0] = NULL;
			}
			else{
				(*args)[i] = getenv((*args)[i]+1);
			}
		}
	}

	return argc;
}

int findFile(char *fileName, char **pathName){
	char **fraggedPath;
	char *PATH, *dirBuffer;
	struct stat fileStat;
	int pathNum, i;
	
	if(fileName[0] == '.' && fileName[1] == '/'){	//Search current directory
		dirBuffer = getcwd(NULL, MAX_DIR_LEN);
		(*pathName) = dirBuffer;
		strcat((*pathName), &fileName[1]);
		return 1;
	}
	if(fileName[0] == '/'){	//Absolute directory
		(*pathName) = fileName;
		return 1;
	}

	//Get PATH environmental variable
	PATH = getenv("PATH");

	//Parse the PATH string
	pathNum = stringParser(PATH, &fraggedPath, ":");
	
	for(i=0;i<pathNum;i++){
		dirBuffer = (char *) calloc(strlen(fraggedPath[i])+sizeof(char)+strlen(fileName),sizeof(char*));//Allocate memory for buffer

		if(strcmp(fraggedPath[i],".") == 0){		
			//Search current Directory
			strcat(dirBuffer, getcwd(NULL, MAX_DIR_LEN));
			strcat(dirBuffer, "/");
			strcat(dirBuffer, fileName);
		}else{
			//Create the potential directory
			strcat(dirBuffer, fraggedPath[i]);
			strcat(dirBuffer,"/");
			strcat(dirBuffer,fileName);
		}
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

void cd(char *curDir, int argc, char **args){
	if(argc < 2 || strcmp(args[1],"~") == 0){
		if(chdir(getenv("HOME")) != 0){	//Change the current directory
			printf("cd command failed\n");
		}
	}else{
		if(chdir(args[1]) != 0){	//Change the current directory
			printf("cd command failed\n");
		}
	}
	return;
}

void export(char *curDir, int argc, char **args){
	int i;
	char *buffer;
	buffer = (char *) calloc(MAX_ENV_VAR_LEN,sizeof(char *));
	if(argc < 2){
		printf("command failed not enough arguments\n");
	}else{
		for(i=1;i<argc;i++){
			strcat(buffer,args[i]);
		}
		putenv(buffer);
	}
}

void unset(char *curDir, int argc, char **args){
	if(argc < 2){
		printf("command failed not enough arguments\n");
	}else{
		if(getenv(args[1]) == NULL){
			printf("%s is not a variable \n", args[1]);
		}else{
			unsetenv(args[1]);
		}
	}
}

void executeExternalCommandSync(char * command, char ** args){
	int status;
	char *pathName;
	pid_t pid = fork();	//Create new process
	
	//Execute the command in a child process
	if(pid == 0){ //Child Process
		if(findFile(command,&pathName) == 0){	//Convert filename to pathname
			printf("Could not find file %s\n", command);
			exit(0);
		}
		execv(pathName,args);
		perror("Failed to execute command");
		exit(0);
	}
	else if(pid > 0){ //Parent Process
		waitpid(pid,&status,0);
	}
	else{ //Error
		printf("Error creating the child process\n");
	}
	return;
}

void executeExternalCommandAsync(char * command, char ** args){
	char *pathName;	
	pid_t pid = fork();	//Create new process
	//Execute the command in a child process

	if(pid == 0){ //Child Process
		if(findFile(command,&pathName) == 0){	//Convert filename to pathname
			printf("Could not find file %s\n", command);
			exit(0);
		}
		execv(pathName,args);
		perror("Failed to execute command");
		exit(0);
	}
	else if(pid > 0){ //Parent Process
		//Dont wait for the child to finish
	}
	else{ //Error
		printf("Error creating the child process\n");
	}
	return;
}
