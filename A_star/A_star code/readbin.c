/*
main code for implementation of astar algorithm for route finding.

Authors: Alexis Oger, Lawrence Adu-Gyamfi
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "astar.h"

//prototype for astar function
void a_star(node *nodes, unsigned long node_start, unsigned long node_goal, int num_nodes);

unsigned long start_node =240949599; //Bas´ılica de Santa Maria del Mar (Pla¸ca de Santa Maria) in Barcelona 
unsigned long goal_node = 195977239; // Giralda (Calle Mateos Gago) in Sevilla 

int main(int argc, char** argv)
{	

	time_t start_time;
	time_t read_time;
	time_t astar_time;

	if(argc!=2)
	{
		printf("this program need 1 argument (the name of a binary file)");
		return(0);
	}
	FILE* fin;
	int nnodes;
	node* nodes;
	int* allsuccessors;
	unsigned long ntotnsucc;
	int i;
	
	// start time measurement for opening and reading file	
	start_time = clock();

	if ((fin = fopen (argv[1], "r")) == NULL)
	{
		printf("error : the data file does not exist or cannot be opened");
		return(0);
	}
	printf("Binary file open succesfully\n");
	/* Global data −−− header */
	if(fread(&ntotnsucc, sizeof(unsigned long), 1, fin) != 1)
	{
		printf("error when reading the number of succesors");
		return(0);
	}
	if(fread(&nnodes, sizeof(int), 1, fin) != 1)
	{
		printf("error when reading the number of nodes");
		return(0);
	}
	/* getting memory for all data */
	if((nodes = (node*)malloc(nnodes*sizeof(node))) == NULL)
	{
		printf("error when allocating memory for the nodes vector");
		return(0);
	}
	if( (allsuccessors = (int*)malloc(ntotnsucc*sizeof(int))) == NULL)
	{
		printf("error when allocating memory for the edges vector");
		return(0);
	}
	/* Reading all data from file */
	if( fread(nodes, sizeof(node), nnodes, fin) != nnodes )
	{
		printf("error when reading nodes from the binary data file");
		return(0);
	}
	if(fread(allsuccessors, sizeof(int), ntotnsucc, fin) != ntotnsucc)
	if( fread(nodes, sizeof(node), nnodes, fin) != nnodes )
	{
		printf("error when reading sucessors from the binary data file");
		return(0);
	}
	fclose(fin);
	/* Setting pointers to successors */
	for(i=0; i < nnodes; i++)
	{
		if(nodes[i].nsucc)
		{
			nodes[i].successors = allsuccessors; allsuccessors += nodes[i].nsucc;
		}
	}
	printf("End reading binary file\n\n");

	// end time measurement
	read_time = clock() - start_time;

	printf("\nTotal time to read binary file: %fs \n", ((double)read_time)/CLOCKS_PER_SEC);
	

	//start time measurement for running a-star
	start_time = clock();

	//Chose starting and ending node
	a_star(nodes, start_node, goal_node, nnodes);
	
	// end time measurement
	astar_time = clock() - start_time;

	printf("\nTotal time to find solution with A_star: %fs \n", ((double)astar_time)/CLOCKS_PER_SEC);

	FILE *file1, *file2;
	char filename_1[100], filename_2[100], c;

	strcpy(filename_1, "result.txt");
	strcpy(filename_2, "result_final.txt");

	//open file for reading
	if ((file1 = fopen(filename_1, "r")) == NULL)
	{
	    printf("Cannot open file %s \n", filename_1);
	    exit(0);
	}
	// open new file for writing
	if ((file2 = fopen(filename_2, "w")) == NULL)
	{
	    printf("Cannot open file %s \n", filename_2);
	    exit(0);
	}
	fprintf(file2, "Total time to read binary file: %fs \n", ((double)read_time)/CLOCKS_PER_SEC);
	fprintf(file2,"Total time to find solution with A_star: %fs \n", ((double)astar_time)/CLOCKS_PER_SEC);
	
	//read contents from old file
	c = fgetc(file1);
	while (c != EOF)
	{
  	   fputc(c, file2);
  	   c = fgetc(file1);
	}
	fclose(file1);
	fclose(file2);
			
	free(nodes[0].successors);
	free(nodes);
	return(0);
}
