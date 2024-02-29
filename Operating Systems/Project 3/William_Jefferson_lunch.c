//Name: William Jefferson
//CWID: 11930076
//CS301 Spring 2023
//Project 3: Lunch

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "mytime.h" //Contains the random sleep function for the threads


//Set max server/customer threads
//Prevents overflow/segfault from user error
#define MAX_CUSTOMERS 100
#define MAX_SERVERS 10

/*Exit flag for the server thread's infinite loop problem.
Was supposed to solve uneven num of srvr/cstmr threads, but
only assists in joining threads at the end.*/
volatile int exit_flag = 0;

struct lunch {
    int num_customers; //Number of customers
    int num_servers; //Number of servers
    int current_ticket; //Integer to increment ticket numbers
    int next_ticket; //Predict next ticket to maintain FIFO and synchronize cust/serv threads
    sem_t customers; //Initialize sync prim for customers
    sem_t servers; //Initialize sync prim for servers
    sem_t server_count; //Intialize sync prim for number of servers
    pthread_mutex_t mutex; //Initialize the mutex lock
    int num_served; //Keep track of customers served. Helps join server threads at the end.
};


void lunch_init(struct lunch *lunch) {
    lunch->num_customers = 0; //Initialize customers as zero
    lunch->num_servers = 0; //Initialize servers as zero
    lunch->current_ticket = 0; //Initialize tickets as zero
    lunch->next_ticket = 1; //Add first ticket
    sem_init(&lunch->customers, 0, 0); //Customers is a private semaphore set to zero
    sem_init(&lunch->servers, 0, 0); //Servers is private and set to zero
    sem_init(&lunch->server_count, 0, 0); //Server_count is private and set to zero
    pthread_mutex_init(&lunch->mutex, NULL); //Initialize our mutex lock
    lunch->num_served = 0; //Set number served to zero
}

int lunch_get_ticket(struct lunch *lunch, unsigned long csId) {
    int t = mytime (0, 5); //Set wait time range for customer
    pthread_mutex_lock(&lunch->mutex); //Customer is working lock crit section
    int ticket = lunch->next_ticket; //Initialize the ticket counter to the next ticket
    lunch->next_ticket++; //Add to ticket
    lunch->num_customers++; //Add to customers
    pthread_mutex_unlock(&lunch->mutex); //Customer is finished, unlock the mutex
    printf("<ThreadId Customer: %lu> enter lunch_get_ticket\n", (unsigned long)csId);
    printf("<ThreadId Customer: %lu> get ticket %d\n", (unsigned long)csId, ticket);
    printf("<ThreadId Customer: %lu> leave lunch_get_ticket\n", (unsigned long)csId);
    //printf("<ThreadId Customer: %lu> sleep for %d sec\n", (unsigned long)csId, t);
    printf("Sleeping Time: %d sec; Thread Id = Customer %lu\n", t, (unsigned long)csId);
    sleep(t); //Customer will now sleep for a random number of seconds
    sem_post(&lunch->customers); //Increment the semaphore value
    return ticket; //Return the ticket the customer took
}


void lunch_wait_turn(struct lunch *lunch, int ticket, unsigned long custId) {
    sem_wait(&lunch->servers); //The customer thread is now waiting on a server thread
    printf("<ThreadId Customer: %lu> enter lunch_wait_turn with ticket %d\n", (unsigned long)custId, ticket);
    printf("<ThreadId Customer: %lu> leave lunch_wait_turn\n", (unsigned long)custId);
}

void lunch_wait_customer(struct lunch *lunch, unsigned long svId) {
    int t = mytime (5, 15); //Set wait time range for server
    sem_wait(&lunch->customers); //Server is waiting on customer
    pthread_mutex_lock(&lunch->mutex); //Lock section, server is working
    int ticket = lunch->current_ticket + 1;
    lunch->current_ticket++;
    lunch->num_customers--;
    lunch->num_served++;
    pthread_mutex_unlock(&lunch->mutex); //Unlock section, server is finished
    sem_post(&lunch->servers); //Increment server semaphore
    sem_post(&lunch->server_count); //Increment server_count semaphore
    printf("<ThreadId server: %lu> enter lunch_wait_customer\n", (unsigned long)svId);
    printf("<ThreadId server: %lu> now serving ticket %d\n", (unsigned long)svId, ticket);
    printf("<ThreadId server: %lu> after served ticket %d\n", (unsigned long)svId, ticket);
    printf("<ThreadId server: %lu> leave lunch_wait_customer\n", (unsigned long)svId);
    //printf("<ThreadId Server: %lu> sleep for %d sec\n", (unsigned long)svId, t);
    printf("Sleeping Time: %d sec; Thread Id = Server %lu\n", t, (unsigned long)svId);
    sleep(t); //Server sleeps for random number of seconds
}

//Function for building a customer thread
void *customer_thread(void *arg) {
    pthread_t cTid = pthread_self(); //Set an unsigned long for the thread ID
    struct lunch *lunch = (struct lunch *)arg;
    int ticket = lunch_get_ticket(lunch, cTid);
    lunch_wait_turn(lunch, ticket, cTid);
    return NULL;
}

//Function for building a server thread
void *server_thread(void *arg) {
    pthread_t sTid = pthread_self();
    struct lunch *lunch = (struct lunch *)arg;
    sem_post(&lunch->server_count);
    while (1) {
        sem_wait(&lunch->server_count); //Wait for a free server
        if(exit_flag){
            break; //If exit flag is set, we break and join threads
        }
        lunch_wait_customer(lunch, sTid);
    }
    sem_post(&lunch->server_count);
    return NULL;
}

int main(int argc, char *argv[]) {
    //Check input syntax
    if (argc != 3) {
        printf("Usage: %s <number of servers> <number of customers>\n", argv[0]);
        return 0;
    }

    //Handle cmdline arguments for server and customer thread count
    int num_customers = atoi(argv[2]);
    int num_servers = atoi(argv[1]);
    int i = 0;

    // Check for valid input
    /*NOTE: This merely checks for real numbers.
    It does not resolve the race condition by ensuring
    enough customers or servers for their counterpart*/
    if (num_customers <= 0 || num_servers <= 0) {
        printf("Please enter valid input for number of customers and servers.\n");
        return 0;
    }

    //Initialize our lunch struct
    struct lunch lunch;
    lunch_init(&lunch);

    //Build threads for customers
    pthread_t customer_threads[num_customers];
    for (i = 0; i < num_customers; i++) {
        pthread_create(&customer_threads[i], NULL, customer_thread, &lunch);
        printf("Customer created!\n");
    }

    //Build threads for servers
    pthread_t server_threads[num_servers];
    for (i = 0; i < num_servers; i++) {
        pthread_create(&server_threads[i], NULL, server_thread, &lunch);
        printf("Server created!\n");
    }

    //Join customer threads
    for (i = 0; i < num_customers; i++) {
        pthread_join(customer_threads[i], NULL);
        printf("Customer thread joined!\n");
    }
    printf("All customer threads joined!\n");

    //Server threads will attempt to wait on customers to finish up
    while (lunch.num_served < num_customers){
        sleep(1);
    }
    //Set exit flag to signal servers to exit
    exit_flag = 1;

    //Join server threads
    for (i = 0; i < num_servers; i++) {
        pthread_join(server_threads[i], NULL);
        printf("Server thread joined!\n");
    }
    printf("All server threads joined!\n");
    printf("Everyone has been served!\n");

    return 0;
}