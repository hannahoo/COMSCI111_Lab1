// UCLA CS 111 Lab 1 main program

#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "command.h"

static char const *program_name;
static char const *script_name;

static void
usage (void)
{
  error (1, 0, "usage: %s [-pt] SCRIPT-FILE", program_name);
}

static int
get_next_byte (void *stream)
{
  return getc (stream);
}

int
main (int argc, char **argv)
{
  int command_number = 1;
  bool print_tree = false;
  bool time_travel = false;
	bool time_travel_limit = false;
	int num_of_processes =0;
	program_name = argv[0];
	char c = ' ';

  for (;;)
    switch (getopt (argc, argv, "ptj"))
      {
      case 'p': print_tree = true;c = 'p'; break;
      case 't': time_travel = true; c = 't';break;
			case 'j': time_travel_limit = true;
								num_of_processes = atoi(argv[2]);
								c = 'j';
								break;
      default: usage (); break;
      case -1: goto options_exhausted;
      }
 options_exhausted:;

  // There must be exactly one file argument.
  if (optind != argc - 1)
	{
		if (argc > 1 && c == 'j' && optind == argc-2)
		{}
		else
		{
    usage ();
		}
	}	
	

  script_name = argv[argc - 1];
  FILE *script_stream = fopen (script_name, "r");
  if (! script_stream)
    error (1, errno, "%s: cannot open", script_name);
  command_stream_t command_stream =
    make_command_stream (get_next_byte, script_stream);

  command_t last_command = NULL;
  command_t command;
  while ((command = read_command_stream (command_stream)))
    {
      if (print_tree)
			{
				printf ("# %d\n", command_number++);
				print_command (command);
			}
      else
			{
				if (time_travel == false && time_travel_limit == false)
				{
				last_command = command;
				execute_command (command, time_travel);
				}
			}
    }
		if (time_travel == true)
		{execute(command_stream);}
		if (time_travel_limit == true)
		{execute_limit(command_stream, num_of_processes);}

  return print_tree || !last_command ? 0 : command_status (last_command);
}
