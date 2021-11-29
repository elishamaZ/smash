//		commands.c
//********************************************
#include "commands.h"
//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure	
//**************************************************************************************
using namespace std;
int ExeCmd(SmallShell* smash, void* jobs, char* lineSize, char* cmdString)
{
	char* cmd; 
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE];
	char* delimiters = " \t\n";  
	int i = 0, num_arg = 0;
	bool illegal_cmd = false; // illegal command
    	cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0; 
   	args[0] = cmd;
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters); 
		if (args[i] != NULL) 
			num_arg++; 
 
	}
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
	if (!strcmp(cmd, "cd") ) 
	{
		char* plastPwd = getcwd(NULL, 0);
		if (num_arg > 1) { //check if there is more than 1 arg - "cd" uses 1 arg
			std::cerr << "smash error: cd: too many arguments" << endl; 
			return 1;
		}
		if (!strcmp(args[1], "-")) { // "cd -" mode
			if (!smash->previousPath) { //previousPath is not set yet
				std::cerr << "smash error: cd: There is NO previous path" << endl; 
				return 1;
			}
			else { //previousPath is set 
				if (chdir(smash->previousPath) == -1) {
					std::cerr << "smash error:> " << '"' << args[1] << '"' << " - No such file or directory" << endl;
					return 1;
				}
				else { //print 
					std::cout << getcwd(NULL, 0) << endl;
				}
			}
		}
		else { // not in "cd -" mode
			if (chdir(args[1]) == -1) {
				std::cerr << "smash error:> " << '"' << args[1] << '"' << " - No such file or directory" << endl;
				return 1;
			}
		}
		smash->previousPath = plastPwd;
		
	} 
	
	/*************************************************/
	else if (!strcmp(cmd, "pwd")) 
	{
		if (num_arg > 0) { //check if there is more than 0 arg - "pwd" doesn't use any arg
			std::cerr << "smash error: pwd: too many arguments" << endl;
			return 1;
		}
		if (getcwd(pwd, MAX_LINE_SIZE) == NULL)//getcwd() fail
			std::cout << "getcwd() error" << endl;
		else
			std::cout << pwd << endl;
	}
	
	/*************************************************/
	else if (!strcmp(cmd, "history"))
	{
		if (num_arg > 0) { //check if there is more than 0 arg - "history" doesn't use any arg
			std::cerr << "smash error: history: too many arguments" << endl;
			return 1;
		}
		smash->history->printHistoryList();
	}
	/*************************************************/
	
	else if (!strcmp(cmd, "jobs")) 
	{
		if (num_arg > 0) { //check if there is more than 0 arg - "jobs" doesn't use any arg
			std::cerr << "smash error: jobs: too many arguments" << endl;
			return 1;
		}
		smash->jobs->printJobsList();
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid")) 
	{
		if (num_arg > 0) { //check if there is more than 0 arg - "showpid" doesn't use any arg
			std::cerr << "smash error: showpid: too many arguments" << endl;
			return 1;
		}
		std::cout << "smash pid is " << smash->smashPID << endl;
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg")) 
	{
		if (num_arg > 1)
		{
			std::cerr << "smash error: fg: too many arguments" << endl;
			return 1;
		}
		int jobIdToFG;
		if (num_arg == 0) { // command: "fg "
			if (smash->jobs->jobsList->empty()) {
				std::cerr << "smash error: fg: jobs list is empty" << endl;
				return 1;
			}
			jobIdToFG = smash->jobs->findCurrMaxJobID();
		}
		else { //command: "fg [command number]"
			try {
				jobIdToFG = stoi(args[1]);
			}
			catch (invalid_argument& e) {
				std::cerr << "smash error: bg: invalid arguments" << endl;
				return 1;
			}
			}

		JobsList::JobItem* job = smash->jobs->getJobById(jobIdToFG);
		if (!job) {
			std::cerr << "smash error: fg: " << "job-id " << jobIdToFG << " does not exist" << endl;
			return 1;
		}
		std::cout << job->cmd << endl;
		if (job->stopped) {
			if (kill(job->PID, SIGCONT) == -1) {
				perror("smash error: kill failed");
				return 1;
			}
		}
		job->stopped = false;
		smash->currentPIDRunning = job->PID;
		strcpy(smash->currentCmdRunning,  job->cmd.c_str());
		int status = -1;
		if (waitpid(job->PID, &status, WUNTRACED) == -1) {
			perror("smash error: waitpid failed");
			return 1;
		}
		if (!WIFSTOPPED(status))
			smash->jobs->removeJobById(job->jobID);
	} 
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
		if (num_arg > 1) { 
			std::cout << "smash error: bg: invalid arguments" << endl;
			return 1;
		}
		int jobIdToCont;
		if (smash->jobs->jobsList->empty()) {
			std::cerr << "smash error: bg: jobs list is empty" << endl;
			return 1;
		}
		if (num_arg == 0) { // command: "bg "
			jobIdToCont = smash->jobs->findMaxStoppedJobID();
			if (jobIdToCont == 0) {
				std::cout << "smash error: bg: there is no stopped jobs to resume" << endl;
				return 1;
			}
		}
		else { //command: "bg [command number]"
			try {
				jobIdToCont = stoi(args[1]);
			}
			catch (invalid_argument& e) {
				std::cerr << "smash error: bg: invalid arguments" << endl;
				return 1;
			}
		}
		JobsList::JobItem* job = smash->jobs->getJobById(jobIdToCont);
		if (!job) {
			std::cerr << "smash error: bg: job-id " << jobIdToCont << " does not exist" << endl;
			return 1;
		}
		if (!job->stopped) {
			std::cerr << "smash error: bg: job-id " << jobIdToCont << " is already running in the background" << endl;
			return 1;
		}	
		if (kill(job->PID, SIGCONT) == -1)
		{
			perror("smash error: kill failed");
			return 1;
		}
		job->stopped = false;
		std::cout << job->cmd << endl;
	}
  		
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
		if (num_arg == 0)// only quit
		{
			exit(0);
		}
		else //command: "quit kill"
		{
			string second_arg = args[1];
			if (second_arg.compare("kill") == 0) {
				smash->jobs->removeFinishedJobs();
				//std::cout << "smash: sending SIGKILL signal to " << smash->jobs->jobsList->size() << " jobs:" << endl;
				smash->jobs->KillAllJobs();
				exit(0);
			}
		}
   		
	} 
	else if (!strcmp(cmd, "diff"))
	{
		if (num_arg == 2)
		{
			char* file1 = args[1];
			char* file2 = args[2];
			ifstream t1;
			t1.open(file1);
			if (!t1)
			{ 
				perror("smash error: > ");
				return 1;
			}
			string str1((istreambuf_iterator<char>(t1)),istreambuf_iterator<char>());
			ifstream t2;
			t2.open(file2);
			if (!t2)
			{
				perror("smash error: > ");
				return 1;
			}
			string str2((istreambuf_iterator<char>(t2)), istreambuf_iterator<char>());
			if (str1 == str2)
				cout << 1<< "\n";
			else
				cout << 0 << "\n";
			t2.close();
			t1.close();
		}
		else // num_arg != 2
		{
			cerr << "Error - Number of args is not equal to 2";
		}

	}
	/*************************************************/
	else if(!(lineSize[strlen(lineSize) - 2] == '&')) // external command
	{
 		
	 	return ExeExternal(smash, args, cmdString, false);
	}
	if (illegal_cmd == true)
	{
		printf("smash error: > \"%s\"\n", cmdString);
		return 1;
	}
    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
int ExeExternal(SmallShell* smash, char* args[MAX_ARG], char* cmdString, bool isBackgroundCmd)
{
	int pID;
	string t  = string("/bin/") + string(args[0]);
	switch (pID = fork())
	{
	case -1: {
		cerr << "smash error: fork failed" << endl;
		return 1;
		break;
	}
	case 0: { //child process
		setpgrp();
		if (execv(t.c_str(), args) == -1) {
		cerr << "smash error: execv failed" << endl;
		kill(pID, SIGKILL);
		return 1;
		}
		break;
	}
	default: { //parent process
		if (isBackgroundCmd) {
			smash->jobs->addJob(cmdString, pID, false);
			cout << "here" << endl;
		}
		else {
			smash->currentPIDRunning = pID;
			strcpy(smash->currentCmdRunning, args[0]);
			if (waitpid(pID, nullptr, WUNTRACED) == -1) {
				cerr << "smash error: waitpid failed" << endl;
				return 1;
			}
		}
	}
	}
	
	return 0;
}
//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char* lineSize)
{
	char ExtCmd[MAX_LINE_SIZE+2];
	char *args[MAX_ARG];
    if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&")))
    {
		// Add your code here (execute a complicated command)
					
		/* 
		your code
		*/
	} 
	return -1;
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(SmallShell* smash, char* lineSize, void* jobs)
{

	char* cmd;
	char* delimiters = " \t\n";
	char *args[MAX_ARG];
	if (lineSize[strlen(lineSize)-2] == '&')
	{
		lineSize[strlen(lineSize)-2] = '\0';
		char* delimiters = " \t\n";
		int i = 0;
		cmd = strtok(lineSize, delimiters);
		if (cmd == NULL)
			return 0;
		args[0] = cmd;
		for (i = 1; i < MAX_ARG; i++)
		{
			args[i] = strtok(NULL, delimiters);
		}
		// Add your code here (execute a in the background)
		return (ExeExternal(smash, args, lineSize, true));
					
		/* 
		your code
		*/
		
	}
	return -1;
}
///////////////////////////////////////////SmallShell Class///////////////////////////////////////////////////////////////////////////

// Static members of SmallShell in order to handle signals
pid_t SmallShell::currentPIDRunning;
JobsList* SmallShell::jobs;
HistoryList* SmallShell::history;
char* SmallShell::currentCmdRunning;

SmallShell::SmallShell()  {
	previousPath = nullptr;
	jobs = new JobsList;
	history = new HistoryList;
	currentPIDRunning = 0;
	currentCmdRunning = new char[MAX_LINE_SIZE];
	smashPID = getpid();
	if (smashPID == -1)
		perror("smash error: getpid failed");
}

SmallShell::~SmallShell() {
	delete jobs;
	delete history;
	delete[] currentCmdRunning;
}

///////////////////////////////////////////HistoryList Class/////////////////////////////////////////////////////////////////////////
HistoryList::HistoryList() {
	historyList = new std::list<string>;
	//historySize = 0;
}
HistoryList::~HistoryList() {
	delete historyList;
}
void HistoryList::addHistory(char* cmdString) {
	string cmdToAdd = string(cmdString);
	if (historyList->size() == MAX_HISTORY_SIZE)
		historyList->pop_front(); //removes oldest command - need to check if delete for this item is required 
	historyList->push_back(cmdToAdd);
	/* { //max size of history is 50		
		historyList->push_back(cmdToAdd);//push newest command to the end of the list
		historyList->pop_front(); //removes oldest command - need to check if delete for this item is required 
	}
	else {
		historyList->push_back(cmdToAdd);
		historySize++;
	}*/
}

void HistoryList::printHistoryList() {
	for (auto it = historyList->begin(); it != historyList->end(); it++){
		std::cout << (*it) << endl;
	}
}
///////////////////////////////////////////Jobslist Class/////////////////////////////////////////////////////////////////////////

JobsList::JobsList() {
	jobsList = new std::list<JobItem*>;
	maxID = 0;
}

JobsList::~JobsList() {
	delete jobsList;
}

bool JobsList::JobsComp(JobItem* first, JobItem* second) {
	return ((*first).jobID < (*second).jobID);
}

void JobsList::addJob(char* cmd, pid_t jobPID, bool isStopped) {
	string cur_cmd = string(cmd);
	if (jobPID == SmallShell::currentPIDRunning) {
		JobItem* tmp = getJobByPID(jobPID);
		if (tmp) {        // Means job is already in jobList
			tmp->startTime = time(nullptr); //Need to check
			tmp->stopped = true;
			return;
		}
	}
	removeFinishedJobs();
	if (jobsList->empty()) {
		JobItem* newJob = new JobItem(1, jobPID, isStopped, time(nullptr), cur_cmd);
		maxID = 1;
		jobsList->push_back(newJob);
	}
	else {

		JobItem* newJob = new JobItem(maxID + 1, jobPID, isStopped, time(nullptr), cur_cmd);
		maxID += 1;
		jobsList->push_back(newJob);
	}
	jobsList->sort(JobsList::JobsComp);
}

void JobsList::removeFinishedJobs() {
	int newMaxID = 0;
	for (auto it = jobsList->begin(); it != jobsList->end(); it++)
	{
		pid_t pid = waitpid((*it)->PID, nullptr, WNOHANG);
		if (pid > 0) { // meaning - job finished running
			jobsList->remove(*it);
			it = jobsList->begin(); // necessery?
		}
		else {
			if ((*it)->jobID > newMaxID) newMaxID = (*it)->jobID;
		}
	}
	maxID = newMaxID;
}

void JobsList::printJobsList() {
	jobsList->sort(JobsList::JobsComp);
	auto it = jobsList->begin();
	while (it != jobsList->end())
	{
		int timePassed = difftime(time(nullptr), (*it)->startTime);
		pid_t pid = waitpid((*it)->PID, nullptr, WNOHANG);
		if (pid > 0) { // meaning - job finished running
			jobsList->remove(*(it++));
			continue;
		}
		if ((*it)->stopped) {
			std::cout << "[" << (*it)->jobID << "] " << (*it)->cmd << " : " <<
				(*it)->PID << " " << timePassed << " secs (stopped)" << endl;
		}

		else {
			std::cout << "[" << (*it)->jobID << "] " << (*it)->cmd << " : " <<
				(*it)->PID << " " << timePassed << " secs" << endl;
		}
		it++;
	}
}

JobsList::JobItem* JobsList::getJobById(int jobId) {
	for (auto it = jobsList->begin(); it != jobsList->end(); it++)
	{
		if ((*it)->jobID == jobId) return *it;
	}
	return nullptr;
}

JobsList::JobItem* JobsList::getJobByPID(pid_t pid) {
	for (auto it = jobsList->begin(); it != jobsList->end(); it++)
	{
		if ((*it)->PID == pid) return *it;
	}
	return nullptr;
}

void JobsList::removeJobById(int jobId) {
	for (auto it = jobsList->begin(); it != jobsList->end(); it++)
	{
		if ((*it)->jobID == jobId) {
			jobsList->remove(*it);
			delete* it;
			if (jobId == maxID) maxID = findCurrMaxJobID();
			break;
		}
	}
}

int JobsList::findCurrMaxJobID() {
	int maxJobID = 1;
	for (auto it = jobsList->begin(); it != jobsList->end(); it++)
	{
		if ((*it)->jobID > maxJobID) maxJobID = (*it)->jobID;
	}
	return maxJobID;
}

int JobsList::findMaxStoppedJobID() {
	int maxStoppedJobID = 0;
	for (auto it = jobsList->begin(); it != jobsList->end(); it++)
	{
		if (((*it)->stopped) && ((*it)->jobID > maxStoppedJobID)) maxStoppedJobID = (*it)->jobID;
	}
	return maxStoppedJobID;
}

void JobsList::KillAllJobs() {
	jobsList->sort(JobsList::JobsComp);
	for (auto it = jobsList->begin(); it != jobsList->end(); it++) {
		if (kill((*it)->PID, SIGTERM) == -1) perror("smash error: kill failed");
		time_t startKill = time(nullptr);
		pid_t pid;
		int timePassed = difftime(time(nullptr), startKill);
		std::cout << (*it)->cmd << " - Sending SIGTERM... ";
		while (timePassed < 5) {
			timePassed = difftime(time(nullptr), startKill);
			pid = waitpid((*it)->PID, nullptr, WNOHANG);
			if (pid > 0) { // meaning - job finished running
				break;
			}
		 }
		if (pid > 0) { // meaning - job finished running
			std::cout << "Done." << endl;
			continue;
		}
		std::cout << "(5 sec passed) Sending SIGKILL... ";
		if (kill((*it)->PID, SIGKILL) == -1) perror("smash error: kill failed");
		std::cout << "Done." << endl;
	}
}

