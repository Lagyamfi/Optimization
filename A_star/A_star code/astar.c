/*
Code for astar algorithm together with helper functions for
list initialization, list management, print result to file, 
binary search.

Authors: Lawrence Adu-Gyamfi, Alexis Oger
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "astar.h"

// Structure datatype for list
typedef struct {
    node *top;
    node *bottom;
    unsigned long size; // number of nodes in list
}list;

//prototype declaration for heuristic functions
double heuristic(node node1, node node2);
double distance(node node1, node node2);

//function for search for node in node list
int binary_search(node* node_list,int nb_node,unsigned long search)
{
	int min,max,test;
	unsigned long a;
	min=0;
	max=nb_node-1;
	if(node_list[max].id==search)
	{
		return(max);
	}
	test=nb_node/2;
	while((a=node_list[test].id)!=search && (min+1)<max)
	{
		if(a>search)
		{
			max=test;
		}
		else
		{
			min=test;
		}
		test=(min+max)/2;
	}
	if(a!=search)
		return(-1);
	return(test);
}

// Function to Initialise open list
void initialize_list(list *open_list){
    open_list->top = NULL;
    open_list->bottom = NULL;
    open_list->size = 0; 
}

// Function to insert node before another node in open list
void insert_above(list *open_list, node *temp_node, node *my_node){
    if (open_list->top == temp_node)
    {
        temp_node->previous = my_node;
        open_list-> top = my_node;
        my_node->previous = NULL;
        my_node->next = temp_node;
    } else 
    {
        my_node->previous = temp_node->previous;
        my_node->next = temp_node;
        (temp_node->previous)-> next = my_node;
        temp_node->previous = my_node;
    }
    open_list->size ++;
}

//Function to insert node after another node in open_list
void insert_below(list *open_list, node *temp_node, node *my_node) 
{
    if(open_list->bottom == temp_node)
    {
        temp_node->next = my_node;
        my_node->previous = temp_node;
        my_node->next = NULL;
        open_list->bottom = my_node;
    } else 
    {
        (temp_node->next)->previous = my_node;
        my_node->next = temp_node->next;
        my_node->previous = temp_node;
        temp_node->next = my_node;
    }
    open_list->size++;
}


//Function to add node to open_list
void insert_in_list(list *open_list, node *my_node)
{
    int i;
    node* temp_node;

    if (open_list->size == 0)
    {
        open_list-> top = my_node;
        open_list-> bottom = my_node;
        my_node->previous = NULL;
        my_node->next = NULL;
        open_list->size++;
	my_node->status='1';
    } else 
    {
        for (i=0, temp_node = open_list->top; temp_node != NULL; i++, temp_node = temp_node->next) 
        { 
            if ((my_node->h + my_node->g) < (temp_node->h + temp_node->g))
            {
                insert_above(open_list, temp_node,my_node);
                my_node->status = '1';
                return;
            }
            
        }
        insert_below(open_list, open_list->bottom, my_node);
        my_node->status='1';
    }
}

// Function to remove node from open list = adding in closed list

void remove_from_list(list *open_list, node *my_node)
{
    if(open_list->top == open_list->bottom)
    {
        open_list->top = open_list->bottom = NULL;
        my_node->previous = my_node->next = NULL;
    }
    else if (my_node == open_list->top)
    {
        open_list->top = my_node->next;
        (my_node->next)->previous = NULL;
        my_node->next = my_node->previous = NULL;
    }
    else if (my_node == open_list->bottom)
    {
        open_list->bottom = my_node->previous;
        (my_node->previous)->next = NULL;
        my_node->next = my_node->previous = NULL; 
    }
    else 
    {
        (my_node->next)->previous = my_node->previous;
        (my_node->previous)->next = my_node->next;
        my_node->next = my_node->previous = NULL;
    }
    open_list->size --;
    my_node->status = '2';

}

//function to print result to file
void printresult(node* node_current, FILE* result)
{
	if(node_current == NULL)
		return;
	else
	{
		printresult(node_current->parent, result);
		fprintf(result, "Node id:%lu \t | Distance:%lf\n", node_current->id, node_current->g);
	}	
}

void print_result(node* node_current)
{
	FILE* result;
	if ((result = fopen ("result.txt", "w")) == NULL)
	{
		printf("error : the output result.txt data file cannot be opened");
		return;
	}
	printresult(node_current, result);
	printf("You can find the path in the file result.txt\n");
	fclose(result);
}

/* ---------- A* ALGORITHM --------------------------------*/

//1. Put node_start in the OPEN list with f (node_start) = h(node_start) (initialization)
void a_star(node *nodes, unsigned long node_start, unsigned long node_goal, int num_nodes)
{
    double successor_current_cost;
    //unsigned long succ_ID;
    node *start_node, *goal_node; 
    node *node_current;
    node *node_successor;
    int i;
    int start_node_index, goal_node_index;
    int iter=0;
    int cont;

    //A_Node *astar_nodes; 

    //Inform user that A star search is starting
    printf("Finding Route with A-Star!\n");

    //search for start and goal nodes in nodes list
    start_node_index = binary_search(nodes, num_nodes ,node_start);
    start_node = &nodes[start_node_index];
    goal_node_index = binary_search(nodes, num_nodes, node_goal);
    goal_node = &nodes[goal_node_index];
    start_node->g = 0;
    start_node->h = heuristic(*goal_node, *start_node); // implement heuristic function

    printf("START NODE INDEX: %d lat: %lf lon: %lf\n", start_node_index, start_node->lat, start_node->lon);
    printf("GOAL NODE INDEX: %d lat: %lf lon: %lf\n", goal_node_index, goal_node->lat, goal_node->lon);


    list open_list;
    initialize_list(&open_list);
    open_list.top = start_node;
    open_list.bottom = start_node;
    start_node->status ='1';
    open_list.size++;

// 2. while the OPEN list is; not empty {
    while (open_list.size != 0)
    {
     //cont=0;
     iter++;
     if(iter%100000==0)
     {
     	printf("Iter = %d, and distance =%lf\n", iter, node_current->g + node_current->h);
     }
        //3.      Take from the open list the node node_current with the lowest
        //4.      f (node_current) = g(node_current) + h(node_current)
        node_current = open_list.top; // top node in open list has lowest F

        //5.      if node_current is node_goal we have found the solution; break
        if (node_current == goal_node)
        {
	    printf("Solution found at iteration %d, id: %lu ,total distance: %lf\n", iter, node_current->id, node_current->g);
	    print_result(node_current);
            return; // goal reached
        }
        //6.      Generate each state node_successor that come after node_current

        //7.      for each node_successor of node_current {

        for (i=0; i < node_current->nsucc; i++)
        {
	    cont=0;
            node_successor = &nodes[node_current->successors[i]];
            //8.          Set successor_current_cost = g(node_current) + w(node_current, node_successor)
            successor_current_cost = node_current->g + heuristic(*node_current, *node_successor);
            //9.          if node_successor is in the OPEN list {
            if (node_successor->status == '1')
            {
                //10.             if g(node_successor) ≤ successor_current_cost continue (to line 20 )
                if(node_successor->g <= successor_current_cost)
                    cont=1;
		else
		    remove_from_list(&open_list, node_successor);
            }
            //11.         } else if node_successor is in the CLOSED list {
            else if(node_successor->status== '2')
            {
                //12.            if g(node_successor) ≤ successor_current_cost continue (to line 20 )
                if (node_successor->g <= successor_current_cost)
                    cont=1;
                //13.            Move node_successor from the CLOSED list to the OPEN list
                //insert_in_list(&open_list, node_successor);
            }
            //14.         } else {
            //15.             Add node_successor to the OPEN list
            //16.             Set h(node_successor) to be the heuristic distance to node_goal
            //17.         }
            else {
                //insert_in_list(&open_list, node_successor);
                node_successor->h = heuristic(*node_successor, *goal_node);
            }
            //18.         Set g(node_successor) = successor_current_cost
            //19.         Set the parent of node_successor to node_current
            //20.     }
            if(cont==0)
            {
                node_successor->g = successor_current_cost;
                node_successor->parent = node_current;
		insert_in_list(&open_list, node_successor);
            }

        }
        //21.     Add node_current to the CLOSED list
        //22.}
        remove_from_list(&open_list, node_current);
    }
    //23.if(node_current != node_goal) exit with error (the OPEN list is empty)
    if (node_current != goal_node)
    {
        return;
        printf("Error, goal_node not found");
    }

}
