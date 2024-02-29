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
#define HIST_SIZE 10 //Maximum history size



int main(void)
{
    char *args[MAX_LINE/2 + 1]; /*Command line arguments*/
    int should_run = 1; /*flag to determine when to exit program*/
    int size = MAX_LINE/2 + 1;
    char *hist[HIST_SIZE];
    char input[MAX_LINE];
    int histc = 0;

        while(should_run)
        {
            int id; //Identify parent/child
            //char input[size]; //Initialize input string
            char ex[size]; //Initialize exit word
            strcpy(ex, "exit"); //Set exit handler
            char* e; //Exit handler
            char* a; //Ampersand handler
            int len; //Length of current input
            int command = 0;

            
            //Display prompt
            int pid = getpid();
            printf("\nJefferson%d>",pid);
            fflush(stdout);
            
            //User input
            fgets(&input[0],size,stdin);
            input[strlen(&input[0]) - 1] = '\0';


            //Handle history input
            if(strcmp("history", &input[0]) == 0) //User enters history
            {
                //Iterate through array and print history
                for (int j = histc - 1; j >= 0; j--)
                {
                    printf("\t%d %s\n", j + 1, hist[j]);
                }
                continue;
            }
            len = strlen(&input[0]); //Set len to current input
            if(input[0] == '!' && (len > 1))
            {
                if(input[1] == '!')
                {
                strcpy(&input[0], hist[histc-1]);
                }
                else if((input[1] > 47) && (input[1] <= 58))
                {
                    if(len == 2)
                    {
                        command = input[1] - 48;
                    }
                    else
                    {
                        command = 10 + input[2] - 48;
                    }

                    if((command <= histc) && (command > 0))
                    {
                        strcpy(&input[0], hist[command - 1]);
                    }
                }
            }

            //storing history
            if(histc < (HIST_SIZE)) //If history isn't full
            {
                strcpy(hist[histc], &input[0]); //Copy input into history
                histc++; //Add to history index
                hist[histc] = (char*) malloc(sizeof(char)); //Allocate for next input
            }
            else //History is full
            {
                //Shift elements of history array
                for(int j = 0; j < (HIST_SIZE - 1); j++)
                {
                    hist[j] = hist[j + 1];
                }
                strcpy(hist[histc], &input[0]); //Copy input into history
            }

            e = strstr(input, ex); //Check for exit
            a = strstr(input,"&"); //Check for ampersand (concurrent)



            //Call fork
            id = fork();

            //Parent
            if(id > 0)
            {   
                //Call wait if there's no &
                if(!a)
                {
                    wait(NULL);
                }

                pid = getpid(); //Set pid to current pid

                //Exit case
                if (e)
                {
                    //printf("Exit called in parent!\n");
                    kill(0, SIGKILL);
                    should_run = 0;
                }
            }

            //Child
            else if(id == 0)
            {
            pid = getpid(); //Set pid to current pid

            //Exit case
            if (e)
            {
                kill(0, SIGKILL);
                should_run = 0;
            }

            //Tokenize string
            int i = 0; //Iteator for array
            char *p = strtok(input, " "); //Tokenized input
            char *array[size]; //Array for storing tokens

            //Feed tokens into array
            while(p != NULL)
            {
                array[i++] = p;
                p = strtok(NULL, " ");
            }

            //Removing the & before passing to execvp
            if(a)
            {
                int amp = i -1; //Index of &
                array[amp] = NULL; //Set index of amp to NULL
            }
            array[i] = 0; //Insert zero for end of statement
            array[i+1] = NULL; //Insert null ptr for execvp
            execvp(array[0], array);
            exit(0);
            }

            //Fork fail
            else if(id == -1)
            {
                printf("Fork creation failed!\n");
            }
            
        }
        return 0;
}