/*
    Created By: Jared Fowler
    Comp 222 Spring 2014
    Assignment #1 - Performance Metrics
*/

#include <stdio.h>

//Global Variables----------------------
            //void

//Prototypes----------------------------
void printMenu();                       //Prints out menu of 5 options
void getParameters(int*,int*,int*);     //Gets frequency, and finds total instruct and CPI
float getAvgCPI(int,int);               //Returns the Average CPI
float getExeTime(int,int);              //Returns the total execution time
float getMIPS(int, int, int);           //Returns MIPS (Million Instructions Per Second)



//--------------------------------------
//-------------FUNCTIONS----------------
//--------------------------------------
int main(){
    
    //variables
    int choice = 1;                 //used locally for user choice
    int frequency = 0;
    int SumOf_CPI_i_TIMES_I_i = 0;  //This represents the value sigma[i = 1,n](CPI_i * I_i)
    int SumOf_I_i = 0;              //This represents the value sigma[i = 1,n](I_i)
    
    do{
        printMenu();
        printf("\n\nEnter selection: ");
        scanf("%d",&choice);
        
        switch (choice) {
            case 1:
                printf("\n\n");
                getParameters(&frequency, &SumOf_CPI_i_TIMES_I_i, &SumOf_I_i);
                break;
            case 2:
                printf("\nThe average CPI of the sequence is: %.2f", getAvgCPI(SumOf_CPI_i_TIMES_I_i, SumOf_I_i));
                break;
            case 3:
                printf("\nThe total CPU time of the sequence is: %.2f msec", (getExeTime(SumOf_CPI_i_TIMES_I_i, frequency) * 1000));
                break;
            case 4:
                printf("\nThe total MIPS of the sequence is: %.2f", getMIPS(SumOf_I_i, SumOf_CPI_i_TIMES_I_i, frequency));
                break;
            case 5:
                return 0;
            default:
                printf("\nInvalid choice.");
        }
        
        printf("\n\n\n");

    }while(choice != 5);
}

//--------------------------------------
void printMenu(){
    
    printf("Menu of Options:\n");
    printf("----------------\n");
    printf("1) Enter parameters\n");
    printf("2) Calculate average CPI of a sequence of instructions\n");
    printf("3) Calculate total execution time of a sequence of instructions\n");
    printf("4) Calculate MIPS of a sequence of instructions\n");
    printf("5) Quit\n");
    
}

//--------------------------------------
void getParameters(int* freq,int* sumCPIandI,int* sumI){
    
    int numInstruct = 0;
    *sumCPIandI = *sumI = 0;	//We need to reset these values
    
    printf("Enter the number of instruction classes: ");
    scanf("%d",&numInstruct);
    printf("Enter the frequency of the machine (MHz): ");
    scanf("%d",freq);
    
    int index;
    int tempVal;
    int tempVal2;
    for(index = 1; index <= numInstruct; index++){
        
        printf("Enter CPI of class %d: ", index);
        scanf("%d",&tempVal);
        printf("Enter instruction count of class %d (millions): ",index);
        scanf("%d",&tempVal2);
        
        *sumCPIandI += (tempVal * tempVal2);
        *sumI += tempVal2;
        
    }	
}

//--------------------------------------
float getAvgCPI(int sumCPIandI,int sumI){

    //We don't want to divide by zero...
    if(sumI == 0)
        return -1;
    else
        return ((float)(sumCPIandI)/(sumI));	
}

//--------------------------------------
float getExeTime(int sumCPIandI,int freq){

    //We don't want to divide by zero...
    if(freq == 0)
        return -1;
    else
        return ((float)(sumCPIandI)/(freq));
        
}

//--------------------------------------
float getMIPS(int sumI, int sumCPIandI,int freq){
    
    return (sumI / getExeTime(sumCPIandI, freq));
        
}