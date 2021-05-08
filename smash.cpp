#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "Commands.h"
#include "signals.h"

int main(int argc, char* argv[]) {

    if(signal(SIGTSTP , ctrlZHandler)==SIG_ERR) {
        perror("smash error: failed to set ctrl-Z handler");
    }
    if(signal(SIGINT , ctrlCHandler)==SIG_ERR) {
        perror("smash error: failed to set ctrl-C handler");
    }

    //TODO: setup sig alarm handler
    struct sigaction alarm_set;
    alarm_set.sa_handler=alarmHandler;
    alarm_set.sa_flags=SA_RESTART;
    sigemptyset(&alarm_set.sa_mask);
    if(sigaction(SIGALRM,&alarm_set,NULL)==-1){
        printf("smash error: failed to set alarm handler");// TODO: switch to stdin.
    }

    SmallShell& smash = SmallShell::getInstance();
    while(true) {
        std::cout <<smash.current_promt<<"> ";
        std::string cmd_line;
        std::getline(std::cin, cmd_line);
        smash.executeCommand(cmd_line.c_str());
    }
    return 0;
}