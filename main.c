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
    event_t* next;              // ... a pointer to the next event in the trace
};

typedef struct {                // a trace is a linked list of events       // I.e abcde
    event_t* head;              // a pointer to the first event in this trace
    event_t* foot;              // a pointer to the last event in this trace
    int      freq;              // the number of times this trace was observed
} trace_t;

typedef struct {                // an event log is an array of distinct traces
                                //     sorted lexicographically (alphabetically? check this henry.)
    trace_t* traces;            // an array of traces
    int      num_distinct;      // the number of distinct traces in this log
    int      capacity;          // the capacity of this event log as the number
                                //     of  distinct traces it can hold
} log_t;

typedef action_t** DF_t;        // a directly follows relation over actions // 2d array of actions

/* PROTOTYPES ----------------------------------------------------------------*/
trace_t* trace_build();
void log_build(log_t* input_log);
trace_t* make_empty_trace(void);
trace_t* insert_at_foot(trace_t* trace, char letter);
int is_trace_present(trace_t* trace, trace_t** list, int num_traces);
int is_empty_trace(trace_t* trace);

/* WHERE IT ALL HAPPENS ------------------------------------------------------*/
int
main(int argc, char *argv[]) {
    
    /* First, store the input event log for later use */
    // maybe use a 'get trace' equivalent of getword ??
    log_t inputs;
    log_build(&inputs);

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
            printf("%c", letter);
           // insert_at_tail function listops.c
           insert_at_foot(trace, letter);

        } else if (event != ','){
            // printf("\nHead: %c, Foot: %c\n", trace->head->actn, trace->foot->actn);
            printf("\n");
            // return an int to signal to call again in main while loop
            return trace;
        }
        event = getchar();
    }
    if (event == EOF) {
        printf("\n");
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
        int found_flag = is_trace_present(cur_trace, trace_list, num_distinct);
        
        if (found_flag == NOT_FOUND) {
            cur_trace->freq=0;
            printf("NOT SEEN\n\n");

        } else if (found_flag == FOUND) {
            cur_trace->freq = cur_trace->freq + 1;
            printf("ALREADY SEEN %d times\n\n", cur_trace->freq);
        } 
        
        // Check if malloc'd list is big enough
        if (num_distinct == current_size) {
            current_size *= 2;
            trace_list = (trace_t**)realloc(trace_list, current_size*sizeof(*trace_list));
            assert(trace_list);
        }

        trace_list[num_distinct] = (trace_t*)malloc(sizeof(trace_t*));
        assert(trace_list[num_distinct]);
        trace_list[num_distinct] = cur_trace;
        num_distinct++;

        // Incremental operations
        printf("------");
        cur_trace = trace_build();
    }

}

/* make_empty_trace() modified from Alistair Moffat, as an example for the book
   "Programming, Problem Solving, and Abstraction with C", Pearson
   Custom Books, Sydney, Australia, 2002; revised edition 2012,
   ISBN 9781486010974.
   See http://people.eng.unimelb.edu.au/ammoffat/ppsaa/ for further
   information.
   Prepared December 2012 for the Revised Edition
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
   "Programming, Problem Solving, and Abstraction with C", Pearson
   Custom Books, Sydney, Australia, 2002; revised edition 2012,
   ISBN 9781486010974.
   See http://people.eng.unimelb.edu.au/ammoffat/ppsaa/ for further
   information.
   Prepared December 2012 for the Revised Edition
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
is_trace_present(trace_t* trace, trace_t** list, int num_traces) {
    // Case where the list is empty
    if (num_traces < 1) {
        return NOT_FOUND;
    }

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
            return FOUND;
        }
    }
    return NOT_FOUND;
}

int
is_empty_trace(trace_t* trace) {
    assert(trace != NULL);
    return trace->head==NULL;
}