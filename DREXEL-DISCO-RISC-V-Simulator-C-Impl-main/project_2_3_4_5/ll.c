
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "set.h"

typedef struct sNode sNode;
typedef struct set_t Set;

struct sNode {
	char* data ;
	struct sNode *next ;
} ;

struct set_t {
	struct sNode *head ;
	size_t n ;
} ;


Set* set()
{
	Set *rv = malloc(sizeof(Set)) ;
	rv->n = 0 ; 
	//rv->head->next = NULL;
	return rv ;
}

size_t	setSize( const Set* s ) 
{
	size_t length = 0 ; 
	
	for (const sNode *current = s->head ; current !=NULL ; current = current->next){ //may be current->next != NULL, TODO: test that
		length++;
	}
	
	return length ;
}

bool setFind( const Set* s, const char* x ) 
{
	
	for (const sNode *current = s->head ; current !=NULL ; current = current->next){ 
		if ( strcmp(current->data , x) == 0)
			return true; //if true doesn't work, use 1
	}
	return false; //may use 0 instead
	
}

bool setInsert( Set* s, const char* x ) 
{
	
//	if (setFind(s , x))
//		return false;
	if (strlen(x) > MAX_WORD_SIZE)
		return false;
	sNode *new_node;
	new_node = malloc(sizeof(sNode));
	
	new_node->data = strdup(x);
	new_node->next = s->head; 

	s->head = new_node ; 

	s->n = s->n + 1; 
	return true;

}

void setKill( Set* s )
{
	
	sNode *current = s->head;
	sNode *next; 

	while (current != NULL){
		next = current->next;
		free(current->data);
		free(current);
		current = next;
	}
	
	//s->head = NULL ;  // MEMORY LEAK!
	free(s);
}
