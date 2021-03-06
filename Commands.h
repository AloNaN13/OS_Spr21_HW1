#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_



#include <unistd.h>
#include <ostream>
#include <vector>
#include <string>
#include <cstring>

#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)

enum specialType {NOT_SPECIAL, OVERRIDE, APPEND, STDOUT, STDERR};

using std::string;

class JobsList;
class TimeOutList;
class Command {


protected:
    char* args_of_command[COMMAND_MAX_ARGS];
    pid_t pid;
    const char* command_line;
    //int job_id;
    bool background;

    int num_args;

// TODO: Add your data members

public:
    Command(const char* cmd_line);
    virtual ~Command();
    virtual void execute() = 0;
    const char* getCommandLine(){return command_line;}
    int getPid(){return pid;}
    //virtual void prepare();
    //virtual void cleanup();
    // TODO: Add your extra methods if needed
};

class BuiltInCommand : public Command {

public:
    BuiltInCommand(const char* cmd_line);
    virtual ~BuiltInCommand() {}
};

class ExternalCommand : public Command {
    //JobsList* jobs;
public:
    ExternalCommand(const char *cmd_line):Command(cmd_line){}
    virtual ~ExternalCommand() {}
    void execute() override;
};

class PipeCommand : public Command {
    // TODO: Add your data members
    specialType type;
    int pipe_inst[2];
    char* cmd_line_1;
    char* cmd_line_2;
public:
    PipeCommand(const char* cmd_line, specialType type);
    ~PipeCommand() override;
    void execute() override;
};

class RedirectionCommand : public Command {
    // TODO: Add your data members
    specialType type;
    int org_fd;
    int redirected_fd;
    char* cmd_line_1;
    char* filename;
public:
    RedirectionCommand(const char* cmd_line, specialType type);
    ~RedirectionCommand() override;
    void execute() override;
    //void prepare() override;
    //void cleanup() override;
};

class ChangePromptCommand : public BuiltInCommand{
public:
    string new_prompt;
    ChangePromptCommand(const char* cmd_line);
    virtual ~ChangePromptCommand() {}
    void execute() override;
};

class ChangeDirCommand : public BuiltInCommand {
    char* new_dir;

// TODO: Add your data members public:
public:
    ChangeDirCommand(const char* cmd_line, char** plastPwd);
    virtual ~ChangeDirCommand() {}
    void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand {
public:
    GetCurrDirCommand(const char* cmd_line);
    virtual ~GetCurrDirCommand() {}
    void execute() override;
};

class ShowPidCommand : public BuiltInCommand {
public:
    ShowPidCommand(const char* cmd_line);
    virtual ~ShowPidCommand() {}
    void execute() override;
};

class QuitCommand : public BuiltInCommand {
// TODO: Add your data members
    JobsList* jobs_list;
    bool to_kill;
public:
    QuitCommand(const char* cmd_line, JobsList* jobs);
    virtual ~QuitCommand() {}
    void execute() override;
};




class JobsList {
public:
    enum JobStatus {stopped,running};
    class JobEntry {

        // TODO: Add your data members
    public :
        int job_id;
        time_t arrived_time;
        JobStatus job_status;//0 stopeed, 1 running
        Command* command_of_job;
        string command_line_of_job;
        pid_t pid_of_job_entry;


        JobEntry(int job_id, time_t arrived_time,JobStatus job_status, Command* command_of_job, string cmd_line_of_job,pid_t pid_of_job_entry):
                job_id(job_id),arrived_time(arrived_time),job_status(job_status),command_of_job(command_of_job),command_line_of_job(cmd_line_of_job),
                pid_of_job_entry(pid_of_job_entry){}
        JobStatus get_status(){return job_status;}
        int get_job_id(){return job_id;}
        int get_pid(){return command_of_job->getPid();}
        void changeStatus(JobStatus new_status){job_status=new_status;}




    };
    // TODO: Add your data members
//public:
    JobsList();
    ~JobsList();
    void addJob(Command* cmd, JobStatus status, pid_t pid_of_job);
    void printJobsList();
    void killJobs();
    void removeFinishedJobs();
    JobEntry * getJobById(int jobId);
    void removeJobById(int jobId);
    void removeJobByPid(int pid_of_removed_job);
    JobEntry * getLastJob(int* lastJobId);
    JobEntry *getLastStoppedJob(int *jobId);
    // TODO: Add extra methods or modify exisitng ones as needed
    std::vector<JobEntry*> jobs_list_vec;
    int max_id;
    //void deleteSpecificJobByID(int id_to_delete);
    void printSpecificJobByID(int id_to_print);
    int getMaxID(){return max_id;};
    std::vector<JobEntry*> getJobsList(){return jobs_list_vec;}
};

class JobsCommand : public BuiltInCommand {
    // TODO: Add your data members
    JobsList* jobs_;
public:
    JobsCommand(const char* cmd_line):BuiltInCommand(cmd_line){}
    virtual ~JobsCommand() {}
    void execute() override;
};

class KillCommand : public BuiltInCommand {
    // TODO: Add your data members
    JobsList* jobs;
public:
    KillCommand(const char* cmd_line, JobsList* jobs):BuiltInCommand(cmd_line), jobs(jobs){}
    virtual ~KillCommand() {}
    void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
    // TODO: Add your data members
    JobsList* jobs;

public:
    ForegroundCommand(const char* cmd_line, JobsList* jobs):BuiltInCommand(cmd_line), jobs(jobs){}
    virtual ~ForegroundCommand() {}
    void execute() override;
};

class BackgroundCommand : public BuiltInCommand {
    JobsList* jobs;

    // TODO: Add your data members
public:
    BackgroundCommand(const char* cmd_line, JobsList* jobs):BuiltInCommand(cmd_line), jobs(jobs){}
    virtual ~BackgroundCommand() {}
    void execute() override;
};

class CatCommand : public BuiltInCommand {
public:
    CatCommand(const char* cmd_line);
    virtual ~CatCommand() {}
    void execute() override;
};


class SmallShell {
private:
    // TODO: Add your data members
public:
    JobsList* jobs;
    string current_promt; //for the cd command
    Command* curr_external_fg_command;
    TimeOutList* time_outs;
    char* last_direction_command;




    SmallShell();
public:
    Command *CreateCommand(const char* cmd_line);
    SmallShell(SmallShell const&)      = delete; // disable copy ctor
    void operator=(SmallShell const&)  = delete; // disable = operator
    static SmallShell& getInstance() // make SmallShell singleton
    {
        static SmallShell instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    ~SmallShell();
    void executeCommand(const char* cmd_line);
    // TODO: add extra methods as needed
    void changePromtString(string new_prompt){
        this->current_promt=new_prompt;
    }
    string getCurrentPrompt(){return this->current_promt;}
    Command* timeOutCommand(const char* cmd_line);
    std::vector<JobsList::JobEntry*> jobsToSendAlarm();


};

class TimeOutList{
public:
    class TimeOutEntry{
    public:
        time_t insertion_time;
        pid_t pid_of_the_time_entry;
        int duration;
        char* clean_commandline;

        TimeOutEntry(time_t insertion_time, pid_t pid,int duration_left,char* clean_commandline):
                insertion_time(insertion_time),pid_of_the_time_entry(pid), duration(duration_left),clean_commandline(clean_commandline) {}
        ~TimeOutEntry(){}

    };
public:
    std::vector<TimeOutEntry*> timeOuts_vec;
    void addTimeOut(time_t insertion_time,pid_t pid, int duration_left, char* clean_cmd);
    void removeTimeOut(int pid_of_timeout_to_remove);
    TimeOutEntry* closestTimeOut();
    TimeOutEntry* findTimeoutCausedAlarm();

};

specialType checkSpecialType(const char *cmd_line,int* special_loc);
#endif //SMASH_COMMAND_H_
