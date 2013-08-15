/*lista.h**/
#include "element.h"
#define PLITHOS 10

typedef struct{
	Element array[PLITHOS];		//array of structs Elements
	int used;			//no of used sheets
	int insert_count;
	int delete_count;
}List;

void create(List * l);
int  empty(List l);
int  full(List l);
int  get_elem_no(List l);
int  delete_elem(List *l,Element *elem);
int  insert_elem(List *l);
int  search_elem(List l);	
