// A simple C program for traversal of a linked list
// to compile to "gcc main.c -lm"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
 
struct Node {
    int opcode;
    int fetch[33]; //pie was copied over lazily and fetch ought to be fetch[33]
    int execResult; //I like camel case lol
    int bitsElevenToSeven; //value based on certain bits of int fetch //populated during 
    int funct3; //value based on certain bits of int fetch
    struct Node* next;
};
 
// This function prints contents of linked list starting from
// the given node
void printList(struct Node* n)
{
    while (n != NULL) {
        printf(" fetch[0]: %d \n", n->fetch[0]);
        printf(" n->opcode: %d \n", n->opcode);
        n = n->next;
    }
}

int bin_to_dec(int n) { 
//ANS: Function takes a binary value and converts it to decimal. Decimal is easier to compare than binary numbers stored in an array
    int dec = 0, i = 0, rem;
    while (n != 0) {
        rem = n % 10;
        n /= 10;
        dec += rem * pow(2, i);
        ++i;
    }
    return dec;
}

int opcode_parse(struct Node* head){
      char deci_1[9] = "\0";
      char a_1[2] = "1";
      char a_0[2] = "0";
      for(int j=6; j>=0; j--){ // loop runs 7 times for 7 bits
          if(head->fetch[j] == 0){
              strcat(deci_1, a_0);
          }
          else{
              strcat(deci_1, a_1);
          }
      }
      int opcode_binary = atoi(deci_1);
      int deci_opcode = bin_to_dec(opcode_binary);

      return deci_opcode;
    } 


 
int main()
{
    struct Node* head = NULL;
    struct Node* inst2 = NULL;
    struct Node* inst3 = NULL;
    struct Node* inst4 = NULL;
    struct Node* inst5 = NULL;
 
    // allocate 5 nodes in the heap
    head = (struct Node*)malloc(sizeof(struct Node));
    inst2 = (struct Node*)malloc(sizeof(struct Node));
    inst3 = (struct Node*)malloc(sizeof(struct Node));
    inst4 = (struct Node*)malloc(sizeof(struct Node));
    inst5 = (struct Node*)malloc(sizeof(struct Node));
 
    int pie[33] = {1,1,0,0,1,1,0,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,0,1,1,0,0,0,0,0,0,0,0};
    int *imme_binary = &*pie;
   // head->fetch[0] = pie[0]; // assign data in first node 

    //forloop to insert the 32 bit inst 
    for(int i = 0; i<=32; i++){
      head->fetch[i] = imme_binary[i];
    }
    head->opcode = opcode_parse(head);
    head->next = inst2; // Link first node with second
 
 /*
    second->data = 2; // assign data to second node
    second->next = third;
 
    third->data = 3; // assign data to third node
    third->next = NULL;
 */
    printList(head);
 
    return 0;
}