/* 
 * set.h - interface for the Set
 * - For all implementations of the Set
 *
 * gcc (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0 on
 * GNU/Linux 5.4.0-66-generic x86_64
 *
 * EDITOR:  tabstop=2, cols=120
 *
 */

#ifndef __MY_SET_H__
#define __MY_SET_H__

#include <stddef.h>

//////  Constants  //////////////////////////////

enum consts { 
	MAX_SET_SIZE = 30000 ,
	MAX_WORD_SIZE = 80 
} ;



//////  Set i/f  //////////////////////////////////

typedef struct set_t Set ;
typedef struct node node;

	// set - factory function
	// Returns a pointer to a new, empty set
	// Should be disposed of using setKill
Set* set() ;

	// setSize - return size of the set pointed to by s
size_t	setSize( const Set* s ) ;

	// return 1 if x in Set pointed to by s
_Bool setFind( const Set* s, const char* x ) ;

	// setInsert - Insert string x into set pointed to by s
	// x is copied into heap memory (see strdup)
	// return true (1) if x was successfully inserted
	// false (0) otherwise (x is already in set, no memory, etc.)
_Bool setInsert( Set* s, int *fetch ) ;

	// setKill - Decommissions 
void setKill( Set* ) ;


#endif // __MY_SET_H__
