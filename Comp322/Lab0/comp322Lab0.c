/*Created By: Jared Fowler
August 26, 2014
CSUN Comp 322L ; Prof. Fitzgerald

"Making Sense of Ones and Zeros"

A command line tool that will read a series of 1's and 0's
from a file.  For each line of 8 bits this program will spit
out: Original Value, ASCII char, Dec. Value, EVEN or ODD parity,
     Error in parity assuming that we are using EVEN parity.

Note: The user has the option to either pass us in a file or a direct
input via stdin.  Example:

$ cat fileName1 fileName2 ... | ./programName

or

$ echo 100100010110101 | ./programName

or

$ ./programName inputFileName

One might try to input both stdin and a file, but this program will currently
only look to handle one or the other.  A file passed in will at this time 
take priority.

*/

#include <stdio.h>  //Used for printing to the screen
#include <unistd.h> //Used to read from a file
#include <fcntl.h>  //Used for opening a file


//typedef's and define's-------------------------------------
typedef unsigned char		BYTE;
typedef unsigned int		UINT32;
typedef signed int		INT32;

//Global Constants-------------------------------------------

const BYTE BITS_PER_BYTE = 8;

/*There are 34 chars that are not represented by single symbols.
  The largest of these chars 'space' has 5 letters + null terminator*/
const char SPECIAL_CHARS[34][6] = { {"NUL"/*0x00*/}, {"SOH"}, {"STX"}, {"ETX"}, {"EOT"}, 
                                    {"ENQ"}, {"ACK"}, {"BEL"}, {"BS"}, {"TAB"}, {"LF"}, 
				    {"VT"}, {"FF"}, {"CR"}, {"SO"}, {"SI"}, {"DLE"}, 
				    {"DC1"}, {"DC2"}, {"DC3"}, {"DC4"}, {"NAK"}, {"SYN"}, 
				    {"ETB"}, {"CAN"}, {"EM"}, {"SUB"}, {"ESC"}, {"FS"}, 
				    {"GS"}, {"RS"}, {"US"}, {"Space"/*0x20*/}, {"DEL"/*0x7F*/} };

//Prototypes-------------------------------------------------
void printMenuHead(void);                     //Prints Menu Column Names and Separating lines 
void printDataLine(char*, char, BYTE, BYTE);  //Takes BITS_PER_BYTE bytes of ascii binary and writes data line
char binaryToAscii(char*);                    //Converts BITS_PER_BYTE bytes of ascii binary to a single char
BYTE binaryToDecimal(char*);                  //Gets a decimal value of BITS_PER_BYTE bytes of binary ascii
BYTE getParity(char*);                        //1 : ODD ; 0 : EVEN


/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/*FUNCTIONS $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

int main(int argc, char * argv[]){


	#if(0)  //Debugging - Prints out passed in args
	int i = 0;
    	for (i = 0; i < argc; i++) {
		printf("argv[%d] = %s\n", i, argv[i]);
    	}
	printf("%d\n\n",argc);
	#endif
	
	
	#if(1)  //Regular Run
	
	/*Help for file open/read was found at: 
	  http://www.techytalk.info/linux-system-programming-open-file-read-file-and-write-file/ */
	
	UINT32 input_fd;            /* Input and output file descriptors */
    	ssize_t ret_in;             /* Number of bytes returned by read() */
    	char buffer[BITS_PER_BYTE]; /* Character buffer */
	char tempChar;              /* Will hold a single ascii value read from file */
	BYTE bitCount = 0;          /* Keeps track of chars read from inputFile */
	
	
	//Determine if the user has passed us a file to read from via argv
	if(argc > 1){ //User has attempted to pass in a file or a '-'
	
		//Is the fileName "-"?
		if((argv[1][0] == '-') && (argv[1][1] == '\0')){
			input_fd = 0; /*stdin*/
		}
		//We have a fileName, is it valid?
		else{
			//Create input file descriptor
			input_fd = open (argv [1], O_RDONLY);
			if (input_fd == -1) {
				perror ("open");
				return 1;
			}
		}	
	}
	//Else, we are dealing with stdin
	else{ input_fd = 0; /*stdin*/ }
	
	//Print the initial menu header
	printMenuHead();
	
	//Continue to loop through until we are done reading from the file
	while((ret_in = read (input_fd, &tempChar, 1)) > 0){
	
		//If we are in here it is b/c we are not at the end of the file
		//Filter out anything that is not a 1 or 0
		if(tempChar == '0' || tempChar == '1'){
		
			//Add value to BITS_PER_BYTE byte buffer
			buffer[bitCount] = tempChar;
			
			//Update the bitCount
			bitCount++;
			
			//If buffer is full, send it to print cycle
			if(bitCount >= BITS_PER_BYTE){
			
				//Send buffer to the print cycle
				printDataLine(buffer, binaryToAscii(buffer), binaryToDecimal(buffer), getParity(buffer));
				
				//Reset the bitCount
				bitCount = 0;
			}
		}
		
	};
	
	//Now that we have reached EOF, we check to see if we read a full BITS_PER_BYTE bytes from the file
	if(bitCount != 0){
	
		//Fill the remaining buffer slots with 0's
		while(bitCount < BITS_PER_BYTE){
			buffer[bitCount] = '0';
			bitCount++;
		}
		
		//Print out final line
		printDataLine(buffer, binaryToAscii(buffer), binaryToDecimal(buffer), getParity(buffer));
	}
	
 
	/* Close file descriptors */
    	close (input_fd);
    
	
	#endif

	return 0;
}

//------------------------------------------------------------
void printMenuHead(void){
	printf("Original ASCII    Decimal  Parity   T. Error\n");
	printf("-------- -------- -------- -------- --------\n");
}

//------------------------------------------------------------
void printDataLine(char* binData, char asciiVal, BYTE decVal, BYTE parVal){

	UINT32 index = 0;

	//Begin by printing the binary
	for (index = 0; index < BITS_PER_BYTE; index++){
		printf("%c", binData[index]);
	}

	//Next, print out the ASCII value
	if (asciiVal < '!')
		printf("%9s", SPECIAL_CHARS[asciiVal]);
	else if (asciiVal == 0x7F)
		printf("%9s", SPECIAL_CHARS[33]);
	else
		printf("%9c", asciiVal);

	//Next, print out the decimal value
	printf("%9d", decVal);

	//Finally, print out EVEN or ODD Parity along with Parity error
	if (parVal){ //ODD PARITY b/c != 0
		printf("%9s", "ODD");
		printf("%9s", "TRUE");
	}
	else{
		printf("%9s", "EVEN");
		printf("%9s", "FALSE");
	}
	
	
	//Return to next line in prep. for next write
	printf("\n");

}

//------------------------------------------------------------
char binaryToAscii(char* binData){

	//We will utilize a method of bit-shifting in order to do this.
	UINT32 index = 0;
	char returnChar = 0x00;

	for (index = 1; index < BITS_PER_BYTE; index++){
		//Get literal 1 || 0 value, shift it, add it
		returnChar += ((binData[index] - '0') << (7 - index));
	}

	return returnChar;
}

//------------------------------------------------------------
BYTE binaryToDecimal(char* binData){

	return binaryToAscii(binData);
}

//------------------------------------------------------------
BYTE getParity(char* binData){

	UINT32 index = 0;
	BYTE total = 0;

	for (index = 0; index < BITS_PER_BYTE; index++){
		total += (binData[index] - '0');
	}

	return (total % 2);
}
