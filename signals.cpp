// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"
#include <iostream>
#include <signal.h>
#include "Commands.h"
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

void ctrlZHandler(int sig_num) {
    std::cout << endl;
    if (SmallShell::currentPIDRunning != 0) {
        SmallShell::jobs->addJob(SmallShell::currentCmdRunning, SmallShell::currentPIDRunning, true);
        if (kill(SmallShell::currentPIDRunning, SIGSTOP) == -1) {
            perror("smash error: kill failed");
            return;
        }
       // std::cout << "smash: process " << SmallShell::currentPIDRunning << " was stopped" << endl;
    }
}

void ctrlCHandler(int sig_num) {
    //std::cout << "smash: got ctrl-C" << endl;
    if (SmallShell::currentPIDRunning != 0) {
        if (kill(SmallShell::currentPIDRunning, SIGKILL) == -1) {
            perror("smash error: kill failed");
            return;
        }
        //std::cout << "smash: process " << SmallShell::currentPIDRunning << " was killed" << endl;
    }
}