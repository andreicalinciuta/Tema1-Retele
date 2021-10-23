#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>


#define FIFO_CLIENT_SERVER "clientServerFifo"
#define FIFO_SERVER_CLIENT "serverClientFifo"
#define NMAX 1024

void handle_errors(char * msg)
{
	perror(msg);
	exit(2);
}

int trim_instruction(char *original, char *cleaned, char *firstWord, char *secondWord, char *thirdWord)
{
    int index = 0;
    char *token;

	for( ; original[index] ; ++index)
		original[index] = tolower(original[index]);

	index = 0;

    token = strtok(original, " ");
	strcat(firstWord, token);

    while(token != NULL)
    {
        if( index )
            strcat(cleaned, " ");
		if( index == 1 )
			strcat(secondWord, token);
		else if ( index == 2 )
			strcat(thirdWord, token);
        strcat(cleaned, token);
        index++;
        //printf("Token: {%s}, Cleaned: {%s}, Index: {%d}\n", token, cleaned, index);

		//printf("firstWord: {%s}, secondWord: {%s}, thirdWord: {%s}\n", firstWord, secondWord, thirdWord);

        token = strtok(NULL, " ");
    }
	free(token);
	
    return index;
}

int main()
{
	char instruction[NMAX];
	int fifo_c2s, fifo_s2c, fd_c2s, fd_s2c;
	int FLAG_logged = false;
	
	fifo_c2s = mknod(FIFO_SERVER_CLIENT, S_IFIFO | 0666, 0);
	fd_c2s = open(FIFO_CLIENT_SERVER, O_WRONLY);
	
	if( fd_c2s < 0 )
	{
		handle_errors("Client:Some problems with opening fifo, line 33?? \0");
	}
	

	printf("Insert command: ");
	while(fgets(instruction, NMAX, stdin))
	{
		char instructionCleaned[NMAX] = "";
        char *instructionV1 = strtok(instruction, "\n");
		char firstWord[NMAX] = "";
		char secondWord[NMAX] = "";
		char thirdWord[NMAX] = "";
		char validStrings[5][NMAX] = {"login", "get-logged-users", "get-proc-info", "logout", "quit"};

        bool FLAG_Command = (trim_instruction(instructionV1, instructionCleaned, firstWord, secondWord, thirdWord) < 4);

		//printf("First word is: {%s} \n", firstWord);

		//printf("firstWord: {%s}, secondWord: {%s}, thirdWord: {%s}\n", firstWord, secondWord, thirdWord);
		//printf("%d\n", strcmp(secondWord, ":"));

		//printf("Client:Am primit instructiunea {%s} \n", instructionCleaned);

		if(FLAG_Command == false)
		{
			printf("\nError: Unknown command!\n\n");
		}
		else
		if(strcmp(firstWord, "login") == 0 && strcmp(secondWord, ":") == 0 && strlen(thirdWord) > 0)
		{
			if(FLAG_logged == true)
			{
				printf("You are currently logged!\n");
			}
			else
			{
				//printf("Client: Logare\n");

				if( write(fd_c2s, instructionCleaned, strlen(instructionCleaned)) == -1 )
				{
					handle_errors("Some problems with writing in fifo, line 33 \0");
				} 


				
				fd_s2c = open(FIFO_SERVER_CLIENT, O_RDONLY);

				if(fd_s2c < 0)
				{
					handle_errors("Client:Some problems with opening fd_s2c, line 33?? \0");
				}

				char from_server[NMAX];

				read(fd_s2c, from_server, NMAX);
				if(from_server[0] == '2')
				{
					printf("\n%s\n", from_server + 3);
					FLAG_logged = true;
				}
				else
					printf("\n%s\n", from_server + 3);

				close(fd_s2c);
			}
		
		} 
		else
		if(strcmp(firstWord, "get-proc-info") == 0 && strcmp(secondWord, ":") == 0 && strlen(thirdWord) > 0)
		{
			if(FLAG_logged == false)
			{
				printf("Please log in first!\n");
			}
			else
			{
				//printf("Client: Get-Proc-Pid\n");
				
				if( write(fd_c2s, instructionCleaned, strlen(instructionCleaned)) == -1 )
				{
					handle_errors("Some problems with writing in fifo, line 33 \0");
				} 


				
				fd_s2c = open(FIFO_SERVER_CLIENT, O_RDONLY);

				if(fd_s2c < 0)
				{
					handle_errors("Client:Some problems with opening fd_s2c, line 33?? \0");
				}

				char message_to_be_received[NMAX];

				read(fd_s2c, message_to_be_received, NMAX);

				char *token = strtok(message_to_be_received, " ");
				int len;
				sscanf(token, "%d", &len);
				token = strtok(NULL, "\0");
				printf("\n%s\n", token);

				close(fd_s2c);
			}
		} 
		else
		if(strcmp(firstWord, "get-logged-users") == 0 && strlen(secondWord) == 0)
		{
			if(FLAG_logged == false)
			{
				printf("Please log in first!\n");
			}
			else
			{
				//printf("Client: Get-Logged-Users\n");
				
				if( write(fd_c2s, instructionCleaned, strlen(instructionCleaned)) == -1 )
				{
					handle_errors("Some problems with writing in fifo, line 33 \0");
				} 


				fd_s2c = open(FIFO_SERVER_CLIENT, O_RDONLY);

				if(fd_s2c < 0)
				{
					handle_errors("Client:Some problems with opening fd_s2c, line 33?? \0");
				}

				char message_to_be_received[NMAX];

				read(fd_s2c, message_to_be_received, NMAX);

				printf("%s", message_to_be_received);

				close(fd_s2c);
			}
		} 
		else
		if(strcmp(firstWord, "logout") == 0 && strlen(secondWord) == 0)
		{
			if(FLAG_logged == false)
			{
				printf("Please log in first!\n");
			}
			else
			{
				printf("Logout\n");
				FLAG_logged = false;
			}
			
		} 
		else
		if(strcmp(firstWord, "quit") == 0 && strlen(secondWord) == 0)
		{
			printf("Quit\n");
			break;
		} 
		else
			printf("\nError: Unknown command!\n\n");
		printf("\nInsert command: ");
	}
	close(fd_c2s);
	
	return 0;
}
