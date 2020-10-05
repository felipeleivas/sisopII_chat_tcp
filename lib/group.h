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
  INT_LIST* connected_sockets;
  pthread_mutex_t user_mutex;
} USER;

typedef struct user_list {
  USER *user;
  struct user_list* next;
} USER_LIST;

typedef struct group_list {
  GROUP *group;
  struct group_list* next;
} GROUP_LIST; 


void print_connection_list(INT_LIST *int_list);
GROUP* find_group(GROUP_LIST *group_list, char* group_name);
GROUP_LIST* add_group_list(GROUP_LIST *group_list, GROUP *group);
void print_group_list(GROUP_LIST *group_list);
void associate_socket_group(int socket, GROUP* group);
GROUP* create_new_group(char* group_name);
void send_message_to_group(GROUP *group, char *message);
void restore_message_for_user(int socket, GROUP *group);
USER *find_user(USER_LIST *user_list, char *user_name);
USER_LIST *add_user_list(USER_LIST *user_list, USER *user);
USER *create_new_user(char *user_name);
void associate_socket_user(int socket, USER *user);
int count_elements(INT_LIST *int_list);
#endif