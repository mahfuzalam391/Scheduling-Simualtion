#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_PROCESSES 1000  // Maximum number of processes supported
#define BUFFER_SIZE 1024    // Buffer size for input/output

// Structure to represent a process in FCFS (First-Come-First-Serve) scheduling
typedef struct {
    int process_id;             // Process ID
    int arrival_time;           // Arrival time of the process
    int burst_time;             // CPU burst time (execution time of the process)
    int start_time;             // Time when the process starts execution
    int finish_time;            // Time when the process finishes execution
    int wait_time;              // Waiting time of the process (time spent waiting in the ready queue)
    int turnaround_time;        // Turnaround time (total time from arrival to completion)
    int response_time;          // Response time (time from arrival to first execution)
    int time_until_first_response;  // Time until the first response (for preemptive systems)
} Process_For_FCFS;

// Structure to represent a process in SJF (Shortest Job First) scheduling
typedef struct {
    int process_id;            // Process ID
    int arrival_time;          // Arrival time of the process
    int start_time;            // Time when the process starts execution
    int completion_time;       // Time when the process finishes execution
    int burst_time;            // CPU burst time
    int waiting_time;          // Waiting time (time spent waiting in the ready queue)
    int turnaround_time;       // Turnaround time (total time from arrival to completion)
    int time_until_first_response;  // Time until first response (in preemptive systems)
    int response_time;         // Response time (time from arrival to first execution)
    bool finished;             // Flag to check if the process is completed
} Process_For_SJF;

// Structure to represent a process in Priority Scheduling
typedef struct {
    int process_id;            // Process ID
    int arrival_time;          // Arrival time of the process
    int burst_time;            // CPU burst time
    int start_time;            // Time when the process starts execution
    int completion_time;       // Time when the process finishes execution
    int waiting_time;          // Waiting time (time spent waiting in the ready queue)
    int turn_around_time;      // Turnaround time (total time from arrival to completion)
    int response_time;         // Response time (time from arrival to first execution)
    float priority;            // Priority value (lower values indicate higher priority)
    int time_until_first_response;  // Time until first response
    bool completed;            // Flag to check if the process is completed
} Process_For_Priority;

// Structure to represent a process in Round-Robin (RR) scheduling
typedef struct {
    int process_id;            // Process ID
    int arrival_time;          // Arrival time of the process
    int burst_time;            // CPU burst time
    int start_time;            // Time when the process starts execution
    int remaining_time;        // Remaining execution time
    int completion_time;       // Time when the process finishes execution
    int waiting_time;          // Waiting time
    int turn_around_time;      // Turnaround time
    int response_time;         // Response time
    int time_until_first_response;  // Time until first response
    bool completed;            // Flag to check if the process is completed
    bool first_response;       // Flag to check if this is the first response
} Process_For_RR;

// Structure to represent a process in Shortest Remaining Time (SRT) scheduling
typedef struct {
    int process_id;                  // Process ID
    int arrival_time;                // Arrival time of the process
    int start_time;                  // Start time
    int finish_time;                 // Finish time
    int response_time;               // Response time
    int time_until_first_response;   // Time until first response
    int turn_around_time;            // Turnaround time
    int waiting_time;                // Waiting time
    int actual_cpu_burst;            // Actual CPU burst time
    int remaining_time;              // Remaining burst time
    float predicted_cpu_burst;       // Predicted CPU burst time (used in exponential averaging)
    float predicted_remaining_time;  // Predicted remaining time (used in exponential averaging)
    bool completed;                  // Flag to check if the process is completed
} Process_For_SRT;

// Function to print the process values after FCFS simulation
void print_values(Process_For_FCFS processes[], int n) {
    int total_wait_time = 0, total_turnaround_time = 0, total_response_time = 0; // Aggregates for averages

    // Print header for table
    printf("+----+---------+-------+-------+--------+---------+----------------+---------------+\n");
    printf("| Id | Arrival | Burst | Start | Finish |    Wait |   Turnaround   | Response Time |\n");
    printf("+----+---------+-------+-------+--------+---------+----------------+---------------+\n");

    // Iterate through the processes
    for (int i = 1; i <= 50; i++) {
        int id = i;
        int arrival = -1;
        int burst = 0;
        int start = -1;
        int finish = -1;
        int wait = 0;
        int turnaround = 0;
        int response_time = 0;

        // Find the relevant data for each process
        for (int j = 0; j < n; j++) {
            if (i == processes[j].process_id) {
                if (arrival == -1 || (arrival != -1 && processes[j].arrival_time < arrival)) {
                    arrival = processes[j].arrival_time;
                }
                if (start == -1 || (start != -1 && processes[j].start_time < start)) {
                    start = processes[j].start_time;
                }
                if (finish == -1 || (finish != -1 && processes[j].finish_time > finish)) {
                    finish = processes[j].finish_time;
                }
                if (response_time == 0 || (response_time != 0 && processes[j].response_time < response_time)) {
                    response_time = processes[j].response_time;
                }
                burst += processes[j].burst_time;
                wait += processes[j].wait_time;
            }
        }

        // Calculate turnaround and response times
        turnaround = finish - arrival;
        response_time = response_time - arrival;

        // Aggregate wait, turnaround, and response times
        total_wait_time += wait;
        total_turnaround_time += turnaround;
        total_response_time += response_time;

        // Print process values
        printf("| %-2d | %-7d | %-5d | %-5d | %-6d | %-7d | %-14d | %-13d |\n",
               id, arrival, burst, start, finish, wait, turnaround, response_time);
    }
    
    printf("+----+---------+-------+-------+--------+---------+----------------+---------------+\n");

    // Print average wait, turnaround, and response times
    printf("\nAverage Waiting Time: %.2f ms\n", (float)total_wait_time / n);
    printf("Average Turnaround Time: %.2f ms\n", (float)total_turnaround_time / n);
    printf("Average Response Time: %.2f ms\n", (float)total_response_time / n);
}

// Function to sort processes based on arrival time (FCFS logic)
void sort_same_arrival_time_processes_fcfs(Process_For_FCFS processes[], int n) {
    // Sort processes by arrival time and then by process ID if arrival times are the same
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (processes[j].arrival_time > processes[j + 1].arrival_time ||
                (processes[j].arrival_time == processes[j + 1].arrival_time &&
                 processes[j].process_id > processes[j + 1].process_id)) {
                // Swap processes
                Process_For_FCFS temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

// Function to simulate FCFS scheduling
void simulate_fcfs(Process_For_FCFS processes[], int n) {
    int current_time = 0;  // Tracks the current time
    int total_wait_time = 0, total_turnaround_time = 0, total_response_time = 0; // Totals for averages

    int space_required = (2 * n) + 1;  // Space required for sequence string
    char *seq = (char *)malloc(space_required * sizeof(char));  // Allocate memory for sequence
    if (seq == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    // Initialize sequence string
    strcpy(seq, "seq = [");

    // Execute processes in order of arrival (FCFS logic)
    for (int i = 0; i < n; i++) {
        if (current_time < processes[i].arrival_time) {
            while (current_time != processes[i].arrival_time) {
                current_time++;  // Advance time if the CPU is idle
            }
        }

        processes[i].start_time = current_time;  // Set start time for process

        // Execute the burst time for the process
        for (int n = 0; n < processes[i].burst_time; n++) {
            current_time++;
        }

        processes[i].finish_time = processes[i].start_time + processes[i].burst_time;  // Set finish time

        if (current_time != processes[i].finish_time) {
            printf("Error: current_time != processes[i].finish_time\n");
            exit(1);
        }

        processes[i].wait_time = processes[i].start_time - processes[i].arrival_time;  // Calculate wait time
        processes[i].turnaround_time = processes[i].finish_time - processes[i].arrival_time;  // Calculate turnaround time
        processes[i].response_time = processes[i].wait_time + processes[i].time_until_first_response + processes[i].arrival_time;  // Calculate response time

        // Append process ID to sequence string
        char str[20];
        sprintf(str, "%d", processes[i].process_id);
        strcat(seq, str);

        if (i < n - 1) {
            strcat(seq, ",");
        }
    }

    // Close sequence string and print it
    strcat(seq, "]");
    printf("%s\n", seq);
    free(seq);  // Free allocated memory

    print_values(processes, n);  // Print process values and averages
}

// Function to sort processes with the same arrival time in SJF scheduling
void sort_same_arrival_time_processes_sjf(Process_For_SJF processes[], int n) {
    // Sort processes by arrival time, and if they have the same arrival time, sort by process ID
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (processes[j].arrival_time > processes[j + 1].arrival_time ||
                (processes[j].arrival_time == processes[j + 1].arrival_time &&
                 processes[j].process_id > processes[j + 1].process_id)) {
                // Swap processes[j] and processes[j + 1]
                Process_For_SJF temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

// Function to find the process with the shortest burst time that has arrived by current_time
int find_shortest_process(Process_For_SJF processes[], int n, int current_time) {
    int min_burst_time = 1000000; // A large number for comparison
    int shortest_index = -1;      // Index of the shortest process
    
    for (int i = 0; i < n; i++) {
        // Check if the process has not finished and has arrived by the current time
        if (!processes[i].finished && processes[i].arrival_time <= current_time) {
            // If this process has the shortest burst time, update the index
            if (processes[i].burst_time < min_burst_time) {
                min_burst_time = processes[i].burst_time;
                shortest_index = i;
            }
        }
    }
    return shortest_index;  // Return the index of the shortest process
}

// Function to simulate Shortest Process Next (SPN) scheduling (non-preemptive SJF)
void simulateSPN(Process_For_SJF processes[], int n) {
    int current_time = 0;              // Tracks the current CPU time
    int completed_processes = 0;       // Tracks how many processes have completed
    int total_waiting_time = 0, total_turnaround_time = 0, total_response_time = 0;  // Totals for averages

    // Memory allocation for sequence output
    int space_required = (2 * n) + 1;  // Space required for sequence string
    char *seq = (char *)malloc(space_required * sizeof(char));  // Allocate memory
    if (seq == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    strcpy(seq, "seq = [");  // Initialize sequence string

    // Simulate SJF scheduling
    while (completed_processes < n) {
        int index = find_shortest_process(processes, n, current_time);  // Find the next shortest process

        if (index != -1) {  // If a process is ready to execute
            Process_For_SJF *current_process = &processes[index];

            // Set the start time of the process
            current_process->start_time = current_time;

            // Execute the process (advance current time by its burst time)
            current_time += current_process->burst_time;
            current_process->turnaround_time = current_time - current_process->arrival_time;  // Calculate turnaround time
            current_process->waiting_time = current_process->turnaround_time - current_process->burst_time;  // Calculate waiting time
            current_process->response_time = current_process->arrival_time + current_process->waiting_time + current_process->time_until_first_response;  // Calculate response time

            current_process->finished = true;  // Mark the process as finished
            current_process->completion_time = current_time;  // Set the completion time

            // Append the process ID to the sequence string
            char str[20];
            sprintf(str, "%d", current_process->process_id);
            strcat(seq, str);

            if (completed_processes < n - 1) {
                strcat(seq, ",");
            }

            completed_processes++;  // Increment completed process count

        } else {
            current_time++;  // If no process is ready, increment current time
        }
    }

    strcat(seq, "]");  // Close the sequence string
    printf("%s\n", seq);  // Print the sequence
    free(seq);  // Free allocated memory
}

// Function to display processes and calculate average times for SJF scheduling
void display_and_calculate_averages_sjf(Process_For_SJF processes[], int n) {
    int total_wait_time = 0, total_turnaround_time = 0, total_response_time = 0;  // Totals for averages

    // Print header for process details
    printf("+----+---------+-------+-------+--------+---------+----------------+---------------+\n");
    printf("| Id | Arrival | Burst | Start | Finish |    Wait |   Turnaround   | Response Time |\n");
    printf("+----+---------+-------+-------+--------+---------+----------------+---------------+\n");

    // Loop through each process
    for (int i = 1; i <= 50; i++) {
        int id = i, arrival = -1, burst = 0, start = -1, finish = -1, wait = 0, turnaround = 0, response_time = 0;

        // Find process details
        for (int j = 0; j < n; j++) {
            if (i == processes[j].process_id) {
                if (arrival == -1 || processes[j].arrival_time < arrival) arrival = processes[j].arrival_time;
                if (start == -1 || processes[j].start_time < start) start = processes[j].start_time;
                if (finish == -1 || processes[j].completion_time > finish) finish = processes[j].completion_time;
                if (response_time == 0 || processes[j].response_time < response_time) response_time = processes[j].response_time;
                burst += processes[j].burst_time;
                wait += processes[j].waiting_time;
            }
        }

        // Calculate turnaround and response times
        turnaround = finish - arrival;
        response_time = response_time - arrival;

        // Accumulate totals
        total_wait_time += wait;
        total_turnaround_time += turnaround;
        total_response_time += response_time;

        // Print process details
        printf("| %-2d | %-7d | %-5d | %-5d | %-6d | %-7d | %-14d | %-13d |\n", id, arrival, burst, start, finish, wait, turnaround, response_time);
    }

    printf("+----+---------+-------+-------+--------+---------+----------------+---------------+\n");

    // Print average times
    printf("\nAverage Waiting Time: %.2f ms\n", (float)total_wait_time / n);
    printf("Average Turnaround Time: %.2f ms\n", (float)total_turnaround_time / n);
    printf("Average Response Time: %.2f ms\n", (float)total_response_time / n);
}




// Priorty Functions 

// Function to sort processes with the same arrival time in Priority scheduling
void sort_same_arrival_time_processes_priority(Process_For_Priority processes[], int n) {
    // Sort processes by arrival time, and if same, by process ID
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (processes[j].arrival_time > processes[j + 1].arrival_time ||
                (processes[j].arrival_time == processes[j + 1].arrival_time &&
                 processes[j].process_id > processes[j + 1].process_id)) {
                Process_For_Priority temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

// Function to find the next process to execute based on priority
int find_next_process(Process_For_Priority process[], int n, int current_time) {
    int highest_priority_index = -1;
    float highest_priority = 10.0;  // Set an arbitrary high value for comparison

    // Find the highest priority process that has arrived
    for (int i = 0; i < n; i++) {
        if (process[i].arrival_time <= current_time && !process[i].completed) {
            if (process[i].priority < highest_priority) {
                highest_priority = process[i].priority;
                highest_priority_index = i;
            }
        }
    }

    return highest_priority_index;  // Return -1 if no process is ready
}

// Function to calculate times for Priority scheduling
void calculate_times(Process_For_Priority process[], int n) {
    int current_time = 0;
    int completed_processes = 0;

    // Memory allocation for sequence output
    int space_required = (2 * n) + 1;
    char *seq = (char *)malloc(space_required * sizeof(char));
    if (seq == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    strcpy(seq, "seq = [");  // Initialize sequence string

    while (completed_processes < n) {
        // Find the next process to execute based on priority
        int next_process_index = find_next_process(process, n, current_time);

        if (next_process_index == -1) {
            current_time++;  // Increment time if no process is ready
            continue;
        }

        // Set the start time of the process
        if (process[next_process_index].start_time == -1) {
            process[next_process_index].start_time = current_time;
        }

        // Execute the process (advance current time by its burst time)
        for (int k = 0; k < process[next_process_index].burst_time; k++) {
            current_time++;
        }

        // Set completion time
        process[next_process_index].completion_time = current_time;

        // Calculate turnaround, waiting, and response times
        process[next_process_index].turn_around_time = process[next_process_index].completion_time - process[next_process_index].arrival_time;
        process[next_process_index].waiting_time = process[next_process_index].turn_around_time - process[next_process_index].burst_time;
        process[next_process_index].response_time = process[next_process_index].waiting_time + process[next_process_index].time_until_first_response;

        // Mark process as completed
        process[next_process_index].completed = true;
        completed_processes++;

        // Append process ID to sequence string
        char str[20];
        sprintf(str, "%d", process[next_process_index].process_id);
        strcat(seq, str);

        if (completed_processes < n - 1) {
            strcat(seq, ",");
        }
    }

    strcat(seq, "]");  // Close sequence string
    printf("%s\n", seq);  // Print sequence
    free(seq);  // Free allocated memory
}

// Function to display processes and calculate average times for Priority scheduling
void display_and_calculate_averages_priority(Process_For_Priority processes[], int n) {
    int total_wait_time = 0, total_turnaround_time = 0, total_response_time = 0;  // Totals for averages

    // Print header for process details
    printf("+----+---------+-------+-------+--------+---------+----------------+---------------+\n");
    printf("| Id | Arrival | Burst | Start | Finish |    Wait |   Turnaround   | Response Time |\n");
    printf("+----+---------+-------+-------+--------+---------+----------------+---------------+\n");

    // Loop through each process
    for (int i = 1; i <= 50; i++) {
        int id = i, arrival = -1, burst = 0, start = -1, finish = -1, wait = 0, turnaround = 0, response_time = -1;

        // Find process details
        for (int j = 0; j < n; j++) {
            if (i == processes[j].process_id) {
                if (arrival == -1 || processes[j].arrival_time < arrival) arrival = processes[j].arrival_time;
                if (start == -1 || processes[j].start_time < start) start = processes[j].start_time;
                if (finish == -1 || processes[j].completion_time > finish) finish = processes[j].completion_time;
                if (response_time == -1 || processes[j].response_time < response_time) response_time = processes[j].response_time;
                burst += processes[j].burst_time;
                wait += processes[j].waiting_time;
            }
        }

        // Calculate turnaround and response times
        turnaround = finish - arrival;
        response_time = response_time - arrival;

        // Accumulate totals
        total_wait_time += wait;
        total_turnaround_time += turnaround;
        total_response_time += response_time;

        // Print process details
        printf("| %-2d | %-7d | %-5d | %-5d | %-6d | %-7d | %-14d | %-13d |\n", id, arrival, burst, start, finish, wait, turnaround, response_time);
    }

    printf("+----+---------+-------+-------+--------+---------+----------------+---------------+\n");

    // Print average times
    printf("\nAverage Waiting Time: %.2f ms\n", (float)total_wait_time / n);
    printf("Average Turnaround Time: %.2f ms\n", (float)total_turnaround_time / n);
    printf("Average Response Time: %.2f ms\n", (float)total_response_time / n);
}






// Functions For Round Robin

void simulateRR( Process_For_RR process[], int n, int q) {

    int quantum = q;
    int total_waiting_time = 0, total_turnaround_time = 0, total_response_time = 0;
    int current_time = 0, completed = 0;
    
    int space_required = (2 * n) + 1 ;

    char *seq = (char *)malloc(space_required * sizeof(char));
    if (seq == NULL) {
        printf("Memory allocation failed\n");
        exit(1) ;
    }

    // Initialize the string
    strcpy(seq, "seq = [");

     
    int top = 0, bottom = 0, pointer = 0;  // Front and bottom pointers for the queue
    pointer = top;

     // Add the first process that arrives at time 0
     for (int i = 0; i < n; i++) {
         if (process[i].arrival_time <= current_time) {
             bottom++;
         }
     }

     while (completed < n) {
        

        if (top == bottom) {
             // No process in the ready queue, move time forward to the next process arrival
             // in the case where there is one process what do we do 
             current_time++; 
            
            for (int i = 0; i < n; i++) {
                if (process[i].arrival_time <= current_time) { 
                    bottom++;
                    
                    // were do i add it to the que though - solved 
                    // int he follwing code then think the pid is gonnna be stedy 
                }
            }
            continue;
        }

        

        if(process[pointer].completed == true){

            if (pointer >= bottom-1){
                    pointer = top;
                }
            else{
                pointer++;
            }
            continue;
        }

        



         // If it's the first time the process is executing, calculate its response time
         if (process[pointer].start_time == -1) {
            process[pointer].start_time = current_time;
             
         }

        

         // Execute the process for either the quantum or its remaining time, whichever is smaller
         //int time_slice = (proc[i].remaining_time > quantum) ? quantum : proc[i].remaining_time;

        int time_slice;
        if (process[pointer].remaining_time > quantum) {
            time_slice = quantum;
        } else {
            time_slice = process[pointer].remaining_time;
        }


        for (int k = 0; k < time_slice ; k++){
            
            process[pointer].remaining_time = process[pointer].remaining_time - 1;
            
            current_time++;
            for (int i = bottom; i < n; i++) {
                if (process[i].arrival_time <= current_time) { 
                    bottom++;
                    
                }
            }

            if((process[pointer].time_until_first_response == (process[pointer].burst_time - process[pointer].remaining_time)) && process[pointer].first_response == false){

                    int tempWaitingTime = (current_time - process[pointer].arrival_time) - process[pointer].time_until_first_response;
                    process[pointer].response_time = tempWaitingTime + process[pointer].time_until_first_response + process[pointer].arrival_time;


            }



        }


         // Check if the process is completed
         if (process[pointer].remaining_time == 0) {
             process[pointer].completion_time = current_time;
             process[pointer].turn_around_time = process[pointer].completion_time - process[pointer].arrival_time;
             process[pointer].waiting_time = process[pointer].turn_around_time - process[pointer].burst_time;
             process[pointer].completed= true;

            char str[20];  // Buffer to hold the string
            sprintf(str, "%d", process[pointer].process_id);
            strcat(seq, str);
            if (completed < n - 1) {
            strcat(seq, ",");  // Add a comma between numbers
            }
             completed++;
         } else {
//             // Don't increa the top parament by one becasue we gonna have to come back to that vlaue 
                // but do move the pointer up 

                if (pointer >= bottom-1){
                    pointer = top;
                }
                else{
                    pointer++;
                }
         }
     }

        strcat(seq, "]");
        printf("%s\n", seq);
        free(seq);
    }


void sort_same_arrival_time_processes_RR(Process_For_RR processes[], int n){

    // Sort processes by arrival time (if same arrival time, sort by process_id)
    // Lines with the same process id in excel file (23,24)
    
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (processes[j].arrival_time > processes[j + 1].arrival_time ||
                (processes[j].arrival_time == processes[j + 1].arrival_time &&
                 processes[j].process_id > processes[j + 1].process_id)) {
                // Swap processes[j] and processes[j + 1]
                Process_For_RR temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }


}

void display_and_calculate_averages_RR(Process_For_RR processes[], int n){

    int total_wait_time = 0, total_turnaround_time = 0, total_response_time = 0; // not sure if i will actually need these 


    printf("+----+---------+-------+-------+--------+---------+----------------+---------------+\n");
    printf("| Id | Arrival | Burst | Start | Finish |    Wait |   Turnaround   | Response Time |\n");
    printf("+----+---------+-------+-------+--------+---------+----------------+---------------+\n");

    for (int i = 1; i <= 50; i++){


        int id = i;
        int arrival = -1;
        int burst = 0; 
        int start = -1; 
        int finish = -1;
        int wait = 0; 
        int turnaround=0; 
        int response_time =-1;

        



        for (int j = 0; j < n; j++){

            if (i == processes[j].process_id){

                if (arrival == -1 || (arrival != -1 && (processes[j].arrival_time < arrival) )){

                    arrival = processes[j].arrival_time;
                }


                if (start== -1 || (start != -1 && (processes[j].start_time < start) )){

                    start = processes[j].start_time;
                }

                if (finish== -1 || (finish != -1 && (processes[j].completion_time > finish) )){

                    finish = processes[j].completion_time;
                }


                if (response_time== -1 || (response_time != -1 && (processes[j].response_time < response_time) )){

                    response_time = processes[j].response_time;
                }

                burst = burst + processes[j].burst_time;
                wait = wait + processes[j].waiting_time;

                
            }
    
        }
        turnaround = finish - arrival;
        response_time = response_time-arrival;


        total_wait_time += wait;
        total_turnaround_time += turnaround;
        total_response_time += response_time;

        printf("| %-2d | %-7d | %-5d | %-5d | %-6d | %-7d | %-14d | %-13d |\n",id,arrival,burst,start ,finish, wait ,turnaround , response_time); 
        
  
}
    printf("+----+---------+-------+-------+--------+---------+----------------+---------------+\n");



    printf("\nAverage Waiting Time: %.2f ms\n", (float)total_wait_time / n);
    printf("Average Turnaround Time: %.2f ms\n", (float)total_turnaround_time / n);
    printf("Average Response Time: %.2f ms\n", (float)total_response_time /n);


}

// Functions for SRT

void display_and_calculate_averages_srt(Process_For_SRT processes[], int n){


    int total_wait_time = 0, total_turnaround_time = 0, total_response_time = 0; 




    printf("+----+---------+-------+-------+--------+---------+----------------+---------------+\n");
    printf("| Id | Arrival | Burst | Start | Finish |    Wait |   Turnaround   | Response Time |\n");
    printf("+----+---------+-------+-------+--------+---------+----------------+---------------+\n");

    for (int i = 1; i <= 50; i++){


        int id = i;
        int arrival = -1;
        int burst = 0; 
        int start = -1; 
        int finish = -1;
        int wait = 0; 
        int turnaround=0; 
        int response_time =0;

        



        for (int j = 0; j < n; j++){

            if (i == processes[j].process_id){

                if (arrival == -1 || (arrival != -1 && (processes[j].arrival_time < arrival) )){

                    arrival = processes[j].arrival_time;
                }


                if (start== -1 || (start != -1 && (processes[j].start_time < start) )){

                    start = processes[j].start_time;
                }

                if (finish== -1 || (finish != -1 && (processes[j].finish_time > finish) )){

                    finish = processes[j].finish_time;
                }


                if (response_time== 0 || (response_time != 0 && (processes[j].response_time < response_time) )){

                    response_time = processes[j].response_time;
                }

                burst = burst + processes[j].actual_cpu_burst;
                wait = wait + processes[j].waiting_time;

                
            }
    
        }
        turnaround = finish - arrival;
        response_time = response_time - arrival;


        total_wait_time += wait;
        total_turnaround_time += turnaround;
        total_response_time += response_time;

        printf("| %-2d | %-7d | %-5d | %-5d | %-6d | %-7d | %-14d | %-13d |\n",id,arrival,burst,start ,finish, wait ,turnaround , response_time); 
  
}
    printf("+----+---------+-------+-------+--------+---------+----------------+---------------+\n");



    printf("\nAverage Waiting Time: %.2f ms\n", (float)total_wait_time / n);
    printf("Average Turnaround Time: %.2f ms\n", (float)total_turnaround_time / n);
    printf("Average Response Time: %.2f ms\n", (float)total_response_time / n);


}

void simulate_preemptive_srt(Process_For_SRT processes[], int num_processes, float alpha) {
    int time = 0;            // Global simulation time
    int completed = 0;       // Number of processes completed
    int current_process = -1; // Currently running process

    int space_required = (2 * num_processes) + 1 ;
    char *seq = (char *)malloc(space_required * sizeof(char));
    if (seq == NULL) {
        printf("Memory allocation failed\n");
        exit(1) ;}
    // Initialize the string
    strcpy(seq, "seq = [");



    while (completed < num_processes) {
        int shortest_index = -1;
        float shortest_time = __FLT_MAX__;

        // Find the process with the shortest remaining time that has arrived and is not completed
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time <= time && !processes[i].completed) {
                if (processes[i].predicted_remaining_time < shortest_time) {
                    shortest_time = processes[i].predicted_remaining_time;
                    shortest_index = i;
                }
            }
        }


        // If no process can run, advance time
        if (shortest_index == -1) {
            time++;
            continue;
        }

        // If a new process has arrived with shorter remaining time, preempt the current process
        if (current_process != shortest_index) {
            
            current_process = shortest_index;

            // Record the first response time for the new process
            if (processes[current_process].start_time == -1) {
                processes[current_process].start_time = time;
            }
        }

        // Execute the process for 1 time unit (preemptively)
        processes[current_process].remaining_time--;
        processes[current_process].predicted_remaining_time--;
        time++;




        // If the process is completed
        if (processes[current_process].remaining_time == 0) {
            completed++;
            processes[current_process].completed=true;
            processes[current_process].finish_time = time;


            char str[20];  // Buffer to hold the string

            sprintf(str, "%d", processes[current_process].process_id);// Convert integer to string

            strcat(seq, str);

            if (completed < num_processes-1 ) {
            strcat(seq, ",");  // Add a comma between numbers
            }
            
            
            processes[current_process].turn_around_time = processes[current_process].finish_time - processes[current_process].arrival_time;
            processes[current_process].waiting_time = processes[current_process].turn_around_time - processes[current_process].actual_cpu_burst;
            processes[current_process].response_time = processes[current_process].waiting_time + processes[current_process].time_until_first_response;

           


            // Update prediction using exponential averaging - do we update for the next one or all of the follwoing process 
            // all process that have not yet arrived  
            int actual_burst = processes[current_process].actual_cpu_burst;
            float tau_next = (alpha * actual_burst) + ((1 - alpha) * processes[current_process].predicted_cpu_burst);

            // updatea all the ones that have not arrived yet

            for (int i = 0; i < num_processes; i++) {

                // do we change the tau for the ones that have not even arrived yet or does also cahged the taou for the ones that have arrived already 
                if (processes[i].process_id == processes[current_process].process_id ) {
                    processes[i].predicted_cpu_burst = (float) tau_next;
                    int time_ran =  processes[i].actual_cpu_burst - processes[i].remaining_time; 
                    processes[i].predicted_remaining_time = (float)processes[i].predicted_cpu_burst - time_ran;
                    
                }
            }
            current_process = -1;  // Reset current process since it has finished
        }
    }

        strcat(seq, "]");
        printf("%s\n", seq);
        free(seq);
}

void sort_same_arrival_time_processes_SRT(Process_For_SRT processes[], int n){

    // Sort processes by arrival time (if same arrival time, sort by process_id)
    // Lines with the same process id in excel file (23,24)
    
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (processes[j].arrival_time > processes[j + 1].arrival_time ||
                (processes[j].arrival_time == processes[j + 1].arrival_time &&
                 processes[j].process_id > processes[j + 1].process_id)) {
                // Swap processes[j] and processes[j + 1]
                Process_For_SRT temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }


}




int main(int argc, char *argv[]) {
    // Check if there are more than 3 arguments or no algorithm is specified
    if (argc > 3) {
        printf("Error: No scheduling algorithm specified.\n");
        return 1;
    }

    // If the scheduling algorithm is FCFS (First Come First Serve)
    if (strcmp(argv[1], "FCFS") == 0) {
        Process_For_FCFS processes[MAX_PROCESSES];  // Array to store processes
        int n = 0;  // Number of processes
        
        char buffer[BUFFER_SIZE];  // Buffer to read input

        fgets(buffer, sizeof(buffer), stdin);  // Skip the first line (header)

        // Read processes in the format: Pid, Arrival Time, Time until first Response, Burst Length
        while (fgets(buffer, sizeof(buffer), stdin)) {
            // Parse input and populate the process array
            if (sscanf(buffer, "%d,%d,%d,%d", 
                    &processes[n].process_id, 
                    &processes[n].arrival_time, 
                    &processes[n].time_until_first_response, 
                    &processes[n].burst_time) == 4) {
                n++;  // Increment process count
            }
        }

        // Sort the processes by arrival time
        sort_same_arrival_time_processes_fcfs(processes, n);

        // Simulate the FCFS algorithm
        simulate_fcfs(processes, n);
    } 
    
    // If the scheduling algorithm is SJF (Shortest Job First)
    else if (strcmp(argv[1], "SJF") == 0) {
        Process_For_SJF processes[MAX_PROCESSES];  // Array to store processes
        int n = 0;  // Number of processes
        
        char buffer[BUFFER_SIZE];  // Buffer to read input
        fgets(buffer, sizeof(buffer), stdin);  // Skip the first line (header)
        
        // Read processes in the format: Pid, Arrival Time, Time until first Response, Burst Length
        while (fgets(buffer, sizeof(buffer), stdin)) {
            // Parse input and populate the process array
            if (sscanf(buffer, "%d,%d,%d,%d", 
                    &processes[n].process_id, 
                    &processes[n].arrival_time, 
                    &processes[n].time_until_first_response, 
                    &processes[n].burst_time) == 4) {
                processes[n].finished = false;  // Initialize process as not finished
                n++;  // Increment process count
            }
        }
        
        // Sort processes by arrival time
        sort_same_arrival_time_processes_sjf(processes, n);

        // Simulate the Shortest Job Next (SPN) algorithm (non-preemptive SJF)
        simulateSPN(processes, n);

        // Display results and calculate average wait, turnaround, and response times
        display_and_calculate_averages_sjf(processes, n);
    }
    
    // If the scheduling algorithm is Priority
    else if (strcmp(argv[1], "Priority") == 0) {
        Process_For_Priority processes[MAX_PROCESSES];  // Array to store processes
        int n = 0;  // Number of processes
        char buffer[BUFFER_SIZE];  // Buffer to read input
        fgets(buffer, sizeof(buffer), stdin);  // Skip the first line (header)

        // Read processes in the format: Pid, Arrival Time, Time until first Response, Burst Length
        while (fgets(buffer, sizeof(buffer), stdin)) {
            // Parse input and populate the process array
            if (sscanf(buffer, "%d,%d,%d,%d", 
                    &processes[n].process_id, 
                    &processes[n].arrival_time, 
                    &processes[n].time_until_first_response, 
                    &processes[n].burst_time) == 4) {
                // Calculate priority as 1 / Process ID (lower process ID = higher priority)
                processes[n].priority = (float) 1.0 / (processes[n].process_id);  
                processes[n].start_time = -1;  // Start time is initially -1 (hasn't started yet)
                processes[n].completed = false;  // Mark process as not completed
                n++;  // Increment process count
            }
        }

        // Sort processes by arrival time
        sort_same_arrival_time_processes_priority(processes, n);

        // Simulate the Priority scheduling algorithm
        calculate_times(processes, n);

        // Display results and calculate average wait, turnaround, and response times
        display_and_calculate_averages_priority(processes, n);
    }
    
    // If the scheduling algorithm is Round Robin (RR)
    else if (argc == 3 && (strcmp(argv[1], "RR") == 0)) {
        Process_For_RR processes[MAX_PROCESSES];  // Array to store processes
        int n = 0;  // Number of processes
        char buffer[BUFFER_SIZE];  // Buffer to read input
        fgets(buffer, sizeof(buffer), stdin);  // Skip the first line (header)
        int q = atoi(argv[2]);  // Quantum time is the second argument

        // Read processes in the format: Pid, Arrival Time, Time until first Response, Burst Length
        while (fgets(buffer, sizeof(buffer), stdin)) {
            // Parse input and populate the process array
            if (sscanf(buffer, "%d,%d,%d,%d", 
                    &processes[n].process_id, 
                    &processes[n].arrival_time, 
                    &processes[n].time_until_first_response, 
                    &processes[n].burst_time) == 4) {
                processes[n].remaining_time = processes[n].burst_time;  // Initially, remaining time is the burst time
                processes[n].first_response = false;  // Has not yet responded
                processes[n].start_time = -1;  // Start time is initially -1 (hasn't started yet)
                n++;  // Increment process count
            }
        }

        // Sort processes by arrival time
        sort_same_arrival_time_processes_RR(processes, n);

        // Simulate the Round Robin algorithm with the given quantum
        simulateRR(processes, n, q);

        // Display results and calculate average wait, turnaround, and response times
        display_and_calculate_averages_RR(processes, n);
    }
    
    // If the scheduling algorithm is Shortest Remaining Time (SRT)
    else if (argc == 3 && (strcmp(argv[1], "SRT") == 0)) {
        Process_For_SRT processes[MAX_PROCESSES];  // Array to store processes
        int n = 0;  // Number of processes
        char buffer[BUFFER_SIZE];  // Buffer to read input
        fgets(buffer, sizeof(buffer), stdin);  // Skip the first line (header)
        float alpha = atof(argv[2]);  // Alpha value for exponential averaging

        // Read processes in the format: Pid, Arrival Time, Time until first Response, Actual CPU Burst
        while (fgets(buffer, sizeof(buffer), stdin)) {
            // Parse input and populate the process array
            if (sscanf(buffer, "%d,%d,%d,%d", 
                    &processes[n].process_id, 
                    &processes[n].arrival_time, 
                    &processes[n].time_until_first_response, 
                    &processes[n].actual_cpu_burst) == 4) {
                processes[n].remaining_time = processes[n].actual_cpu_burst;  // Initially, remaining time is the actual CPU burst
                processes[n].start_time = -1;  // Start time is initially -1 (hasn't started yet)
                processes[n].predicted_cpu_burst = 10;  // Predicted CPU burst is initially set to 10
                processes[n].predicted_remaining_time = processes[n].predicted_cpu_burst;  // Predicted remaining time
                processes[n].completed = false;  // Mark process as not completed
                n++;  // Increment process count
            }
        }

        // Sort processes by arrival time
        sort_same_arrival_time_processes_SRT(processes, n);

        // Simulate the preemptive Shortest Remaining Time (SRT) algorithm with the given alpha
        simulate_preemptive_srt(processes, n, alpha);

        // Display results and calculate average wait, turnaround, and response times
        display_and_calculate_averages_srt(processes, n);
    }
    
    // If the specified algorithm is not recognized
    else {
        printf("Error: Unsupported scheduling algorithm '%s'.\n", argv[1]);
        return 1;  // Return with an error code
    }

    return 0;  // Return success
}
