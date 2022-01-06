/*
Public Invention's Ox Project is an open source hardware design for an oxygen
concentrator for use by field hospitals around the world. This team aims to
design an oxygen concentrator that can be manufactured locally while overcoming
challenges posed by human resources, hospital location (geographically),
infrastructure and logistics; in addition, this project attempts the minimum
documentation expected of their design for international approval whilst
tackling regulatory requirements for medical devices. Copyright (C) 2021
Robert Read, Ben Coombs, and Darío Hereñú.

This program includes free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

/*#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

struct thread_info {
    pthread_t thread_id;
    int thread_num;
};

void* my_task(void* data) {
    int mydata = *((int*)data);
    pthread_detach(pthread_self());
    printf("Task1: %d\n", mydata);
    while (1) {
        // using stderr because it is unbuffered
        // alternatively use stdout and fflush()
        fprintf(stderr, "-");
        sleep(1);
    }
    pthread_exit(NULL);
}

void* my_task2(void* data) {
    int mydata = *((int*)data);
    pthread_detach(pthread_self());
    printf("Task2: %d\n", mydata);
    for (int i = 0; i < 10; i++) {
        fprintf(stderr, "."); 
        sleep(1);
    }
    pthread_exit(NULL);
}

void* my_task3(void* data) {
    int mydata = *((int*)data);
    pthread_detach(pthread_self());
    printf("Task3: %d\n", mydata);
    const struct timespec ts = {0, 500000000L};
    
    while (1) {
        fprintf(stderr, "^");
        nanosleep(&ts, NULL);//sleep(2);
    }
    pthread_exit(NULL);
}


pthread_t create_task(void* thread_func, pthread_attr_t *attr, void* data) {
    printf("create_task\n");
    pthread_t ptid; //ulong
    printf("New thread: %lu\n", ptid);
    pthread_create(&ptid, attr, thread_func, data);
    int result = pthread_detach(ptid);
    if (result != 0) {
        printf("Failed to detatch thread: %lu with error: %d\n", ptid, result);
    }
    return ptid;
}

void start() {
    int num_threads = 3;
    ssize_t stack_size = 0x7D00; //PTHREAD_STACK_MIN + 0x4000; // ~32kb
    pthread_attr_t attr;
    int rc;

    rc = pthread_attr_init(&attr);                                               
    if (rc == -1) {                                                              
        perror("error in pthread_attr_init");                                     
        exit(1);                                                                  
    }
    rc = pthread_attr_setstacksize(&attr, stack_size);                                   
    if (rc == -1) {                                                              
        perror("error in pthread_attr_setstacksize");                             
        exit(2);                                                                  
    }                                                       
    size_t size;                                                                                                                                                                                                                                    
    if (pthread_attr_getstacksize(&attr, &size) == -1) {                         
        perror("error in pthread_attr_getstackstate()");                           
        exit(2);                                                                   
    }                                                                                                                     
    printf("The stack size is %d.\n", (int) size);
 //   printf("PTHREAD_STACK_MIN: %d\n", PTHREAD_STACK_MIN);

    printf("Main thread: %lu\n", pthread_self());
    printf("start\n");

    int t = 11;
    int v = 15;
    pthread_t ptid1 = create_task(&my_task, &attr, &t);
    pthread_t ptid2 = create_task(&my_task2, &attr, &v);
    pthread_t ptid3 = create_task(&my_task3, &attr, &v);
    
    pthread_exit(NULL);
}

void init() {

}


void run_task(void* thread_func) {
    pthread_t ptid; //ulong
    printf("New thread: %lu\n", ptid);
    pthread_create(&ptid, NULL, thread_func, NULL);
    int result = pthread_detach(ptid);
    if (result != 0) {
        printf("Failed to detatch thread: %lu with error: %d\n", ptid, result);
    }
}

void suspend_task(void* thread_func) {

}*/

/*
// Driver code
int main()
{
    int num_threads = 3;
    ssize_t stack_size = PTHREAD_STACK_MIN + 0x4000; // ~32kb
    pthread_attr_t attr;
    int rc;

    rc = pthread_attr_init(&attr);                                               
    if (rc == -1) {                                                              
        perror("error in pthread_attr_init");                                     
        exit(1);                                                                  
    }
    rc = pthread_attr_setstacksize(&attr, stack_size);                                   
    if (rc == -1) {                                                              
        perror("error in pthread_attr_setstacksize");                             
        exit(2);                                                                  
    }                                                       
    size_t size;                                                                                                                                                                                                                                    
    if (pthread_attr_getstacksize(&attr, &size) == -1) {                         
        perror("error in pthread_attr_getstackstate()");                           
        exit(2);                                                                   
    }                                                                                                                     
    printf("The stack size is %d.\n", (int) size);
    printf("PTHREAD_STACK_MIN: %d\n", PTHREAD_STACK_MIN);

    printf("Main thread: %lu\n", pthread_self());
    printf("start\n");

    int t = 11;
    int v = 15;
    pthread_t ptid1 = create_task(&my_task, &attr, &t);
    pthread_t ptid2 = create_task(&my_task2, &attr, &v);
    pthread_t ptid3 = create_task(&my_task3, &attr, &v);

    //pthread_join(ptid1, NULL);
    //pthread_join(ptid2, NULL);
    //int result = pthread_attr_destroy(&attr);
    //if (result != 0) {
    //   printf("Failed to destroy pthread attributes!\n");
    //}
    
    pthread_exit(NULL);
    //fun();
    return 0;
}*/