/*
code for reading csv file, creating graph of nodes and storing
in a binary file.

Authors: Alexis Oger, Lawrence Adu-Gyamfi

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "astar.h"

int count_node(FILE* fichier, char* line)
{
	int i;
	int nb_node = 0;
	ssize_t nb_read;
	size_t len = 80000;//length max of the line

	for(i = 0; i < 3; i++)
	{
		if((nb_read = getline(&line, &len, fichier)) == -1)
		{
			printf("Fail reading 3 first lines\n");
			return(0);
		}	
	}
	while((nb_read = getline(&line, &len, fichier)) != -1)
	{
		if(line[0] == 'n')
		{
			nb_node++;
		}
		if(line[0] == 'w')
		{
			break;
		}
	}
	return(nb_node);
}

int store_binary(char* name, int nb_node, node* node_list)
{
	FILE* fin;
	int i;
	unsigned long ntotnsucc=0UL;
	/* Computing the total number of successors */
	for(i=0; i < nb_node; i++)
	{
		ntotnsucc += node_list[i].nsucc;
	}
	if ((fin = fopen (name, "wb")) == NULL)
	{
		printf("error : the output binary data file cannot be opened");
		return(0);
	}
	printf("\nbinary file open");
	/* Global data −−− header */
	if(fwrite(&ntotnsucc, sizeof(unsigned long), 1, fin) != 1 )
	{
		printf("error when initializing the output binary data file");
		return(0);
	}
	if(fwrite(&nb_node, sizeof(int), 1, fin) != 1 )
	{
		printf("error when initializing the output binary data file");
		return(0);
	}
	/* Writing all nodes */
	if( fwrite(node_list, sizeof(node), nb_node, fin) != nb_node )
	{
		printf("error when writing nodes to the output binary data file");
		return(0);
	}
	/* Writing sucessors in blocks */
	for(i=0; i < nb_node; i++) if(node_list[i].nsucc)
	{
		if( fwrite(node_list[i].successors, sizeof(int), node_list[i].nsucc, fin) != node_list[i].nsucc )
		{
			printf("error when writing edges to the output binary data file");
			return(0);
		}
	}
	printf("\nbinary file created\n");
	fclose(fin);
	return(1);
}

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

void addway(node* node_list,int node_left,int node_right)
{
	if(node_list[node_left].nsucc >= node_list[node_left].successors_size)
	{
		node_list[node_left].successors = (int*)realloc(node_list[node_left].successors, (node_list[node_left].successors_size+3)*sizeof(int));
		node_list[node_left].successors_size+=3;
	}
	node_list[node_left].successors[node_list[node_left].nsucc]=node_right;
	node_list[node_left].nsucc+=1;
}


int main(int argc, char** argv)
{
	if(argc!=3)
	{
		printf("this program need 2 arguments (the name of a csv file and a binary file)");
		return(0);
	}
	
	//Find total number of nodes in the csv
	FILE* fichier = NULL;
	fichier = fopen(argv[1], "r");
	if(fichier == NULL)
	{
		printf("Fail opening the csv\n");
		return(0);
	}
	printf("csv open\n");

	int nb_node = 0;
	char* line;
	size_t len = 80000;//length max of the line
	line = (char*)malloc(len * sizeof(char));
	if(line == NULL)
	{
		printf("error allocating memory of line");
		return(0);
	}

	if((nb_node = count_node(fichier, line)) == 0)
		return(0);
	printf("nb nodes found:%d\n", nb_node);
	fseek(fichier, 0, SEEK_SET);
	printf("Go to the start of the file\n");

	//Create the graph
	//Create variables
	node* node_list;
	node_list = (node*)malloc(nb_node * sizeof(node));
	if(node_list == NULL)
	{
		printf("error allocating memory of node_list");
		return(0);
	}
	char* token;
	int nb_found;
	int nb_way = 0;
	int previous_node = 0;
	int node_left;
	int node_right;
	char* ptr;
	int oneway;
	int i;
	ssize_t nb_read;
	token = (char*)malloc(20 * sizeof(char));
	if(token == NULL)
	{
		printf("error allocating memory of token");
		return(0);
	}
	
	for(i = 0; i < nb_node; i++)//initialisation
	{
		node_list[i].successors_size=3;
		node_list[i].successors = (int*)malloc(3 * sizeof(int));
		node_list[i].g = 0;
		node_list[i].h = 0;
		node_list[i].status = '0';
		node_list[i].previous = NULL;
		node_list[i].next = NULL;
		node_list[i].parent = NULL;
		node_list[i].nsucc=0;
		if(node_list[i].successors == NULL)
		{
			printf("error allocating memory of successors");
			return(0);
		}
	}
	for(i = 0; i < 3; i++)
	{
		if((nb_read = getline(&line, &len, fichier)) == -1)
		{
			printf("Fail reading 3 first lines\n");
			return(0);
		}	
	}
	nb_node = 0;
	//Read the file and create the graph
	while((nb_read = getline(&line, &len, fichier)) != -1)
	{
		if(line[0] == 'n')
		{
			nb_found = 0;
			while((token = strsep(&line, "|")) != NULL)
			{
				if(nb_found == 1)
				{
					node_list[nb_node].id = atoi(token);
					previous_node = atoi(token);
				}
				//if(nb_found == 2)
					//strcpy(node_list[nb_node].name, token);
				if(nb_found == 9)
					node_list[nb_node].lat = atof(token);
				if(nb_found == 10)
					node_list[nb_node].lon = atof(token);
				nb_found++;
			}
			nb_node++;
		}
         	else if(line[0] == 'w')
		{
			nb_found = 0;
			while((token = strsep(&line, "|")) != NULL)
			{
				if(nb_found > 9)
				{
					node_right = binary_search(node_list, nb_node, strtoul(token, &ptr, 10));
					if(node_right != -1 && node_left != -1)
					{
						addway(node_list,node_left, node_right);
						if(oneway==0)
						{
							addway(node_list,node_right, node_left);
						}
					}
					if(node_right != -1)
					{
						node_left=node_right;
					}
				}
				else if(nb_found == 7)
				{
					if(token[0] == 'o')
						oneway=1;
					else
						oneway=0;
				}
				else if(nb_found == 9)
				{
					node_left = binary_search(node_list, nb_node, strtoul(token, &ptr, 10));
				}
				nb_found++;
			}
			nb_way++;
		}
	}
	printf("nb_node in csv: %d\n", nb_node);
	printf("nb_way in csv: %d\n", nb_way);
	fclose(fichier);
	
	//store as binary
	if(store_binary(argv[2], nb_node, node_list) != 1)
		return(0);
	//free
	for(i=0; i < nb_node; i++)
	{
		free(node_list[i].successors);	
	}
	free(token);
	free(line);
	free(node_list);
	return(0);
}
