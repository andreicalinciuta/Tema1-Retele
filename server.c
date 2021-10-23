#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>


#define FIFO_CLIENT_SERVER "clientServerFifo"
#define FIFO_SERVER_CLIENT "serverClientFifo"
#define NMAX 124

void handle_errors(char * msg)
{
	perror(msg);
	exit(2);
}

bool test_logger(char *name)
{
    FILE *fp;
    char *line = NULL;
    bool FLAG_Valid = false;
    size_t len;
    
    fp = fopen("/home/andrei-calin/rc/tema1/config", "r");

    if(fp == NULL)
    {
        exit(2);
    }

    while((getline(&line, &len, fp)) != -1)
    {
        char *firstWord = strtok(line, " \n");
        if(strcmp(firstWord, name) == 0)
            FLAG_Valid = true;
        printf("This is the line {%s} \n", firstWord);
    }

    fclose(fp);
    if(line)
        free(line);
    return FLAG_Valid;
}

void split_instruction(char *original, char *firstWord, char *secondWord, char *thirdWord)
{
    int index = 0;
    char *token;

	
	index = 0;

    token = strtok(original, " ");
	strcat(firstWord, token);

    while(token != NULL)
    {
        
		if( index == 1 )
			strcat(secondWord, token);
		else if ( index == 2 )
			strcat(thirdWord, token);
        index++;

        token = strtok(NULL, " ");
    }
	firstWord[strlen(firstWord)] = '\0';
	secondWord[strlen(secondWord)] = '\0';
	thirdWord[strlen(thirdWord)] = '\0';
    return;
}

int main()
{
	char instruction[NMAX], forClient[NMAX];
	int fifo_c2s, fifo_s2c, fd_c2s, fd_s2c;

	fifo_c2s = mknod(FIFO_CLIENT_SERVER, S_IFIFO | 0666, 0);
	
	if( fifo_c2s < 0 )
	{
		handle_errors("Server:Some problems with mknod, line 11 \0");
	}

	fd_c2s = open(FIFO_CLIENT_SERVER, O_RDONLY);
	
	if( fd_c2s < 0 )
	{
		handle_errors("Server:Some problems with opening fifo, line 33?? \0");
	}
	int num;
	do
	{
		if((num = read(fd_c2s, instruction, NMAX)) == -1)
			handle_errors("Nu e ok la scriere \0");
		else
		{
			//printf("Server22: A venit instructiunea {%s}-{%d} \n", instruction, num);
			if(num == 0)
			{
				return 0;
			}
			instruction[num] = '\0';		
			char firstWord[NMAX] = "";
			char secondWord[NMAX] = "";
			char thirdWord[NMAX] = "";
			split_instruction(instruction, firstWord, secondWord, thirdWord);
			fd_s2c = open(FIFO_SERVER_CLIENT, O_WRONLY);

			if(fd_s2c < 0)
			{
				handle_errors("Server:Some problems with opening fd_s2c, line 33?? \0");
			}

			
			if(strcmp(firstWord, "login") == 0)
			{
				if(test_logger(thirdWord))
				{
					write(fd_s2c, "22 Successfully logged!\0", NMAX);
				}
				else
					write(fd_s2c, "47 You Are Unauthorized To Perform This Request!\0", NMAX);
			}
		}
		printf("Server: A venit instructiunea {%s} \n", instruction);
		instruction[0] = '\0'; 
	}while(num > 0);
	
	return 0;
}



























