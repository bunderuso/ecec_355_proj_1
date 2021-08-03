#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    //defining instruction
    char instruction[100] = "ld x9, 0(x10)";

    //tokening 
    char *token;
    char token_copy[100];
    char *copied;

    //isolating the ld
    token = strtok(instruction, " ");
    printf("%s\n", token);

    //isolating the x9 
    token = strtok(NULL, " ");
    printf("%s\n", token);

    //isolating the 0(x10)
    token = strtok(NULL, ", ");
    printf("%s\n", token);

    //creating a copy of the token
    sprintf(token_copy, "%s", token);
    printf("%s\n", token_copy);

    //isolating the 0
    copied = strtok(token_copy, "(");
    printf("%s\n", copied);

    //grabbing the x10
    copied = strtok(NULL, ")");
    printf("%s\n", copied);

    //char *test_string = "0(x10)";
    //printf("%s", test_string);



    return 0;
}