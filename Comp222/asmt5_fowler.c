/*
    Created By: Jared Fowler
    Comp 222 Spring 2014
    Assignment #5 - Pipelining/Superscalar Architecture
*/

#include <stdio.h>
#include <stdlib.h>

//Global Constants----------------------
    //Void
            
//Structures----------------------------
struct node{
    int dest;
    int src1, src2;
    int delay;
};

typedef struct node instr;


//Prototypes----------------------------
void printMenu();
void getParameters(instr**, int*); 
void option2(instr*, int);
void option3(instr*, int);
void print_chart(instr*, int);
void cleanUp(instr**);              //This will give dynamic memory back to the operating system.

//--------------------------------------
//-------------FUNCTIONS----------------
//--------------------------------------
int main(){

    //Variables
    instr* set = NULL;
    int numInstr;

    //Local Variables
    char choice;			//To be used for main menu option

    //Initiate a do-while loop.  
    do{
        printMenu();
        printf("\n\nEnter selection: ");
        
        do
        choice = getchar();
        while (isspace(choice)); //This avoids white space and carraige return
        
        switch (choice) {
            case '1':
                printf("\n\n");
                getParameters(&set, &numInstr);
                break;
            case '2':
                printf("\n\n");
                option2(set, numInstr);
                break;
            case '3':
                printf("\n\n");
                option3(set, numInstr);
                break;
            case '4':
                //Clean up
                cleanUp(&set);
                printf("\n\nThanks for the great semester!\n\n");
                return 0;
            default:
                printf("\nInvalid choice.");
        }
        
        printf("\n\n\n");
    
    }while(choice != '4');

}

//------------------------------------------------------------------------------------------------------------------
void printMenu(){

    printf("Pipelined/Superscalar instruction performance:\n");
    printf("----------------------------------------------\n");
    printf("1) Enter instructions\n");
    printf("2) Calculate/Chart total cycles on a 6-stage pipelined architecture\n");
    printf("3) Calculate/Chart total cycles on a 6-stage superscalar architecture \n");
    printf("4) Quit\n");
    
}

//------------------------------------------------------------------------------------------------------------------
void getParameters(instr** set, int* numInstr){

    //Local Variables
    char tempStr[9];  //We restrict the user to only enter 8 chars. We take into account the null-terminator
    
    

    //Prompt user for number of instructions
    do{ //prevents from having 0 or negative instructions which would cause program to break when freeing memory
        printf("Enter total number of instructions: ");
        scanf("%d",numInstr);
        
        if(*numInstr <= 0)
            printf("Error: Instruction count must be greater than ZERO.\n\n");
        else
            break;
    }while(1);
    
    //Set up dynamic memory for our structure
    //If we have already allocated memory, delete it!
    cleanUp(set);
    //Now, set us up some new memory allocation
    *set = (instr *)malloc(((*numInstr) + 1) * sizeof(instr));
    //We set index 0 to contain a special case...
    (*set)[0].dest = -1;
    //Because we technically begin with instruction 1, we set its delay to zero
    (*set)[1].delay = 0;
    
    //Give us some space
    printf("\n");
    
    //Loop through and prompt user for each instruction
    int i;
    for(i = 1; i <= (*numInstr); i++){
        printf("%d) ",i);
        scanf("%s", tempStr);
        (*set)[i].dest = tempStr[1] - '0';
        (*set)[i].src1 = tempStr[4] - '0';
        (*set)[i].src2 = tempStr[7] - '0';
    }
}

//------------------------------------------------------------------------------------------------------------------
void option2(instr* set, int numInstr){ //Calculate and chart for a 6-stage pipeline
    
    //Local Variables
    int i;
    int delay = 0;
    int overlap = 0;
    
    //for each instruction check for independences
    for(i = 2; i <= numInstr; i++){
        
        delay = 0;
    
        //Check for raw dependency between instructions i and i-2
        if((set[i-2].dest == set[i].src1) || (set[i-2].dest == set[i].src2)){
            if(overlap == 0){
                overlap = 1;
                delay = 1;
            }
            else{
                overlap = 0;
                delay = 0;
            }
        }
        else //No raw dependency
            overlap = 0;
        
        if((set[i-1].dest == set[i].src1) || (set[i-1].dest == set[i].src2)){
            delay = 2;
            overlap = 1;
        }
        
        //finally, set the delay...
        set[i].delay = delay + set[i-1].delay + 1;
        
    }
    
    //get the total_delay
    delay = set[numInstr].delay + 6;  //We have to take into consideration that although instr. 1 starts at 0, it must end at 6.
    //Print this out as the total cycles
    printf("Total number of cycles: %d \n\n", delay);
    
    //Print out the chart
    print_chart(set, numInstr);
    
    
}

//------------------------------------------------------------------------------------------------------------------
void option3(instr* set, int numInstr){
    /*
    Delays:
        2 if i -> i + 1,
        2 if i -> i + 2,
        
        1 if i -> i + 2 && supposed to be coupled with previous
        1 if i -> i + 3,
        1 if i -> i + 4 && not coupled with the previous
        
        0 otherwise
    */
    
    
    
    //Local Variables
    int i;
    int delay = 0;
    int overlap1 = 0;
    int overlap2 = 0;
    int overlap3 = 0;
    int overlap4 = 0;
    int overlap5 = 0;
    int pipe_delay = 1;
    
    
    //for each instruction check for independences (2 to n)
    for(i = 2; i <= numInstr; i++){
    
        delay = 0;   //Start assuming the delay is zero
        pipe_delay = 1 - pipe_delay;  //This needs to be oscillated.
        
        //Check if valid instructions to compare
        if(i >= 5){
            //Check for a RAW (read after write) dependency between instructions i-5) and i)
            if((set[i-5].dest == set[i].src1) || (set[i-5].dest == set[i].src2)){
                //Check for an overriding overlapping delay
                if((overlap1 == 0) && (overlap2 == 0) && (overlap3 == 0) && (overlap4 == 0)){
                    overlap5 = 1;
                    if(pipe_delay == 1)
                        delay = 0;
                    else
                        delay = 0;   //This is redundant but serves as a template.
                    
                    pipe_delay = 1;  //Force a de-coupling from the previous instruction.
                }
                else{
                    overlap5 = 0;  //Reset overlap5.
                }
            } //End of if there is a raw dependency
            
            else{
                overlap5 = 0; //Reset overlap5
            }
        }
        
        //Check if valid instructions to compare
        if(i >= 4){
            //Check for a RAW (read after write) dependency between instructions i-5) and i)
            if((set[i-4].dest == set[i].src1) || (set[i-4].dest == set[i].src2)){
                //Check for an overriding overlapping delay
                if((overlap1 == 0) && (overlap2 == 0) && (overlap3 == 0)){
                    overlap4 = 1;
                    if(pipe_delay == 1)
                        delay = 1;
                    else
                        delay = 0;   //This is redundant but serves as a template.
                    
                    pipe_delay = 1;  //Force a de-coupling from the previous instruction.
                }
                else{
                    overlap4 = 0;  //Reset overlap5.
                }
            } //End of if there is a raw dependency
            
            else{
                overlap4 = 0; //Reset overlap5
            }
        }
        
        //Check if valid instructions to compare
        if(i >= 3){
            //Check for a RAW (read after write) dependency between instructions i-5) and i)
            if((set[i-3].dest == set[i].src1) || (set[i-3].dest == set[i].src2)){
                //Check for an overriding overlapping delay
                if((overlap1 == 0) && (overlap2 == 0)){
                    overlap3 = 1;
                    if(pipe_delay == 1)
                        delay = 1;
                    else
                        delay = 1;   //This is redundant but serves as a template.
                    
                    pipe_delay = 1;  //Force a de-coupling from the previous instruction.
                }
                else{
                    overlap3 = 0;  //Reset overlap5.
                }
            } //End of if there is a raw dependency
            
            else{
                overlap3 = 0; //Reset overlap5
            }
        }
        
        //Check if valid instructions to compare
        if(i >= 2){
            //Check for a RAW (read after write) dependency between instructions i-5) and i)
            if((set[i-2].dest == set[i].src1) || (set[i-2].dest == set[i].src2)){
                //Check for an overriding overlapping delay
                if((overlap1 == 0)){
                    overlap2 = 1;
                    if(pipe_delay == 1)
                        delay = 2;
                    else
                        delay = 1;   //This is redundant but serves as a template.
                    
                    pipe_delay = 1;  //Force a de-coupling from the previous instruction.
                }
                else{
                    overlap5 = 0;  //Reset overlap5.
                }
            } //End of if there is a raw dependency
            
            else{
                overlap5 = 0; //Reset overlap5
            }
        }
        
        //Check if valid instructions to compare
        if(i >= 1){
            //Check for a RAW (read after write) dependency between instructions i-5) and i)
            if((set[i-1].dest == set[i].src1) || (set[i-1].dest == set[i].src2)){
                //Check for an overriding overlapping delay
                delay = 2;
                pipe_delay = 1;
            } //End of if there is a raw dependency
            
            else{
                overlap1 = 0; //Reset overlap5
            }
        }
        
        //finally, set the delay...
        set[i].delay = delay + set[i-1].delay + pipe_delay;
        
    }
    
    //get the total_delay
    delay = set[numInstr].delay + 6;  //We have to take into consideration that although instr. 1 starts at 0, it must end at 6.
    //Print this out as the total cycles
    printf("Total number of cycles: %d \n\n", delay);
    
    //Print out the chart
    print_chart(set, numInstr);
            
}

//------------------------------------------------------------------------------------------------------------------
void print_chart(instr* set, int numInstr){  //Prints the pipe with the individual instructions
    //Local Variables
    int i;
    int j;
    
    for(i = 1; i <= numInstr; i++){
        for(j = 0; j < set[i].delay; j++){
            printf("\t");
        }
        printf("FI\tDI\tCO\tFO\tEI\tWO\n");
    }
}

//------------------------------------------------------------------------------------------------------------------
void cleanUp(instr** theSet){
    if(*theSet != NULL)
        free(*theSet);
    *theSet = NULL;
}