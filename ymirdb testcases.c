/**
 * comp2017 - assignment 2
 * Andrew Cheon
 * ache0855
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <limits.h>
#include "ymirdb.h"


// help and bye commands, print required output

void cmd_bye() {
	printf("bye\n");
}


void cmd_help() {
	printf("%s\n", HELP);
}



// frees all entries starting from the root entry;
void free_entry(entry* root) {

	entry* temp;

	// traverse until the last entry 
	while (root != NULL) {

		// freeing all values in entry struct before freeing the entry
		if (root->values != NULL) {
			free(root->values);
			root->values = NULL;
		}

		if (root->forward != NULL) {
			free(root->forward);
			root->forward = NULL;
		}

		if (root->backward != NULL) {
			free(root->backward);
			root->backward = NULL;
		}

		temp = root;
		root = root->next;

		free(temp);
	}
}



// frees all snapshot starting from root snapshot 
void free_snapshot(snapshot* root) {

	snapshot* temp;

	// traverse unitl the last snapshot 
	while (root != NULL) {

		// freeing entries first before freeing the snapshot
		if (root->entries != NULL) {
			free_entry(root->entries);
			root->entries = NULL;
		}
		
		temp = root;
		root = root->next;

		free(temp);
	}
}



// initialize and return snapshot 
snapshot* snapshot_init() {

    snapshot* head = malloc(sizeof(snapshot));

	// set id to -1 as default, avoid having id with valid snapshot
	head->id = -1;

	// set all other entries to null
	head->entries = NULL;
	head->next = NULL;
	head->prev = NULL;

    return head;
}



// initialize and return entry 
entry* entry_init() {

	entry* head = malloc(sizeof(entry));

	// set key to "" avoid having name with valid entry
	strcpy(head->key, "");

	// set is_simple to 1 (entry is simple) as default
	head->is_simple = '1';

	// dynamic array values uninitialised as defualt 
	head->values = NULL;
	head->capacity = 0;
	head->length = 0;

	//following values are set to default for later commands
	head->seen = 0;
	head->del = 0;
	head->dup = 0;
	head->min = INT_MAX;
	head->max = INT_MIN;
	head->sum = 0;
	head->output_len = 0;

	// initially no next or prev
	head->next = NULL;
	head->prev = NULL;

	// dynamic array forward uninitialised as default 
	head->forward_size = 0;
	head->forward_max = 0;
	head->forward = NULL;
	
	// dynamic array backward uninitialised as default 
	head->backward_size = 0;
	head->backward_max = 0;
	head->backward = NULL;

	return head;
}



// initialses mememory for values array for an entry 
void init_element_space(entry* new_entry) {

	// checks for null (null == uninitialised)
	if (new_entry->values == NULL) {

		// initialising values set memory for 2 elements 
		size_t size = sizeof(element)*2;
		new_entry->values = malloc(size);

		// update capcity to reflect size of values array 
		new_entry->capacity = size;
	}
}




// increases memory for values array when capacity is hit 
void increase_element_space(entry* new_entry) {

	// number of elements equals capacity of values array
	if (new_entry->capacity/sizeof(element) == new_entry->length) {

		// double the amount of space and reallocate memory 
		new_entry->capacity *=2;
		new_entry->values = realloc(new_entry->values, new_entry->capacity);
	} 

}



// decreases memory for values array when number of element < capacity/2 
void decrease_element_space(entry* new_entry) {

	// number of element < capacity/2 
	int test_1 = new_entry->length < new_entry->capacity/(2 *sizeof(element));

	// values array is not empty 
	int test_2 = new_entry->length != 0;

	if (test_1 && test_2) {

		// reduce values array by half and reallocate memory 
		new_entry->capacity = new_entry->capacity/2;
		new_entry->values = realloc(new_entry->values, new_entry->capacity);
	}
}




// initialises memory for forward array 
void init_forward_size(entry* new_entry) {

	// if forward array is uninitialised 
	if (new_entry->forward == NULL) {

		// allocate memory for 1 entry and update forward max 
		new_entry->forward_max = sizeof(entry);
		new_entry->forward = malloc(new_entry->forward_max);
	}
}


// increases memory for forward array when forward max is hit 
void increase_forward_size(entry* new_entry) {

	// number of entry in forward equal to max 
	if (new_entry->forward_max == new_entry->forward_size) {

		// double forward max and reallocate memory 
		size_t new = new_entry->forward_max*2;
		new_entry->forward_max = new;
		new_entry->forward = realloc(new_entry->forward, new);
	}
}



// decreases memory for forward array when max/2 > forward entries in forward 
void decrease_forward_size(entry* new_entry) {

	// max/2 >  forward entries in forward 
	int test_1 = new_entry->forward_max/2 > new_entry->forward_size;
	
	// forward is not empty 
	int test_2 = new_entry->forward_size != 0;

	if (test_1 && test_2) {
	
		// half forward max and reallocate memeory 
		size_t new = new_entry->forward_max/2;
		new_entry->forward_max = new;
		new_entry->forward = realloc(new_entry->forward, new);
	}
}



// initialises memory for backward array 
void init_backward_size(entry* new_entry) {

	// backward array is not initialised 
	if (new_entry->backward == NULL) {

		// allocate memory for 1 entry and update backward max
		new_entry->backward_max = sizeof(entry);
		new_entry->backward = malloc(new_entry->backward_max);
	}
}



// increases memory for backward array wheen max is hit 
void increase_backward_size(entry* new_entry) {

	// entries in backward equal to backward max
	if (new_entry->backward_max == new_entry->backward_size) {

		// double backward max and reallocate memory
		size_t new = new_entry->backward_max*2;
		new_entry->backward_max = new;
		new_entry->backward = realloc(new_entry->backward, new);
	}
}



// decreases memory for backward array when backward max/2 > entries in backward
void decrease_backward_size(entry* new_entry) {

	// backward max/2 > entries in backward
	int test_1 = new_entry->backward_max/2 > new_entry->backward_size;

	// backward array is not empty 
	int test_2 = new_entry->backward_size != 0;

	if (test_1 && test_2) {

		// halve backward max and reallocate ememory 
		size_t new = new_entry->backward_max/2;
		new_entry->backward_max = new;
		new_entry->backward = realloc(new_entry->backward, new);
	}
}




// remove entries in forward for an entry that are not valid forwards entries 
void update_forward(entry* target_entry) {

	int index = target_entry->forward_size/sizeof(entry);

	int shift = 0;   // decreases index for all forwards 

	// loop through all elements in forward 
	for (int count = 0; count < index; count++) {

		// marked as no longer valid 
		if (target_entry->forward[count]->seen) {

			// shift all other forward index by 1
			shift++;

			// decrease forward size, forward entry is removed 
			target_entry->forward_size -= sizeof(entry);

		} else {

			// shift the forward entry index by shift 
			target_entry->forward[count-shift] = target_entry->forward[count];
		}
	}

	// decrease capacity of forward 
	decrease_forward_size(target_entry);
}



// sets all forward entries as invald or valid (is_seen)
void set_seen_forwards(entry* target_entry, int is_seen) {

	int index = target_entry->forward_size/sizeof(entry);

	// loop through all forward entries ant set as is_seen
	for (int count = 0; count < index; count++) {
		target_entry->forward[count]->seen = is_seen;
	}
}



// sets all pre existing forward entries as invalid 
void reset_seen_forwards(entry* target_entry) {

	int index = target_entry->forward_size/sizeof(entry);

	// loop through all forward entries
	for (int count = 0; count < index; count++) {

		// forward entry is pre exisitng (del = 1) set to invalid
		if (target_entry->forward[count]->del) {
			target_entry->forward[count]->seen = 1;

		// otherwise, forward entry is new, set to valid 
		} else {
			target_entry->forward[count]->seen = 0;
		}
	}

}



// in target entry, visit each backward entry
// each visited backward entry remove invalid forward entry
void update_backward_helper(entry* target_entry) {

	entry* temp = NULL;

	// set all pre exisitng forward entries in target entry as invalid
	reset_seen_forwards(target_entry);

	int index = target_entry->backward_size/sizeof(entry);

	// loop through target entry backward array 
	for (int count = 0; count < index; count++) {

		// each backward arrray remove invalid forward entry
		temp = target_entry->backward[count];
		update_forward(temp);
	}

	// set all forwards entries in target entry as valid 
	set_seen_forwards(target_entry, 0);
	
}


// loops through all backward entires and sets as invalid or valid (is_seen)
void set_seen_backward(entry* target_entry, int is_seen) {

	int index = target_entry->backward_size/sizeof(entry);

	// loop through all backward entries 
	for (int count = 0; count < index; count++) {
		target_entry->backward[count]->seen = is_seen;
	}
}



// removes all invalid backward entries in target entry
void update_backward(entry* target_entry){

	int index = target_entry->backward_size/sizeof(entry);
	int shift = 0;   // decreases index for all backward 

	// loop through all backward entries 
	for (int count = 0; count < index; count++) {

		// backward entry is invalid 
		if (target_entry->backward[count]->seen) {

			// shift all other backward index by 1 
			shift++;

			// decrease backward size, backward entry is removed
			target_entry->backward_size -= sizeof(entry);
			
		} else {

			// shift the backward entry by value given by shift 
			target_entry->backward[count-shift] = target_entry->backward[count];
		}
	}

	// decrease capcity of backward 
	decrease_backward_size(target_entry);
}



// in target entry, visit each forward entry
// each visited forward entry remove invalid backward entry
void update_forward_helper(entry* target_entry, int is_update){

	entry* temp = NULL;

	// set target entry as invalid 
	target_entry->seen = 1;

	// set all target entry backward entries as invalid 
	set_seen_backward(target_entry, 1);
	
	
	int index = target_entry->forward_size/sizeof(entry);

	// loop through all forward entries in target entry
	// each forward entry remove invalid backward entry 
	for (int count = 0; count < index; count++) {
		
		temp = target_entry->forward[count];

		// if updating target entry (set, append, push)
		// and forward entry is an pre exisitng entry 
		if (temp->del && is_update) {
			update_backward(temp);

		// other commmand (pop, pluck, del)
		} else if (!is_update) {
			update_backward(temp);
		}
		
	}

	// set target entry, all target entry backward entries as valid 
	target_entry->seen = 0;
	set_seen_backward(target_entry, 0);
	
}




// sets forward and backward entries for ref entry and new entry
int forward_backward_helper(entry* ref_entry, entry* new_entry) {

	// avoids setting duplicate forward backward entry 
	if (!ref_entry->seen) {
		ref_entry->seen = 1;

		// if ref entry is pre existing or marked as duplicate
		if (ref_entry->del || ref_entry->dup) {

			// mark ref entry as new entry
			ref_entry->del = 0;

			// mark as duplicate and return from function 
			ref_entry->dup = 1;
			return 1;
		}

		// initialise forward for new entry, backward for ref entry
		init_forward_size(new_entry);
		init_backward_size(ref_entry);
	
		int f_index =  new_entry->forward_size/sizeof(entry);
		int b_index =  ref_entry->backward_size/sizeof(entry);

		// set ref entry as a forwards entry of new entry
		new_entry->forward[f_index] = ref_entry;

		// set new entry as backward entry of ref entry
		ref_entry->backward[b_index] = new_entry;

		// increment forward and backward size 
		new_entry->forward_size += sizeof(entry);
		ref_entry->backward_size += sizeof(entry);

		// increase backward of ref entry, forward of new entry
		increase_forward_size(new_entry);
		increase_backward_size(ref_entry);
	}

	return 0;
}




// calls forward_backward_helper for ref entry, new entry 
void set_forward_backward(entry* ref_entry, entry* new_entry){

	// set backward forward relation with ref entry and new entry
	forward_backward_helper(ref_entry, new_entry);

	int index = ref_entry->forward_size/sizeof(entry);

	// loop through all ref entry forward entries 
	for (int count = 0; count < index; count++) {

		// set backward forward with ref entry forward entry and new entry
		forward_backward_helper(ref_entry->forward[count], new_entry);
	}


	index = new_entry->backward_size/sizeof(entry);

	// loop thoguh all new entry backward entries 
	for (int count = 0; count < index; count++) {

		ref_entry->seen = 0;

		// set backward forward with new entry backward entry and ref entry
		forward_backward_helper(ref_entry, new_entry->backward[count]);
	}

	// reset ref_entry seen and dup to default values
	ref_entry->dup = 0;
	ref_entry->seen = 0;

}



// loop through all entries staring from root, set all entries seen to default
void reset_seen(entry* root) {

	while (root != NULL) { 
		root->seen = 0; 
		root = root->next;
	}
}



// loops though all entries, finds and sets entry as element for new entry
int reference_entry(entry* new_entry, entry* root_entry, char* name, 
                    int index) {

	// loop through all entries starting from root entry
	while (root_entry != NULL) {

		// entry set as value is found 
		if (!strcmp(root_entry->key, name)) { 

			// set the element in values 
			new_entry->values[index].entry = root_entry;

			// update the element values to reflect an entry
			new_entry->values[index].type = ENTRY;
			new_entry->seen = 1;
			new_entry->is_simple = '0';

			// set forward backward relation with found entry 
			set_forward_backward(root_entry, new_entry);

			// increase capacity of element array 
			increase_element_space(new_entry);
			return 1;
		}

		root_entry = root_entry->next;
	}

	// entry doesn't exist, print error
	printf("no such key\n");
	return 0;
}



// shifts all elements new entry values by 1 
void push_helper(entry* new_entry) {

	// loops through all elements in values and shifts index by +1
	for (int count = new_entry->length-1; count >= 0; count--) {
		new_entry->values[count+1] = new_entry->values[count];
	}
}




// removes all pre existing forward if valid or all new forward if invalid
void remove_invalid_forwards(entry* target_entry, int is_valid) {

	int index = target_entry->forward_size/sizeof(entry);
	int shift = 0;
	entry* temp;

	// loop through all forward values in target entry
	for (int count = 0; count < index; count++) {
	
		temp = target_entry->forward[count];

		// temp is pre existing + update on target entry is valid 
		// or temp is new + update on target entry is invalid
		if ((!temp->del && ! is_valid) || (temp->del && is_valid)){

			// decrement entry size from forward size 
			target_entry->forward_size -= sizeof(entry);

			// reset del to default 
			target_entry->forward[count]->del = 0;

			// update on target entry is invalid 
			if (!is_valid) {

				// remove last added backward entry from temp 
				temp->backward_size -= sizeof(entry); 
				decrease_backward_size(temp);
			}

			// increment shift to shift all forward entries by 1
			shift++;
		
		} else {

			// reset del to default 
			temp->del = 0;

			// set forward entries to new index set by shift 
			target_entry->forward[count-shift] = temp;

		}
	}

	// decrease capacity of new entry forward array 
	decrease_forward_size(target_entry);
}




// replaces invalid forwards and backwards for target entry 
void replace_helper(entry* target_entry, int is_valid, int is_update) {

	// command was and update to target entry
	if (is_update) {

		// update to target entry was valid 
		if (is_valid) {

			// update backward array for target entry forwards entries 
			update_forward_helper(target_entry, 1);

			// update forward array for target entry backward entries 
			update_backward_helper(target_entry);
		}

		// update forward entry for target entry 
		remove_invalid_forwards(target_entry, is_valid);
	}
}



// removes pre existing values if valid or removes new values if invlaid 
void remove_invalid_elements(entry* target_entry, int is_valid, int is_update){

	int shift = 0;
	element temp;
	int index = target_entry->length;

	// loop through all values in elements of target entry
	for (int count = 0; count < index; count++){

		temp = target_entry->values[count];

		// if temp is new value and is update to entry is not valid
		// or temp is pre exising value and update to entry is valid
		// and set is used to update target entry
		if ((!temp.seen && !is_valid) || (temp.seen && is_valid && is_update)) {

			temp.seen = 0;

			// remove value by decrementing length and incrementing shift 
			target_entry->length --;
			shift++;
	
		} else {

			// set index of value to new index from shift 
			temp.seen = 0;
			target_entry->values[count-shift] = temp;
		}
	}

	// decrease capacity of elements array 
	decrease_element_space(target_entry);
}



// frees created entry if values are invalid (set command on new entry)
int invalid_helper(int is_valid, int is_set, int update, entry* new_entry) {

	if (is_valid) {
		return 1;
	}

	// set command on a new entry and invalid values 
	if (is_set && !is_valid) {

		// check if prev entry exists, if it does not the root entry
		if (new_entry->prev != NULL) {
			new_entry->prev->next = NULL;   //set prev entry next to NULL
			free_entry(new_entry);   // free the invalid new entry

		// invalid new entry is a root entry
		} else {

			// free its values and set values to NULL 
			free(new_entry->values);
			new_entry->values = NULL;
		}
	} 

	return 0;
}



// sets given values from set, append, push to new entry
int init_elements(entry* new_entry, entry* root_entry, char* input,  
                  int is_push) {

	// check if given value has same key as new entry 
	if (!strcmp(input, new_entry->key)) {
		printf("not permitted\n");   //print error then return 0
		return 0;
	}

	int index = 0;
	int is_valid = 1;
	int num = strtol(input, NULL, 10);

	// if command is push, call function to make space for index 0
	if (is_push) {
		push_helper(new_entry); 

	} else {
		index = new_entry->length;  // set index to last index
	}

	// check if input is "0" 
	if (input[0] == '0' && strlen(input) == 1) {
		is_valid = 0;  
	}

	// num = 0 (input is a entry key) and input is not "0"
	if (num == 0 && is_valid) {

		// reference_entry returns flag 0 (no entry with key == input)
		if (!reference_entry(new_entry, root_entry, input, index)) {
			return 0;
		}

	// input is an integer 
	} else {
		new_entry->values[index].type = INTEGER;
		new_entry->values[index].value = num;
	}

	// set seen to 0 (indicate a new value)
	new_entry->values[index].seen = 0;

	// increment length and increase capacity of elements array
	new_entry->length += 1;
	increase_element_space(new_entry);

	return 1;
}




// set all values in elements array in target entry to is_seen 
void set_seen_values(entry* target_entry, int is_seen) {

	// loop through all values sets all elements to is_seen
	for (int count = 0; count < target_entry->length; count++) {
		target_entry->values[count].seen = is_seen;
	}
}


// set all forward entries in target_entry to is_del 
void set_del_forwards(entry* target_entry, int is_del) {

	int index = target_entry->forward_size/sizeof(entry);

	// loop through all forward entries 
	for (int count = 0; count < index; count++) {
		target_entry->forward[count]->del = is_del; 
	}
}




// adds elements to values for new entry (new entry or an updated entry)
int append(char* line, char* delimit, entry* new_entry, entry* root_entry, 
           char* input, int is_push, int is_set, int update) {

	int is_valid = 0;

	// command is not set and is not an update using set
	if (!is_set) {
		set_seen_values(new_entry, 1);  // set all pre existing values to seen 
	}

	// if command is set and is an update to existing entry
	if (update) {
		set_seen_forwards(new_entry, 0); // set all forward entries to not seen
		set_del_forwards(new_entry, 1);  // set all forward entries to del = 1 
	}

	// loop by seperating values from input 
	while ((input = strsep(&line, delimit)) != NULL) {

		// ignore when input == ""
		if (strcmp("", input)) {

			// initialise value to elements, store returned flag as is_valid
			is_valid = init_elements(new_entry, root_entry, input, is_push);

			// if a value is invalid (non existing entry key) then break 
			if (!invalid_helper(is_valid, is_set, update, new_entry)) {
				break;
			}
		}
    }

	// remove values in elements depending if valid or not valid update 
	if (!is_set) {
		remove_invalid_elements(new_entry, is_valid, update);
	}

	// removes forward and backward entries
	replace_helper(new_entry, is_valid, update);

	
	if (is_valid) {
		printf("ok\n");
	}

	reset_seen(root_entry);  // set all entries to not seen 

	return 1;
}




// creates new entry or initialises the root entry from set command 
int set(char* line, char* delimit, entry* last_entry, entry* root_entry, 
         char* input) {

	// if key to initialise entry is too long
	if (strlen(input) > 15){
		printf("key is too long\n");
		return 0;
	}

	entry* new_entry = NULL;

	// last_entry is not the root entry
	if (last_entry->values != NULL) {

		// create new entry and set forward backward relation 
		new_entry = entry_init();
		last_entry->next = new_entry;
		new_entry->prev = last_entry;

	
	// last entry is the root entry
	} else {
		new_entry = last_entry;
	}

	strcpy(new_entry->key, input);   // set the key for last_entry
	init_element_space(new_entry);   // initialise values array for last_entry
	
	// sets elements into values array for last_entry
	append(line, delimit, new_entry, root_entry, input, 0, 1, 0);

	return 1;
}



// prints all elements in values array in target entry
void get(entry* target_entry) {
	
	printf("[");

	// loop through all elements 
	for (int count = 0; count < target_entry->length; count++) {
		
		// print with printf format depending on type 
		if (target_entry->values[count].type == ENTRY) {
			printf("%s", target_entry->values[count].entry->key);

		} else {
			printf("%d", target_entry->values[count].value);
		}

		// print white space until last element
		if (count != target_entry->length-1) {
			printf(" ");
		}
	}
	printf("]\n");
}



// computes the min value for target entry
void min(entry* target_entry) {

	int min_val = INT_MAX;
	int value = 0;

	// loop through all elements in target entry
	for (int count = 0; count < target_entry->length; count++) {

		// if element is an integer, set value to the integer value
		if (target_entry->values[count].type == INTEGER) {
			value = target_entry->values[count].value;

		// if element is an entry, set value to its min value
		} else {
			value = target_entry->values[count].entry->min;
		}

		// set min_val if value is lower 
		if (value < min_val) {
			min_val = value;
		}
	}

	target_entry->min = min_val;
}



// computes max value for target entry
void max(entry* target_entry) {

	int max_val = INT_MIN;
	int value = 0;

	// loop through all elements in target entry
	for (int count = 0; count < target_entry->length; count++) {

		// if element is an integer, set value to the integer value
		if (target_entry->values[count].type == INTEGER) {
			value = target_entry->values[count].value;

		// if element is an entry, set value to its max value
		} else {
			value = target_entry->values[count].entry->max;
		}

		// set max_val if value is larger
		if (value > max_val) {
			max_val = value;
		}
	}

	target_entry->max = max_val;
}



// computes sum value for target entry
void sum(entry* target_entry){

	int sum_val = 0;

	// loop through all elements in target entry
	for (int count = 0; count < target_entry->length; count++) {

		// if element is an integer increment its value 
		if (target_entry->values[count].type == INTEGER) {
			sum_val += target_entry->values[count].value;

		// if element is an entry, increment its sum value
		} else {
			sum_val += target_entry->values[count].entry->sum;
		}
	}

	target_entry->sum = sum_val;
}



// computes output len value for target entry
void len(entry* target_entry) {

	int len_val = target_entry->length;

	// loop through all elements in target entry
	for (int count = 0; count < target_entry->length; count++) {

		// if element is an entry, increment its output_len value
		if (target_entry->values[count].type == ENTRY) {
			len_val += target_entry->values[count].entry->output_len;
			len_val --;       // decrement by 1 account for length key takes 
		}
	}

	target_entry->output_len = len_val;
}



// calls min, max, sum, len functions depending on cmd
void update(char* cmd, entry* target_entry) {

	if (!strcasecmp(cmd, "MIN")) {
		min(target_entry);

	} else if (!strcasecmp(cmd, "MAX")) {
		max(target_entry);

	} else if (!strcasecmp(cmd, "SUM")) { 
		sum(target_entry);

	} else if (!strcasecmp(cmd, "LEN")) { 
		len(target_entry);
	}
}



// preprocesssing for min, max, sum, len functions
void preprocess(entry* root_entry, char* cmd) {

    entry* target_entry = root_entry;

	// traverse through all entries 
	while (target_entry != NULL) {

		// preprocess for simple entries 
        if (target_entry->is_simple == '1') {
            update(cmd, target_entry);
		}

		target_entry = target_entry->next;
	}
 
    target_entry = root_entry; 

	// traverse through all entries 
    while (target_entry != NULL) {

		// preprocess for entries with no forward entries 
        if (target_entry->forward_size == 0) {
            update(cmd, target_entry);
        }

		target_entry = target_entry->next;
	}


    target_entry = root_entry;

	// traverse through all entries 
    while (target_entry != NULL) {

		// preprocess for all unprocessed entries 
        if (target_entry->forward_size > 0
		   && target_entry->is_simple == '0') {
            update(cmd, target_entry);
        }

		target_entry = target_entry->next;
	}
}




//updates if entry is simple or general
void simple_test(entry* target_entry) {

	int flag = 0;

	// loop though all entry elements in target entry
	for (int count = 0; count < target_entry->length; count++) {

		if (target_entry->values[count].type == ENTRY) {
			flag = 1;     // target entry is not simple 
		}
	}

	// set to simple or general accordingly
	if (flag) {
		target_entry->is_simple = '0';

	} else {
		target_entry->is_simple = '1';
	}
}



// removes element from target entry values 
void pluck(entry* target_entry, int index) { 

	// if removed element is an entry
	if (target_entry->values[index].type == ENTRY) {

		// update backwards in all forward entries of target entry
		update_forward_helper(target_entry, 0);   
	}

	// loop through elements starting from index in target entry
	for (int count = index; count < target_entry->length-1; count++) {

		// shift index of looped elements to remove element
		target_entry->values[count] = target_entry->values[count+1];
	}

	target_entry->length -= 1;  //decrement, 1 element is removed 
	simple_test(target_entry);  // update to general or simple
	decrease_element_space(target_entry);  // decrease capacity of values 
}




// prints element in target_entry specified by a number (target_index)
int pick(entry* target_entry, int is_pluck, char* target_index) {

	// convert target_index to an integer and decrement to get index 
	int index = strtol(target_index, NULL, 10) - 1;

	// if index is out of range, print error then return 0
	if (index < 0 || index > target_entry->length-1) {
		printf("index out of range\n");
		return 0;
	}

	// print using printf format depending on if its an integer or entry
	if (target_entry->values[index].type == INTEGER) { 
		printf("%d\n", target_entry->values[index].value);

	} else {
		printf("%s\n", target_entry->values[index].entry->key);
	}

	// if command is pluck or pop, call pluck
	if (is_pluck) {
		pluck(target_entry, index);
	}

	return 1;
}


// function for qsort to sort entries by key 
int cmp_entry(const void * element_1, const void * element_2) {

	char letter_1 = (*(const entry**) element_1)->key[0];
	char letter_2 = (*(const entry**) element_2)->key[0];
	
	/* USYD CODE CITATION ACKNOWLEDGEMENT
	 * I declare that the following lines of code have been copied and modified 
	 * from the website titled: "stackoverflow" and it is not my own work. 
     * 
     * Original URL
     * https://stackoverflow.com/questions/65485650/how-to-sort-an-array-in-ascending-order-in-c
     * Last access April, 2021
     */

	return ((letter_1 > letter_2) - (letter_1 < letter_2));

	/* end of copied code */
}



// prints and sorts forwards in target entry in ascending order 
 void forward(entry* target_entry) {

	int index =  target_entry->forward_size/sizeof(entry);

	if (index == 0) {
		printf("nil\n");    // no forward entries, print error

	} else {

		// sort forward entries in ascending order
		qsort(target_entry->forward, index, sizeof(entry*), cmp_entry);

		// loop through all forward entries and print their key
		for (int count = 0; count < index; count++) {

			if (count == index-1) {
				printf("%s\n", target_entry->forward[count]->key);

			} else {
				printf("%s, ", target_entry->forward[count]->key);
			}
		}
	}
}




// prints and sorts backwards in target entry in ascending order 
void backward(entry* target_entry) {

	int index =  target_entry->backward_size/sizeof(entry);

	if (index == 0) {
		printf("nil\n");   // no backward entries, print error

	} else {

		// sort backward entries in ascending order
		qsort(target_entry->backward, index, sizeof(entry*), cmp_entry);

		// loop through all backward entries and print their key
		for (int count = 0; count < index; count++) {

			if (count == index-1) {
				printf("%s\n", target_entry->backward[count]->key);

			} else {
				printf("%s, ", target_entry->backward[count]->key);
			}
		}
	}
}



// prints if an entry is general or simple
void type(entry* target_entry) {

	if (target_entry->is_simple == '1') {
		printf("simple\n");

	} else {
		printf("general\n");
	}
}



// traverses through all entries returns last added entry
entry* get_last_entry(entry* root_entry) {

	while (root_entry->next != NULL) { 
		root_entry = root_entry->next;
	}

	return root_entry;
}



// prints keys and elements of all entries depending on cmd 
void list_keys(char* cmd, entry* root_entry) {

	// root entry is initialised, there exists entries 
	if (root_entry->values != NULL) {

		// get last added entry
		root_entry = get_last_entry(root_entry);

		// traverse though all entries until root entry is reached
		while (root_entry != NULL) {

			// if cmd is entries, print key and its elements
			if (!strcasecmp(cmd, "ENTRIES")) {
				printf("%s ", root_entry->key);
				get(root_entry);

			// if cmd is keys, print key 
			} else {
				printf("%s\n", root_entry->key);
			}

			root_entry = root_entry->prev;
		}
	
	} else {

		// print error message when there are no entries
		if (!strcasecmp(cmd, "ENTRIES")) {
			printf("no entries\n");

		} else {
			printf("no keys\n");
		}
	}
}



// traverses through all snapshots returns last added snapshot
snapshot* get_last_snapshot(snapshot* root) {
	
	while (root->next != NULL) {
		root = root->next;
	}

	return root;
}


// prints all snapshot ids
void list_snapshots(snapshot* root_snapshot) {

	// root snapshot is initialised, there exists snapshots; 
	if (root_snapshot->entries != NULL) {

		// get last added snapshot
		root_snapshot = get_last_snapshot(root_snapshot);

		// traverse through all snapshots and print id
		while (root_snapshot != NULL) {

			printf("%d\n", root_snapshot->id);

			root_snapshot = root_snapshot->prev;
		}

	// no snapshots exists print error 
	} else {
		printf("no snapshots\n");
	}
}



// call functions for the list command to print keys, entries or ids
void list(char* cmd, entry* root_entry, snapshot* root_snapshot) {

	if (!strcasecmp(cmd, "SNAPSHOTS")) {
		list_snapshots(root_snapshot);

	} else if (!strcasecmp(cmd, "KEYS") || !strcasecmp(cmd, "ENTRIES")) {
		list_keys(cmd, root_entry);
	} 
}



// reverses elements in values of target entry if its simple
void rev(entry* target_entry) {

	if (target_entry->is_simple == '1') {

		element temp;

		// index for elements in 2nd half of elements 
		int last = target_entry->length-1;

		// loop through all elements in 1st half of elements 
		for (int count = 0; count < target_entry->length/2; count++) {

			// switching places of element in 1st half with element in 2nd half
			temp = target_entry->values[count];

			target_entry->values[count] = target_entry->values[last];

			target_entry->values[last] = temp;

			last--;   // decrement last to get next element in 2nd half
		}

		printf("ok\n");

	// target entry is not simple
	} else {
		printf("nil\n");
	}
}



// removes corresponding duplicates in target entry values if its simple
void uniq(entry* target_entry) {

	if (target_entry->is_simple == '1') {
		
		int max_index = target_entry->length;
		int shift = 0;     // decreases index to remove duplicate
		element temp;

		// loop through all elements in target entry values
		for (int count = 1; count < max_index; count++) {

			temp = target_entry->values[count];

			// comapare previous element to current element (temp)
			if (target_entry->values[count-1].value == temp.value) {

				// decrement length and increment shift, duplicate exists 
				target_entry->length--;   
				shift++;	
				
			} else {

				// shift non duplicate elements to remove the duplicate 
				target_entry->values[count-shift] = temp;
			}
		}
		
		printf("ok\n");

		// decrease capacity of values in target entry
		decrease_element_space(target_entry);
		
	} else {
		printf("nil\n");
	}
}




// function for qsort to sort values in ascending order 
int cmp_simple(const void * element_1, const void * element_2) {

	int value_1 = (*(const element*) element_1).value;
	int value_2 = (*(const element*) element_2).value;

		/* USYD CODE CITATION ACKNOWLEDGEMENT
	 * I declare that the following lines of code have been copied and modified 
	 * from the website titled: "stackoverflow" and it is not my own work. 
     * 
     * Original URL
     * https://stackoverflow.com/questions/65485650/how-to-sort-an-array-in-ascending-order-in-c
     * Last access April, 2021
     */

	return ((value_1 > value_2) - (value_1 < value_2));

	/* end of copied code */
}


// sorts elements in target entry in ascending order if its is simple
void sort(entry* target_entry) {

	// if simple, sort elements in target entry in ascending order 
	if (target_entry->is_simple == '1') {

		qsort(target_entry->values, target_entry->length, 
		      sizeof(element), cmp_simple);

		printf("ok\n");

	} else {
		printf("nil\n");

	}
}



// traverses through all entries from target entry until root is found
entry* get_root(entry* target_entry) {

	while (target_entry->prev != NULL) {
		target_entry = target_entry->prev;
	}

	return target_entry;
}


// deletes given target snaphsot, returns a root snapshot 
snapshot* delete_snapshot(snapshot* target_snapshot, snapshot* root_snapshot) {

	snapshot* temp;

	// target snapshot is not root snapshot
	if (target_snapshot->prev != NULL) {

		// target snapshot has a next snapshot
		if (target_snapshot->next != NULL) { 

			// set the next snapshot prev to previous snapshot;
			target_snapshot->next->prev = target_snapshot->prev;

			// set the previous snapshot next to next snapshot 
			target_snapshot->prev->next =  target_snapshot->next;

			target_snapshot->next = NULL;

		} else {

			target_snapshot->prev->next = NULL;
		}

		free_snapshot(target_snapshot);    // free target snapshot 

		// set retuned snapshot as current root snapshot
		target_snapshot = root_snapshot;   

	// target snapshot is root snapshot 
	} else {

		// if root snapshot has next snapshots 
		if (target_snapshot->next != NULL) { 

			// store next snapshot to temp, it become new root snapshot
			temp = target_snapshot->next;
			temp->prev = NULL;

			// free target snapshot
			target_snapshot->next = NULL;
			free_snapshot(target_snapshot);

			// set retuned snapshot as new root snapshot
			target_snapshot = temp;

		} else {

			free_snapshot(target_snapshot);  // free target snapshot

			// return newly initialised snapshot
			target_snapshot = snapshot_init(); 
		}
	}

	// return new or current root snapshot
	return target_snapshot;
}


// deletes entry, returns a new or current root entry
entry* del_helper(entry* target_entry, entry* root_entry, int is_purge) {

	if (!is_purge) {
		printf("ok\n");   // print if command is not purge
	}

	// each target entry forward entry, modify each backwards array
	update_forward_helper(target_entry, 0);

	entry* new_root; 

	// target entry is root entry 
	if (target_entry->prev == NULL){

		// target entry has next entries 
		if (target_entry->next != NULL){

			// set new root entry as the next entry from root 
			new_root = target_entry->next;

			new_root->prev = NULL;    // free current root entry
			target_entry->next = NULL;
			free_entry(target_entry);

		} else {
			free_entry(target_entry);  // free current root entry
			new_root = entry_init();   // new root is newly initialised entry
		}

		return new_root;

	// target entry is not root entry
	} else {

		// target entry has next entries 
		if (target_entry->next != NULL){

			// set next entry prev to previous entry 
			target_entry->prev->next = target_entry->next;

			// set previous entry next to next entry 
			target_entry->next->prev = target_entry->prev;

			target_entry->next = NULL;

		} else {
			target_entry->prev->next = NULL;

		}

		// free target entry and return current root entry
		free_entry(target_entry); 
		return root_entry;
	}
}



// traverses through all entries and finds entry with key for deletion
// is_check indicate deleting entry or only checking if deletion is valid
entry* del(entry* target_entry, char* name, int* flag, int is_purge, 
           int is_check) {

	entry* root_entry = target_entry;

	// traverse through all entries starting from root 
	while (target_entry != NULL) {

		// name matches with a key of existing entry 
		if (!strcmp(target_entry->key, name)) {

			// if backward entries exist for the found entry
			if (target_entry->backward_size > 0) {

				// print error
				printf("not permitted\n");

				(*flag)++;  //incrmeent to indicate invalid deletion

				return root_entry;
			}

			// if call to del is not a check call del_helper return its root
			if (!is_check) {
				return del_helper(target_entry, root_entry, is_purge);
			}
		}

		target_entry = target_entry->next;
	}

	// if command was purge, print error 
	if (!is_purge) {
		printf("no such key\n");
	}
	
	return root_entry;
}




// creates a deep copy of an array, og_entry values are copied to transfer entry
void deep_cpy_entry(entry* og_entry, entry* transfer_entry) {

	
	strcpy(transfer_entry->key, og_entry->key);  // copy key

	transfer_entry->is_simple = og_entry->is_simple;  // copy is_simple

	// if values exists, create memory for transfer entry values
	if (og_entry->values != NULL) {

		// copy capacity and length
		transfer_entry->capacity = og_entry->capacity;
		transfer_entry->length = og_entry->length;

		// allocate memory for transfer entry
		transfer_entry->values = realloc(transfer_entry->values, 
		                                  og_entry->capacity);
	} else {
		transfer_entry->values = NULL;

	}

	// copy values to transfer entry 
	for (int count = 0; count < og_entry->length; count++) {
		transfer_entry->values[count] = og_entry->values[count];
	}

	// copy backward size
	transfer_entry->backward_size = og_entry->backward_size;

	transfer_entry->backward = NULL;

}



// initialises copy entries and sets copied values to these entries
entry* clone_entries(entry* root_entry) {

	entry* new_root = entry_init();  // create root entry for snapshot
	entry* clone_entry = new_root;

	// loop through all curent entries while creating clone entries
	while (root_entry != NULL) {

		deep_cpy_entry(root_entry, clone_entry); // deep copy to clone entry

		// next current extry exists 
		if (root_entry->next != NULL) {
			
			// create a copy for the next current entry 
			clone_entry->next = entry_init();

			// link it to current clone entry
			clone_entry->next->prev = clone_entry; 

		} else {
			clone_entry->next = NULL;
		}

		clone_entry = clone_entry->next;
		root_entry = root_entry->next;
	}

	return new_root;
}





// initialses snapshot or adds new snapshots to snapshot linked list
void snapshot_process(entry* root_entry, snapshot* current_snapshot,  
                      int* num) {

	snapshot* prev_snapshot;

	// last snapshot is not root snaphshot
	if (current_snapshot->entries != NULL) {

		// get last snapshot
		prev_snapshot = get_last_snapshot(current_snapshot); 

		current_snapshot = snapshot_init(); // initialise new snapshot

		// set next prev relation with last snapshot and new snapshot
		prev_snapshot->next = current_snapshot;

		current_snapshot->prev = prev_snapshot;
	}

	// clone all current entries and set it to snapshot entries 
	current_snapshot->entries = clone_entries(root_entry);

	current_snapshot->id = (*num);  // set id 

	printf("saved as snapshot %d\n", *num);

	(*num)++; // increment id for next snapshot
}




// re adds forward backward relation with snapshot entries 
void add_backward_helper(entry* target_entry) {

	element temp; 

	// loop through all elements in target entry
	for (int count = 0 ; count < target_entry->length; count++) {

		temp = target_entry->values[count];

		// if element is entry, create backward forward relation
		if (temp.type == ENTRY) {
			forward_backward_helper(temp.entry, target_entry);
			target_entry->values[count].entry->seen= 0;
		}
	}
}



// visits all entries from checkout, calls add_backward_helper for general entries
void add_backward_entres(entry* target_entry) {

	target_entry = get_last_entry(target_entry); // get last entry

	// loop through all entries from checkout
	while (target_entry != NULL) {

		// call add_backward_helper if general entry
		if (target_entry->is_simple == '0'){
			add_backward_helper(target_entry);
		}
		
		target_entry = target_entry->prev;
	}
}

	

// creates new entries to make up deleted entries after snapshot
// or frees entries that were added after the snapshot
void leftover_entry(entry* replace_temp, entry* root_temp, entry* prev) {

	// entries still exist that were deleted after snapshot
	if (replace_temp != NULL) {

		// create remaining entries 
		entry* remaining = clone_entries(replace_temp);

		// set next prev relation with last created entry (prev)
		prev->next = remaining;
		remaining->prev = prev;

	// entries exist which were added after snapshot
	} else if (root_temp != NULL) {

		// free all entries that were added after snapshot
		free_entry(root_temp); 

	} 

}

// removes all new snapshots from the current snapshot
void checkout_removal(snapshot* target_snapshot, snapshot* root_snapshot,
                      int is_checkout){

	// if command was not checkout and current snapshot has next snapshots
	if (!is_checkout && target_snapshot->next != NULL) {

		// free all next snapshots
		free_snapshot(target_snapshot->next);
		target_snapshot->next = NULL;
	}
}



// frees forward and backwards of root_temp if it exists 
void free_forward_backward(entry* root_temp) {

	// if backward exists 
	if (root_temp->backward != NULL) {

		// set backward and associated values to default
		free(root_temp->backward);
		root_temp->backward = NULL;
		root_temp->backward_size = 0;
		root_temp->backward_max = 0;
	}

	// if forwards exists
	if (root_temp->forward != NULL) { 

		// set forward and associated values to default
		free(root_temp->forward);
		root_temp->forward = NULL;
		root_temp->forward_size = 0;
		root_temp->forward_max = 0;
	}
}



// reverts current entries to the entries stored in target snapshot 
void checkout(snapshot* target_snapshot, snapshot* root_snapshot,
              entry* root_entry, int is_checkout) {

	
	entry* replace_temp = target_snapshot->entries;  // entries from snapshot 
	entry* root_temp = root_entry;    // current entries 

	entry* temp = NULL;   // pointer to store newly created entries 
	entry* new_forward = NULL;   // stores entry of newly created linked list

	int condition_1 = 0;
	int condition_2 = 0;
	int condition_3 = 0;

	// loop traverses through replace_tmp and root_temp until either is null
	while (1) {

		// both current entry and entry from snapshot have no backward entries 
		condition_2 = (replace_temp->backward_size == 0) && 
		              (root_temp->backward_size == 0);

		// entry from snapshot has backward entry and both keys match
		condition_1 = (replace_temp->backward_size > 0) && 
		              !strcmp(replace_temp->key, root_temp->key);

		condition_3 = (root_temp->backward_size > 0);

		// free forwards and backwards in root_temp if forwards backwards exist 
		free_forward_backward(root_temp);

		 
		
		if (condition_2 || condition_1) { 

			//reset backward size from copied value to 0
			replace_temp->backward_size = 0;  

			// copy struct values of replace temp to root_temp
			deep_cpy_entry(replace_temp, root_temp);

			// set prev next relation with new_forward and root_temp
			root_temp->prev = new_forward;

			if (new_forward != NULL){
				new_forward->next = root_temp;
			}

			new_forward = root_temp; // update new_forward

			// visit next replace_temp and root_temp 
			replace_temp = replace_temp->next;
			root_temp = root_temp->next;


		} else if (condition_3) {

			// create new entry and copy struct values of root_temp
			temp = entry_init();
			deep_cpy_entry(replace_temp, temp);


			// set prev next relation with new_forward and temp 
			temp->prev = new_forward;
			
			if (new_forward != NULL){
				new_forward->next = temp;
			}

			new_forward = temp; // update new_forward
			replace_temp = replace_temp->next; //visit next replace_temp

		} else {
			replace_temp = replace_temp->next; //visit next replace_temp

		}

		// break when either is null
		if (replace_temp == NULL || root_temp == NULL) {
			break;
		}
	}

	new_forward->next = NULL; // last entry of new initialised linked list

	leftover_entry(replace_temp, root_temp, new_forward);
	add_backward_entres(new_forward);
	checkout_removal(target_snapshot, root_snapshot, is_checkout);
}






// loops through all snapshots, finds snapshot with id, calls checkout/ rollback
void snapshot_cmd(snapshot* root_snapshot, entry* root_entry, int id, 
                  char* cmd) {

	snapshot* target_snapshot = root_snapshot;

	// loop through all snapshots
	while (target_snapshot != NULL) {

		// snapshot with id is found 
		if (target_snapshot->id == id) {

			printf("ok\n");

			// call checkout with is_checkout parameter set to 1 or 0
			if (!strcasecmp("CHECKOUT", cmd)) {
				return checkout(target_snapshot, root_snapshot, root_entry, 1);
			
			} else if (!strcasecmp("ROLLBACK", cmd)) {
				return checkout(target_snapshot, root_snapshot, root_entry, 0);
			}
		} 

		target_snapshot = target_snapshot->next;
	}
	
	printf("no such snapshot\n");  // error when no id match is found
}




// traverses through all snapshots find snapshot to be deleted
snapshot* drop(snapshot* root_snapshot, int id) {

	snapshot* target_snapshot = root_snapshot;

	// loop through all snapshots 
	while (target_snapshot != NULL) {

		// id matches with a snapshot
		if (target_snapshot->id == id) {
			printf("ok\n");
			// delete the snapshot, return the returned root snapshot
			return delete_snapshot(target_snapshot, root_snapshot);
		}

		target_snapshot = target_snapshot->next;
	}

	// no matches, print error and return root snapshot
	printf("no such snapshot\n");
	return root_snapshot;	
}





// loops through all snapshots checks if deletion of entry is valid
// deletes if valid deletion, returns flag if not 
int purge_snapshot(snapshot* root_snapshot, char* name) {

	int flag = 0;
	snapshot* root =  root_snapshot;

	// loop through all snapshots 
	while (root != NULL) {

		// call del function with parameter is_check and is_purge set to 1
		del(root->entries, name, &flag, 1, 1);

		// flag is set to 1, deletion is invalid, return flag
		if (flag) {
			return 0;
		}

		root = root->next;
	}

	root =  root_snapshot;

	// loop through all snapshots 
	while (root != NULL) {

		// call del function to delete entry with key name, deletion is valid
		root->entries = del(root->entries, name, &flag, 1, 0);
		root = root->next;
	}

	printf("ok\n");

	return 1;
}



// traverses trhough all entries and return entry with key == name
entry* get_entry(char* name, entry* target_entry, int is_set) {

	entry* prev;

	// traverse through all entries 
	while (target_entry != NULL) {

		// if there is a match return the matched entry
		if (!strcmp(target_entry->key, name)) {
			return target_entry;
		}

		prev = target_entry;  // stores visited last entry 
		target_entry = target_entry->next;
	} 

	// if command was set, return last visited entry
	if (is_set) {
		return prev;
	}

	printf("no such key\n");
	return NULL;
}


// checks if entry values is NULL
int check_null_values(entry* root_entry) {

	// prints error and returns flag if values is NULL
	if (root_entry->values == NULL) {
		printf("no such key\n");
		return 0;
	}
	return 1;
}






// calls relevant function depending on command given 
int process_cmd_4(char* line, entry* root_entry, char* delimit, char* cmd) {

	// command is forward 
	if (!strcasecmp(cmd, "FORWARD")) {

		// get the entry that forward spcifies 
		root_entry = get_entry(strsep(&line, delimit), root_entry, 0);

		// call forward function if specified entry exists 
		if (root_entry != NULL) {
			forward(root_entry);
		}
		return 1;

	
	// for the remaning else if statments same process applies
	// command matches, get entry specified by command
	// call relevant function if specified entry exists 

	} else if (!strcasecmp(cmd, "BACKWARD")) {

		root_entry = get_entry(strsep(&line, delimit), root_entry, 0); 

		if (root_entry != NULL){
			backward(root_entry);
		}
		return 1;


	} else if (!strcasecmp(cmd, "TYPE")) {

		root_entry = get_entry(strsep(&line, delimit), root_entry, 0);

		if (root_entry != NULL) {
			type(root_entry);
		}
		return 1;
	}
	return 0;
}



// calls relevant function depending on command given 
int process_cmd_3(char* line, entry* root_entry, char* delimit, char* cmd) {

	// command is rev
	if (!strcasecmp(cmd, "REV")) {

		// get the entry that rev specifies
		root_entry = get_entry(strsep(&line, delimit), root_entry, 0);

		// call rev function if specified entry exists 
		if (root_entry != NULL) {
			rev(root_entry);
		}

		return 1;

		
	// for the remaning else if statments same process applies
	// command matches, get entry specified by command 
	// call relevant function if specified entry exists 

	} else if (!strcasecmp(cmd, "UNIQ")) {

		root_entry = get_entry(strsep(&line, delimit), root_entry, 0);

		if (root_entry != NULL) {
			uniq(root_entry);
		}

		return 1;


	} else if (!strcasecmp(cmd, "SORT")) {

		root_entry = get_entry(strsep(&line, delimit), root_entry, 0);

		if (root_entry != NULL) {
			sort(root_entry);
		}

		return 1;
	} 

	return 0;
}






// calls relevant function depending on command given 
int process_cmd_2(char* line, entry* root_entry, char* delimit, char* cmd) {

	entry* target_entry;

	// command is min
	if (!strcasecmp(cmd, "MIN")) {

		// get the entry that min specifies
		target_entry = get_entry(strsep(&line, delimit), root_entry, 0);

		// preprocess with min as command
		// call min function if specified entry exists 
		if (target_entry != NULL) {
			preprocess(root_entry, cmd);
			printf("%d\n",target_entry->min);
		}

		return 1;

		
	// for the remaning else if statments same process applies
	// command matches, get entry specified by command,
	// if entry exists, preprocess with command as parameter
	// call relevant function

	} else if (!strcasecmp(cmd, "MAX")) {

		target_entry = get_entry(strsep(&line, delimit), root_entry, 0);

		if (target_entry != NULL) {
			preprocess(root_entry, cmd);
			printf("%d\n",target_entry->max);
		}

		return 1;

	} else if (!strcasecmp(cmd, "SUM")) {

		target_entry = get_entry(strsep(&line, delimit), root_entry, 0);

		if (target_entry != NULL) {
			preprocess(root_entry, cmd);
			printf("%d\n",target_entry->sum);
		}

		return 1;


	} else if (!strcasecmp(cmd, "LEN")) {

		target_entry = get_entry(strsep(&line, delimit), root_entry, 0);

		if (target_entry != NULL) {
			preprocess(root_entry, cmd);
			printf("%d\n",target_entry->output_len);
		}

		return 1;
	}

	return 0;
}




// calls relevant function depending on command given 
int process_cmd_1(char* line, entry* root_entry, char* delimit, char* cmd) {

	// command is pick
	if (!strcasecmp("PICK", cmd)) {

		// get the entry that pick specifies 
		cmd = strsep(&line, delimit);
		root_entry = get_entry(cmd, root_entry, 0);

		// if entry exists and values are not null call pick function
		if (root_entry != NULL && check_null_values(root_entry)) {
			pick(root_entry, 0, strsep(&line, delimit));
		}
		
		return 1;

	// for the else if statement with pluck same process applies
	// command matches, if entry exists and values are not null
	// call pick function with parameter is_pluck set as 1


	} else if (!strcasecmp("PLUCK", cmd)) {

		cmd = strsep(&line, delimit);
		root_entry = get_entry(cmd, root_entry, 0);

		if (root_entry != NULL && check_null_values(root_entry)) {
			pick(root_entry, 1, strsep(&line, delimit));
		}

		return 1;


	// command is pop
	} else if (!strcasecmp("POP", cmd)) { 

		// get the entry that pop specifies 
		cmd = strsep(&line, delimit);
		root_entry = get_entry(cmd, root_entry, 0);

		// if entry exists and values are not null
		if (root_entry != NULL&& check_null_values(root_entry)) {

			// if values is empty print error 
			if (root_entry->length == 0) {
				printf("nil\n");
			
			// else call pick function with parameter is_pluck set as 1 
			// and parameter index set as "1"
			} else {
				pick(root_entry, 1, "1\n");
			}
		}
		
		return 1;
	}
	return 0;
}



// calls relevant function depending on command given 
int process_cmd_0(char* line, entry* root_entry, snapshot* root_snapshot,
                  char* delimit, char* cmd) {

	entry* last_entry;

	// command is help print HELP
	if (!strcasecmp("HELP", cmd)) {
		printf("%s", HELP);
		return 1;

	// command is list, call list function
	} else if (!strcasecmp("LIST", cmd)) {
		list(strsep(&line, delimit), root_entry, root_snapshot);
		return 1;

	

	// command is set 
	} else if (!strcasecmp(cmd, "SET")) {

		// get entry specified by set 
		cmd = strsep(&line, delimit);
		last_entry = get_entry(cmd, root_entry, 1);

		// if key of entry specifed by set matches with found entry (last_entry)
		if (!strcmp(cmd, last_entry->key)) {

			// call append with relevant parameters 
			append(line, delimit, last_entry, root_entry, cmd, 0, 0, 1);

		// else call set 
		} else {
			set(line, delimit, last_entry, root_entry, cmd);

		}
		return 1;


	// command is get
	} else if (!strcasecmp(cmd, "GET")) {

		// get entry specified by get 
		last_entry = get_entry(strsep(&line, delimit), root_entry, 0);

		// call get function if entry is exists 
		if (last_entry != NULL) {
			get(last_entry);
		}

		return 1;


	// command is push
	} else if (!strcasecmp(cmd, "PUSH")) {

		// get entry specified by push
		last_entry = get_entry(strsep(&line, delimit), root_entry, 0);

		// call append function with relevant paramters if entry exists 
		if (last_entry != NULL) {
			append(line, delimit, last_entry, root_entry, cmd, 1, 0, 0);
		}

		return 1;


	// command is append
	} else if (!strcasecmp(cmd, "APPEND")) {
		
		// get entry specified by append
		last_entry = get_entry(strsep(&line, delimit), root_entry, 0);

		// call append function with relevant paramters if entry exists 
		if (last_entry != NULL) {
			append(line, delimit, last_entry, root_entry, cmd, 0, 0, 0);
		}

		return 1;
	}

	return 0;
}




// helper function that make calls to all process_cmd functions 
int process_cmd(char* line, entry* root_entry, snapshot* root_snapshot,
                char* delimit, char* cmd) {

	// command is bye, call bye function return 0;
	if (!strcasecmp("BYE", cmd)) {
		cmd_bye();
		return 0;
	}

	// call all process_cmd functions 
	// if a process_cmd function returns 1, 1 is returned 
	if (process_cmd_0(line, root_entry, root_snapshot, delimit, cmd)) {
		return 1; 

	} else if (process_cmd_1(line, root_entry, delimit, cmd)) {
		return 1;

	} else if (process_cmd_2(line, root_entry, delimit, cmd)) {
		reset_seen(root_entry);
		return 1;

	} else if (process_cmd_3(line, root_entry, delimit, cmd)) {
		return 1;

	} else if (process_cmd_4(line, root_entry, delimit, cmd)) {
		return 1;
	}

	printf("Not a valid command\n");  // error if no valid command 
	return 1;
}



int main(void) {

	char line[MAX_LINE];

	char* cmd;  // primarily stores command after splitting fgets string
	char* input;  // pointer to char array line for strsep
	char* delimit = " \n"; // delimiter values to split string from fgets

	int id_num = 1; // id number for snapshots
	int flag = 0;  // flags for purge command 
	int id = 0;  // stores snapshot id specified by command 

	// initialise root snapshot and root entry before loop 
	snapshot* root_snapshot = snapshot_init();
	entry* root_entry = entry_init();

	// loop until break is reached 
	while (true) {
		printf("> ");
		
		if (NULL == fgets(line, MAX_LINE, stdin)) {
			printf("\n");
			cmd_bye();
			break;
		}

		input = line;
		flag = 0;
		cmd = strsep(&input, delimit); 

		// command is drop
		if (!strcasecmp("DROP", cmd)) {

			// convert specified id to integer
			id = strtol(strsep(&input, delimit), NULL, 10);  

			// set root snapshot to root returned by drop
			root_snapshot = drop(root_snapshot, id);


		// command is snapshot, call snapshot process function
		} else if (!strcasecmp("SNAPSHOT", cmd)) {
			snapshot_process(root_entry, root_snapshot, &id_num);


		// command is checksum or rollback
		} else if (!strcasecmp("CHECKOUT", cmd) || 
		           !strcasecmp("ROLLBACK", cmd)) {

			// convert specified id to integer
			id = strtol(strsep(&input, delimit), NULL, 10);

			// call snapshot_cmd function 
			snapshot_cmd(root_snapshot, root_entry, id, cmd);

			// set root entry to the the new root entry
			root_entry = get_root(root_entry);

		// command is del
		} else if (!strcasecmp("DEL", cmd)) {

			// call del function and set returned root to root_entry
			root_entry = del(root_entry, strsep(&input, delimit), &flag, 0, 0);

		// command is purge 
		} else if (!strcasecmp("PURGE", cmd)) {

			// get key specified by purge 
			cmd = strsep(&input, delimit);

			// call del with is_check parameter as 1 
			del(root_entry, cmd, &flag, 1, 1);

			// if flag was not incremented (valid deletion)
			if (!flag) {

				// call purge_snapshot, if purge snapshot was valid 
				if (purge_snapshot(root_snapshot, cmd)){

					// call del to delete entry with specified key
					root_entry = del(root_entry, cmd, &flag, 1, 0);
				}	
			}

		// call process_cmd to check remaining commands 
		} else if (!process_cmd(input, root_entry, root_snapshot, 
		           delimit, cmd)) {
			break; // bye is called from process_cmd 
		}
		
		printf("\n");
  	}

	// free all entries and all snapshots 
	free_entry(root_entry);
	free_snapshot(root_snapshot);

	return 0;
}