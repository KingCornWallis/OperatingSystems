#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <deque>
#include <sys/wait.h>

#define MAX_LINE 80 /* The maximum length command */

using namespace std;
// No header file
void parseInput(char** args, string command, int& argc);
bool parseBackground(string& command); //for background processes
void showHistory(deque<string>& history_queue);
void startFork(char** args, int argc, bool inBackground, int& should_run, deque<string>& history_queue);
void run(string command, int& should_run, deque<string>& history_queue);

int main(void)
{
	int should_run = 1; /* flag to determine when to exit program */
	deque <string> history_queue;
  	string command;
  
  	while(should_run)
    	{
      		printf("osh>");
      		fflush(stdout);
      		getline(cin, command);
      		run(command, should_run, history_queue);
    	}
  	return 0;
}

void run(string command, int& should_run, deque<string>&history_queue)
{
	char *args[MAX_LINE/2 + 1]; //command line arguments
	int argc = 0;
	bool inBackground = false;
	
  	for(int i = 0; i < command.length(); i++)
    	{
      		if(command[i] == ' ')
		{
			if(command[i + 1] == ' ')
	    		{	
	     			command.erase(i + 1, 1);
	         		i--;
	    		}
		}
	}
	if(!command.empty())
    	{
      		inBackground = parseBackground(command);
      		parseInput(args, command, argc);	  
      		history_queue.push_back(command);

      		if(strcmp(args[0],"history") == 0)	
			showHistory(history_queue);
		
      		else if(strcmp(args[0], "!!") == 0)
		{
	  		history_queue.pop_back();
	  		if(history_queue.empty())
	    			perror("There is nothing in the history.\n");
	    	}
		else
	    		run(history_queue[history_queue.size() - 1], should_run, history_queue); 
	}

	else if(args[0][0] == '!' && isdigit(args[0][1]))
	{
	 	string toInt;
	 	int nth;
		history_queue.pop_back();
	 	string :: size_type sz;
	 	toInt = command.substr(1, strlen(args[0]));
	 	nth = stoi(toInt, &sz);
		int queue_size = history_queue.size();
	 
		if((nth <= queue_size ) && (nth > (queue_size -10 )) && (nth>0))
	     		run(history_queue[nth - 1], should_run, history_queue);
	 	else	
	     		cout << "Not in history" << endl;
	}
      	else if(strcmp(args[0], "exit") == 0)
		should_run = 0;
	
      	else
		startFork(args, argc, inBackground, should_run, history_queue);
}

void startFork(char** args, int argc, bool inBackground, int& should_run, deque<string>& history_queue)
{
	pid_t pid;
	pid = fork();
  	if(pid < 0)
    		perror("Error: Can't fork\n");
  	if(pid == 0)
    	{
      		if(!inBackground)
		{ // normal case no background work
			if (execvp(args[0], args) < 0) // excute the command
	  		{
	    			cout << "Error: Process failed" << endl;
				exit(1);
	  		}	    
      		}
		else
		{   // run process in background
	  		pid_t pid2;
	  		pid2 = fork();
	  		if(pid2 == 0)
	    		{
	      			if (execvp(args[0], args) < 0) // excute the command
				{
		  			cout << "Error: Process failed" << endl;
		  			exit(1);
				}	
	    		}
	  		else
	      			exit(0);
	    
		}
      		return;
	}	
  	else // parent process
      		wait(NULL);     
}

bool parseBackground(string& command)
{
	if(command.size() > 2 && command[command.size() - 2] == ' ' && command[command.size() - 1] == '&')
    	{	
      		command = command.substr(0, command.size() - 2);
      		return true;
    	}
  	else
     		return false;
}

void parseInput(char** args,string command,int& argc)
{
	int commandIndex = 0;
	for(int index = 0, startingIndex = 0; index <= command.length(); index++)
	{
		if(command[index] == ' ' || command[index] == '\0' || command[index] == '\n') //isNullOrWhiteSpace
	    	{
	      		string substring = command.substr(startingIndex, index - startingIndex);
	      		args[commandIndex] = new char[substring.length() + 1];
	     		copy(substring.begin(), substring.end(), args[commandIndex]);
	      		args[commandIndex][substring.size()] = '\0';
	      		commandIndex++;
	      		startingIndex = index + 1;
	    	}
	  
	}
  	argc = commandIndex;
  	args[argc] = NULL;
}

void showHistory(deque<string>& history_queue)
{
	if(history_queue.size() <= 10)
    	{
      		for(int i = history_queue.size() - 1; i >= 0; i--)
			cout << i + 1 <<". " << history_queue[i] << endl; 
    	}
  	else
    	{	
      		for(int i = history_queue.size() - 1; i >= history_queue.size() - 10; i--)
			cout << i + 1 << ". " << history_queue[i] << endl;
    	}
}
