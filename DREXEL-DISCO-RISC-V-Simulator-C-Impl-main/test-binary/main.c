#include <stdio.h>


//creating the signed binary function
int *signedBinary(int n){
    int c, k;

    //defining the array
    static int binary[12];

    //creating the binary number
    for (c = 11; c >= 0; c--)
    {
        k = n >> c;

        if (k & 1){
        //printf("1");
        binary[c] = 1;
        }
        else{
        //printf("0");
        binary[c] = 0;
        }
    }

    //flipping the bits of the binary number 
    for(int i = 11; i >= 0; i--){
        if (binary[i] == 1){
            binary[i] = 0;
        }
        else{
            binary[i] = 1;
        }
    }

    //adding the one to the number
    for(int h = 0; h < 12; h++){
        if (binary[h] == 1){
            binary[h] = 0;
        }
        else{
            binary[h] = 1;
            break;
        }
    }

    return binary;
}



int main()
{

    int n, c, k;

    //defining the array
    int *binary;

    printf("Enter an integer in decimal number system\n");
    scanf("%d", &n);

    printf("%d in binary number system is:\n", n);

    binary = signedBinary(n);

    //test printing 
    for(int j = 11; j >=0; j--){
        printf("%d", binary[j]);
    }
    printf("\n");

    /*
    //creating the binary number
    for (c = 11; c >= 0; c--)
    {
        k = n >> c;

        if (k & 1){
        //printf("1");
        binary[c] = 1;
        }
        else{
        //printf("0");
        binary[c] = 0;
        }
    }

    //flipping the bits of the binary number 
    for(int i = 11; i >= 0; i--){
        if (binary[i] == 1){
            binary[i] = 0;
        }
        else{
            binary[i] = 1;
        }
    }

    //adding the one to the number
    for(int h = 0; h < 12; h++){
        if (binary[h] == 1){
            binary[h] = 0;
        }
        else{
            binary[h] = 1;
            break;
        }
    }

    */
    return 0;
    
}