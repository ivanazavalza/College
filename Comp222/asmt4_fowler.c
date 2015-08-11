/*
    Created By: Jared Fowler
    Comp 222 Spring 2014
    Assignment #4 - Virtual Memory
*/

#include <stdio.h>
#include <stdlib.h>

//Global Constants----------------------
const int DEFAULT_TAG = -1;
            
//Structures----------------------------
struct struct_node{
    int vp;  //Virtual Page
    int pf;  //Page Frame
};
typedef struct struct_node row; //Alias

//Prototypes----------------------------
void printMenu();
void getParameters(int*,int*,int*,row**);           //Pointer to pointer needed to update pointer in function main
void mapOperation(int,int,int,row*);
void printPageTable(int, row*);                     //Takes in number of pages and the struct with the values
void cleanUp(row**);                                //This will give dynamic memory back to the operating system.

//--------------------------------------
//-------------FUNCTIONS----------------
//--------------------------------------
int main(){
    
    //Variables and Pointers
    row* pageTable = NULL;
    int mmSize = -1;
    int pageSize = -1;
    int replacePolicy = -1;  // 0:LRU , 1:FIFO
    
    char choice;            //To be used for main menu option

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
                getParameters(&mmSize, &pageSize, &replacePolicy,&pageTable);
                break;
            case '2':
                printf("\n\n");
                mapOperation(mmSize,pageSize,replacePolicy, pageTable);
                break;
            case '3':
                printf("\n\n");
                printPageTable((mmSize / pageSize), pageTable);
                break;
            case '4':
                //Clean up
                cleanUp(&pageTable);
                return 0;
            default:
                printf("\nInvalid choice.");
        }
        
        printf("\n\n\n");
    
    }while(choice != '4');

}

//------------------------------------------------------------------------------------------------------------------
void printMenu(){

    printf("Virtual Address Mapping:\n");
    printf("------------------------\n");
    printf("1) Enter parameters\n");
    printf("2) Map virtual address\n");
    printf("3) Print page table\n");
    printf("4) Quit\n");
    
}

//------------------------------------------------------------------------------------------------------------------
void getParameters(int* mms, int* ps, int* rp, row** ary){

    //First, lets free up existing used dynamic memory from any previous option 1
    if(*ps != DEFAULT_TAG)
        cleanUp(ary);

    int notValid;  //we need to make sure that input data is valid
    
    
    do{
        notValid = 1;
        //Get the Main Memory Size----------------------------------------
        do{
            printf("Enter main memory size(words): ");
            scanf("%d",mms);
            //Is the input within range?
            if(*mms <= 0){
                printf("\nError: Invalid Parameter - Value must be greater than 0\n\n");
            }
            else{
                notValid = 0;
            }
        }while(notValid);
        
        notValid = 1;   //Reset our validity checker
        //Get the Page Size---------------------------------------------
        do{
            printf("Enter page size (words/page): ");
            scanf("%d",ps);
            //Is the input within range?
            if(*ps <= 0 || *ps > *mms){
                printf("\nError: Invalid Parameter - Value must range from 1 to %d\n\n", *mms);
            }
            else{
                notValid = 0;
            }
        }while(notValid);
        
        notValid = 1;   //Reset our validity checker
        //Does the Page size evenly divide Main Memory?
        if(((*mms)%(*ps) != 0)){
            printf("\nError: Invalid Parameter - Page Size must divide Main Memory Size evenly.\n\n");
        }
        else{
            notValid = 0;
        }
        
    }while(notValid);
    
    notValid = 1;
    //
    do{
        printf("Enter replacement policy (0=LRU, 1=FIFO): ");
        scanf("%d",rp);
        //Is the input within range?
        if(*rp != 0 && *rp != 1){
            printf("\nError: Invalid Parameter - Value must be either 0 or 1\n\n");
        }
        else{
            notValid = 0;
        }
    }while(notValid);
    
    
    //---------Now that we have our needed info, lets allocate dynamic memory for our pointers-----------
    
    //Pointer to "Main Memory"
    *ary = (row*)malloc(((*mms)/(*ps)) * sizeof(row));
    //Initialize the values...
    int index;;
    for(index = 0; index < ((*mms)/(*ps)); index++){
        (*ary)[index].vp = DEFAULT_TAG;
    }
}

//------------------------------------------------------------------------------------------------------------------
void mapOperation(int mms,int ps,int rp,row* ary){
    
    //local variables
    int vmAddress = -1;
    int vp = -1;
    int offset = -1;
    int valid = 1;
    int numEntries = (mms/ps);
    
    //Has the user already entered parameters?
    if(ary == NULL){
        printf("\nError: Void Parameter - Please enter parameters before starting this operation.\n\n");
        return;
    }
    
    //Prompt user for the virtual memory address
    do{
        printf("Enter virtual memory address to access: ");
        scanf("%d",&vmAddress);
        
        //Is the input valid?
        if(vmAddress >= 0)
            valid = 0;
        else
            printf("\nError: Invalid Parameter - Virtual memory address must be greater than or equal to 0.\n\n");
            
    }while(valid);
    
    //Translate the address into two things. A virtual page value and an offset
    vp = vmAddress / ps;		//Virtual Page = Virtual Memory Address / Page Size
    offset = vmAddress % ps;	//Offset = Virtual Memory Address % Page Size
    
    //We now search the table for this entry
    int index = 0;
    while((index < numEntries) && (ary[index].vp != DEFAULT_TAG) && (ary[index].vp != vp))
        index++;
    
    //We now have three cases to consider-------------------
    //Case 1: index == Number of Entries
    if(index == numEntries){
    
        //Print Page Fault
        printf("Page fault!");
        
        //We need to replace an element in the table with our local element.  In this case,
        //both FIFO and LRU are the same.  We start by backing up the page frame value from
        //element 0. We then bump all the elements up. Finally, we write the local vp and 
        //what used to be the zero-th elements pf to the last element.
        int tempPfVal = ary[0].pf;
        for(index = 0; index < (numEntries - 1); index++){
            ary[index] = ary[index + 1];
        }
        ary[numEntries - 1].vp = vp;
        ary[numEntries - 1].pf = tempPfVal;
    }
    
    //Case 2: vp == -1    a.k.a  We have come to as list of -1's which means our value isn't in the table and there is space available
    else if(ary[index].vp == DEFAULT_TAG){
    
        //Print Page Fault
        printf("Page fault!");
        
        //We simply insert the local value at that index location, with the pf being the index itself
        ary[index].vp = vp;
        ary[index].pf = index;
    }
    
    //Case 3: We have a hit!   
    else{
        //We have a couple cases here..
        //FIFO Replacement Policy
        if(rp == 1){
            //DO NOTHING
        }
    
        //LRU Replacement Policy
        if(rp == 0){
            int i;
            int tempPf = ary[index].pf;
            for(i = index; i < (numEntries - 1); i++){
                if(ary[i + 1].vp == DEFAULT_TAG)
                    break;
                ary[i] = ary[i + 1];
            }
            
            //Set the values of original inquiry
            ary[i].vp = vp;
            ary[i].pf = tempPf;	
            
            //Update to the correct index
            index = i;
        }
        
        //Map va to pa
        int physicalAddress = ((ary[index].pf) * ps) + offset;
        
        //Print out message
        printf("Virtual address %d maps to physical address %d", vmAddress, physicalAddress);
    }
}

//------------------------------------------------------------------------------------------------------------------
void printPageTable(int size, row* ary){

    //Go through the array of row structs. So long as the virtual page isn't equal to -1, print out the pair
    int index = 0;
    while(index < size && ary[index].vp != DEFAULT_TAG){
        printf("VP %4d --> PF %4d\n", ary[index].vp , ary[index].pf);
        index++;
    }
}

//------------------------------------------------------------------------------------------------------------------
void cleanUp(row** ary){

    free(*ary);
    ary = NULL;
}