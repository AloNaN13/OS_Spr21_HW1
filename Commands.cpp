#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
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

SmallShell::SmallShell() {
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
    return nullptr;
}

void SmallShell::executeCommand(const char *cmd_line) {
    /** delete done gobs*/

    int index_of_first_space= get_index_of_first_space_in_command (cmd_line);
    string full_coammand(cmd_line);
    string first_word=full_coammand.substr(0,index_of_first_space);






    // TODO: Add your implementation here
    // for example:
    // Command* cmd = CreateCommand(cmd_line);
    // cmd->execute();
    // Please note that you must fork smash process for some commands (e.g., external commands....)
}


int get_index_of_first_space_in_command(const char *cmd_line){
    int i=0;
    char current_char='1';
    while(current_char!=' ' && current_char!=0){
        current_char=cmd_line[i];
        i++;


    }
    return i;
}

int execute_built_in_commands (const char *cmd_line,string first_word){
    if(first_word==string("chpromt")){
        /*letapel bapkuda*/
        return 1;
    }
    if(first_word==string("showpid")){
        /*letapel bapkuda*/
        return 1;
    }
    if(first_word==string("pwd")){
        /*letapel bapkuda*/
        return 1;
    }
    if(first_word==string("cd")){
        /*letapel bapkuda*/
        return 1;
    }
    if(first_word==string("jobs")){
        /*letapel bapkuda*/
        return 1;
    }
    if(first_word==string("kill")){
        /*letapel bapkuda*/
        return 1;
    }
    if(first_word==string("fg")){
        /*letapel bapkuda*/
        return 1;
    }
    if(first_word==string("bg")){
        /*letapel bapkuda*/
        return 1;
    }
    if(first_word==string("quit")){
        /*letapel bapkuda*/
        return 1;
    }
    return -1;


}



/*implementation of built in commands*/


BuiltInCommand::BuiltInCommand(const char *cmd_line) :Command(cmd_line){
    char *dup_cmd_line = strdup(cmd_line);
    _removeBackgroundSign(dup_cmd_line);
    num_args = _parseCommandLine(dup_cmd_line, args_of_command);
    //this was for the basic commands


    //need to add for the other commands
}


//assumes all arguments are in args_of_command
ChangeDirCommand::ChangeDirCommand(const char* cmd_line, char** lastPwd)
        : BuiltInCommand(cmd_line) {

    char* current_dir = get_current_dir_name();//for the case of no arguments
    new_dir=current_dir; //deafault
    if(num_args>=2){     //to much arguments
        std::cout<<"smash error: cd: too many arguments"<<endl;
        return;
    }

    else if(num_args==0){
        return;
    }

    // we got exaclty one argument
    if(args_of_command[1]=="-"){
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
        new_prompt=args_of_command[1];
    }
    else{//need to reser the promt
        new_prompt="smash";

    }


}

void ChangeDirCommand::execute(){
    if (chdir(new_dir)!=0) {//if chdir wasn't succcesfull
        perror("smash error: chdir failed");
    }

}

void GetCurrDirCommand::execute(){
    char *current_dir_name = get_current_dir_name();
    std::cout << string(current_dir_name) << endl;
    //now we have to free because current_dir_name does malloc
    free(current_dir_name);

}
void ShowPidCommand::execute(){
    //pid_t pid = getpid();
    std::cout<< " smash pid is "<<this->pid<<endl;
}


KillCommand::KillCommand(const char* cmd_line, JobsList* jobs) :
        BuiltInCommand(cmd_line){



}


void KillCommand::execute(){
}




void QuitCommand::execute(){

}
JobsList::JobEntry* JobsList::getJobById(int job_id){
    for(auto iter= jobs_list.begin();iter!=jobs_list.end();++iter){
        if(job_id==iter->job_id){
            return &(*iter);
        }
    }
    return NULL;

}
void JobsList::deleteSpecificJobByID(int id_to_delete){
    for(auto iter= jobs_list.begin();iter!=jobs_list.end();++iter){
        if(id_to_delete==iter->job_id){
            jobs_list.erase(iter);
            break;
        }



    }
}

void JobsList::removeFinishedJobs(){
    pid_t result_of_wait;
    std::vector<JobEntry> jobs_to_erase;
    for(auto iter= jobs_list.begin();iter!=jobs_list.end();++iter){
        result_of_wait=waitpid(iter->get_pid(),NULL,WNOHANG);
        //CHECK IF RESULT WAS BAD

        jobs_to_erase.push_back(*iter);
    }
    //now its time to delete

    for(auto iter= jobs_to_erase.begin();iter!=jobs_to_erase.end();++iter){
        deleteSpecificJobByID(iter->job_id);
    }
    //find max+id
    int max_id=0;
    for(auto iter= jobs_list.begin();iter!=jobs_list.end();++iter){
        if(max_id<iter->job_id){
            max_id=iter->job_id;
        }
    }
    this->max_id=max_id;
}


void JobsCommand::execute(){
    for(auto iter= jobs->jobs_list.begin();iter!=jobs->jobs_list.end();++iter){

        std::cout<<"["<<iter->get_job_id()<<"] "<<string(iter->command_of_job->getCommandLine())<<" : "<<
                 iter->get_pid()<<" "<<difftime(time(NULL),iter->arrived_time)<<" secs";
        if(iter->get_status()==0){ //stoped
            std::cout<<" (stopped)";
        }
        std::cout<<endl;
    }
}

/*void KillCommand::execute(){
  if (num_args!=3){
        std::cout<<"smash error: kill: invalid arguments"<<endl;
        return;
  }
  JobsList::JobEntry* job_to_kill=getJobById( job_id);

}*/

