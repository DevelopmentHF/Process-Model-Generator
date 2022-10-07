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

  Signed by: [Enter your full name and student number here before submission]
  Dated:     [Enter the date that you "signed" the declaration]

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
    action_t actn;              // ... an action that triggered it and ...
    event_t* next;              // ... a pointer to the next event in the trace
};

typedef struct {                // a trace is a linked list of events
    event_t* head;              // a pointer to the first event in this trace
    event_t* foot;              // a pointer to the last event in this trace
    int      freq;              // the number of times this trace was observed
} trace_t;

typedef struct {                // an event log is an array of distinct traces
                                //     sorted lexicographically
    trace_t* trcs;              // an array of traces
    int      ndtr;              // the number of distinct traces in this log
    int      cpct;              // the capacity of this event log as the number
                                //     of  distinct traces it can hold
} log_t;

typedef action_t** DF_t;        // a directly follows relation over actions

/* Stage 0 Prototypes*/
trace_t* trace_build();
trace_t* make_empty_trace(void);
trace_t* insert_at_foot(trace_t* trace, char letter);
int is_empty_trace(trace_t* trace);
log_t log_build();
int is_trace_present(trace_t input_trace, trace_t* list, int ndtr, int* located_index);
int identical_traces(event_t* input, event_t* checker);
int trace_cmp(trace_t* t1, trace_t* t2);

/* WHERE IT ALL HAPPENS ------------------------------------------------------*/
int
main(int argc, char *argv[]) {
    log_t log;
    log = log_build();
   
    for (int i = 0; i<log.ndtr; i++) {
        printf("freq %d = %d\n", i, log.trcs[i].freq);
    }

    return EXIT_SUCCESS;        // remember, algorithms are fun!!!
}

trace_t*
trace_build() {     
    /* Get pointer to malloc'd empty linked list (trace)*/
    trace_t* trace = make_empty_trace();
    /* Get first character from input text */
    char event = getchar();
    
    /* Loop over all characters until the end of the input file */
    while(event != EOF) {
        /* Deals with case where the character is a letter*/
        if (isalpha(event)) {
            /* Insert the character to end of linked list */
            trace = insert_at_foot(trace, event);
        /* Skips over commas and returns at end of trace*/
        } else if (event == '\n'){
            return trace;
        }
        /* Get the next character */
        event = getchar();
    }

    return trace;
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
is_empty_trace(trace_t* trace) {
    assert(trace != NULL);
    return trace->head==NULL;
}

log_t
log_build() {
    log_t log;
    /* Initialise list of traces */
    trace_t* trace_list;
    size_t current_size = INITIAL;
    int ndtr = 0;

    /* Build traces dynamic array */
    trace_list = (trace_t*)malloc(INITIAL*sizeof(*trace_list)); // allocate space for trace_t's
    assert(trace_list);

    /* Gets current trace */
    trace_t cur_trace = *trace_build();

    while(is_empty_trace(&cur_trace) != 1) {
        
        int located_index;          // Where in the list of traces it was found

        /* Check if trace has already been found */
        if (is_trace_present(cur_trace, trace_list, ndtr, &located_index) == NOT_FOUND) {
            /* First time this element has been seen */
            cur_trace.freq = 1;
            
            /* Check if we need to re-size the array of traces */
            if (ndtr == current_size) {
                current_size *= 2;
                trace_list = (trace_t*)realloc(trace_list, current_size * sizeof(*trace_list));
                assert(trace_list);
            }

            /* Assign current trace into the array of traces */
            trace_list[ndtr++] = cur_trace;

            /* Perform insertion sort on the newly added element */
            if (ndtr > 1) {             // No need to sort an array with one element
                for (int i=ndtr-1; i>0; i--) {
                    if (trace_cmp(&trace_list[i], &trace_list[i-1]) == -1) {
                        /* Swap the 'bigger trace' with the smaller */
                        trace_t temp = trace_list[i];
                        trace_list[i] = trace_list[i-1];
                        trace_list[i-1] = temp;
                        
                    } else {break;} // Now in order
                }
            }

        } else {
            /* Trace has already been seen */
            trace_list[located_index].freq++;
        } 

        /* Incremental operation required at end of while loop */
        cur_trace = *trace_build();
    }
    log.trcs = trace_list;
    log.ndtr = ndtr;
    return log;
}

int
is_trace_present(trace_t input_trace, trace_t* list, int ndtr, int* located_index) {
    /* If list is empty, it must be its first occurence*/
    if (ndtr < 1) {
        return NOT_FOUND;
    }

    for (int i=0; i<ndtr; i++) {
        /* Initialisations to check each element of linked lists*/
        event_t* input_event = input_trace.head; 
        event_t* checker_event = list[i].head; 

        /* Check each trace in the list of traces for being identical */
        if (identical_traces(input_event, checker_event) == EQUAL) {
            *located_index = i;
            return FOUND;
        }
    }

    return NOT_FOUND;
}

int
identical_traces(event_t* input, event_t* checker) {
    while (input != NULL && checker != NULL) {
        /* Checks if each event is the same */
        if (input->actn != checker->actn) {
            return NOT_EQUAL;
        }
        /* Move on to next event */
        input = input->next;
        checker = checker->next;
    }
    /* If both events aren't null here, they're not equal */
    if (input == NULL && checker == NULL) {return EQUAL;} 
    return NOT_EQUAL;
}

int
trace_cmp(trace_t* t1, trace_t* t2) {
    /* Initialises first event to be checked */
    event_t* cur_event1 = t1->head;
    event_t* cur_event2 = t2->head;

    /* Checks until it reaches the end of a trace */
    while (cur_event1 != NULL && cur_event2 != NULL) {

        if(cur_event1->actn > cur_event2->actn) {
            return 1;
        } else if (cur_event1->actn < cur_event2->actn) {
            return -1;
        } 
        /* Continue traversing the trace */
        cur_event1 = cur_event1->next;
        cur_event2 = cur_event2->next;
    }

    /* Do one last check on the final nodes */
    if (cur_event1 == NULL && cur_event2 != NULL) {
        return -1;
    } else if (cur_event1 != NULL && cur_event2 == NULL) {
        return 1;
    } 
    
    return 0;
}