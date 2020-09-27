#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/group.h"

GROUP *find_group(GROUP_LIST *group_list, char *group_name)
{
	//TODO this is a critical section, and should be handle as so
	while (group_list != NULL)
	{
		if (strcmp(group_list->group->name, group_name) == 0)
		{
			return group_list->group;
		}
		group_list = group_list->next;
	}
	return NULL;
}

GROUP_LIST *add_group_list(GROUP_LIST *group_list, GROUP *group)
{
	//TODO this is a critical section, and should be handle as so
	GROUP_LIST *first_element_list = group_list;
	GROUP_LIST *new_group_list = (GROUP_LIST *)malloc(sizeof(GROUP_LIST));
	new_group_list->group = group;
	new_group_list->next = NULL;
	if (group_list != NULL)
	{

		GROUP_LIST *before = NULL;
		while (group_list != NULL)
		{
			before = group_list;
			group_list = group_list->next;
		}
		before->next = new_group_list;
		return first_element_list;
	}
	else
	{
		return new_group_list;
	}
}

INT_LIST *add_socket_list(INT_LIST *int_list, int socket)
{
	//TODO this is a critical section, and should be handle as so
	INT_LIST *first_element_list = int_list;
	INT_LIST *new_list = (INT_LIST *)malloc(sizeof(INT_LIST));
	new_list->pid = socket;
	new_list->next = NULL;
	if (int_list != NULL)
	{

		INT_LIST *before = NULL;
		while (int_list != NULL)
		{
			before = int_list;
			int_list = int_list->next;
		}
		before->next = new_list;
		return first_element_list;
	}
	else
	{
		return new_list;
	}
}



void print_group_list(GROUP_LIST *group_list){
	printf("\n Groups: ");
	while(group_list != NULL){
		printf("\n%s, connected_users: ", group_list->group->name);
		INT_LIST* int_list = group_list->group->connected_users;
		while(int_list != NULL){
			printf(" %d,", int_list->pid);
			int_list = int_list->next;
		}
		group_list = group_list->next;
	}

}

void associate_socket_group(int socket, GROUP* group){
	group->connected_users = add_socket_list (group->connected_users, socket);
}

// int main()
// {
// 	GROUP group1;
// 	group1.connected_users = NULL;
// 	group1.name = "Nati<1";
// 	GROUP group2;
// 	group2.connected_users = NULL;
// 	group2.name = "Nati<2";
// 	GROUP group3;
// 	group3.connected_users = NULL;
// 	group3.name = "Nati<3";
// 	GROUP group4;
// 	group4.connected_users = NULL;
// 	group4.name = "Nati<4";

// 	GROUP_LIST *groups = add_group_list(NULL, &group1);
// 	groups = add_group_list(groups, &group2);
// 	groups = add_group_list(groups, &group3);
// 	groups = add_group_list(groups, &group4);

// 	associate_socket_group(1, &group1);
// 	associate_socket_group(2, &group1);
// 	associate_socket_group(3, &group1);
// 	associate_socket_group(4, &group2);
// 	associate_socket_group(5, &group3);
// 	associate_socket_group(6, &group3);
// 	associate_socket_group(7, &group1);
// 	associate_socket_group(8, &group2);
// 	associate_socket_group(9, &group4);
// 	associate_socket_group(10, &group4);
// 	print_group_list(groups);
// }