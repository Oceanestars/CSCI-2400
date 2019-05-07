// 
// tsh - A tiny shell program with job control
// 
// <Oceane Andreis>
//
//cd /bin/ then ls see all the commands you can do (try with -a after)
// reference shell and tsh.cc are the important file
//look at make file to see what to run to check the stuff
//make rtest01 is correct and make test01 is ours it need to look the same
//control d - to exit the shell
//need a space after ampersand
//jobs will tell you the process that are running in the background
//in shell, ./myspin 13 & then do fg %1 
//gdb tsh
//make test03 run foreground which is a quit function
// a job array has all of our processes,so we keep track of it for the shell
//tsh> bg %2 , we added the second job to the background
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string>

#include "globals.h"
#include "jobs.h"
#include "helper-routines.h"

//
// Needed global variable definitions
//

static char prompt[] = "tsh> ";
int verbose = 0;

//
// You need to implement the functions eval, builtin_cmd, do_bgfg,
// waitfg, sigchld_handler, sigstp_handler, sigint_handler
//
// The code below provides the "prototypes" for those functions
// so that earlier code can refer to them. You need to fill in the
// function bodies below.
// 

void eval(char *cmdline);
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

//
// main - The shell's main routine 
//
int main(int argc, char **argv) 
{
  int emit_prompt = 1; // emit prompt (default)

  //
  // Redirect stderr to stdout (so that driver will get all output
  // on the pipe connected to stdout)
  //
  dup2(1, 2);

  /* Parse the command line */
  char c;
  while ((c = getopt(argc, argv, "hvp")) != EOF) {
    switch (c) {
    case 'h':             // print help message
      usage();
      break;
    case 'v':             // emit additional diagnostic info
      verbose = 1;
      break;
    case 'p':             // don't print a prompt
      emit_prompt = 0;  // handy for automatic testing
      break;
    default:
      usage();
    }
  }

  //
  // Install the signal handlers
  //

  //
  // These are the ones you will need to implement
  //
//    ctrl-c (ctrl-z) should cause a SIGINT (SIGTSTP) signal to be sent to the current fore- ground job, 
//     as well as any descendents of that job (e.g., any child processes that it forked). 
//     If there is no foreground job, then the signal should have no effect. 
    
  Signal(SIGINT,  sigint_handler);   // ctrl-c
  Signal(SIGTSTP, sigtstp_handler);  // ctrl-z
  Signal(SIGCHLD, sigchld_handler);  // Terminated or stopped child

  //
  // This one provides a clean way to kill the shell
  //
  Signal(SIGQUIT, sigquit_handler); 

  //
  // Initialize the job list
  //
  initjobs(jobs);

  //
  // Execute the shell's read/eval loop
  //
  for(;;) {
    //
    // Read command line
    //
    if (emit_prompt) {
      printf("%s", prompt);
      fflush(stdout);
    }

    char cmdline[MAXLINE];

    if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin)) {
      app_error("fgets error");
    }
    //
    // End of file? (did user type ctrl-d?)
    //
    if (feof(stdin)) {
      fflush(stdout);
      exit(0);
    }
      //test01

    //
    // Evaluate command line
    //
    eval(cmdline);
    fflush(stdout);
    fflush(stdout);
  } 

  exit(0); //control never reaches here
}
  
/////////////////////////////////////////////////////////////////////////////
//
// eval - Evaluate the command line that the user has just typed in
// 
// If the user has requested a built-in command (quit, jobs, bg or fg)
// then execute it immediately. Otherwise, fork a child process and
// run the job in the context of the child. If the job is running in
// the foreground, wait for it to terminate and then return.  Note:
// each child process must have a unique process group ID so that our
// background children don't receive SIGINT (SIGTSTP) from the kernel
// when we type ctrl-c (ctrl-z) at the keyboard.

// sigprocmask blocks SIGCHLD signals before it forks the child,
// and then unblock these signals, again using sigprocmask after it adds the child to the job list by
// calling addjob. Since children inherit the blocked vectors of their parents, the child must be sure
// to then unblock SIGCHLD signals before it execs the new program.
//
void eval(char *cmdline) 
{
  /* Parse command line */
  //
  // The 'argv' vector is filled in by the parseline
  // routine below. It provides the arguments needed
  // for the execve() routine, which you'll need to
  // use below to launch a process.
  //
  char *argv[MAXARGS];
    pid_t pid; /*Proces Id*/ //variable
    sigset_t mask;

  //
  // The 'bg' variable is TRUE if the job should run
  // in background mode or FALSE if it should run in FG
  //
  int bg = parseline(cmdline, argv); 
  if(argv[0] == NULL)   //argv[0] contains our command
    return;   /* ignore empty lines */
    
    //page 791 trace 2
    //child=0
    
  if(!builtin_cmd(argv)){
      sigprocmask(SIG_BLOCK, &mask, NULL); //blocking synchronous, in pdf
      
       if((pid = fork()) == 0){ //calls fork & creates a child process & if this is a child
           
           sigprocmask(SIG_UNBLOCK, &mask,NULL); //to temporary block the receipt of  signals, unblocking
           setpgid(0,0); //set process group id p795 trace6 child id process group id, changes pid to pgid 
           
           if(execve(argv[0],argv,environ) < 0){//deleted environ after argv
               printf("%s: Command not found. \n", argv[0]); //if it doesn't work
               exit(0); //if it does work then do execve 
               
           }
       }
      int state;
      
       if(bg)
           state = BG; //macro
       else
           state=FG; //we wouldn't add the stop state to jobs array
      
      //addjob( jobs, pid, state, cmdline); //trace4  bg?BG:FG <- 
      
      if(!bg)  //if not bg child
      {
              if(addjob( jobs, pid, state, cmdline)){
                  sigprocmask(SIG_UNBLOCK,&mask ,NULL);//unblock after we add the job to the list) 
                  waitfg(pid);// one foreground at a time trace 5 fix 
                  //wait for fg process to terminate
              }

              else{
                  kill(-pid,SIGINT); //send the signal to every process in the process group with process pid 
              }
          
       }
       else

       {
           if(addjob( jobs, pid, state, cmdline)){
              sigprocmask(SIG_UNBLOCK,&mask ,NULL);
              printf("[%d] (%d) %s",pid2jid(pid),pid,cmdline); 
          }
           else{
               kill(-pid,SIGINT);
           }
       }
           
   }

  return;
}


/////////////////////////////////////////////////////////////////////////////
//
// builtin_cmd - If the user has typed a built-in command then execute
// it immediately. The command name would be in argv[0] and
// is a C string. We've cast this to a C++ string type to simplify
// string comparisons; however, the do_bgfg routine will need 
// to use the argv array as well to look for a job number.
//
int builtin_cmd(char **argv) 
{
  if(!strcmp(argv[0],"quit")) /*quit command*/
      exit(0);
  if(!strcmp(argv[0],"&"))  /*ignore singleton &*/
      return 1;
  if(!strcmp(argv[0],"jobs")){ //trace 5 if the command is jobs then it'll Print the job list 
      listjobs(jobs);
      return 1;
    }
  if(!strcmp(argv[0],"bg")|| !strcmp(argv[0],"fg")) 
    {
        do_bgfg(argv);
        return 1;
    }
  return 0;     /* not a builtin command */
    
}

/////////////////////////////////////////////////////////////////////////////
//
// do_bgfg - Execute the builtin bg and fg commands
//
void do_bgfg(char **argv) 
{
  struct job_t *jobp=NULL;
    
  /* Ignore command if no argument */
  if (argv[1] == NULL) {
    printf("%s command requires PID or %%jobid argument\n", argv[0]);
    return;
  }
    
  /* Parse the required PID or %JID arg */
  if (isdigit(argv[1][0])) {
    pid_t pid = atoi(argv[1]);
    if (!(jobp = getjobpid(jobs, pid))) {
      printf("(%d): No such process\n", pid);
      return;
    }
  }
  else if (argv[1][0] == '%') {
    int jid = atoi(&argv[1][1]);
    if (!(jobp = getjobjid(jobs, jid))) {
      printf("%s: No such job\n", argv[1]);
      return;
    }
  }	    
  else {
    printf("%s: argument must be a PID or %%jobid\n", argv[0]);
    return;
  }

  //
  // You need to complete rest. At this point,
  // the variable 'jobp' is the job pointer
  // for the job ID specified as an argument.
  //
  // Your actions will depend on the specified command
  // so we've converted argv[0] to a string (cmd) for
  // your benefit.
  //
  string cmd(argv[0]);
    
    //fg and bg perform two operations
    //trace 9
    if(jobp) //job pointer for the job id specified as an argument
    {
        kill(-(jobp->pid),SIGCONT);// trace10
        if(cmd=="bg") // if the process is already bg and they want it to become bg it will go here. start of trace 9
        {
            jobp->state =BG;
            printf("[%d] (%d) %s", jobp->jid, jobp->pid, jobp->cmdline);
        }
        else if (cmd == "fg")//if the process is already bg and they want it to become fg it will go here Start of trace 10
        {
            jobp->state = FG;
            waitfg(jobp->pid);
        }
    }
    

  return;
}

/////////////////////////////////////////////////////////////////////////////
//
// waitfg - Block until process pid is no longer the foreground process
//
void waitfg(pid_t pid)
{
    //call fgpid with jobs and compare it to pid
    //and sleep
    //trace 8
    while(fgpid(jobs)==pid){
        sleep(1);
    }
  return;
}

/////////////////////////////////////////////////////////////////////////////
//
// Signal handlers
//


/////////////////////////////////////////////////////////////////////////////
//
// sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
//     a child job terminates (becomes a zombie), or stops because it
//     received a SIGSTOP or SIGTSTP signal. The handler reaps all
//     available zombie children, but doesn't wait for any other
//     currently running children to terminate.  
//     P781
//      p783
//waitpid() waits only for terminated children, 
//> 0	-> meaning wait for the child whose process ID is equal to the value of pid.
//when you send a sigstop to the child can you then revive them?

/* 
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.
 */
// cases
// -child terminate naturally - 1st if
// -ctrlc c and ctrl z - terminated child  -2nd if
/*WNOHANG|WUNTRACED: Return immediately, with a return value of 0, if none of the children in the wait set has stopped or terminated,
 or with a return value equal to the PID of one of the stopped or terminated children.*/
//Reap a zombie child,Using waitpid to reap zombie children in the order they were created
//  WIFSTOPPED(status): Returns true if the child that caused the return is currently stopped.
//WSTOPSIG(status): Returns the number of the signal(not as in how many...) that caused the child to stop. This status is only defined if WIFSTOPPED(status) returned true.
//WIFSIGNALED(status): Returns true if the child process terminated because of a signal that was not caught.
//WTERMSIG(status): Returns the number of the signal that caused the child process to terminate. This status is only defined if WIFSIGNALED(status) returned true.

void sigchld_handler(int sig)
{
    pid_t pid;
    int status;
    
	while((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0)
    {
		if(WIFSTOPPED(status))
		{
			getjobpid(jobs, pid)->state = ST;
			printf("Job [%d] (%d) stopped by signal %d\n", pid2jid(pid), pid, WSTOPSIG(status));
			
			continue;
		}
		if (WIFSIGNALED(status))
			printf("Job [%d] (%d) terminated by signal %d\n", pid2jid(pid), pid, WTERMSIG(status));
			
		deletejob(jobs,pid);
	}

  return;

}

/////////////////////////////////////////////////////////////////////////////
//
// sigint_handler - The kernel sends a SIGINT to the shell whenver the
//    user types ctrl-c at the keyboard.  Catch it and send it along
//    to the foreground job.  
// trace 7
void sigint_handler(int sig) 
{
 if(fgpid(jobs)!=0)  //if there are jobs in the forground process id group (hence !=0) then kill all the jobs(so if there was no jobs then we dont't need to kill)
 {
     kill (-fgpid(jobs), sig);
 }
  return;
}

/////////////////////////////////////////////////////////////////////////////
//
// sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
//     the user types ctrl-z at the keyboard. Catch it and suspend the
//     foreground job by sending it a SIGTSTP.  
// trace 8
void sigtstp_handler(int sig) 
{
    if(fgpid(jobs)!=0) //if there are jobs in the forground process id group (hence !=0) then kill all the jobs(so if there was no jobs then we dont't need to kill)
     {
     kill (-fgpid(jobs), sig);
     }
  return;
}

/*********************
 * End signal handlers
 *********************/




