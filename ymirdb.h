#ifndef YMIRDB_H
#define YMIRDB_H

#define MAX_KEY 16
#define MAX_LINE 1024

#include <stddef.h>
#include <sys/types.h>



enum item_type {
    INTEGER=0,
    ENTRY=1
};

typedef struct element element;
typedef struct entry entry;
typedef struct snapshot snapshot;



				
struct element {
  enum item_type type;
  int seen;
  union {
    int value;
    struct entry *entry;
  };
};


struct entry {
  char key[MAX_KEY];
  char is_simple;
  element * values;
  size_t capacity;
  int length;
  int seen;
  int del;
  int dup;
  int sum;
  int min;
  int max;
  int output_len;

  entry* next;
  entry* prev;

  size_t forward_size; 
  size_t forward_max; 
  entry** forward;  
    
  size_t backward_size; 
  size_t backward_max; 
  entry** backward; 
};

struct snapshot {
  int id;
  entry* entries;
  snapshot* next;
  snapshot* prev;
};






// function declarations 
void cmd_bye();

void cmd_help();

void free_entry(entry* root);

snapshot* snapshot_init();

entry* entry_init();

void init_element_space(entry* new_entry);

void increase_element_space(entry* new_entry);

void decrease_element_space(entry* new_entry);

void init_forward_size(entry* new_entry);

void increase_forward_size(entry* new_entry);

void decrease_forward_size(entry* new_entry);

void init_backward_size(entry* new_entry);

void increase_backward_size(entry* new_entry);

void decrease_backward_size(entry* new_entry);

void update_forward(entry* target_entry);

void set_seen_forwards(entry* target_entry, int is_seen);

void reset_seen_forwards(entry* target_entry);

void update_backward_helper(entry* target_entry);

void set_seen_backward(entry* target_entry, int is_seen);

void update_backward(entry* target_entry);

void update_forward_helper(entry* target_entry, int is_update);

int forward_backward_helper(entry* ref_entry, entry* new_entry);

void set_forward_backward(entry* ref_entry, entry* new_entry);

void reset_seen(entry* root);

int reference_entry(entry* new_entry, entry* root_entry, char* name, 
                    int index);

void push_helper(entry* new_entry);

void remove_invalid_forwards(entry* target_entry, int is_valid);

void replace_helper(entry* target_entry, int is_valid, int is_update);

void remove_invalid_elements(entry* target_entry, int is_valid, int is_update);

int invalid_helper(int is_valid, int is_set, int update, entry* new_entry);

int init_elements(entry* new_entry, entry* root_entry, char* input,  
                  int is_push);

void set_seen_values(entry* target_entry, int is_seen);

void set_del_forwards(entry* target_entry, int is_del);

int append(char* line, char* delimit, entry* new_entry, entry* root_entry, 
           char* input, int is_push, int is_set, int update);

int set(char* line, char* delimit, entry* last_entry, entry* root_entry, 
         char* input);

void get(entry* target_entry);

void min(entry* target_entry);

void max(entry* target_entry);

void sum(entry* target_entry);

void len(entry* target_entry);

void update(char* cmd, entry* target_entry);

void preprocess(entry* root_entry, char* cmd);

void simple_test(entry* target_entry);

void pluck(entry* target_entry, int index);

int pick(entry* target_entry, int is_pluck, char* target_index);

int cmp_entry(const void * element_1, const void * element_2);

void forward(entry* target_entry);

void backward(entry* target_entry);

void type(entry* target_entry);

entry* get_last_entry(entry* root_entry);

void list_keys(char* cmd, entry* root_entry);

snapshot* get_last_snapshot(snapshot* root);

void list_snapshots(snapshot* root_snapshot);

void list(char* cmd, entry* root_entry, snapshot* root_snapshot);

void rev(entry* target_entry);

void uniq(entry* target_entry);

int cmp_simple(const void * element_1, const void * element_2);

void sort(entry* target_entry);

entry* get_root(entry* target_entry);

snapshot* delete_snapshot(snapshot* target_snapshot, snapshot* root_snapshot);

entry* del_helper(entry* target_entry, entry* root_entry, int is_purge);

entry* del(entry* target_entry, char* name, int* flag, int is_purge, 
           int is_check);

void deep_cpy_entry(entry* og_entry, entry* transfer_entry);

entry* clone_entries(entry* root_entry);

void snapshot_process(entry* root_entry, snapshot* current_snapshot,  
                      int* num);
void add_backward_helper(entry* target_entry);

void add_backward_entres(entry* target_entry);

void leftover_entry(entry* replace_temp, entry* root_temp, entry* prev);

void checkout_removal(snapshot* target_snapshot, snapshot* root_snapshot,
                      int is_checkout);

void checkout(snapshot* target_snapshot, snapshot* root_snapshot,
              entry* root_entry, int is_checkout);

void snapshot_cmd(snapshot* root_snapshot, entry* root_entry, int id, 
                  char* cmd);

snapshot* drop(snapshot* root_snapshot, int id);

int purge_snapshot(snapshot* root_snapshot, char* name); 

entry* get_entry(char* name, entry* target_entry, int is_set);

int check_null_values(entry* root_entry);

int process_cmd_4(char* line, entry* root_entry, char* delimit, char* cmd);

int process_cmd_3(char* line, entry* root_entry, char* delimit, char* cmd);

int process_cmd_2(char* line, entry* root_entry, char* delimit, char* cmd);

int process_cmd_1(char* line, entry* root_entry, char* delimit, char* cmd);

int process_cmd_0(char* line, entry* root_entry, snapshot* root_snapshot,
                  char* delimit, char* cmd);

int process_cmd(char* line, entry* root_entry, snapshot* root_snapshot,
                char* delimit, char* cmd);


void free_forward_backward(entry* root_temp);


const char* HELP =
	"BYE   clear database and exit\n"
	"HELP  display this help message\n"
	"\n"
	"LIST KEYS       displays all keys in current state\n"
	"LIST ENTRIES    displays all entries in current state\n"
	"LIST SNAPSHOTS  displays all snapshots in the database\n"
	"\n"
	"GET <key>    displays entry values\n"
	"DEL <key>    deletes entry from current state\n"
	"PURGE <key>  deletes entry from current state and snapshots\n"
	"\n"
	"SET <key> <value ...>     sets entry values\n"
	"PUSH <key> <value ...>    pushes values to the front\n"
	"APPEND <key> <value ...>  appends values to the back\n"
	"\n"
	"PICK <key> <index>   displays value at index\n"
	"PLUCK <key> <index>  displays and removes value at index\n"
	"POP <key>            displays and removes the front value\n"
	"\n"
	"DROP <id>      deletes snapshot\n"
	"ROLLBACK <id>  restores to snapshot and deletes newer snapshots\n"
	"CHECKOUT <id>  replaces current state with a copy of snapshot\n"
	"SNAPSHOT       saves the current state as a snapshot\n"
	"\n"
	"MIN <key>  displays minimum value\n"
	"MAX <key>  displays maximum value\n"
	"SUM <key>  displays sum of values\n"
	"LEN <key>  displays number of values\n"
	"\n"
	"REV <key>   reverses order of values (simple entry only)\n"
	"UNIQ <key>  removes repeated adjacent values (simple entry only)\n"
	"SORT <key>  sorts values in ascending order (simple entry only)\n"
	"\n"
	"FORWARD <key> lists all the forward references of this key\n"
	"BACKWARD <key> lists all the backward references of this key\n"
	"TYPE <key> displays if the entry of this key is simple or general\n";

#endif



