#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"

using namespace std;

void ctrlZHandler(int sig_num) {
    // TODO: Add your implementation
    std::cout<<"smash: got ctrl-Z"<<endl;
    if(SmallShell::getInstance().curr_external_fg_command==nullptr){
        //the current process running in the fg is the smash himself
        //-thence we need to ignore it
        return;
    }
    int pid_for_sig=SmallShell::getInstance().curr_external_fg_command->getPid();
    if(kill(pid_for_sig,SIGSTOP)<0){
        perror("smash error: kill failed");
    }
    SmallShell::getInstance().jobs->addJob
            (SmallShell::getInstance().curr_external_fg_command,JobsList::stopped);
    std::cout<<"smash: process "<<pid_for_sig<< " was stopped"<<endl;

    SmallShell::getInstance().curr_external_fg_command=nullptr;

}

void ctrlCHandler(int sig_num) {
    // TODO: Add your implementation
    std::cout<<"smash: got ctrl-C"<<endl;
    if(SmallShell::getInstance().curr_external_fg_command==nullptr){
        //the current process running in the fg is the smash himself
        //-thence we need to ignore it
        return;
    }
    int pid_for_sig=SmallShell::getInstance().curr_external_fg_command->getPid();
    if(kill(pid_for_sig,SIGINT)<0){
        perror("smash error: kill failed");
    }
    std::cout<<"smash: process "<<pid_for_sig<< " was killed"<<endl;

    //delete SmallShell::getInstance().curr_external_fg_command;
    SmallShell::getInstance().curr_external_fg_command=nullptr;
}

void alarmHandler(int sig_num) {
    // TODO: Add your implementation
    std::cout << "smash: got an alarm"<<endl;
    SmallShell* smash_inst =&(SmallShell::getInstance());

    //run on the list



    TimeOutList::TimeOutEntry* closest_time_out=SmallShell::getInstance().time_outs->closestTimeOut();
    if(closest_time_out!=nullptr){
        int for_alarm=closest_time_out->duration_left-(difftime(closest_time_out->insertion_time,time(NULL)));
        alarm(for_alarm);
    }



}

