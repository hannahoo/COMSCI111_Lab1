// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"

#include "alloc.h"

#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>


#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>

void free_command(command_t c);
void get_files(command_t c, char ***input, char ***output, int *ip_size, int *op_size);
void copy_file(char **storage, char *file_name, int *file_idx);
int* create_dependencygraph(command_stream_t stream);



bool check_dependency(fstruct_t command1, fstruct_t command2);

fstruct_t create_fstruct()
{
	fstruct_t new_struct = (fstruct_t)checked_malloc(sizeof(struct fstruct));
	new_struct->op_array = (char**)checked_malloc(8*sizeof(char*));
	new_struct->ip_array = (char**)checked_malloc(8*sizeof(char*));
	new_struct->input_size = 0;
	new_struct->output_size = 0;
	return new_struct;
}

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

void
free_command (command_t c)
{
	if(c->type == SIMPLE_COMMAND)
	{
		/*free(c->input);
		free(c->output);
		free(c->c.word);*/
	}
	else if(c->type == SUBSHELL_COMMAND)
	{
		free_command(c->u.subshell_command);
	}
	else
	{
		free_command(c->u.command[0]);
		free_command(c->u.command[1]);
	}
	free(c);
}

int
command_status (command_t c)
{
	/* 
	0: success
	1: fail
	-1: unknown 
	*/
	return c->status;
}

void
execute_command (command_t c, bool time_travel)
{
	/*if (c == NULL)
		error(1, 0, "Unrecognized command");
		// error (1, 0, "command execution not yet implemented");
	
	fstruct_t test_file = create_fstruct();
	
	get_files(c, &test_file->ip_array, &test_file->op_array, &test_file->input_size, &test_file->output_size);
	int i;
	for (i = 0; i < test_file->input_size; i++)
	{
		printf("%s\n", test_file->ip_array[i]);
	}
	for (i = 0; i < test_file->output_size; i++)
	{
		printf("%s\n", test_file->op_array[i]);
	}
	exit(0);*/
	

	if (time_travel == true)
	{error (1, 0, "command execution not yet implemented");}
	 else
	 {
		switch (c->type)
		{
		case AND_COMMAND:
			execute_command(c->u.command[0], time_travel);
			if (command_status(c->u.command[0]) == 0)
			{
				execute_command(c->u.command[1], time_travel);
				c->status = command_status(c->u.command[1]);
			}
			else
			{
				c->status = command_status(c->u.command[0]);
			}
			break;
		
		
		case OR_COMMAND:
			execute_command(c->u.command[0], time_travel);
			if (command_status(c->u.command[0]) == 0)
			{
				c->status = 0;
			}
			else
			{
				execute_command(c->u.command[1], time_travel);
				c->status = command_status(c->u.command[1]);
			}
			break;
			
		case SEQUENCE_COMMAND:
			execute_command(c->u.command[0], time_travel);
			execute_command(c->u.command[1], time_travel);
			c->status = command_status(c->u.command[1]);
			break;
		
		case SUBSHELL_COMMAND:
			{
					int stored_stdout = dup(1);
					int stored_stdin = dup(0);
					
					/*if there is an input file, redirect the standard input file descriptor */
					if (c->input != NULL)
					{
						int fd_input;
						fd_input = open(c->input, O_RDONLY);
						if (fd_input == -1)
						{
							error(1, 0, "File input: %s could not be opened", c->input); 
						}
						dup2(fd_input, 0);
						close(fd_input);
					}
					
					if (c->output != NULL)
					{
						int fd_output;
						fd_output = open(c->output, O_WRONLY | O_CREAT |O_TRUNC, 0666);
						if (fd_output == -1)
						{
							error(1, 0, "File output: %s could not be opened", c->input); 
						}
						dup2(fd_output, 1);
						close(fd_output);
					}
				
					/*execute the command */
					execute_command(c->u.subshell_command, time_travel);
					c->status = c->u.subshell_command->status;
					dup2(stored_stdin,0);
					dup2(stored_stdout,1);
					close(stored_stdin);
					close(stored_stdout);
			}
			/*execute_command(c->u.subshell_command, time_travel);
			c->status = command_status(c->u.subshell_command);*/
			break;
			
		case PIPE_COMMAND:
		{
			int pipefd[2];
			int input_status;
			int output_status;
			if (pipe(pipefd) == -1)
			{
				error(1, 0, "piping on file descriptors failed");
			}
			
			/*fork a child process for the input*/
			pid_t input_pid = fork();
			
			/*we are in child process for taking in the input */
			if (input_pid == 0)
			{
				/* set the standard output, STDOUT_FILENO, equal to the pipe input */
				dup2(pipefd[1],1);
				
				/* close the pipe's file descriptors we only need the standard output now */
				close(pipefd[1]);
				close(pipefd[0]);
				
				execute_command(c->u.command[0], time_travel);
				_exit(command_status(c->u.command[0]));
				
			}
			else if (input_pid > 0)
			{
				/*for a child process for the output */
				pid_t output_pid = fork();
				
				if (output_pid == 0)
				{
					dup2(pipefd[0], 0);
					close(pipefd[1]);
					execute_command(c->u.command[1], time_travel);
					_exit(command_status(c->u.command[1]));
				}
				else if (output_pid > 0)
				{
					close(pipefd[0]);
					close(pipefd[1]);
					
					waitpid(input_pid, &input_status, 0);
					waitpid(output_pid, &output_status, 0);
					c->status = output_status;				
				}
			
			}
			else if (input_pid < 0)
			{
				error(1, 0, "error with forking on the pid");
			}
		}
		break;
		
		case SIMPLE_COMMAND:
			{
				pid_t simple_pid = fork();
				
				int child_status;
				
				/*in child process */
				if (simple_pid == 0)
				{
					/*if there is an input file, redirect the standard input file descriptor */
					if (c->input != NULL)
					{
						int fd_input;
						fd_input = open(c->input, O_RDONLY);
						if (fd_input == -1)
						{
							error(1, 0, "File input: %s could not be opened", c->input); 
						}
						
						dup2(fd_input, 0);
						close(fd_input);
					}
					
					if (c->output != NULL)
					{
						int fd_output;
						fd_output = open(c->output, O_WRONLY | O_CREAT| O_TRUNC, 0666);
						if (fd_output == -1)
						{
							error(1, 0, "File output: %s could not be opened", c->input); 
						}
						
						dup2(fd_output, 1);
						close(fd_output);
					}
					
					/*special condition where first command is exec*/
					if (strcmp(c->u.word[0], "exec") == 0)
					{
					execvp(c->u.word[1], c->u.word+1);
					}
					else
					{
					/*execute the command */
					execvp(c->u.word[0], c->u.word);
					}
					
					/*after executing, it should leave the child process*/
					/*if not, output to error */
					error(1, 0, "command %s could not be executed", c->u.word[0]);
				}
				else if (simple_pid > 0)
				{
					/*wait for the child process to finish and get its status*/
					waitpid(simple_pid, &child_status, 0);					
					c->status = child_status;
				}
				else if (simple_pid < 0)
				{
					error(1, 0, "simple command did not fork correctly");
				}		
			}
			break;
			
		case LEFT_PAREN:
		case RIGHT_PAREN:
		case GREATER_THAN:
		case LESS_THAN:
			error(1, 0, "not a valid type");
		}
		
		
	 }
  // error (1, 0, "command execution not yet implemented");
}

void execute(command_stream_t stream)
{
	int size = stream->size;
	int* graph = (int*) checked_malloc(stream->size * sizeof(int));
	graph = create_dependencygraph(stream);
	int i;
	int iterations = 0;
	int start = 0;
	/*pid_t pid = -1;*/
	
	/*pid_t* childpids = (pid_t *) checked_malloc(size * sizeof(pid_t));*/
	
	
	
	for (i = 0; i < size; i++)
	{
		
		if (graph[i] > iterations)
		{
			
			iterations = graph[i];
		}
	}
	
	while (start <= iterations)
	{
		/*fork off the shell to execute all the commands for one dependency*/
		/*pid_t pid = fork();*/
		
		pid_t* childpids =  (pid_t*) checked_malloc(size * sizeof(pid_t));
	
		int intervalcount = 0;
	/*	if (pid == 0)
		{*/
		pid_t currentpid;
			for (i = 0; i< size; i++)
			{
				if (graph[i] == start)
				{
					currentpid = fork();
					if (currentpid < 0)
					{
						error(1, 0, "error");
					}
					else if(currentpid == 0)
					{
					execute_command(stream->command_array[i], false);
					_exit(stream->command_array[i]->status);
					}
					childpids[intervalcount] = currentpid;
					intervalcount++;
				}
			
			}
			
			pid_t newpid;
			int status;
			for (i = 0; i < intervalcount; i++)
			{
					newpid = waitpid(childpids[i], &status,0);
					if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
					{
						printf("status is %d", status);
						error(1, 0, "did not exit correctly");
					}
					if (newpid < 0)
						{error(1, 0, "wait error");}
						//intervalcount--;
			}
			intervalcount = 0;
			
	/*	}
		else
		{*/
		
		/*wait for the child processes to finish */
	/*	int status;
		pid_t newpid = waitpid(-1, &status, 0);
		if (newpid < 0)
		{
			error(1, 0, "exit wrong");
		}
		}*/
	
		start++;

	}
}

void execute_limit(command_stream_t stream, int num_of_processes)
{
	int size = stream->size;
	int* graph = (int*) checked_malloc(stream->size * sizeof(int));
	graph = create_dependencygraph(stream);
	int i;
	int iterations = 0;
	int start = 0;
	
		
	bool still_running = true;
	
	for (i = 0; i < size; i++)
	{
		if (graph[i] > iterations)
		{
			
			iterations = graph[i];
		}
	}
	
	while (start <= iterations)
	{
		still_running = true;
	
		while (still_running)
		{
		pid_t* childpids =  (pid_t*) checked_malloc(size * sizeof(pid_t));
	
		int intervalcount = 0;
		
		still_running = false;
		
		int subprocess_size;
		
		if (num_of_processes > 0)
		{
			subprocess_size = num_of_processes;
		}
		else
		{
			subprocess_size = 1;
		}

		pid_t currentpid;
			for (i = 0; i < size; i++)
			{
				if (graph[i] != -1)
				{						
					if (graph[i] == start)
					{
						if (intervalcount < subprocess_size)
						{
						graph[i] = -1;
						currentpid = fork();
						still_running = true;
						if (currentpid < 0)
						{
							error(1, 0, "error");
						}
						else if(currentpid == 0)
						{
						execute_command(stream->command_array[i], false);
						_exit(stream->command_array[i]->status);
						}
						childpids[intervalcount] = currentpid;
						
						intervalcount++;
						
						}
						else
						{break;}
					}
				}
			
			}
			
			pid_t newpid;
			int status;
			for (i = 0; i < intervalcount; i++)
			{
					newpid = waitpid(childpids[i], &status,0);
					if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
					{
						printf("status is %d", status);
						error(1, 0, "did not exit correctly");
					}
					if (newpid < 0)
						{error(1, 0, "wait error");}
			}
			intervalcount = 0;
			
		}
	
		start++;

	}
}


/* Time Travel */

int*
create_dependencygraph(command_stream_t stream)
{
	
	int i, j;
	
	int *dependency_array = (int*) checked_malloc(stream->size * sizeof(int*));
	

	fstruct_t* files_array = (fstruct_t*) checked_malloc(stream->size * sizeof(fstruct_t));
	
	/*initially there are no dependencies */
	for (i = 0; i < stream->size; i++)
	{
		dependency_array[i] = 0;
	}
	
	/*first get all the files for each command in the command stream and store them in a fstruct array */
	for (i = 0; i < stream->size; i++)
	{
		files_array[i]= create_fstruct();
		get_files(stream->command_array[i], &files_array[i]->ip_array, &files_array[i]->op_array, &files_array[i]->input_size, &files_array[i]->output_size);
	}

	/*check the files array and set the dependencies accordingly 
	we start at 1 since the first command never has any dependencies*/
	for (i = 1; i < stream->size; i++)
	{
		for (j = 0; j < i; j++)
		{
			if (check_dependency(files_array[j], files_array[i]))
			{
				if ((dependency_array[j] + 1)> dependency_array[i])
				{
					dependency_array[i] = dependency_array[j] +1;
				}
			}
		}
	}
	return dependency_array;
}

bool check_dependency(fstruct_t command1, fstruct_t command2)
{
	int i, j;

	for (i = 0; i< command1->output_size; i++)
	{
		for (j = 0; i< command2->input_size; i++)
		{
			if (command1->op_array[i] == NULL|| command2->ip_array[j] == NULL)
			{
				break;
			}
			if (strcmp(command1->op_array[i], command2->ip_array[j]) == 0)
				{return true;}
		}
	}
	
	for (i = 0; i< command1->output_size; i++)
	{
		for (j = 0; i< command2->output_size; i++)
		{
			if (command1->op_array[i] == NULL|| command2->op_array[j] == NULL)
			{
				break;
			}
		
			if (strcmp(command1->op_array[i], command2->op_array[j]) == 0)
				{return true;}
		}
	}
	
	
	for (i = 0; i< command1->input_size; i++)
	{
		for (j = 0; i< command2->output_size; i++)
		{
			if (command1->ip_array[i] == NULL|| command2->op_array[j] == NULL)
			{
				break;
			}
			if (strcmp(command1->ip_array[i], command2->op_array[j]) == 0)
				{return true;}
		}
	}
	
	return false;
}

void
get_files(command_t cmd, char ***ip_array, char ***op_array, int *input_size, int *output_size)
{
	if(cmd == NULL) {
		return;
	}
	
	switch(cmd->type) {
		case AND_COMMAND:
		case SEQUENCE_COMMAND:
		case OR_COMMAND:
		case PIPE_COMMAND:
			get_files(cmd->u.command[0],ip_array,op_array,input_size,output_size);
			get_files(cmd->u.command[1],ip_array,op_array,input_size,output_size);
			break;
		case SUBSHELL_COMMAND:
			if(cmd->input != NULL) {
				*ip_array = (char**)checked_realloc(*ip_array,(*input_size+1)*sizeof(char*));
				copy_file(*ip_array,cmd->input,input_size);
			}
			if(cmd->output != NULL) {
				*op_array = (char**)checked_realloc(*op_array,(*output_size+1)*sizeof(char*));
				copy_file(*op_array,cmd->output,output_size);
			}
			get_files(cmd->u.subshell_command,ip_array,op_array,input_size,output_size);
			break;
		case SIMPLE_COMMAND:
			if(cmd->input != NULL) {
				*ip_array = (char**)checked_realloc(*ip_array,(*input_size+1)*sizeof(char*));
				copy_file(*ip_array,cmd->input,input_size);
			}
			if(cmd->output != NULL) {
				*op_array = (char**)checked_realloc(*op_array,(*output_size+1)*sizeof(char*));
				copy_file(*op_array,cmd->output,output_size);
			}
			char **w = cmd->u.word;
			
			/*start at the second word, first word is the command */
			w++;
			while(*w) {
				*ip_array = (char**)checked_realloc(*ip_array,(*input_size+1)*sizeof(char*));
				copy_file(*ip_array,*w,input_size);
				w++;
			}
			break;
		default:
			return;
	}
	return;
}

void
copy_file(char **storage, char *file_name, int *file_idx)
{
	/*if(storage)
		storage = (char**)checked_realloc(storage,(*file_idx+1)*sizeof(char*));
	else
		{storage = (char**)checked_malloc(sizeof(char*));} */
	
	storage[*file_idx] = (char *)checked_malloc(strlen(file_name)+1);	
	strcpy(storage[*file_idx], file_name);
	(*file_idx)++;
}