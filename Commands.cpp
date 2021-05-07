

//#include <cassert.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include <sys/file.h>
#include <stdio.h>
#include "Commands.h"


using namespace std;

//FOR WHITSPACE
const std::string WHITESPACE = " \n\r\t\f\v";


#if 0
#define FUNC_ENTRY()  \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

string _ltrim(const std::string& s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

string _rtrim(const std::string& s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string _trim(const std::string& s)
{
    return _rtrim(_ltrim(s));
}

int _parseCommandLine(const char* cmd_line, char** args) {
    FUNC_ENTRY()
    int i = 0;
    std::istringstream iss(_trim(string(cmd_line)).c_str());
    for(std::string s; iss >> s; ) {
        args[i] = (char*)malloc(s.length()+1);
        memset(args[i], 0, s.length()+1);
        strcpy(args[i], s.c_str());
        args[++i] = NULL;
    }
    return i;

    FUNC_EXIT()
}

bool _isBackgroundComamnd(const char* cmd_line) {
    const string str(cmd_line);
    return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(char* cmd_line) {
    const string str(cmd_line);
    // find last character other than spaces
    unsigned int idx = str.find_last_not_of(WHITESPACE);
    // if all characters are spaces then return
    if (idx == string::npos) {
        return;
    }
    // if the command line does not end with & then return
    if (cmd_line[idx] != '&') {
        return;
    }
    // replace the & (background sign) with space and then remove all tailing spaces.
    cmd_line[idx] = ' ';
    // truncate the command line string up to the last non-space character
    cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}

// TODO: Add your implementation for classes in Commands.h

SmallShell::SmallShell():jobs(new JobsList()),current_promt("smash"),
                         curr_external_fg_command(nullptr),time_outs(new TimeOutList()){
// TODO: add your implementation
}

SmallShell::~SmallShell() {
// TODO: add your implementation
}

/**
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
Command * SmallShell::CreateCommand(const char* cmd_line) {

    // For example:
/*
  string cmd_s = _trim(string(cmd_line));
  string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));

  if (firstWord.compare("pwd") == 0) {
    return new GetCurrDirCommand(cmd_line);
  }
  else if (firstWord.compare("showpid") == 0) {
    return new ShowPidCommand(cmd_line);
  }
  else if ...
  .....
  else {
    return new ExternalCommand(cmd_line);
  }
  */




    SmallShell::getInstance().jobs->removeFinishedJobs();

    //took from idan, need to change later, just want to see if the functions work
    /*string cmd_s = string(cmd_line);
    _ltrim(cmd_s);
    if (cmd_s.find("chprompt") == 0) {
        return new ChangePromptCommand(cmd_line);
    }
    else if (cmd_s.find("showpid") == 0) {
        return new ShowPidCommand(cmd_line);
    } else if (cmd_s.find("pwd") == 0) {
        return new GetCurrDirCommand(cmd_line);
    } else if (cmd_s.find("cd") == 0) {
        return new ChangeDirCommand(cmd_line, &(this->last_direction_command));
    }else if (cmd_s.find("jobs") == 0) {
        return new JobsCommand(cmd_line,  this->jobs);
    } else if (cmd_s.find("kill") == 0) {
        return new KillCommand(cmd_line, this->jobs);
    } else if (cmd_s.find("fg") == 0) {
        return new ForegroundCommand(cmd_line, this->jobs);
    } else if (cmd_s.find("bg") == 0) {
        return new BackgroundCommand(cmd_line, this->jobs);
    } else if (cmd_s.find("quit") == 0) {
        return new QuitCommand(cmd_line,this->jobs );
    } else if (cmd_s.find("timeout") == 0) {
        //do timeout
     }else {
        return new ExternalCommand(cmd_line, SmallShell::getInstance().jobs);//need to deal with this
    }*/



    string cmd_s = _trim(string(cmd_line));
    string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));

    if (firstWord.compare("chprompt") == 0) {
        return new ChangePromptCommand(cmd_line);
    } else if (firstWord.compare("showpid") == 0) {
        return new ShowPidCommand(cmd_line);
    } else if (firstWord.compare("pwd") == 0) {
        return new GetCurrDirCommand(cmd_line);
    }else if (firstWord.compare("cd") == 0) {
        return new ChangeDirCommand(cmd_line, &(this->last_direction_command));
    }else if (firstWord.compare("jobs") == 0) {
        return new JobsCommand(cmd_line,  this->jobs);
    }else if (firstWord.compare("kill") == 0) {
        return new KillCommand(cmd_line, this->jobs);
    } else if (firstWord.compare("fg") == 0) {
        return new ForegroundCommand(cmd_line, this->jobs);
    } else if (firstWord.compare("bg") == 0) {
        return new BackgroundCommand(cmd_line, this->jobs);
    } else if (firstWord.compare("quit") == 0) {
        return new QuitCommand(cmd_line,this->jobs );
    } /*else if (firstWord.compare("timeout") == 0) {
         return timeOutCommand(cmd_line);
     }*/else {
        return new ExternalCommand(cmd_line, SmallShell::getInstance().jobs);//need to deal with this
    }


    //need to add special commands
    return nullptr;
}

//time-outs


/*Command* SmallShell::timeOutCommand(const char *cmd_line){
    //background=_isBackgroundComamnd(cmd_line);
    char* args_with_timeout[COMMAND_MAX_ARGS];
    char* cmd_line_without_background_sign=strdup(cmd_line);

    int num_args_of_allcommand= _parseCommandLine(cmd_line,args_with_timeout);
     int duration_of_timeout=atoi(args_with_timeout[1]);
     if(duration_of_timeout==0||num_args_of_allcommand==2){
         perror("smash error: timeout: invalid arguments");
         return nullptr;
     }
     string command_without_timeout="";
     for (int i = 2; i < num_args_of_allcommand; i++)
     {
         command_without_timeout=command_without_timeout+string(args_with_timeout[i]);
     }
     char* cmd_without_timeout=&command_without_timeout[0];
     Command *new_command=CreateCommand(cmd_without_timeout);
     if(new_command==nullptr){
         return nullptr;
     }
     SmallShell::getInstance().time_outs->addTimeOut(time(NULL),new_command->getPid(),duration_of_timeout);
     alarm(duration_of_timeout);
     return new_command;


}
void TimeOutList::addTimeOut(time_t insertion_time,pid_t pid, int duration_left){
        TimeOutList::TimeOutEntry* new_time_out=
                new TimeOutList::TimeOutEntry(insertion_time,pid,duration_left);

}
void   TimeOutList::removeFinishedTimeOuts(){
    pid_t result_of_wait;
    std::vector<TimeOutEntry*> timeOuts_to_erase;
    for(auto iter= timeOuts_list.begin();iter!=timeOuts_list.end();++iter){
        result_of_wait=waitpid((*iter)->pid_of_the_time_entry ,NULL,WNOHANG);
        //CHECK IF RESULT WAS BAD
        if (result_of_wait>0){
            timeOuts_to_erase.push_back(*iter);
        }


    }
    //now its time to delete

    for(auto iter= timeOuts_to_erase.begin();iter!=timeOuts_to_erase.end();++iter){
        removeJobById((*iter)->job_id);
    }
    //find max+id
    int max_id=0;
    for(auto iter= jobs_list.begin();iter!=jobs_list.end();++iter){
        if(max_id<(*iter)->job_id){
            max_id=(*iter)->job_id;
        }
    }
    this->max_id=max_id;
}*/


TimeOutList::TimeOutEntry* TimeOutList:: closestTimeOut(){
    if(timeOuts_vec.size()==0){
        return nullptr;
    }
    time_t current_time=time(NULL);
    auto iter= timeOuts_vec.begin();
    double runs_for=difftime((*iter)->insertion_time,current_time);
    double min_left_duration=(*iter)->duration_left-runs_for;
    TimeOutEntry* closest_time_tenry=(*iter);

    for(;iter!=timeOuts_vec.end();++iter){
        runs_for=difftime((*iter)->insertion_time,current_time);
        if( min_left_duration>(*iter)->duration_left-runs_for){
            min_left_duration=(*iter)->duration_left-runs_for;
            closest_time_tenry=(*iter);
        }
    }
    return closest_time_tenry;
}



void SmallShell::executeCommand(const char *cmd_line) {
    /** delete done gobs*/

    /*int index_of_first_space= get_index_of_first_space_in_command (cmd_line);
    string full_coammand(cmd_line);
    string first_word=full_coammand.substr(0,index_of_first_space);*/


    // TODO: Add your implementation here
    // for example:
    // Command* cmd = CreateCommand(cmd_line);
    // cmd->execute();
    // Please note that you must fork smash process for some commands (e.g., external commands....)

    int loc;
    specialType type = checkSpecialType(cmd_line, &loc);
    if(type != NOT_SPECIAL){ // special command
        if(type == OVERRIDE || type == APPEND){ // redirection command
            //redirection
            Command* cmd = new RedirectionCommand(cmd_line, type);
            if(cmd == nullptr){

                delete cmd;
                return;
            }
            cmd->execute();
            delete cmd;

        }
        else{ // pipe command
            //PIPE - TODO
            Command* cmd =new PipeCommand(cmd_line, type);
            if(cmd == nullptr){

                delete cmd;
                return;
            }
            cmd->execute();
            delete cmd;
        }
    }
    else{ // regular command
        Command* cmd = CreateCommand(cmd_line);
        if(cmd == nullptr){
            return;
        }
        cmd->execute();
    }
}

std::vector<JobsList::JobEntry*> SmallShell:: jobsToSendAlarm(){


}




/*Functions of command*/

Command::Command(const char* cmd_line): command_line(cmd_line) {


    background=_isBackgroundComamnd(cmd_line);
    char* cmd_line_without_background_sign=strdup(cmd_line);
    num_args=_parseCommandLine(cmd_line_without_background_sign,args_of_command);


    //free cmd_line_without_background_sign?i
    this->pid=getpid();

}
Command::~Command(){
    //delete [] args_of_command;
}

/*implementation of built in commands*/


BuiltInCommand::BuiltInCommand(const char *cmd_line) :Command(cmd_line){
    /*char *dup_cmd_line = strdup(cmd_line);
    _removeBackgroundSign(dup_cmd_line);
    num_args = _parseCommandLine(dup_cmd_line, args_of_command);*/
    //this was for the basic commands


    //need to add for the other commands
}


//assumes all arguments are in args_of_command
ChangeDirCommand::ChangeDirCommand(const char* cmd_line, char** lastPwd)
        : BuiltInCommand(cmd_line) {

    char* current_dir = get_current_dir_name();//for the case of no arguments
    new_dir=current_dir; //deafault
    if(num_args>2){     //to much arguments
        std::cout<<"smash error: cd: too many arguments"<<endl;
        return;
    }

    else if(num_args==1){
        return;
    }

    // we got exaclty one argument
    //if(args_of_command[1]=="-"){
    if(strcmp(args_of_command[1],"-")==0){
        if(*lastPwd==NULL){
            std::cout<<"smash error: cd: OLDPWD not set"<<endl;
            return;
        }
        //lastPwd is ok
        new_dir=strdup(*lastPwd);
        *lastPwd=strdup(current_dir);
        return;
    }

    //its a relative  or full cd

    //to see that it works, maybe later move it to execute

    string string_for_dir = string(current_dir)+"/"+
                            string(args_of_command[1]);
    new_dir=strdup(string_for_dir.c_str());
    int result = chdir(new_dir);
    if (result==0) {
        *lastPwd = strdup(current_dir);
    }

}

ChangePromptCommand::ChangePromptCommand(const char* cmd_line):
        BuiltInCommand(cmd_line){

    if(num_args>=2){
        this->new_prompt=args_of_command[1];

    }
    else{//need to reser the promt
        this->new_prompt="smash";

    }


}
void ChangePromptCommand::execute(){
    SmallShell::getInstance().current_promt= string(this->new_prompt );
}


void ChangeDirCommand::execute(){
    if (chdir(new_dir)!=0) {//if chdir wasn't succcesfull
        perror("smash error: chdir failed");
    }

}

GetCurrDirCommand:: GetCurrDirCommand(const char* cmd_line):BuiltInCommand(cmd_line){

}

void GetCurrDirCommand::execute(){
    char *current_dir_name = get_current_dir_name();
    std::cout << string(current_dir_name) << endl;
    //now we have to free because current_dir_name does malloc
    free(current_dir_name);

}

ShowPidCommand:: ShowPidCommand(const char* cmd_line):BuiltInCommand(cmd_line){

}

void ShowPidCommand::execute(){
    //pid_t pid = getpid();
    std::cout<< " smash pid is "<<this->pid<<endl;
}


QuitCommand::QuitCommand(const char* cmd_line, JobsList* jobs) : BuiltInCommand(cmd_line), jobs_list(jobs), to_kill(false){
    //assert(jobs);
    for(int i=1; i < num_args; ++i){
        if(!strcmp(args_of_command[i], "kill")){
            to_kill = true;
        }
    }
}

void QuitCommand::execute(){
    if(to_kill) {
        //(assert(jobs_list));
        jobs_list->killJobs();
    }
    exit(0); // correct way to exit?
}


/////////////////////////////////////////functions of jobslist

JobsList::JobsList():jobs_list(vector <JobEntry*> ()),max_id(0){}
void JobsList::killJobs() {
    // loop to kill all jobs
    int jobs_num = jobs_list.size();
    std::cout << "smash: sending SIGKILL signal to " << jobs_num << " jobs:" << endl;
    for (auto iter = jobs_list.begin(); iter != jobs_list.end(); ++iter) {
        pid_t pid = (*iter)->get_pid();
        if(kill(pid, SIGKILL) == -1){
            perror("smash error: kill failed"); // ok to do so? needed?
        }
        std::cout << pid << ": " << (*iter)->command_of_job->getCommandLine() << endl;
    }
    jobs_list.clear();
}

JobsList::JobEntry* JobsList::getJobById(int job_id){
    for(auto iter= jobs_list.begin();iter!=jobs_list.end();++iter){
        if(job_id==(*iter)->job_id){
            return (*iter);
        }
    }
    return NULL;

}

void JobsList::removeJobById(int id_to_delete){
    for(auto iter= jobs_list.begin();iter!=jobs_list.end();++iter){
        if(id_to_delete==(*iter)->job_id){
            jobs_list.erase(iter);
            break;
        }
    }
}

void JobsList::printSpecificJobByID(int id_to_print){
    for(auto iter= jobs_list.begin();iter!=jobs_list.end();++iter){
        if(id_to_print==(*iter)->job_id){
            std::cout << string((*iter)->command_of_job->getCommandLine()) << " : " << (*iter)->get_pid() << endl;

            break;
        }
    }
}

JobsList::JobEntry* JobsList::getLastStoppedJob(int *jobId){
    JobEntry* last_stopped_job=nullptr;
    for(auto iter= jobs_list.begin();iter!=jobs_list.end();++iter){
        if((*iter)->get_status()==stopped){
            last_stopped_job=(*iter);
        }
    }
    return last_stopped_job;
}

void JobsList::removeFinishedJobs(){
    pid_t result_of_wait;
    std::vector<JobEntry*> jobs_to_erase;
    for(auto iter= jobs_list.begin();iter!=jobs_list.end();++iter){
        result_of_wait=waitpid((*iter)->get_pid(),NULL,WNOHANG);
        //CHECK IF RESULT WAS BAD

        if (result_of_wait>0){
            jobs_to_erase.push_back(*iter);
        }


    }
    //now its time to delete

    for(auto iter= jobs_to_erase.begin();iter!=jobs_to_erase.end();++iter){
        removeJobById((*iter)->job_id);
    }
    //find max+id
    int max_id=0;
    for(auto iter= jobs_list.begin();iter!=jobs_list.end();++iter){
        if(max_id<(*iter)->job_id){
            max_id=(*iter)->job_id;
        }
    }
    this->max_id=max_id;
}

void JobsList::addJob(Command* cmd, JobStatus status ){
    int id_of_added_job=this->getMaxID()+1;
    /*JobStatus status_of_job;
    if (isStopped) {
        status_of_job = stopped;
    } else {
        status_of_job = running;
    }*/



    JobsList::JobEntry* job_to_add =
            new JobsList::JobEntry(id_of_added_job,time(NULL),status,cmd);
    jobs_list.push_back(job_to_add);
    SmallShell::getInstance().jobs->max_id=id_of_added_job;



}




void JobsCommand::execute(){
    for(auto iter= jobs->jobs_list.begin();iter!=jobs->jobs_list.end();++iter){

        std::cout<<"["<<(*iter)->get_job_id()<<"] "<<string((*iter)->command_of_job->getCommandLine())<<" : "<<
                 (*iter)->get_pid()<<" "<<difftime(time(NULL),(*iter)->arrived_time)<<" secs";
        if((*iter)->get_status()==JobsList::stopped){ //stoped
            std::cout<<" (stopped)";
        }
        std::cout<<endl;
    }
}

void KillCommand::execute(){
    if (num_args!=3){//check if not
        std::cout<<"smash error: kill: invalid arguments"<<endl;
        return;
    }
    if(this->args_of_command[1][0]!='-'){
        std::cout<<"smash error: kill: invalid arguments"<<endl;
        return;
    }

    int signal_to_send =atoi ((args_of_command[1]+1));
    //shou i check that signal to send consists only of numbers

    int job_id=atoi (args_of_command[2]);
    if(job_id==0 ||signal_to_send==0){ //the string of job id or of signal doesn't consists of numbers
        std::cout<<"smash error: kill: invalid arguments"<<endl;
        return;
    }
    if(signal_to_send<1||signal_to_send>31){
        std::cout<<"smash error: kill: invalid arguments"<<endl;
        return;
    }


    JobsList::JobEntry* job_to_kill= this->jobs->getJobById(job_id);
    if (job_to_kill==NULL){//there is no such job
        std::cout<<"smash error: kill: job-id" << job_id << "does not exist"<<endl;
        return;
    }
    pid_t pid_of_job_to_kill=job_to_kill->get_pid();
    if(kill(pid, signal_to_send)!=0){
        perror("smash error: kill failed");
        return;
    }
    cout<< "signal number"<<signal_to_send<<"was sent to pid"<<pid_of_job_to_kill<<endl;

}

void ForegroundCommand::execute(){
    int job_id_to_fg=jobs->getMaxID();
    int jobs_max_id=jobs->getMaxID();
    JobsList::JobEntry* job_to_fg=nullptr;
    if(num_args>2){
        std::cout << "smash error: fg: invalid arguments" << endl;
        return;
    }
    if(num_args==1){// there is no arguments beside the command
        if(jobs_max_id==0){ //jobsList is empty
            std::cout << "smash error: fg: jobs list is empty" << endl;
            return;
        }
        job_to_fg= this->jobs->getJobById(jobs_max_id);
    }

    else{
        job_id_to_fg=atoi (args_of_command[1]);
        if(job_id_to_fg==0){//format of the id is bad
            std::cout << "smash error: fg: invalid arguments" << endl;
            return;
        }

        job_to_fg= this->jobs->getJobById(job_id_to_fg);
        if(job_to_fg==nullptr){
            std::cout << "smash error: fg: job-id "<< job_id_to_fg <<" does not exist"<<endl;
            return;
        }
    }
    jobs->printSpecificJobByID(job_id_to_fg);
    //now we need to move  the job to fg

    SmallShell::getInstance().curr_external_fg_command=job_to_fg->command_of_job;
    int pid_of_job_to_fg=job_to_fg->get_pid();
    kill(pid_of_job_to_fg, SIGCONT);
    if(waitpid(pid_of_job_to_fg,NULL, WUNTRACED)==-1){
        perror("smash error: wait failed");
    }
    jobs->removeJobById(job_id_to_fg);
    //  (SmallShell::getInstance().cur_fg_job) = nullptr;

}

void BackgroundCommand::execute(){
    JobsList::JobEntry* job_to_bg=nullptr;
    int job_id_to_bg;
    if(num_args>2){
        std::cout << "smash error: bg: invalid arguments" << endl;
        return;
    }
    if(num_args==1){// there is no arguments beside the command
        job_to_bg=jobs->getLastStoppedJob(&job_id_to_bg);
        if(job_to_bg==nullptr){ //no stopped jobs
            std::cout << "smash error: bg: there is no stopeed jobs to resume" << endl;
            return;
        }
    }
    else{
        int job_id_to_bg=atoi (args_of_command[1]);
        if(job_id_to_bg==0){//format of the id is bad
            std::cout << "smash error: bg: invalid arguments" << endl;
            return;
        }
        job_to_bg=jobs->getJobById(job_id_to_bg);
        if(job_to_bg==nullptr){
            std::cout << "smash error: bg: job-id "<< job_id_to_bg <<" does not exist"<<endl;
            return;
        }
        if(job_to_bg->get_status()==JobsList::stopped){
            std::cout << "smash error: bg: job-id "<< job_id_to_bg <<" is already running in the backgroung"<<endl;
        }
    }
    jobs->printSpecificJobByID(job_id_to_bg);
    //now to bring it to bg
    if(kill(job_id_to_bg,SIGCONT)==-1){
        perror("smash error: kill failed");
        return;
    }
    job_to_bg->changeStatus(JobsList::running);
}

//EXTERNAL COMMANDS

void ExternalCommand::execute() {


    /*char *command = new char[COMMAND_ARGS_MAX_LENGTH + 1];
    strcpy(command, command_line);
    _removeBackgroundSign(command);*/

    //from yuval change check if works


    char *cleancommand=strdup(command_line);
    _removeBackgroundSign(cleancommand);

    char* const args_for_exec[]={(char*)"/bin/bash",(char*)"-c",cleancommand,NULL};


    pid_t pid_of_fork=fork();
    if(pid_of_fork==0){ //young boy

        setpgrp();

        /*execl("/bin/bash", "bash", "-c", command_line, NULL);*/

        if(execv("/bin/bash",args_for_exec)==-1){
            perror("smash error: exec failed");
            //delete[] command;
            return;

        }


    }else if (pid < 0){ //error
        perror("smash error: fork failed");
        return;
    }
    else{//parent

        this->pid=pid_of_fork; //the pid of external command=pid of son
        if( background==true){
            //we need to add to job list
            /*if(jobs==nullptr){
                id_of_add_job=1;

            }else{
                id_of_add_job=jobs->getMaxID()+1;
            }*/

            SmallShell::getInstance().jobs->addJob(this,JobsList::running);



        }
        else {//background==false

            SmallShell::getInstance().curr_external_fg_command=this;//this cmmand now runs in the foregroung
            if(waitpid(pid_of_fork,NULL,WUNTRACED)<0){
                perror("smash error: waitpid failed");
            }
            SmallShell::getInstance().curr_external_fg_command=nullptr;
        }

    }
}


//SPECIAL COMMANDS

specialType checkSpecialType(const char* cmd_line, int* special_loc){

    // return to pointer the location of the special char
    // loop to check if any char criteria is met (specialType)
    // change specialType according to enum

    for(int i = 0; cmd_line[i]!=0 ;++i){
        *special_loc = i;
        if(cmd_line[i] == '>'){
            if(cmd_line[i+1] == '>>'){
                *special_loc = i;
                return APPEND;
            }
            *special_loc = i;
            return OVERRIDE;
        }
        else if(cmd_line[i] == '|'){
            if(cmd_line[i+1] == '&'){
                *special_loc = i;
                return STDERR;
            }
            *special_loc = i;
            return STDOUT;
        }
    }
    *special_loc = -1;
    return NOT_SPECIAL;
}

// splitting special commands into commands - one if redirection, two if pipe
bool splitToCommands(specialType type, const char* cmd_line, char** cmd_line_1, char** cmd_line_2){
    // check spaces before or after?
    int loc;
    specialType type1 = checkSpecialType(cmd_line, &loc);
    *cmd_line_1 = strndup(cmd_line,loc);
    if(*cmd_line_1 == nullptr){
        return false;
    }
    int spec_char_size = 1;
    if(type == APPEND || type == STDERR){
        spec_char_size++;
    }
    *cmd_line_2 = strdup(cmd_line + loc + spec_char_size + 1);
    if(*cmd_line_1 == nullptr){
        free(cmd_line_1);
        return false;
    }
    return true;
}

RedirectionCommand::RedirectionCommand(const char* cmd_line, specialType type) : Command(cmd_line), type(type), org_fd(1), redirected_fd(-1),
                                                                                 cmd_line_1(nullptr), filename(nullptr) {
    splitToCommands(type, cmd_line, &cmd_line_1, &filename);
}

void RedirectionCommand::execute(){

    // redirect the output

    // dup the original STDOUT
    org_fd = dup(STDOUT_FILENO);
    if(org_fd == -1){
        perror("smash error: dup failed");
    }
    // open a new fd for the filename
    if(type == OVERRIDE){
        redirected_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0600); // is that the correct permission??
    }
    else{ // type == APPEND
        redirected_fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0600); // is that the correct permission??
    }
    if(redirected_fd == -1){
        perror("smash error: open failed");
        //redirected_fd = open("/dev/null", O_WRONLY); is this needed????
    }
    // dup2 the fd of the filename into the STDOUT
    int dup2_res = dup2(redirected_fd, STDOUT_FILENO);
    if(dup2_res == -1){
        perror("smash error: dup2 failed");
    }

    // create and execute the real command
    Command* cmd = SmallShell::getInstance().CreateCommand(cmd_line_1);
    if(cmd == nullptr){
        return;
    }
    cmd->execute();
}

RedirectionCommand::~RedirectionCommand(){
    // free command args?????
    if(redirected_fd != 1){
        close(redirected_fd);
        dup2(org_fd, STDOUT_FILENO);
    }
}

PipeCommand::PipeCommand(const char* cmd_line, specialType type) : Command(cmd_line), type(type), cmd_line_1(nullptr), cmd_line_2(nullptr) {
    splitToCommands(type, cmd_line, &cmd_line_1, &cmd_line_2);
}

void PipeCommand::execute(){

    // open pipe
    int pipe_res = pipe(pipe_inst);
    if(pipe_res == -1){
        perror("smash error: pipe failed");
        return;
    }

    //create child proccesses for the redirection of input/ouput
    pid_t pid_child_1, pid_child_2;
    (pid_child_1 == fork() && pid_child_2 == fork());
    if(pid_child_1 == 0){ // young boy's 1 code - the 1st command
        // close pipe[write]
        close(pipe_inst[1]); // check for failure?
        // Redirect pipe[read] into STDOUT
        int output = ((type == STDOUT) ? STDOUT_FILENO : STDERR_FILENO);
        int dup2_res = dup2(pipe_inst[0], output);
        if(dup2_res == -1){
            perror("smash error: dup2 failed");
            return;
        }
        // Execute command
        Command* cmd =SmallShell::getInstance().CreateCommand(cmd_line_1);
        if(cmd == nullptr){
            return;
        }
        cmd->execute();
        // Close pipe[read]
        close(pipe_inst[0]); // check for failure??
        exit(0); // right way to close child_1 proccess?
    }
    else if(pid_child_2 == 0){ // young boy's 2 code - the 2nd command
        // Close pipe[read]
        close(pipe_inst[0]); // check for failure?
        // Redirect pipe[write] into STDIN
        int dup2_res = dup2(pipe_inst[0], STDIN_FILENO);
        if(dup2_res == -1){
            perror("smash error: dup2 failed");
            return;
        }
        // Execute command
        Command* cmd = SmallShell::getInstance().CreateCommand(cmd_line_2);
        if(cmd == nullptr){
            return;
        }
        cmd->execute();
        // Close pipe[write]
        close(pipe_inst[1]); // check for failure??
        exit(0); // right way to close child_2 proccess?
    }
    else if(pid_child_1 < 0 || pid_child_2 < 0){ // failure
        perror("smash error: fork failure");
    }
    else{ // father
        // Wait for child processes to end
        if(waitpid(pid_child_1,NULL,WUNTRACED) < 0 || waitpid(pid_child_2,NULL,WUNTRACED) < 0){ // check if this is correct
            perror("smash error: waitpid failed");
            return;
        }
    }
    // setpgrp??
}

PipeCommand::~PipeCommand(){
    // Close pipe[read] and pipe[write]
    close(pipe_inst[0]); // check for failure?
    close(pipe_inst[1]); // check for failure?
}

CatCommand::CatCommand(const char* cmd_line):BuiltInCommand(cmd_line){

}

void CatCommand::execute(){
    if(num_args==1){
        std::cout << "smash error: cat: not enough arguments"<<endl;
        return;
    }
    int buff_from_reading;
    for (int i=1; i<num_args;i++){
        FILE* current_file= fopen(args_of_command[i],"r");
        int fd_of_current_file=fileno(current_file);

        //check sucess?
        while (read(fd_of_current_file,&buff_from_reading,1)){//read untill finish
            write(STDOUT_FILENO,&buff_from_reading,1);
        }
        close(fd_of_current_file);
    }

}
