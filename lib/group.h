#ifndef GROUP_HEADER
#define GROUP_HEADER
typedef struct int_list {
  int pid;
  struct int_list* next; 
} INT_LIST;


typedef struct group {
  char *name;
  INT_LIST*  connected_users;
  int seqn;
  pthread_mutex_t group_mutex;
} GROUP;

typedef struct user {
  char *name;

} USER;

typedef struct user_list {
  USER *user;
  struct user_list* next;
} USER_LIST;

typedef struct group_list {
  GROUP *group;
  struct group_list* next;
} GROUP_LIST; 


GROUP* find_group(GROUP_LIST *group_list, char* group_name);

GROUP_LIST* add_group_list(GROUP_LIST *group_list, GROUP *group);
void print_group_list(GROUP_LIST *group_list);
void associate_socket_group(int socket, GROUP* group);
GROUP* create_new_group(char* group_name);
void send_message_to_group(GROUP *group, char *message);

#endif