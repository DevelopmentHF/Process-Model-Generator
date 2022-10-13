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
#include <math.h>

/* #DEFINE'S -----------------------------------------------------------------*/
#define GOOD_LUCK   "GOOD LUCK CLASS!!!\n"      // good luck message
#define INITIAL (2)             // initial size of a malloc'd array
#define FOUND (1)               // used when an object has been located
#define NOT_FOUND (0)           // ...    ...      ...     not been located
#define EQUAL (1)               // used to be explicit when two things are equal
#define NOT_EQUAL (0)           // used to be explicit when two things != 

/* TYPE DEFINITIONS ----------------------------------------------------------*/

typedef struct event event_t;   // an event ...
struct event {                  // ... is composed of ...
    int actn;                   // ... an action that triggered it and ...
    event_t* next;              // ... a pointer to the next event in the trace
};

typedef struct {                // a trace is a linked list of events
    event_t* head;              // a pointer to the first event in this trace
    event_t* foot;              // a pointer to the last event in this trace
    int      freq;              // the number of times this trace was observed
} trace_t;

typedef struct {
    int actn1;                  // the 'first' action
    int actn2;                  // the action that follows the 'first action'
    int freq;                   // how many times this relationship appears
    int weight;                 // total weight of this relationship
} rel_t;

typedef struct {                // an event log is an array of distinct traces
                                //     sorted lexicographically
    trace_t* trcs;              // an array of traces
    int      ndtr;              // the number of distinct traces in this log
	int* events;                // list of distinct events
    int      nevnt;             // number of distinct events
} log_t;

/*-----------------------------------------------------------------------------*/
/* Stage 0 Prototypes*/
trace_t* trace_build();
trace_t* make_empty_trace(void);
trace_t* insert_at_foot(trace_t* trace, int letter);
int is_empty_trace(trace_t* trace);
log_t log_build();
int is_trace_present(
        trace_t input_trace, trace_t* list, int ndtr, int* located_index);
int identical_traces(event_t* input, event_t* checker);
int trace_cmp(trace_t* t1, trace_t* t2);
log_t find_num_events(log_t log);
int get_trace_len(trace_t trace);
int total_events(log_t* log);
int trace_num_details(log_t* log, int* max);
void print_trace(log_t* log, int indice);
void get_event_freq(log_t* log);
int int_cmp(const void* l1, const void* l2);
void stage0_printer(log_t* log);
/* Stage 1 Prototypes */
rel_t* rel_build(log_t* log);
void get_relationship_freq(rel_t* relationships, log_t* log);
int is_rel_present(rel_t relationship, trace_t* trace);
void relationship_freq_printer(rel_t* relationships, log_t* log);
rel_t* find_seq_candidates(
        rel_t* relationships, log_t* log, int* num_candidates);
int max_candidate_index(rel_t* candidates, int num_candidates);
void change_to_abstract(log_t* log, rel_t* max_candidate, int abstract_num);
void remove_repeated_abstracts(log_t* log, int abstract_num);
void free_list(trace_t* list);


/* WHERE IT ALL HAPPENS ------------------------------------------------------*/
int
main(int argc, char *argv[]) {

    /* ----- Stage 0 ----- */
    /* Initialise + build a log of traces to work with */
    log_t log;
    log = log_build();
    /* Fill the log event data */
    log = find_num_events(log);
    stage0_printer(&log);
    

    /* ----- Stage 1 ----- */
    /* Initialise array of directly follows relationships */
    rel_t* relationships;
    relationships = rel_build(&log);
    get_relationship_freq(relationships, &log);
   
    /* Initialise for start of while loop */
    int num_candidates = 0, abstract_num = 256, num_loops = 0;
    rel_t* candidates = find_seq_candidates(relationships, 
        &log, &num_candidates);

    /* Run until there are no more candidate relationships to choose from */
    while (num_candidates > 0) {
        /* Stage 1 formatting */
        if (num_loops > 0) {
            printf("=====================================\n     ");
        } else {
            printf("==STAGE 1============================\n     ");
        }
        relationship_freq_printer(relationships, &log);

        /* Find the maximum weighted candidate available */
        int max_index = max_candidate_index(candidates, num_candidates);
        rel_t max_candidate = candidates[max_index];
        printf("%d = SEQ(%c,%c)\n", abstract_num, 
            max_candidate.actn1, max_candidate.actn2);

        /* Get the number of events removed */
        printf("Number of events removed: %d\n", max_candidate.freq);
        /* Change all occurences of candidate events to the abstract number */
        change_to_abstract(&log, &max_candidate, abstract_num);
        /* Now, check if there is a repeated abstract numbers */
        remove_repeated_abstracts(&log, abstract_num);

        /* Incremental operations for the end of a while loop */
        log = find_num_events(log);
        get_event_freq(&log);
        relationships = rel_build(&log);
        get_relationship_freq(relationships, &log);
        num_candidates = 0;
        abstract_num++;
        candidates = find_seq_candidates(relationships, &log, &num_candidates);
        num_loops++;
    }


    /* ----- Stage 2 ----- */
    printf("==STAGE 2============================\n");

    /* ----- Free all malloc'd memory */
    /* Free relationships array */
    free(relationships);
    relationships = NULL;
    /* Free log event list */
    free(log.events);
    log.events = NULL;
    /* Free each linked list of events */
    for (int i=0; i<log.ndtr; i++) {
        free_list(&log.trcs[i]);
    }
    free(log.trcs);
    log.trcs = NULL;
    /* Free candidates array */
    free(candidates);
    candidates = NULL;
    
    return EXIT_SUCCESS;        // remember, algorithms are fun!!!
}

/* Builds a linked lists of events from the input file to form a trace */
trace_t*
trace_build() {     
    /* Initialise empty linked list (trace) */
    trace_t* trace = make_empty_trace();
    /* Get first character from input file */
    int event = getchar();
    
    /* Loop over all characters until the end of the input file */
    while(event != EOF) {
        /* Deals with case where the event is valid */
        if (isalpha((char)event)) {
            /* Insert event to end of linked list */
            trace = insert_at_foot(trace, event);
        /* Skips over commas and returns at end of trace*/
        } else if ((char)event == '\n'){
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
insert_at_foot(trace_t* trace, int letter) {
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

/* Checks if a trace is empty */
int
is_empty_trace(trace_t* trace) {
    assert(trace != NULL);
    return trace->head==NULL;
}

/* Fills in a log with data regarding traces previously built */
log_t
log_build() {
    log_t log;
    /* Initialise list of traces */
    trace_t* trace_list;
    size_t current_size = INITIAL;
    int ndtr = 0;

    /* Build traces dynamic array */
    trace_list = (trace_t*)malloc(INITIAL*sizeof(*trace_list)); 
    assert(trace_list);

    /* Gets current trace */
    trace_t cur_trace = *trace_build();

    while(is_empty_trace(&cur_trace) != 1) {
        
        int located_index;          // Where in the list of traces it was found

        /* Check if trace has already been found */
        if (is_trace_present(cur_trace, trace_list, ndtr, &located_index)
             == NOT_FOUND) {
            /* First time this element has been seen */
            cur_trace.freq = 1;
            
            /* Check if we need to re-size the array of traces */
            if (ndtr == (int)current_size) {
                current_size *= 2;
                trace_list = (trace_t*)realloc(trace_list, 
                    current_size * sizeof(*trace_list));
                assert(trace_list);
            }

            /* Assign current trace into the array of traces */
            trace_list[ndtr++] = cur_trace;

            /* Perform insertion sort on the newly added element */
            if (ndtr > 1) {        // No need to sort an array with one element
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
    /* Assign values to the log */
    log.trcs = trace_list;
    log.ndtr = ndtr;
    return log;
}

/* Check if a input trace is already contained within the array of traces */
int
is_trace_present(trace_t input_trace, trace_t* list, int ndtr,
                int* located_index) {
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

/* Checks if 2 traces are the exact same */
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

/* Compares 2 input traces based on alphabetical ordering of events */
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

/* Finds the number of distinct events in a completed input log.
    Futhermore, it also creates an array of these events and stores
    it for later use within the log */
log_t
find_num_events(log_t log) {
    /* Initialise variables */
    size_t current_size = INITIAL;
    int num_distinct = 0, event_presence = NOT_FOUND;

    /* Initialise malloc'd list of distinct events */
    log.events = (int*)malloc(INITIAL * sizeof(*log.events));
    assert(log.events);

    /* Loop over list of traces */
    for (int i=0; i<log.ndtr; i++) {
        event_t* current_event = log.trcs[i].head;
        /* Goes through each traces*/
        while (current_event != NULL) {
            event_presence = NOT_FOUND;
            /* Now check if event matches a found event */
            for (int j=0; j<num_distinct; j++) {
                if (current_event->actn == log.events[j]) {
                    event_presence = FOUND;
                    break;
                } 
            }

            /* Adds event to array of events if its first time being seen */
            if (event_presence == NOT_FOUND) {
                /* Check if there is room to add the event */
                if (num_distinct == (int)current_size) {
                    current_size *= 2;
                    log.events = (int*)realloc(log.events, 
                        current_size*sizeof(*log.events));
                    assert(log.events);
                }
                /* Add the event to the array */
                log.events[num_distinct++] = current_event->actn;
            }

            /* Increment as standard for end of while loop */
            current_event = current_event->next;
        }
    }
    /* Assign to the log */
    log.nevnt = num_distinct;
    return log;
}

/* Gets the lenght of a given trace */
int
get_trace_len(trace_t trace) {
    int count = 0;
    /* Loop over the trace and increment a counter */
    event_t* current = trace.head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

/* Returns the total number of events in a event log */
int
total_events(log_t* log) {
    /* Loop over each trace */
    int total_sum = 0;
    for (int i=0; i<log->ndtr; i++) {
        /* Sum each length of trace multiplied by its frequency */
        total_sum += (get_trace_len(log->trcs[i]) * (log->trcs[i].freq));
    }
    return total_sum;
}

/* Gets the total number of traces, and the maximum trace frequency */
int
trace_num_details(log_t* log, int* max) {
    int sum = 0, max_freq = 0;
    /* Loop through all traces */
    for (int i=0; i<log->ndtr; i++) {
        int cur_freq = log->trcs[i].freq;
        /* Standard maximum counter notation */
        if (cur_freq > max_freq) {
            max_freq = cur_freq;
            *max = max_freq;
        }
        sum += cur_freq;
    }
    return sum;
}

/* Prints a given trace out onto the CL */
void
print_trace(log_t* log, int indice) {
    trace_t trace = log->trcs[indice];
    /* Traverse the linked list, printing each character */
    event_t* cur_event = trace.head;
    while(cur_event!= NULL) {
        printf("%c", cur_event->actn);
        cur_event = cur_event->next;
    }
    printf("\n");
}

/* Gets the frequency of each distinct event */
void
get_event_freq(log_t* log) {
    /* First sort the array of distinct events */
    qsort(log->events, log->nevnt, sizeof(int), int_cmp);
    for (int i=0; i<log->nevnt; i++) {
        /* Get each action */
        int action = log->events[i];
        /* Now sum its frequencies in the list of traces */
        int sum = 0;    // Number of appearances multiplied by trace nums
        for (int j=0; j<log->ndtr; j++) {
            int value = 0;  // Number of times it appears in one trace
            event_t* current_event = log->trcs[j].head;
            /* Traverse the list */
            while (current_event != NULL) {
                if (current_event->actn == action) {
                    value++;
                }
                current_event = current_event->next;
            }
            sum += value * log->trcs[j].freq;
        }
        /* Deals with character / integer formatting */
        if (action < 255) {
            printf("%c = %d\n", action, sum);
        } else {
            printf("%d = %d\n", action, sum);
        }
        
    }
}

/* Used to compare 2 integers sizes */
int
int_cmp(const void* l1, const void* l2) {
    return *(int*)l1 - *(int*)l2;
}

/* Prints out all necessary details for Stage 0 */
void stage0_printer(log_t* log) {
    int max_frequency;
    printf("==STAGE 0============================\n");
    printf("Number of distinct events: %d\n", log->nevnt);
    printf("Number of distinct traces: %d\n", log->ndtr);
    printf("Total number of events: %d\n", total_events(log));
    printf("Total number of traces: %d\n", trace_num_details(
            log, &max_frequency));
    printf("Most frequent trace frequency: %d\n", max_frequency);

    /* Go through array of traces; if it has max freq, print it */
    for (int i=0; i<log->ndtr; i++) {
        if (log->trcs[i].freq == max_frequency) {
            print_trace(log, i);
        }
    }
    get_event_freq(log);
}

/* Stage 1 Functions */

/* Builds an array of relationships from the given event log */
rel_t*
rel_build(log_t* log) {
    /* Initialise array of DF relationships to later return */
	size_t max_size = log->nevnt * log->nevnt; 	//  nvent^2 possible pairings
    rel_t* relationships = malloc(max_size * sizeof(*relationships));
	
    /* Loop through each pairing of characters */
	int count = 0;
    for (int i=0; i<log->nevnt; i++) {
        for (int j=0; j<log->nevnt; j++) {
            rel_t current_relationship;
			
			/* Set values */
			current_relationship.freq = 0;
			current_relationship.actn1 = log->events[i];
			current_relationship.actn2 = log->events[j];
			
			relationships[count++] = current_relationship;
			
        }
    }
    return relationships;
}

/* Gets the frequency of each relationship in the relationship array */
void
get_relationship_freq(rel_t* relationships, log_t* log) {
    /* Get current relationship */
    for (int i=0; i<log->nevnt * log->nevnt; i++) {
        /* Loop over each trace and find if the current relationship occurs */
        for (int j=0; j<log->ndtr; j++) {
            trace_t cur_trace = log->trcs[j];

            /* Check if relationship is present in the current trace */
            if (is_rel_present(relationships[i], &cur_trace) == FOUND) {
                relationships[i].freq+=cur_trace.freq;
            }
        }
    }
}

/* Check if a given relationship is already present in list of relationships */
int
is_rel_present(rel_t relationship, trace_t* trace) {
    /* Traverse the trace */
    event_t* cur_event1 = trace->head;
    event_t* cur_event2 = cur_event1->next;

    while (cur_event2 != NULL) {
        /* Check if pattern matches*/
        if ((relationship.actn1 == cur_event1->actn) &&
             (relationship.actn2 == cur_event2->actn)) {
            return FOUND;
        }
        /* Incemental operations for while loop */
        cur_event1 = cur_event2;
        cur_event2 = cur_event2->next;
    }
    return NOT_FOUND;
}

/* Prints out frequency's of relationships matrix */
void
relationship_freq_printer(rel_t* relationships, log_t* log) {
    /* Print the event header row */
    for (int i=0; i<log->nevnt; i++) {
        if (log->events[i] < 255) {
            printf("%5c", log->events[i]);
        } else {
            printf("%5d", log->events[i]);
        }
	}
	printf("\n");

    /* Iterate through and print each value */
    for (int i=0; i<log->nevnt * log->nevnt; i++) {
		
		if (i % log->nevnt == 0) {
			if(i!=0) {printf("\n");}    // formatting for start of matrix
            if (relationships[i].actn1 < 255) {
                printf("%5c", relationships[i].actn1);
            } else {
                printf("%5d", relationships[i].actn1);
            }

		}
		
		printf("%5d", relationships[i].freq);
	}
    printf("\n-------------------------------------\n");
}

/* Creates an array of potential candidate relationships to be removed */
rel_t*
find_seq_candidates(rel_t* relationships, log_t* log, int* num_candidates) {
    /* Initialise candidates array */
    rel_t* candidates;
    size_t current_size = INITIAL;
    candidates = (rel_t*)malloc(INITIAL * sizeof(*candidates));
    assert(candidates);

    /* Loop through the relationships */
    for (int i=0; i<((log->nevnt) * (log->nevnt)); i++) {
        /* Now, we have current relationship struct */
        // dont use the diagonal line y=-x
        if (relationships[i].actn1 != relationships[i].actn2) { 
            /* Find its 'complement' */
            for (int j=0; j<((log->nevnt) * (log->nevnt)); j++) {
                if (relationships[i].actn1 == relationships[j].actn2 &&
                     relationships[i].actn2 == relationships[j].actn1) {
                    /* Complement found */
                    /* Check that sup(x,y) > sup(y,x) */
                    if (relationships[i].freq > relationships[j].freq &&
                        relationships[i].actn1 < 255 && relationships[j].actn1){
                        int cur_pd = (100 * abs(relationships[i].freq -
                             relationships[j].freq))
                             / relationships[i].freq;
                        int weight = abs(50 - cur_pd) * relationships[i].freq;

                        if (cur_pd > 70) {
                            /* Add this to array of candidates */
                            /* first, check if there is room in the array of
                                 candidate relations */
                            if (*num_candidates == (int)current_size) {
                                current_size *= 2;
                                candidates = (rel_t*)realloc(candidates, 
                                    current_size*sizeof(*candidates));
                                assert(candidates);
                            }
                            relationships[i].weight = weight;
                            candidates[*num_candidates] = relationships[i];
                            *num_candidates = *num_candidates + 1;
                        }
                    }
                    break;
                }
            }
        }
    }
    return candidates;
}

/* Returns the index of the candidate with the highest weight */
int
max_candidate_index(rel_t* candidates, int num_candidates) {
    int max_weight = 0, max_index;
    /* Traverse the candidates */
    for (int i=0; i<num_candidates; i++) {
        if (candidates[i].weight > max_weight) {
            max_weight = candidates[i].weight;
            max_index = i;
        }
    }
    return max_index;
}

/* Change all occurences of candidate events to be the abstract number */
void
change_to_abstract(log_t* log, rel_t* max_candidate, int abstract_num) {
    /* Loop through all traces */
    for (int i=0; i<log->ndtr; i++) {
        event_t* current_event = log->trcs[i].head;
        while (current_event != NULL) {
            /* If this should be abstract, repalce its value */
            if (current_event->actn == max_candidate->actn1 || 
                current_event->actn == max_candidate->actn2) {

                current_event->actn = abstract_num;
            }
            current_event = current_event->next;
        }
    }
} 

/* Removes subsequences of repeated abstract events in a trace */
void
remove_repeated_abstracts(log_t* log, int abstract_num) {
    /* Loop through all traces */
    for(int i=0; i<log->ndtr; i++) {
        event_t* current_event = log->trcs[i].head;
        event_t* next = current_event->next;
        while (next != NULL && current_event != NULL) {
            /* Checks if two side by side events are the same */
            if (current_event->actn == abstract_num && 
                next->actn == abstract_num) {
                current_event->next = next->next;
                current_event = next->next;
                if (current_event != NULL) {
                    next = current_event->next;
                }   
            /* Deal with niche case at end of a linked list */
            } else {
                current_event = current_event->next;
                if (current_event != NULL) {
                    next = current_event->next;
                }
            }
        }
    }
}

/* free_list() modified from Alistair Moffat, as an example for the book
   "Programming, Problem Solving, and Abstraction with C"
*/
void
free_list(trace_t* list) {
	event_t* curr;
    event_t* prev;
	assert(list!=NULL);

	curr = list->head;
	while (curr != NULL) {
		prev = curr;
		curr = curr->next;
		free(prev);
	}
}