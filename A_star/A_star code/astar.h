#ifndef ASTAR_H
#define ASTAR_H
typedef struct node{
	unsigned long id;
	//char name[184];
	double lat;
	double lon;
	unsigned short nsucc;
	unsigned short successors_size;
	int* successors;
	double g;
	double h;
	char status;
	struct node* previous;
	struct node* next;
	struct node* parent;
}node;
#endif
