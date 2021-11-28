#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <list>
#include<iostream>
#include <fstream>
#define MAX_LINE_SIZE 80
#define MAX_ARG 20
#define MAX_HISTORY_SIZE 50
//typedef enum { FALSE , TRUE } bool;
using namespace std;
class HistoryList {
public:
    std::list<string>* historyList;
    int historySize;
    HistoryList();
    ~HistoryList();
    void addHistory(char* cmdString);
    void printHistoryList();
};
class JobsList {
public:
    class JobItem {
    public:
        JobItem(int jobid, pid_t pid, int stop, time_t time, string command) : jobID(jobid), PID(pid), stopped(stop), startTime(time), cmd(command) {}
        int jobID;
        pid_t PID;
        bool stopped;
        time_t startTime;
        string cmd;
    };
    std::list<JobItem*>* jobsList;
    int maxID;
    JobsList();
    ~JobsList();
    static bool JobsComp(JobItem* first, JobItem* second);
    void addJob(char* cmd, pid_t jobPID, bool isStopped = false);
    void printJobsList();
    void removeFinishedJobs();
    JobItem* getJobById(int jobId);
    JobItem* getJobByPID(pid_t pid);
    void removeJobById(int jobId);
    JobItem* getLastJob(int* lastJobId);
    JobItem* getLastStoppedJob(int* jobId);
    int findCurrMaxJobID();
    int findMaxStoppedJobID();
    void KillAllJobs();
};

class SmallShell {
//private:

public:
    SmallShell();
    pid_t smashPID;
    char* previousPath;
    static HistoryList* history;
    static JobsList* jobs;
    //TO ADD HISTORY
    static pid_t currentPIDRunning;
    static char* currentCmdRunning;
    //SmallShell(SmallShell const&) = delete; // disable copy ctor
    //void operator=(SmallShell const&) = delete; // disable = operator
    ~SmallShell();
};
int ExeComp(char* lineSize);
int BgCmd(SmallShell* smash, char* lineSize, void* jobs);
int ExeCmd(SmallShell* smash, void* jobs, char* lineSize, char* cmdString);
int ExeExternal(SmallShell* smash, char* args[MAX_ARG], char* cmdString, bool isBackgroundCmd);


#endif

