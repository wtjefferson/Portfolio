/*
NAME: William Jefferson
CWID: 11930076
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX_LINE 80 /*The maximum length command*/



int main(void)
{
    char *args[MAX_LINE/2 + 1]; /*Command line arguments*/
    int should_run = 1; /*flag to determine when to exit program*/
    int size = MAX_LINE/2 + 1;

        while(should_run)
        {
            int id; //Identify parent/child
            char input[size]; //Initialize input string
            char ex[size]; //Initialize exit word
            strcpy(ex, "exit"); //Set 
            char* e; //Exit handler
            char* a; //Ampersand handler

            
            //Display prompt
            int pid = getpid();
            printf("\nJefferson%d>",pid);
            fflush(stdout);
            
            //User input
            fgets(&input[0],size,stdin);
            input[strlen(&input[0]) - 1] = '\0';

            //len = streln(&input[0]);

            e = strstr(input, ex); //Check for exit
            a = strstr(input,"&"); //Check for ampersand (concurrent)



            //Call fork
            id = fork();

            //Parent
            if(id > 0)
            {   
                //Check for ampersand
                if(!a)
                {
                    wait(NULL);
                }
                //Identify process and pid
                pid = getpid();

                //Exit case
                if (e)
                {
                    kill(0, SIGKILL);
                    should_run = 0;
                }
            }

            //Child
            else if(id == 0)
            {
            //Identify process and pid
            pid = getpid();

            //Exit case
            if (e)
            {
                kill(0, SIGKILL);
                should_run = 0;
            }

            //Tokenize string
            int i = 0;
            char *p = strtok(input, " ");
            char *array[size];

            //Feed tokens into array
            while(p != NULL)
            {
                array[i++] = p;
                p = strtok(NULL, " ");
            }
            if(a)
            {
                int amp = i -1; //Index of &
                array[amp] = NULL;
            }
            


            //Insert zero and null pointer
            array[i] = 0;
            array[i+1] = NULL;
            //Execute execvp
            execvp(array[0], array);
            exit(0);
            }

            else if(id == -1)
            {
                printf("Fork creation failed!\n");
            }
        }
        return 0;
}