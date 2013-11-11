// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"
#include "alloc.h"

#include <error.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

int check_for_simple(char check);

command_t make_simple_command(char *simple_string);
command_t make_special_command(enum command_type input);

void join_commands(command_t symb, command_t cmd1, command_t cmd2);
void subshell_commands(command_t symb, command_t sub);
void redirect_commands(command_t symb, command_t input, command_t output);

command_t retrieve_simple_command(char* char_buffer, int* index);

int check_for_special_token(char check);

command_t make_tree(command_t* temp_array, int num_cmds, int* start_index, int num_subshells);




command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{

  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
	
	/* create a previous byte character to see what the previous character was */
	/*char previous_byte = get_next_byte(get_next_byte_argument); */

	/* create a current_byte char to look at the current byte */
	char current_byte; 

	/* create a temporary array to hold all the commands that we will even put together into a tree */
	command_t temp_command = NULL;

	/*char buffer array to take in all the characters from the file in the beginning*/
	char* char_buffer;

	/*index for what element in the character buffer we are currently looking at */
	int buf_index = 0;

	/*malloc initialize size of char buffer to 16, change size of character to indicate that */
	char_buffer = (char *)checked_malloc(256 * sizeof(char));
	size_t size = (size_t) 256 * sizeof(char);
	
	/*for changing subshell commands*/
	int* start_index = (int*) checked_malloc(sizeof(int*));

	/*to create array of commands */
	size_t array_size = (sizeof(command_t) * 1024);
	command_t* temp_array = (command_t*) checked_malloc(array_size);
	int temp_idx = 0;
	int num_cmds = 0;
	
	/*create primary array of commands*/
	command_stream_t output = (command_stream_t) checked_malloc(sizeof(struct command_stream));
	output->command_array = (command_t*) checked_malloc((size_t) sizeof(command_t) * 8);
	output->index = 0;
	output->size = 0;
	
	current_byte = get_next_byte(get_next_byte_argument);
	
	int placeholder;
	
	
	while (current_byte != EOF)
	{
		
		/*realloc memory */
		if (buf_index * sizeof(char) == size)
			{char_buffer = (char *) checked_grow_alloc(char_buffer, &size);}
		char_buffer[buf_index] = current_byte;
		buf_index++ ;
		current_byte = get_next_byte(get_next_byte_argument);
		
	}
	
	char_buffer[buf_index] = current_byte;
	
	/* set buf_index back to 0 once we are done reading */
	buf_index = 0;
	
	/* keep track of line for return error */
	int line_num = 1;

	/* create variable to check if we are in a subshell or not */
	int open_bracket = 0;

	
	char current_buffer = char_buffer[buf_index];
	
	/* checked the bytes in the command buffer we have just created one by one to create commands */	
	while (current_buffer != EOF)
	{
		/* check correct use of paranthesis */
		
		/* store what we are currently looking at in the buffer into a char*/
		

		/* ignore white spaces, tabs, and comment symbol */
		while((current_buffer == ' ') || (current_buffer == '\t') || (current_buffer == '#'))
		{
			/* if comment, ignore until newline */
			if(current_buffer == '#')
					{
						if (buf_index >0)
						{
							if (char_buffer[buf_index-1] != ' ' && char_buffer[buf_index-1] != '\n')
							{
								error(1, 0, "invalid character before #");
							}
						}
					while(current_buffer != '\n')
 					{buf_index++;
					current_buffer = char_buffer[buf_index];}
					}
			else
				{buf_index++;
				current_buffer = char_buffer[buf_index];}
		}
	
		if (check_for_simple(current_buffer) == 1)
		{
			/* put code to retrieve following simple command, and put it into a simple command struct */
			temp_command = retrieve_simple_command(char_buffer, &buf_index);

		}
		else if (check_for_special_token(current_buffer) == 1)
		{
			/* check to see which special token the character currently is and put the command into the proper special command struct" */
			if(current_buffer == '&')
			{
			
			if (temp_idx > 0)
			{
				if(temp_array[temp_idx-1]->type != SIMPLE_COMMAND &&
					temp_array[temp_idx-1]->type != RIGHT_PAREN)
				{ error(1,0, "Two consecutive special commands in line: %i",line_num);	}
			}
				buf_index++;
				current_buffer = char_buffer[buf_index];
				/* check if AND_COMMAND is valid */
				if(current_buffer == '&')
				{
				placeholder = buf_index;
				buf_index++;
					current_buffer = char_buffer[buf_index];
					 
				while(current_buffer == ' ' || current_buffer == '\n' || current_buffer == '\t' || current_buffer == '#')
				{
					/* if comment, ignore until newline */
					if(current_buffer == '#')
					{
					
					if (buf_index >0)
						{
							if (char_buffer[buf_index-1] != ' ' && char_buffer[buf_index-1] != '\n')
							{
								error(1, 0, "invalid character before #");
							}
						}
					while(current_buffer != '\n')
 					{buf_index++;
					current_buffer = char_buffer[buf_index];}
					}
					buf_index++;
					current_buffer = char_buffer[buf_index];
				}
				if(current_buffer == '(' || check_for_simple(current_buffer) == 1)
				{
					
				temp_command = make_special_command(AND_COMMAND);
				buf_index = placeholder;
				current_buffer = char_buffer[buf_index];
				
				}
				else
				{ error(1,0, "Invalid token after '&' in line: %i",line_num);	}		
				}
				/*also check if next command in array of commands is a simple command or a LEFT PAREN */
				
				
							
	/* add and command to array of commands */
					
			}
			else if(current_buffer == '|')
			{
			
			if (temp_idx > 0)
			{
			if(temp_array[temp_idx-1]->type != SIMPLE_COMMAND &&
					temp_array[temp_idx-1]->type != RIGHT_PAREN)
				{ error(1,0, "Two consecutive special commands in line: %i",line_num);	}
			}
				/* check if OR_COMMAND or PIPE_COMMAND is valid */
				buf_index++;
				current_buffer = char_buffer[buf_index];
				if(current_buffer == '|')
				{
					placeholder = buf_index;
					buf_index++;
					current_buffer = char_buffer[buf_index];
							while(current_buffer == ' ' || current_buffer == '\n' || current_buffer == '\t' || current_buffer == '#')
							{
							
							
								/* if comment, ignore until newline */
									if(current_buffer == '#')
									{
									if (buf_index >0)
									{
										if (char_buffer[buf_index-1] != ' ' && char_buffer[buf_index-1] != '\n')
										{
											error(1, 0, "invalid character before #");
										}
									}
									while(current_buffer != '\n')
									{buf_index++;
									current_buffer = char_buffer[buf_index];}
									}
							buf_index++;
							current_buffer = char_buffer[buf_index];
							}
								if(current_buffer == '(' || check_for_simple(current_buffer) == 1)
								{
								temp_command = make_special_command(OR_COMMAND);
								buf_index = placeholder;
								current_buffer = char_buffer[buf_index];
								}
								else
								{
									error(1, 0, "invalid token after '||'");
								}
					buf_index = placeholder;
					current_buffer = char_buffer[buf_index];
					
					
				}
				else if(current_buffer == ' ' || current_buffer == '\n' || 
				current_buffer == '\t' || current_buffer == '(' || check_for_simple(current_buffer) == 1)
					{
					placeholder = buf_index-1;
					while(current_buffer == ' ' || current_buffer == '\n' || current_buffer == '\t' || current_buffer == '#')
							{
								/* if comment, ignore until newline */
									if(current_buffer == '#')
									{
									if (buf_index >0)
									{
										if (char_buffer[buf_index-1] != ' ' && char_buffer[buf_index-1] != '\n')
										{
											error(1, 0, "invalid character before #");
										}
									}
									
									while(current_buffer != '\n')
									{buf_index++;
									current_buffer = char_buffer[buf_index];}
									}
							buf_index++;
							current_buffer = char_buffer[buf_index];
							}
					if(current_buffer == '(' || check_for_simple(current_buffer) == 1)
								{	
								temp_command = make_special_command(PIPE_COMMAND);
								buf_index = placeholder;
								current_buffer = char_buffer[buf_index];
								}
								else
								{
									error(1, 0, "invalid token after '||'");
								}		
					buf_index = placeholder;
					current_buffer = char_buffer[buf_index];
					}
			}
			else if(current_buffer == '(')
			{
				open_bracket++;
				placeholder = buf_index;
				buf_index++;
				current_buffer = char_buffer[buf_index];
				while (current_buffer == ' ' || current_buffer == '\n' || current_buffer == '\t' || current_buffer == '#')
				{
				/* if comment, ignore until newline */
									if(current_buffer == '#')
									{
										if (buf_index >0)
										{
											if (char_buffer[buf_index-1] != ' ' && char_buffer[buf_index-1] != '\n')
											{
												error(1, 0, "invalid character before #");
											}
										}
									while(current_buffer != '\n')
									{buf_index++;
									current_buffer = char_buffer[buf_index];}
									}
					buf_index++;
					current_buffer = char_buffer[buf_index];
				}
				
				if(check_for_simple(current_buffer) == 1)
					{ temp_command = make_special_command(LEFT_PAREN); }
				else
					{ error(1,0, "Invalid token after '(' in line: %i",line_num);	}
				buf_index = placeholder;
				current_buffer = char_buffer[buf_index];
				
			}
			else if(current_buffer == ')')
			{
				
				open_bracket--;
				if(open_bracket < 0)
				{ error(1,0, "')' cannot precede '(' in line: %i",line_num);	}
				temp_command = make_special_command(RIGHT_PAREN);
			}
			else if(current_buffer == '<')
			{
				if (temp_idx >= 2)
				{
					if (temp_array[temp_idx -2]->type == GREATER_THAN)
					{error(1,0, "Invalid token after '<' in line: %i",line_num);}
				}
				placeholder = buf_index;
				buf_index++;
				current_buffer = char_buffer[buf_index];
				while(current_buffer == ' ' || current_buffer == '\t')
				{
				buf_index++;
				current_buffer = char_buffer[buf_index];
				}
				if (check_for_simple(current_buffer))
				{
					temp_command = make_special_command(LESS_THAN);
					buf_index = placeholder;
					current_buffer = char_buffer[buf_index];
				}
				else
				{ error(1,0, "Invalid token after '<' in line: %i",line_num);}	
			}
			else if(current_buffer == '>')
			{
				placeholder = buf_index;
				buf_index++;
				current_buffer = char_buffer[buf_index];
				while(current_buffer == ' ' || current_buffer == '\t')
				{
				buf_index++;
				current_buffer = char_buffer[buf_index];
				}
				if (check_for_simple(current_buffer))
				{
					temp_command = make_special_command(GREATER_THAN);
					buf_index = placeholder;
					current_buffer = char_buffer[buf_index];
				}
				else
				{ error(1,0, "Invalid token after '>' in line: %i",line_num);	}
			}
			else if(current_buffer == ';' && open_bracket > 0 && temp_idx != 0)
			{
					placeholder = buf_index;
					buf_index++;
					current_buffer = char_buffer[buf_index];
					while (current_buffer == ' ' || current_buffer== '\n' || 
						current_buffer == '\t' || current_buffer == '#')
					{
						if (current_buffer == '#')
						{
						if (buf_index >0)
										{
											if (char_buffer[buf_index-1] != ' ' && char_buffer[buf_index-1] != '\n')
											{
												error(1, 0, "invalid character before #");
											}
										}
						
						while(current_buffer != '\n')
									{buf_index++;
									current_buffer = char_buffer[buf_index];}
									}
						buf_index++;
						current_buffer = char_buffer[buf_index];
					}
					if( check_for_simple(current_buffer) == 1)
					{
						temp_command = make_special_command(SEQUENCE_COMMAND);
						buf_index = placeholder;
						current_buffer = char_buffer[buf_index];
					}
					else if (current_buffer == '(')
					{
						temp_command = make_special_command(SEQUENCE_COMMAND);
						buf_index = placeholder;
						current_buffer = char_buffer[buf_index];
					}
					else if (current_buffer == ')')
					{
						buf_index = placeholder;
						current_buffer = char_buffer[buf_index];
					}
					else
					{ error(1,0, "Invalid token after ';' in line: %i",line_num);	}
			}
			else if(current_buffer == ';'  && temp_idx == 0)
			{error(1,0, "Invalid token in line: %i",line_num);}
			
		}
		else if (current_buffer == '#')
			{
				if(buf_index >0)
				{
					if (char_buffer[buf_index-1] != ' ')
					{error(1,0, "Invalid token in line: %i",line_num);}
				}	
				
				while(current_buffer != '\n')
 					{buf_index++;
					current_buffer = char_buffer[buf_index];}
			}
		else if (current_buffer != '\n' && current_buffer != EOF)
		{error(1,0, "Invalid token in line: %i",line_num);}
		
		/*check if we are at a newline and we are not in a subshell command */
		if(((current_buffer == '\n') || (current_buffer == ';')) && (temp_idx != 0))
		{
			if (open_bracket == 0)
			{
				if (temp_array[temp_idx-1]->type == SIMPLE_COMMAND||
				temp_array[temp_idx-1]->type == RIGHT_PAREN)
				{				
				output->command_array[output->size] = make_tree(temp_array, num_cmds, start_index, 0);
				output->size++;
				
				*start_index = 0;
				
				free(temp_array);
					
				array_size = (size_t) (sizeof(command_t) * 1024);
				temp_array = (command_t*) checked_malloc(array_size);
				temp_idx = 0;
				num_cmds = 0;
				}		
			
			}
			else if (open_bracket>0)
			{
				if (current_buffer == '\n')
				{
					if (temp_idx > 0)
					{
						if (temp_array[temp_idx-1]->type == SIMPLE_COMMAND)
						{	
							placeholder = buf_index;
							buf_index++;
							current_buffer = char_buffer[buf_index];
							while (current_buffer == ' ' || current_buffer == '\n')
							{
								buf_index++;
								current_buffer = char_buffer[buf_index];
							}
							if (current_buffer == '(' || check_for_simple(current_buffer))
							{
							temp_command = make_special_command(SEQUENCE_COMMAND);
							/*check if we need to resize */
							if ((temp_idx * sizeof(command_t)) == array_size)
							temp_array = (command_t*) checked_grow_alloc(temp_array, &array_size);
			
							temp_array[temp_idx] = temp_command;
							temp_idx++;
							num_cmds++;
							}
							buf_index = placeholder;
							current_buffer = char_buffer[buf_index];
							
						}
					
					}
					/* else do nothing */
				}
				else if (current_buffer == ';')
				{
					if (temp_idx > 0)
					{
						if (temp_command->type == SEQUENCE_COMMAND)
						{
							if (temp_array[temp_idx-1]->type == SIMPLE_COMMAND)
							{
								/*check if we need to resize */
								if ((temp_idx * sizeof(command_t)) == array_size)
								temp_array = (command_t*) checked_grow_alloc(temp_array, &array_size);
				
								temp_array[temp_idx] = temp_command;
								temp_idx++;
								num_cmds++;
							}
						}
					}
				}
			}
		}
			
		/*add command to temporary command array */
		if (current_buffer != '\n' && current_buffer != ';' && current_buffer != EOF)
			{		
				if(temp_idx == 0)
				{
					if(temp_command->type != SIMPLE_COMMAND && temp_command->type != LEFT_PAREN)
					{ error(1,0, "The first command must be a simple command or open parenthesis in line: %i",line_num-1);	}
				}
				
				/*check if we need to resize */
				if ((temp_idx * sizeof(command_t)) == array_size)
				temp_array = (command_t*) checked_grow_alloc(temp_array, &array_size);
				
				temp_array[temp_idx] = temp_command;
				temp_idx++;
				num_cmds++;
			}
		
		buf_index++;
		current_buffer = char_buffer[buf_index];
	
		if((current_buffer == EOF) && (temp_idx != 0))
		{
				if (open_bracket > 0)
				{ error(1,0, "Unclosed parenthesis in line: %i",line_num-1);}
				if (temp_idx > 0)
				{
					if (temp_array[temp_idx-1]->type != SIMPLE_COMMAND && temp_array[temp_idx-1]->type != RIGHT_PAREN)
					{	
					error(1, 0, "last command is not a simple command or right paren");
					}
				}
				output->command_array[output->size] = make_tree(temp_array, num_cmds, start_index, 0);
				output->size++;
				
				*start_index = 0;
				
				free(temp_array);	
		}
	
	}

	
	/* previous byte becomes current byte */
	/*previous_byte = current_byte;*/

	return output;
}

command_t retrieve_simple_command(char* char_buffer, int* index)
{
	
	/* code to retrieve simple command 
	should pass its char buffer into make_simple_command to create the actual struct
	*/

	/*temporary initial size of buffer, will resize if needed */
	size_t buffer_size = (size_t) (sizeof(char) * 16);

	/*char buffer for array */
	char* buffer;
	
	buffer = (char*) checked_malloc(buffer_size); 

	int position = 0;
	
	/*will be the char of the head of the stream we are looking at to retrieve the simple command */ 
	char head = char_buffer[*index];

	while (head != EOF && head != '\n' && head != ';')
	{	
			/* we have a valid simple word character that we need to attach to the buffer */
			if (check_for_simple(head) == 1 || head == ' ')
			{
				if (((position+1) * sizeof(char)) == buffer_size)
					{
						buffer = (char*) checked_grow_alloc(buffer, &buffer_size);
					}
					
			
						buffer[position] = head;
						
						position++;
						*index = *index + 1;
						head = char_buffer[*index];
						
			}
			else
			{
			buffer[position] = '\0';
				/*free the buffer since we are done using it */
				*index = *index - 1;
				/*we are done retrieving all the simple command characters so we can create the simple command */			
				return make_simple_command(buffer);
				
			}
	}	
	
	
	buffer[position] = '\0';

	*index = *index - 1;
	
	command_t temp = make_simple_command(buffer);

	return temp;
}

/*code to create a simple command struct */
command_t make_simple_command(char *simple_string)
{

	/*malloc a new command struct */
	command_t simplecommand = (command_t)checked_malloc(sizeof(struct command));

	int position = 0;

	/*split string into words by using strtok */
	char *split_string = strtok(simple_string, " ");
	
	/*set its appropriate variables */
	simplecommand->type = SIMPLE_COMMAND;

	simplecommand->status = 0;

	simplecommand->input = NULL;

	simplecommand->output = NULL;
	
	simplecommand->has_parent = false;

	/* temporary size of words array, 8 bytes to store 4 words */
	size_t newsize = (size_t) (8 * sizeof(char*));

	simplecommand->u.word = (char **)checked_malloc(newsize);
	while(split_string != NULL)
	{
	
	if ((position * sizeof(char*)) == newsize)
	{simplecommand->u.word = (char **)checked_grow_alloc(simplecommand->u.word, &newsize);}
		
	/*set the word variable in the union of the command struct to be the simple command word */
	

	simplecommand->u.word[position] = (char *)checked_malloc(strlen(split_string) + 1);
	
	strcpy(simplecommand->u.word[position], split_string);

	position++;

	/*get the next word in the splitted string */
	split_string = strtok(NULL, " ");

	}
	
	simplecommand->u.word[position] = '\0';
	
	return simplecommand;

}

command_t
make_special_command(enum command_type type_input)
{
	command_t cmd = (command_t)checked_malloc(sizeof(struct command));
	
	cmd->type = type_input;
	cmd->status = 2;
	cmd->input = NULL;
	cmd->output = NULL;
	cmd->layer = 0;

	return cmd;
}

void
join_commands(command_t symb, command_t cmd1, command_t cmd2)
{
	symb->u.command[0] = cmd1;
	symb->u.command[1] = cmd2;
	cmd1->has_parent = 1;
	cmd2->has_parent = 1;
}

void
subshell_commands(command_t symb, command_t sub)
{
	symb->u.subshell_command = sub;
}

void redirect_commands(command_t symb, command_t command_input, command_t command_output)
{	
	size_t input_size= 32 * sizeof(char);
	int index = 0;
	command_input->input = (char *) checked_malloc(32 * sizeof(char));
	command_input->input[0] = '\0';
	
	size_t output_size= 32 * sizeof(char);
	command_output->output = (char *) checked_malloc(32 * sizeof(char));
	command_output->output[0] = '\0';
	
	char* nothing = (char*) checked_malloc(sizeof(char*));
	nothing[0] = '\0';
	
	if (symb-> type == GREATER_THAN)
	{
		/*if (command_output->output != NULL)
		{ error(1,0, "Invalid token for output in line: 0 ");} */
	
			if (command_input->u.word[index] != NULL)
			{
			if (sizeof(command_input->u.word[index]) >= output_size)
				{command_output->output = (char*) checked_grow_alloc(command_output->output, &output_size);}
			strcat(command_output->output, command_input->u.word[index]);
			index++;
			}

	}
	
	index = 0;
	
	if (symb-> type == LESS_THAN)
	{
		/*if (command_input->input != NULL)
		{ error(1,0, "Invalid token for input in line: 0");} */
	
			if (command_output->u.word[index] != NULL)
			{

			if (sizeof(command_output->u.word[index]) >= input_size)
				{command_input->input = (char*) checked_grow_alloc(command_input->input, &input_size);}
			
			strcat(command_input->input, command_output->u.word[index]);
			index++;
			
			}

	}
	
	
	
	
		if (symb->type == LESS_THAN)
	{
		symb->has_parent = 1;
		command_output->has_parent = 1;
	}
	
	if (symb->type == GREATER_THAN)
	{
		symb->has_parent = 1;
		command_input->has_parent = 1;
	}
		
}

int check_for_simple(char check)
{
	if ( isalpha(check) || isdigit(check) || check == '!' || check == '%' || check == '+' || check == ',' || check == '-' || check == '.' || check == '/' || check == ':' || check == '@' || check ==  '^' || check == '_')
	{return 1;}
else {return 0;}
				
}

int check_for_special_token(char check)
{
	if (check == ';' || check == '|' || check == '&' ||
     check == '(' || check == ')' || check == '<' || check == '>')
	return 1;
	else return 0; 
}

command_t
make_tree(command_t* temp_array, int size, int* start_index, int num_subshells)
{

	int idx = *start_index;
	int permanent_start = *start_index;
	int bidx;
	int fidx;
	int num_cmds = size;
	
	/*temporary command for subshell*/
	command_t cmd = (command_t)checked_malloc(sizeof(struct command));
	
	/*temporary command for subshell used to set parent*/
	command_t cmd2 = (command_t)checked_malloc(sizeof(struct command));
	
	/* check if first command is simple */
	if(temp_array[0]->type != SIMPLE_COMMAND && temp_array[0]->type != LEFT_PAREN)
	{ error(1,0, "First ccommand must be simple command or subshell command");	}
	
	/*
	while(idx < num_cmds)
	{
		if(temp_array[idx]->type == LEFT_PAREN)
		{ layer++; max_layer++; temp_array[idx]->has_parent = 1; }
		else if(temp_array[idx]->type == RIGHT_PAREN)
		{ layer--; temp_array[idx]->has_parent = 1; }
		else
		{ temp_array[idx]->layer = layer; }

		idx++;
	}
	*/

	
		
		while(idx < num_cmds)
		{
			if (temp_array[idx]->type == LEFT_PAREN)
			{
				/* create a subshell command */
				/* set the value of make_tree to it */
	
			cmd->type = SUBSHELL_COMMAND;
			cmd->status = 2;
			cmd->input = NULL;
			cmd->output = NULL;
			
			*start_index = idx + 1;
			
			cmd2 = make_tree(temp_array, num_cmds, start_index, num_subshells+1);
			cmd2->has_parent =1;
		
			cmd->u.subshell_command = cmd2;
			
			
			/*set the left paren slot in the array to the new subshell command we have created */
			temp_array[idx] = cmd;
			
			}

			idx++;
		}
		
		idx = permanent_start;
	
		while(idx < num_cmds)
		{
			/* return the tree that we have finished up one level from the subshell command */
			if (temp_array[idx]->type == RIGHT_PAREN && num_subshells > 0 && temp_array[idx]->layer ==0)
			{
				num_cmds = idx;
				
				temp_array[idx]->layer = 1;
				temp_array[idx]->has_parent = 1;	
			}
			
			idx++;
		}
	
		idx = permanent_start;
		while(idx < num_cmds)
		{
			if((temp_array[idx]->type != LESS_THAN && 
				temp_array[idx]->type != GREATER_THAN))
			{ idx++; }
			else
			{
				if (temp_array[idx]->has_parent ==0)
				{
				bidx = idx-1;
				fidx = idx+1;
			
				while(temp_array[bidx]->has_parent == 1)
				{bidx--; }
				while(temp_array[fidx]->has_parent == 1)
				{
				
				fidx++; 
				}			
			
				/*code to set the redirection of simple commands for input*/
				if (temp_array[idx]->type == LESS_THAN)
				{
					redirect_commands(temp_array[idx], temp_array[bidx], temp_array[fidx]); 
				}
			
				/*code to set the redirection of simple commands for output*/
				if (temp_array[idx]->type == GREATER_THAN)
				{
					redirect_commands(temp_array[idx], temp_array[fidx], temp_array[bidx]); 
				}
				
				}
				else
				{
				idx++;
				}
			}
		}
	
		idx = permanent_start;
	
		while(idx < num_cmds)
		{
			if(temp_array[idx]->type != PIPE_COMMAND)
			{ idx++; }
			else
			{	
				if (temp_array[idx]->has_parent ==0)
				{
				temp_array[idx]->layer--;
				bidx = idx-1;
				fidx = idx+1;
				while(temp_array[bidx]->has_parent == 1)
				{ bidx--; }
				while(temp_array[fidx]->has_parent == 1)
				{ fidx++; }
				join_commands(temp_array[idx], temp_array[bidx], temp_array[fidx]);
				idx = fidx + 1;
				}
				else
				{idx++;}
			}
		}	
	
		idx = permanent_start;
	
		while(idx < num_cmds)
		{
			if((temp_array[idx]->type != AND_COMMAND &&
				temp_array[idx]->type != OR_COMMAND))
			{ idx++; }
			else
			{
			if (temp_array[idx]->has_parent ==0)
				{
				temp_array[idx]->layer--;
				bidx = idx-1;
				fidx = idx+1;
				while(temp_array[bidx]->has_parent == 1)
				{ bidx--; }
				while(temp_array[fidx]->has_parent == 1)
				{ fidx++; }
				join_commands(temp_array[idx], temp_array[bidx], temp_array[fidx]);
				idx = fidx + 1;
				
				}
				else
				{
				idx++;
				}
			}
		}
	
		idx = permanent_start;
	
		while(idx < num_cmds)
		{
			if(temp_array[idx]->type != SEQUENCE_COMMAND)
			{ idx++; }
			else
			{
			
			if (temp_array[idx]->has_parent ==0)
				{
				temp_array[idx]->layer--;
				bidx = idx-1;
				fidx = idx+1;
				while(temp_array[bidx]->has_parent == 1)
				{ bidx--; }
				while(temp_array[fidx]->has_parent == 1)
				{ fidx++; }
				join_commands(temp_array[idx], temp_array[bidx], temp_array[fidx]);
				idx = fidx + 1;
				}
				else
				{
				idx++;
				}
			}
		}
	
	idx = permanent_start;
	
	/* find and return the only node without parents (head node) */
		while(idx < num_cmds)
		{
			if(temp_array[idx]->has_parent == false)
			{ return temp_array[idx]; }
			idx++;
		}
	
	return NULL;
}

command_t
read_command_stream (command_stream_t s)
{
  int temp = s->index;
	s->index++;
	if (temp < s->size)
	{
	return s->command_array[temp];
	}
	else
	{return NULL;}
}
