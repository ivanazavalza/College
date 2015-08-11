/*
    Created By: Jared Fowler
    Comp 222 Spring 2014
    Assignment #3 - Error detection/correction
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//global variables-----------------------------
    //void
//global constants-----------------------------
    //void
//prototypes-----------------------------------
void printMenu();
void getParameters(int*,char**);
void checkHammingCode(int, char*);
void cleanUp(char**);

//-------------FUNCTIONS-----------------------
//---------------------------------------------
int main(){
    
    //Variables
    char* binary = NULL;
    int parity = 0;
    char choice;
    
    do{
        printMenu();
        printf("\n\nEnter selection: ");
        
        do
        choice = getchar();
        while (isspace(choice)); //This avoids white space and carraige return
        
        switch (choice) {
            case '1':
                printf("\n\n");
                getParameters(&parity, &binary);
                break;
            case '2':
                printf("\n\n");
                checkHammingCode(parity, binary);
                break;
            case '3':
                if(binary != NULL)
                    cleanUp(&binary);
                return 0;
            default:
                printf("\nInvalid choice.");
        }
        
        printf("\n\n\n");
    
    }while(choice != '3');

}

//------------------------------------------------------------------------------------------------------------------
void printMenu(){

    printf("Hamming Code Checker:\n");
    printf("---------------------\n");
    printf("1) Enter parameters\n");
    printf("2) Check Hamming code\n");
    printf("3) Quit\n");
    
}

//------------------------------------------------------------------------------------------------------------------
void getParameters(int* parity, char** binary){
    //Variables
    int max_length;
    int valid = 1;
    
    //Prompt user for max length of Hamming code
    do{
        printf("Enter the maximum length: ");
        scanf("%d",&max_length);
    }while(max_length < 0);
    
    //Prompt user for the parity 1 or 0
    do{
        printf("Enter the parity (0=even, 1=odd): ");
        scanf("%d",parity);
    }while(*parity != 1 && *parity != 0);
    
    //Allocate dynamic memory for pointer.  Free original memory if in use
    if(*binary != NULL)
        cleanUp(binary);
    
    *binary = (char*)malloc(max_length * sizeof(char));
}

//------------------------------------------------------------------------------------------------------------------
void checkHammingCode(int parity, char* binary){
    //Variables
    int actual_length;
    int numParityBits;
    
    //Prompt for Hamming code
    printf("Enter the Hamming code: ");
    scanf("%s",binary);
    
    //Get the actual length
    actual_length = strlen(binary);
    
    //Get number of parity bits
    //numParityBits = ceil( log(actual_length) / log(2) );  //we actually don't need this...
    
    //Check Hamming Code for erros and correct
    //Should be something like:  1,3,5,7,9,11,13,15		2,3,6,7,10,11,14,15		4,5,6,7,12,13,14,15		8,9,10,11,12,13,14,15
    int i,j,k,parityFlag;
    int errorPosition = 0;
    
    
    for(i = 1; i < actual_length; i *= 2){  //For each parity bit
        //initialize current parity flag
            parityFlag = parity;
            
        for(j = i; j <= actual_length; j += 2*i){   //For each sequence starting position

            for(k = j; (k < (j + i)) && (k <= actual_length ); k++){   //For each position to be checked
                //We not only use xor on all the sequence values, but we xor it all with the parity bit type.
                //This allows us to avoid the need to compare the parity type
                parityFlag = parityFlag ^ (binary[actual_length - k] - '0'); //'^' is the xor operator. We subtract '0' b/c binary is array of chars.
            }
        }
        
        //Find Error location
        errorPosition = errorPosition + (parityFlag * i);
    }
    
    //If no error, print and return'
    if(errorPosition == 0){
        printf("There is no bit error");
        return;
    }
    
    //else, Correct error position
    if(binary[actual_length - errorPosition] == '0')
        binary[actual_length - errorPosition] = '1';
    else
        binary[actual_length - errorPosition] = '0';
        
    printf("There is an error in bit: %d\n", errorPosition);
    printf("The corrected Hamming code is: %s", binary);
    
}

//------------------------------------------------------------------------------------------------------------------
void cleanUp(char** ary){
    free(*ary);
}