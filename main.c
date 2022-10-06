/* Program to discover a process model from an event log.

  Skeleton program written by Artem Polyvyanyy, artem.polyvyanyy@unimelb.edu.au,
  August 2022, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.

  Signed by: Henry Fielding -- 1356291
  Dated:     03/10/2022

*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

/* #DEFINE'S -----------------------------------------------------------------*/
#define GOOD_LUCK   "GOOD LUCK CLASS!!!\n"      // good luck message
#define INITIAL (2)
#define FOUND (1)
#define NOT_FOUND (0)
#define EQUAL (1)
#define NOT_EQUAL (0)
/* TYPE DEFINITIONS ----------------------------------------------------------*/
typedef unsigned int action_t;  // an action is identified by an integer

typedef struct event event_t;   // an event ...
struct event {                  // ... is composed of ...
    char actn;              // ... an action that triggered it and ...
    int freq;                   // num times that this event appeared (only used sometimes)
    event_t* next;              // ... a pointer to the next event in the trace
};

typedef struct {                // a trace is a linked list of events       // I.e abcde
    event_t* head;              // a pointer to the first event in this trace
    event_t* foot;              // a pointer to the last event in this trace
    int      freq;              // the number of times this trace was observed
} trace_t;

typedef struct {                // an event log is an array of distinct traces
                                //     sorted lexicographically (alphabetically? check this henry.)
    trace_t** traces;            // an array of pointers to traces
    int      num_distinct;      // the number of distinct traces in this log
    char*    distinct_events;
    int      num_events;          // the capacity of this event log as the number
    int*     max_freq_traces; // linked list of trace indices in the array of traces that have max freq
                                //     of  distinct traces it can hold
} log_t;

typedef action_t** DF_t;        // a directly follows relation over actions // 2d array of actions



/* PROTOTYPES ----------------------------------------------------------------*/
trace_t* trace_build();
void log_build(log_t* input_log);
int trace_cmp(trace_t* t1, trace_t* t2);
trace_t* make_empty_trace(void);
trace_t* insert_at_foot(trace_t* trace, char letter);
int is_trace_present(trace_t* trace, trace_t** list, int num_traces, int* index);
int is_empty_trace(trace_t* trace);
int find_num_events(log_t* log);
int get_trace_len(trace_t* trace);
void get_event_frequencies(log_t* log);
int trace_num_details(log_t* log, int* max);
void print_trace(log_t* log, int max_indice);
void stage0_printer(log_t* log);

/* WHERE IT ALL HAPPENS ------------------------------------------------------*/
int
main(int argc, char *argv[]) {
    
    /* First, store the input event log for later use */
    log_t inputs;
    log_build(&inputs);
    stage0_printer(&inputs);

    return EXIT_SUCCESS;        // remember, algorithms are fun!!!
}

trace_t*
trace_build() {     // remove this argyment and put it in the log_build()
    // Needs to build a linked list of arbitrary size
    trace_t* trace = make_empty_trace();
    char event = getchar();

    while(event != EOF) {
        char letter;
        if (isalpha(event)) {
            /* BUILD EACH TRACE IN HERE */
            letter = event;
            insert_at_foot(trace, letter);

        } else if (event != ','){
            // return an int to signal to call again in main while loop
            return trace;
        }
        event = getchar();
    }
    if (event == EOF) {
        // printf("\n");
        return trace;
    } else {
        return NULL;
    }
}

void
log_build(log_t* input_log) {
    // Initialise list of traces
    trace_t** trace_list;
    size_t current_size = INITIAL;
    int num_distinct = 0;

    // Build traces dynamic array
    trace_list = (trace_t**)malloc(INITIAL*sizeof(*trace_list));
    assert(trace_list);

    /* Gets current trace and adds it to the array */
    trace_t* cur_trace = trace_build();
    while(is_empty_trace(cur_trace) != 1) {
        // Check if this trace is already present in the array
        // also could just freq frequency of a thing first to make more efficient
        int found_index;
        int found_flag = is_trace_present(cur_trace, trace_list, num_distinct, &found_index);
        
        if (found_flag == NOT_FOUND) {

            cur_trace->freq=1;
            
            if (num_distinct == current_size) {
                current_size *= 2;
                trace_list = (trace_t**)realloc(trace_list, current_size*sizeof(*trace_list));
                assert(trace_list);
            }

            trace_list[num_distinct] = (trace_t*)malloc(sizeof(trace_t*));
            assert(trace_list[num_distinct]);
            trace_list[num_distinct++] = cur_trace;
            // Do insertion sort here -----------------
            if (num_distinct > 1) {
                for (int i=num_distinct-1; i>0; i--) {
                    // change the guard to compare two linked lists
                    if (trace_cmp(trace_list[i], trace_list[i-1]) == -1) {
                        // swap the two 
                        trace_t* temp = trace_list[i];
                        trace_list[i] = trace_list[i-1];
                        trace_list[i-1] = temp;
                        printf("swapped");
                    } else {
                        break;
                    }
                }
            }

        } else if (found_flag == FOUND) {
            trace_list[found_index]->freq++;
        } 

        // Incremental operations
        // printf("------");
        cur_trace = trace_build();
    }
    input_log->traces = trace_list;
    input_log->num_distinct = num_distinct;
    // input_log->capacity
}

int
trace_cmp(trace_t* t1, trace_t* t2) {
    event_t* cur_event1 = t1->head;
    event_t* cur_event2 = t2->head;

    // traverses the LL's and checks
    while (cur_event1->next != NULL || cur_event2->next != NULL) {
        if(cur_event1->actn > cur_event2->actn) {
            return 1;
        } else if (cur_event1->actn < cur_event2->actn) {
            return -1;
        } else {
            cur_event1 = cur_event1->next;
            cur_event2 = cur_event2->next;
        }
    }
    // One more check for final nodes
    if(cur_event1->actn > cur_event2->actn) {
        return 1;
    } else if (cur_event1->actn < cur_event2->actn) {
        return -1;
    } 
    return 0;
}


/* make_empty_trace() modified from Alistair Moffat, as an example for the book
   "Programming, Problem Solving, and Abstraction with C"
*/
trace_t*
make_empty_trace(void) {
	trace_t* trace;
	trace = (trace_t*)malloc(sizeof(*trace));
	assert(trace!=NULL);
    trace->head = trace->foot = NULL;
	return trace;
}


/* insert_at_foot() modified from Alistair Moffat, as an example for the book
   "Programming, Problem Solving, and Abstraction with C"
*/
trace_t*
insert_at_foot(trace_t* trace, char letter) {
    event_t* new;

    new = (event_t*)malloc(sizeof(*new));
    assert(trace!=NULL && new!=NULL);
    
    new->actn = letter;
    new->next = NULL;

    if (trace->foot == NULL) {
        /* First insertion in list */
        trace->head = trace->foot = new;
    } else {
        trace->foot->next = new;
        trace->foot = new;
    }
    return trace;
}

int
is_trace_present(trace_t* trace, trace_t** list, int num_traces, int* index) {
    // Case where the list is empty
    if (num_traces < 1) {
        return NOT_FOUND;
    }

    // needs to also return a indicie
    int equal_flag;

    for (int i=0; i<num_traces; i++) {
        trace_t* checker_trace = list[i];
        event_t* trace_event=trace->head;
        event_t* checker_event=checker_trace->head;

        while (trace_event!=NULL && checker_event!=NULL) {
            
            if (trace_event->actn != checker_event->actn) {
                equal_flag = NOT_EQUAL;
                break;
            } else {
                trace_event = trace_event->next;
                checker_event = checker_event->next;
                equal_flag = EQUAL;
            }
        }
       
        if (equal_flag == EQUAL) {
            *index = i;
            return FOUND;
        }
    }
    return NOT_FOUND;
}

/* change name to not copy alistair */
int
is_empty_trace(trace_t* trace) {
    assert(trace != NULL);
    return trace->head==NULL;
}

int
find_num_events(log_t* log) {
    /* maybe can implement this when getting each letter to save 1 loop */
    char* event_list;
    size_t current_size = INITIAL;
    int num_distinct_events = 0;

    event_list = (char*)malloc(INITIAL * sizeof(*event_list));
    assert(event_list);

    int event_present_flag = NOT_FOUND;
    
    // Loops over all traces to find all events (chars)
    for (int i=0; i<log->num_distinct; i++) {
        event_t* cur_event = log->traces[i]->head;

        // loop thru each trace
        int end_flag = 0;
        while(cur_event->next!=NULL || end_flag == 1) {
            // Checks if char is present in found array of chars
            // no null byte > not a string but simply array of chars
            // can optimise but dont yet
            for (int j=0; j<num_distinct_events; j++) {
                if (cur_event->actn == event_list[j]) {
                    event_present_flag = FOUND;
                    // printf("%c found!\n", cur_event->actn);
                    break;
                } else {
                    // printf("%c not found!\n", cur_event->actn);
                    event_present_flag = NOT_FOUND;
                }
            }

            if (event_present_flag == NOT_FOUND) {
                // printf("%c not found!\n", cur_event->actn);
                if (num_distinct_events == current_size) {
                    current_size *= 2;
                    event_list = (char*)realloc(event_list, current_size*sizeof(*event_list));
                    assert(event_list);
                } 
                event_list[num_distinct_events++] = cur_event->actn;
            }

            // runs one more time if at the end of the linked list (i think)
            if (end_flag == 1) {
                break;
            } else {
                cur_event = cur_event->next;
                if (cur_event->next==NULL) {
                    end_flag = 1;                       // CHANGE OVER FROM MAGIC NUMBER BOZO!!!!!!
                }
            }
        }   
    }
    log->num_events = num_distinct_events;
    log->distinct_events = event_list;
    return num_distinct_events;
}

int
total_events(log_t* log) {
    // Go through all traces
    // Add up length of each trace
    int sum = 0;
    for (int i=0; i<log->num_distinct; i++) {
        int value = get_trace_len(log->traces[i]) * (log->traces[i]->freq);
        sum += value;
    }
    return sum;
}

int
get_trace_len(trace_t* trace) {
    int sum = 0;
    event_t* current = trace->head;
    while (current != NULL) {
        sum++;
        current = current->next;
    }
    return sum;
}


void
get_event_frequencies(log_t* log) {
    // for finding how many times each event happens
    // could use the event list created in find_num_events() -> make sure its free'd after use as well? maybe.
    // could then sort it alpabetically to get print order correct
    // then.. find how many times each event appears in a distinct trace
    // multiply by frequency 
    for (int i=0; i<log->num_events; i++) {
        printf("Distinct Letter [%d]: %c\n", i, log->distinct_events[i]);
        // Now check each trace for how many times its present
        
    }
}

int
trace_num_details(log_t* log, int* max) {
    int sum = 0, max_freq = 0;
    for (int i=0; i<log->num_distinct; i++) {
        int cur_freq = log->traces[i]->freq;
        printf("cur freq = %d    max freq = %d\n", cur_freq, max_freq);
        if (cur_freq > max_freq) {
            max_freq = cur_freq;
            // SEE NOTE AT BOTTOM
            *max = max_freq;
        }
        // printf("freq of this = %d", log->traces[2]->freq);
        sum += cur_freq;
    }
    return sum;
}

void
print_trace(log_t* log, int indice) {
    trace_t* max_freq_trace = log->traces[indice];
    // Traverse the linked list
    event_t* cur_event = max_freq_trace->head;
    while(cur_event->next != NULL) {
        printf("%c", cur_event->actn);
        cur_event = cur_event->next;
    }
    printf("%c", cur_event->actn);
    printf("\n");
    
}

void
stage0_printer(log_t* log) {
    /* Print Details */
    int max_trace = 0;
    printf("==STAGE 0============================\n");
    printf("Number of distinct events: %d\n", find_num_events(log));
    printf("Number of distinct traces: %d\n", log->num_distinct);
    printf("Total number of events: %d\n", total_events(log));
    printf("Total number of traces: %d\n", trace_num_details(log, &max_trace));
    printf("Most frequent trace frequency: %d\n", max_trace);
    // GO through array of LL's. if it has max freq, print it
    for (int i=0; i<log->num_distinct; i++) {
        if (log->traces[i]->freq == max_trace) {
            print_trace(log, i);
        }
    }
    // now find quantities of each event
    get_event_frequencies(log);
}

