/*
    Created By: Jared Fowler
    Comp 222 Spring 2014
    Assignment #2 - Cache Memory
*/

#include <stdio.h>
#include <stdlib.h>

//Global Constants----------------------
const int MAX_MEMORY_ALLOCATION = 100000;
const int MAX_CACHE_ALLOCATION  = 100000;
const int DEFAULT_TAG = -1;
            
//Structures----------------------------
struct struct_cache{
    int tag;
    int* block;
};
typedef struct struct_cache cache; //Alias

//Prototypes----------------------------
void printMenu();
void getParameters(int*,int*,int*,int**,cache**);   //Pointer to pointer needed to update pointer in function main
void cacheOperation(int,int,int,int*,cache*);
void cleanUp(int**,cache**,int);                    //This will give dynamic memory back to the operating system.

//--------------------------------------
//-------------FUNCTIONS----------------
//--------------------------------------
int main(){
    
    //Variables and Pointers
    int* mmPtr = NULL;      //Pointer to "Main Memory" - An array of type int
    cache* cPtr = NULL;     //Pointer to "Cache" - An array of user-defined type cache
    int mainMemorySize = 0;
    int cacheSize = 0;
    int blockSize = 0;      //(words/block)
    
    char choice;            //To be used for main menu option

    //Initiate a do-while loop.  
    do{
        printMenu();
        printf("\n\nEnter selection: ");
        //scanf("%c",&choice);
        do
        choice = getchar();
        while (isspace(choice)); //This avoids white space and carraige return
        
        switch (choice) {
            case '1':
                printf("\n\n");
                getParameters(&mainMemorySize, &cacheSize, &blockSize,&mmPtr,&cPtr);
                break;
            case '2':
                cacheOperation(mainMemorySize,cacheSize,blockSize,mmPtr,cPtr);
                break;
            case '3':
                //Clean up
                if(blockSize != 0)
                    cleanUp(&mmPtr,&cPtr,(cacheSize/blockSize));
                return 0;
            default:
                printf("\nInvalid choice.");
        }
        
        printf("\n\n\n");
    
    }while(choice != '3');

}

//------------------------------------------------------------------------------------------------------------------
void printMenu(){

    printf("Cache memory allocation and mapping:\n");
    printf("------------------------------------\n");
    printf("1) Enter parameters\n");
    printf("2) Access cache for reading/writing and transfer data\n");
    printf("3) Quit\n");
    
}

//------------------------------------------------------------------------------------------------------------------
void getParameters(int* mms, int* cs, int* bs, int** mmPtr, cache** cPtr){

    //First, lets free up existing used dynamic memory from any previous option 1
    if(*bs != 0)
        cleanUp(mmPtr,cPtr,(*cs/(*bs)));

    int notValid = 1;  //we need to make sure that input data is valid
    
    //Get the Main Memory Size----------------------------------------
    do{
        printf("Enter main memory size(words): ");
        scanf("%d",mms);
        //Is the input within range?
        if(*mms < 0 || *mms > MAX_MEMORY_ALLOCATION){
            printf("\nError: Invalid Parameter - Value must range from 0 to %d\n\n", MAX_MEMORY_ALLOCATION);
        }
        else{
            notValid = 0;
        }
    }while(notValid);
    
    //Get the Cache Size---------------------------------------------
    notValid = 1;	//Reset our validity checker
    do{
        do{
            printf("Enter cache size (words): ");
            scanf("%d",cs);
            //Is the input within range?
            if(*cs < 0 || *cs > MAX_CACHE_ALLOCATION){
                printf("\nError: Invalid Parameter - Value must range from 0 to %d\n\n", MAX_CACHE_ALLOCATION);
            }
            else{
                notValid = 0;
            }
        }while(notValid);
        
        //Get the Block Size-----------------------------------------
        notValid = 1;	//Reset our validity checker
        printf("Enter block size (words/block): ");
        scanf("%d",bs);
        //Valid? Does this number evenly divide the cache?
        if(*bs < 1 || ((*cs)%(*bs) != 0)){
            printf("\nError: Invalid Parameter - Block Size must be positive and divide the Cache evenly.\n\n");
        }
    else{
                notValid = 0;
        }
        
    }while(notValid);
    
    //---------Now that we have our needed info, lets allocate dynamic memory for our pointers-----------
    
    //Pointer to "Main Memory"
    *mmPtr = (int*)malloc((*mms) * sizeof(int));
    //Initialize the values...
    int index;;
    for(index = 0; index < (*mms); index++){
        (*mmPtr)[index] = (*mms - index);
    }
    
    //Pointer to "Cache"
    //Note to self:  This array represents a 1-Dimension of rows in the cache.  The actual individual words which are located
    //on each row will be taken care of by the pointer inside the structure.
    *cPtr = (cache*)malloc( ((*cs)/(*bs)) * sizeof(cache) );	//Essentially, #rows * size of cache struct
    //Initialize the values...
    for(index = 0; index < ((*cs)/(*bs)); index++){
        (*cPtr)[index].block = NULL;
        (*cPtr)[index].tag = DEFAULT_TAG;
    }
    
}

//------------------------------------------------------------------------------------------------------------------
void cacheOperation(int mms,int cs,int bs,int* mmPtr,cache* cPtr){
    
    //local variables
    int operation       //Read == 0; Write == 1
    int mma;            //Main Memory Address
    int valueToWrite;   //Value to be written
    int tag;
    int block;          //Block
    int word;
    int base;           //Start of the block
    int value;          //value read from the cache
    
    
    //Read or Write?------------------------------
    do{
        printf("Select read (0) or write (1): ");
        scanf("%d",&operation);
        //Check for input validity...
        if(operation != 0 && operation != 1){
            //Error message
            printf("\nError. Check Choice.\n");
        }
        else
            break;
    }while(1);
    
    //Get Main Memory Address---------------------
    do{
        if(operation == 0)
            printf("\nEnter main memory address to read from: ");
        else
            printf("\nEnter main memory address to write to: ");
    
        scanf("%d",&mma);
        
        //Check for input validity...
        if(mma < 0 || mma > (mms-1)){
            printf("\nError.  Address must be between 0 - %d.\n",(mms-1));
        }
        else
            break;
    }while(1);
    
    
    //Get value to write--------------------------
    if(operation == 1){
        printf("Enter value to write: ");
        scanf("%d",&valueToWrite);
    }
    
    
    //Get tag, block, word, base -----------------
    tag  = mma / cs;
    block = (mma%cs) / bs;
    word = mma%bs;
    base = (mma/bs) * bs;
    
    
    //Case 1: miss in the cache: unallocated block
    if(cPtr[block].tag == -1){
        //Allocate the block
        cPtr[block].block = (int*)malloc(bs * sizeof(int));
    }
    
    
    //Case 2: miss in the cache: non-matching tag
    if(cPtr[block].tag != tag){
        //Overwrite the cache block with the block in main memory
        int index;
        for(index = 0; index < bs; index++){
            cPtr[block].block[index] = mmPtr[base + index];
        }
        //Update the tag
        cPtr[block].tag = tag;
        //Print "miss" message
        if(operation == 0)
            printf("Read miss!");
        else{
            printf("Write miss!");
            mmPtr[mma] = valueToWrite;
            cPtr[block].block[word] = valueToWrite;
        }
    }
    
    
    //Case 3: Hit in the cache: matching tags
    else{
        if(operation == 0){
            //get value from the cache
            value = cPtr[block].block[word];
        }
        else{
            cPtr[block].block[word] = valueToWrite;
            mmPtr[mma] = valueToWrite;
        }
        
        //Print "hit" message
        if(operation == 0)
            printf("Read hit!");
        else
            printf("Write hit!");
    }
    
    
    //Write final message-------------------------
    printf("\nWord %d of block %d with tag %d contains value %d",word,block,tag,cPtr[block].block[word]);
}

//------------------------------------------------------------------------------------------------------------------
void cleanUp(int** mmPtr,cache** cPtr,int size){

    //We need to step through the cache and free up every individual block.
    //I did side program experiment to verify this need... ie. free() does not take care of embedded dynamic memory.
    int index;
    for(index = 0; index < size; index++){
        free((*cPtr)[index].block);
    }
    //Now, free up the outer pointers
    free(*mmPtr);
    free(*cPtr);
}