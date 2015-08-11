;************************************************************
;  Jared Fowler
;  CS M30 Fall 2013 
;  Project 1
;
;  This program prompts the user to input a string.  The string 
;  is then moved into a byte array in reverse order using indirect 
;  addressing.  
;
;************************************************************


                    .586
                    .MODEL flat, stdcall

                    include win32api.asm

                    .STACK 4096

                    .DATA

CR                  equ     13
LF                  equ     10
MAX_LENGTH          equ     255

UserInput           byte     MAX_LENGTH dup(0)                         ;Variable to store user input
msg1                byte    "Please input a string", CR, LF            ;Message to prompt user to input string

CharsRead           dword    0                                         ;Returned by ReadConsole

BytesWritten        dword    0

hStdOut             dword    0
hStdIn              dword    0

;Data added by Jared Fowler---------------------------------------

msg2                byte     MAX_LENGTH dup(0)   ;Variable to store reversed user input

;-----------------------------------------------------------------

                    .CODE

Main                Proc

                    ;*********************************************
                    ;In order to write to windows console screen
                    ;we need to get a handle to the screen
                    ;*********************************************
                    invoke  GetStdHandle, STD_OUTPUT_HANDLE
                    mov     hStdOut,eax                    ;Save output handle
                                
                    ;*********************************************
                    ;Get a handle to the standard input device
                    ;*********************************************
                    invoke  GetStdHandle, STD_INPUT_HANDLE
                    mov     hStdIn,eax                    ;Save input handle
        
                    ;*********************************************
                    ;Prompt user to input a string
                    ;*********************************************
                    invoke  WriteConsoleA, hStdOut, OFFSET msg1, SIZEOF msg1, OFFSET BytesWritten, 0

                    ;*********************************************
                    ;Set Console Mode for Line Input
                    ;*********************************************
                    invoke SetConsoleMode, hStdIn, ENABLE_LINE_INPUT + ENABLE_ECHO_INPUT 


                    ;*********************************************
                    ;Get User Input
                    ;*********************************************
                    invoke ReadConsoleA, hStdIn, OFFSET UserInput, SIZEOF UserInput, OFFSET CharsRead, 0


                    ;*********************************************
                    ;If the last character is a carriage return character, then we need
                    ;to subtract one from the number of characters read
                    ;*********************************************
                    mov edx,CharsRead               ;number of character user entered
                    dec edx                         ;subtract one since we are zero based
                    cmp UserInput[edx],CR           ;is the last character a carriage return?
                    jne NoCR                        ;no...then jump
                    dec CharsRead                   ;yes...then so subtract 1 from length

                    ;-------------Code added by Jared Fowler-------

NoCR:
                    cmp CharsRead,0                 ;if user inputs nothing
                    je  NoIN                        ;jump to end of code

                    mov ecx,CharsRead               ;prepare ecx for decrementation, and offset in UserInput
                    mov edi,0                       ;prepare edi for offset in msg2
                    mov eax, offset UserInput       ;eax will indirectly address UserInput
                    mov ebx, offset msg2            ;ebx will indirectly address msg2, where the reversed string will go

ReverseInput:
                    mov dh,byte ptr [eax][ecx - 1]  ;dh will hold current last element in UserInput
                    mov byte ptr[ebx][edi],dh       ;last element from UserInput moved into current element of msg2
                    inc edi                         ;Increase the next offset of msg2


                    loopd ReverseInput

NoIN:

                    ;-------------end of added code------------------

                                
                    ;*************************
                    ;Terminate Program
                    ;*************************
                    invoke  ExitProcess,0
Main                ENDP
                    END     Main