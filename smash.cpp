/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands.h"
#include "signals.h"
#define MAX_LINE_SIZE 80
#define MAXARGS 20

//char* L_Fg_Cmd;
void* jobs = NULL; //This represents the list of jobs. Please change to a preferred type (e.g array of char*)
char lineSize[MAX_LINE_SIZE]; 
//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
    char cmdString[MAX_LINE_SIZE]; 	   

	
	//signal declaretions
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	if (signal(SIGTSTP, ctrlZHandler) == SIG_ERR) {
		perror("smash error: failed to set ctrl-Z handler");
	}
	if (signal(SIGINT, ctrlCHandler) == SIG_ERR) {
		perror("smash error: failed to set ctrl-C handler");
	}
	/************************************/
	// Init globals 
	SmallShell smash;

	
	//L_Fg_Cmd =(char*)malloc(sizeof(char)*(MAX_LINE_SIZE+1));
	//if (L_Fg_Cmd == NULL) 
	//		exit (-1); 
	//L_Fg_Cmd[0] = '\0';
	
    	while (1)
    	{
	 	printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize); 
		cmdString[strlen(lineSize)-1]='\0';
		/*
					// perform a complicated Command
		if (!ExeComp(lineSize))
		{
			smash.history->addHistory(cmdString);
			continue;
		}
		*/
					// background command	
		//if (!BgCmd(&smash, jobs, lineSize, cmdString)) //FIXME - necessary?
		//{
		//	smash.history->addHistory(cmdString);
		//	continue;
		//}
					// built in commands
		ExeCmd(&smash, jobs, lineSize, cmdString);
		//if (strlen(cmdString) != 0) { //not adding empty commands
		//	smash.history->addHistory(cmdString);
		//}
		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdString[0]='\0';
	}
    return 0;
}

