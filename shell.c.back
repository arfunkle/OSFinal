/**
Author: Nicholas Dill
This is a shell which implents some of the basic features of the BASH shell, namely command piping and output redirection.
*/
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "parse.h"

int main() {
    char *line;
    while ((line = readline("> ")) != NULL) {
        struct root *r = parse(line);
        if (!r->valid) {
            fprintf(stderr, "Parse error, try again\n");
            continue;
        }
        // My line is syntactically correct.
        struct command *cmd = r->first_command;

		// sourcePipe
		int sourcePipe[2];
		sourcePipe[0] = 0;
		sourcePipe[1] = 0;
		// destPipe
		int destPipe[2];
		destPipe[0] = 0;
		destPipe[1] = 0;

		// output file descriptor
		int outfile = 0;

        while (cmd != NULL) {

			if (cmd->next != NULL){
				int rc = pipe(destPipe);
				if (rc < 0) {
					// pipe failed; exit
					fprintf(stderr, "destPipe failed\n");
					exit(1);
				}
			} 

			if (cmd->outfile) {
				outfile = open(cmd->outfile, O_WRONLY | O_CREAT, 0644);
				if(outfile < 0){printf("Failed to open file.\n");}
			}

			int rc = fork();
			if (rc < 0) {
				fprintf(stderr, "fork failed\n");
				exit(1);
			} else if (rc == 0) {
				// route data through the proper channel and close loose ends
				if (sourcePipe[0] != 0) {
					close(STDIN_FILENO);
					dup(sourcePipe[0]);
					close(sourcePipe[0]);
					close(sourcePipe[1]);
				}
				// route data through the proper channel and close loose ends
				if (destPipe[0] != 0) {
					close(STDOUT_FILENO);
					dup(destPipe[1]);
					close(destPipe[0]);
					close(destPipe[1]);
				}
				if (outfile) {
					close(STDOUT_FILENO);
					dup(outfile);
					close(outfile);
				}

				// start the program
				char *myargs[cmd->argc + 1];
				for (int i=0 ; i<cmd->argc; ++i) {
					myargs[i] = cmd->argv[i];
				}
				myargs[cmd->argc] = NULL;
				int rc =  execvp(myargs[0], myargs);
				if( rc < 0){printf("Command not found.\n");}
				exit(1);
			}	


			if (sourcePipe[0] != 0) { close(sourcePipe[0]); }
			if (sourcePipe[1] != 0) { close(sourcePipe[1]); }
			sourcePipe[0] = destPipe[0];
			sourcePipe[1] = destPipe[1];
			destPipe[0] = 0;
			destPipe[1] = 0;

			if (outfile != 0) { close(outfile); }
			outfile = 0;

            cmd = cmd->next;
        }

		if (sourcePipe[0] != 0) { close(sourcePipe[0]); }
		if (sourcePipe[1] != 0) { close(sourcePipe[1]); }

		parse_end(r);
		free(line);
		while(wait(NULL)>0){;}
    }
	return 0;
}




