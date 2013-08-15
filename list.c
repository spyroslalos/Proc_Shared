/*list.c**/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include "list.h"

void create(List * l){

	l->used=0;
}
int  empty(List l){
	if(!(l.used))
		return 1;
	else
		return 0;	
}
int  full(List l){
	if(l.used==PLITHOS)
		return 1;
	else
		return 0;
}
int  get_elem_no(List l){

	return (l.used);
}
/*"deletes" last element of the list*/
int  delete_elem(List *l,Element *elem){
	if(empty(*l)){
		printf("My list is empty , i cannot delete\n");
		return 0;			
	}	
	else{
		*elem=l->array[(l->used)-1];
		printf("I deleted the element with string %s\n",l->array[(l->used)-1].line);	
		l->used--;
		return 1;
	}
}
//inserts element in the end of lisyt-array

int  insert_elem(List *l){

	FILE *fp;
	int i=0;
	int nLines=0;
	int randLine=0;
	char Line[128];
	if(full(*l)){
		printf("My list is full , i cannot insert\n");
		return 0;		
	}
	
	fp=fopen("leitou.txt","r");		//anoigei to arxeio leitou.txt gia diavasma
	while((fgets(Line,sizeof(Line),fp))!=NULL){
    	
	        nLines++; 

	}
    	randLine = rand() % nLines;			//dialegei mia tyxaia grammi apo to arxeio
    	printf("Chose %d line of %d lines\n", randLine, nLines);

	/* 2nd pass - find the line we want. */
   	fseek(fp, 0, SEEK_SET);					//pigaine sthn arxi
    	for(i = 0; !feof(fp) && i <= randLine; i++)
        	fgets(Line,sizeof(Line), fp);

    	printf("%s", Line);
	strncpy(l->array[l->used].line,Line,sizeof(Line));
//	l->array[l->used].line=Line;
	l->used++;
	printf("Size is %d...\n",l->used);
	fclose(fp);
	return 1;
	
}
//anazhta kai typwnei ena tyxaio stoixeio apo th lista mas
int  search_elem(List l){
	int x=0;
	if(empty(l)){
		printf("My list is empty , i cannot search\n");
		return 0;			
	}
	x=rand()%(l.used);
	printf("%s\n",l.array[x].line);	

}	

