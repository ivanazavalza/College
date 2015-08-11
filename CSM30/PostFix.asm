;************************************************************
;  Jared Fowler
;  CS M30 Fall 2013 
;  Project 3
;
;  PostFix   V_1.00
;
;  Purpose - Intake PostFix Expression, Evaluate it, Display answer
;               -Will be evaluating a 32bit signed interger
;                   -Division will result in an int
;                   -Output will be in decimal notation, not hex
;
;  -PostFix Arithmatic
;       -Program will ask user for a postfix expression
;           -Store expression into byte array
;           -If nothing entered, jump to end of code
;           p through entire byte array pusing and popping from stack
;           -Test if byte is operator - If so, pop 2 bytes off stack and do operation
;               -Push result onto the stack
;               -If pop goes beyond stack start, jump to InvalidPFError and end program
;           -Test if byte is valid value (0 - 9)
;               -If valid, convert to 32bit and push onto stack, otherwise, jump to InvalidPFError and end program
;           -Test to see whether all UserInput has been processed
;       -If number of item on stack > 1, then jump to InvalidPFError and end program
;           -Otherwise, pop remaining value, translate into decimal and print to console for display
;           
;************************************************************


        .586
        .MODEL flat, stdcall

include Win32API.asm

        .STACK	4096

;-------------------------------------------------------------------------------------------
;DATA
;-------------------------------------------------------------------------------------------

                    .DATA


; EQUATES ----------------------------------------------------------------------------------

CR                  EQU         0Dh
LF                  EQU         0Ah

SUM                 EQU         2Bh     ; + operator
DIFFERENCE          EQU         2Dh     ; - operator
DIVISION            EQU         2Fh     ; / operator
MULTIPY             EQU         2Ah     ; * operator

MAX_LENGTH          EQU         255     ;max chars user can input
MAX_SIGS            EQU         255     ;max chars allowed in answer

ZERO                EQU         30h     ; char 0
NINE                EQU         39h     ; char 9

;VARIABLES ---------------------------------------------------------------------------------

errorInvalidPF      byte        "ERROR: The entered PostFix expression is invalid", CR, LF
errorDivideZero     byte        "ERROR: Can't divide by zero", CR, LF

UserInput           byte        MAX_LENGTH dup(0)
msg1                byte        "Please input a valid PostFix Expression: "
msg2                byte        "                                 Answer: "
msg3                byte        CR, LF          ;this just takes us down to the next line

CharsRead           dword       0
BytesWritten        dword       0

hStdOut             dword       0
hStdIn              dword       0

NumStack            word        0               ;keeps track of number of items pushed onto the stack
NumInput            word        0               ;keeps track of number of elements read from UserInput

Answer              byte        MAX_SIGS dup(0) ;signed int, limited to 20 sig figs
LengthOfAns         dword       0               ;holds how many chars hav
IsNegative          byte        0               ;0 is false, 1 is true

;-------------------------------------------------------------------------------------------
;CODE
;-------------------------------------------------------------------------------------------

                    .CODE

Main                Proc

                    ;The following code was taken from StringReverse. I did not write this code.

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

                    ;End of what I took from StringReverse


NoCR:
                    cmp     CharsRead,0             ;If user inputs nothing
                    je      TheEnd                  ;jump to the end


                    mov     esi,offset UserInput    ;have esi point to UserInput

;**************************************************************************************************
;PushNPop
;
;This will loop through the UserInput, check for invalid input, and call the appropriate functions.
;This function ends when an error is found, or when NumInput == CharsRead
;
;**************************************************************************************************
PushNPop:               

    
                    ;****************************************************************
                    ;Have we gone through all the UserInput?
                    ;****************************************************************
                    movzx   eax,NumInput            ;copy number of items read from UserInput
                    cmp     eax,CharsRead           ;compare to number of total # of items in UserInput
                    jz      FinalTest               ;if equal, then jump to final validity test
                    ;****************************************************************

                    mov     bl,byte ptr [esi][eax]  ;store value into bl
                    inc     NumInput                ;update offset

                    ;****************************************************************
                    ;Test if byte is operator
                    ;****************************************************************
                    cmp     bl,SUM                  ; if bl == SUM
                    jz      AddFun                  ; ...jump to add function

                    cmp     bl,DIFFERENCE           ; if bl == DIFFERENCE
                    jz      SubFun                  ; ...jump to subtract function

                    cmp     bl,DIVISION             ; if bl == DIVISION
                    jz      DivFun                  ; ...jump to division function

                    cmp     bl,MULTIPY              ; if bl == MULTIPLY
                    jz      MULFUN                  ; ...jump to multiply funciton
                    ;****************************************************************


                    ;****************************************************************
                    ;Test if byte is valid
                    ;****************************************************************
                    cmp     bl,ZERO                 ;if bl < 0
                    jb      InvalidPFError          ; ...jump to error code

                    cmp     bl,NINE                 ;if bl > 9
                    ja      InvalidPFError          ; ...jump to error code
                    ;****************************************************************


                    ;****************************************************************
                    ;((Byte != operator) && (0 <= Byte <= 9))  therefore, add it to stack
                    ;****************************************************************
                    sub     bl,30h                  ;convert ASCII char to hex value
                    movzx   eax,bl                  ;convert byte to 32bit int
                    push    eax                     ;push int onto the stack
                    inc     NumStack                ;increment number of items on stack
                    ;****************************************************************

                    jmp     PushNPop

                    
;**************************************************************************************************
;AddFun
;
;Pops off two elements from the stack and adds them.  Pushes the result back onto the stack
;
;**************************************************************************************************
AddFun:

                    cmp     NumStack,2              ;are there at leat 2 elements on the stack?
                    jb      InvalidPFError          ;if not, jump to error code
                    pop     eax                     ;pop first signed int into eax
                    pop     ebx                     ;pop second signed int into ebx
                    add     eax,ebx                 ;add them, putting answer into eax
                    push    eax                     ;push eax back onto stack
                    dec     NumStack                ;decrement count by 1  :  InitialCount -2 + 1 == InitialCount -1
                    jmp     PushNPop                ;jump back to PushNPop


;**************************************************************************************************
;SubFun
;
;Pops off two elements from the stack and subtracts them.  Pushes the result back onto the stack
;
;**************************************************************************************************
SubFun:

                    cmp     NumStack,2              ;are there at leat 2 elements on the stack?
                    jb      InvalidPFError          ;if not, jump to error code
                    pop     ebx                     ;pop first signed int into ebx
                    pop     eax                     ;pop second signed int into eax
                    sub     eax,ebx                 ;subtract them, putting answer into eax
                    push    eax                     ;push eax back onto stack
                    dec     NumStack                ;decrement count by 1  :  InitialCount -2 + 1 == InitialCount -1
                    jmp     PushNPop                ;jump back to PushNPop


;**************************************************************************************************
;DivFun
;
;Pops off two elements from the stack and divides them.  Pushes the result back onto the stack
;Note: This will be int division, so we are not concerned about the remainder
;
;**************************************************************************************************
DivFun:

                    cmp     NumStack,2              ;are there at leat 2 elements on the stack?
                    jb      InvalidPFError          ;if not, jump to error code
                    pop     ebx                     ;pop first signed int into ebx
                    cmp     ebx,0                   ;is denominator 0?
                    je      DivideByZeroError       ;if so, jump to error and end program
                    pop     eax                     ;pop second signed int into eax
                    cdq                             ;this will sign extend eax into edx
                    idiv    ebx                     ;divide them, putting answer into eax
                    push    eax                     ;push eax back onto stack
                    dec     NumStack                ;decrement count by 1  :  InitialCount -2 + 1 == InitialCount -1
                    jmp     PushNPop                ;jump back to PushNPop


;**************************************************************************************************
;MulFun
;
;Pops off two elements from the stack and multiplies them.  Pushes the result back onto the stack
;
;**************************************************************************************************
MulFun:

                    cmp     NumStack,2              ;are there at leat 2 elements on the stack?
                    jb      InvalidPFError          ;if not, jump to error code
                    pop     ebx                     ;pop first signed int into ebx
                    pop     eax                     ;pop second signed int into eax
                    imul    eax,ebx                 ;multiply them, putting answer into eax
                    push    eax                     ;push eax back onto stack
                    dec     NumStack                ;decrement count by 1  :  InitialCount -2 + 1 == InitialCount -1
                    jmp     PushNPop                ;jump back to PushNPop


;**************************************************************************************************
;FinalTest
;
;This function will test if only 1 element is left on the stack (the fianl answer).  If this is
;the case, it will jump to the Display function, otherwise, it will jump to the error message 
;
;**************************************************************************************************
FinalTest:

                    cmp     NumStack,1              ;is there only 1 item left on stack?
                    jz      Display                 ;if true, jump to display
                    jnz     InvalidPFError          ;else, jump to error code


;**************************************************************************************************
;Display
;
;This function will print the last element popped off the stack
;
;**************************************************************************************************
Display:
                    
                    pop     ebx                     ;pop value into ebx
                    dec     NumStack                ;decrement number of elements on stack
                    call    HexToDec                ;convert hex value to decimal and store value in Answer variable

                    invoke  WriteConsoleA, hStdOut, OFFSET msg3, SIZEOF msg3, OFFSET BytesWritten, 0                    ;end line
                    invoke  WriteConsoleA, hStdOut, OFFSET msg2, SIZEOF msg2, OFFSET BytesWritten, 0                    ;print to screen
                    invoke  WriteConsoleA, hStdOut, OFFSET Answer, LengthOfAns, OFFSET BytesWritten, 0                  ;print to screen
                    invoke  WriteConsoleA, hStdOut, OFFSET msg3, SIZEOF msg3, OFFSET BytesWritten, 0                    ;end line

                    jmp     TheEnd                  ;jump over error message


;**************************************************************************************************
;InvalidPFError   -   Invalid Post Fix Error
;
;Will be called when invalid char is read, if trying to pop from a stack with height 0, or if 
;there is more than 1 element left on stack at the end of evaluating UserInput
;
;**************************************************************************************************
InvalidPFError:         

                    invoke  WriteConsoleA, hStdOut, OFFSET msg3, SIZEOF msg3, OFFSET BytesWritten, 0                        ;end line
                    invoke  WriteConsoleA, hStdOut, OFFSET errorInvalidPF, SIZEOF errorInvalidPF, OFFSET BytesWritten, 0    ;output error message
                    jmp     TheEnd                                                                                          ;jump to end of code


;**************************************************************************************************
;DivideByZeroError
;
;Will be called when denominator is equal to 0 in the divide function
;
;**************************************************************************************************
DivideByZeroError:  
                    
                    invoke  WriteConsoleA, hStdOut, OFFSET msg3, SIZEOF msg3, OFFSET BytesWritten, 0                        ;end line
                    invoke  WriteConsoleA, hStdOut, OFFSET errorDivideZero, SIZEOF errorDivideZero, OFFSET BytesWritten, 0  ;output error message
                    jmp     TheEnd                                                                                          ;jump to end of code


;**************************************************************************************************
;TheEnd
;
;                       Hip-Hip-Horray!!!!!!
;
;**************************************************************************************************
TheEnd:
                    invoke  WriteConsoleA, hStdOut, OFFSET msg3, SIZEOF msg3, OFFSET BytesWritten, 0                        ;end line

                    ;*************************
                    ;Terminate Program
                    ;*************************
                    invoke  ExitProcess,0
Main                ENDP


;**************************************************************************************************
;HexToDec Procedure
;
;Will convert value stored in ebx to a decimal value and store that value in variable Answer
;
;**************************************************************************************************
HexToDec            Proc


                    Local   var1:dword              ;variable to hold 10t
                    mov     var1,10t                ;Mod will contain value 10t
                    mov     ecx,0                   ;have ecx keep count of items pushed onto stack
                    mov     edi,0                   ;have edi keep track of items written to answer
                    mov     esi,offset Answer       ;have esi point to Answer
                    

                    ;****************************************************************
                    ;If value is negative, set IsNegative variable to true, and then 
                    ;take the 2's compliment of the value in ebx
                    ;****************************************************************
                    mov     eax,ebx                 ;make a copy of the original value
                    and     eax,80h                 ;if positive eax == 00h, if negative eax == 80h
                    cmp     eax,0                   ;is eax negative?
                    jz      HexToDecCont0           ;if not so, jump to rest of code
                    mov     IsNegative,1            ;set IsNegative to true
                    neg     ebx                     ;take 2's compliment of ebx
                    ;****************************************************************

HexToDecCont0:

                    mov     eax,ebx                 ;put into eax for division algorithm	
                                    

HexToDecCont1:

                    ;****************************************************************
                    ;By taking mod(10) of eax, we can translate its value into decimal.
                    ;The remainder is taken as the dec value, and the qotient is
                    ;moded again until q = 0.   These values will be pushed onto the stack
                    ;to later be popped off in most significant order
                    ;****************************************************************
                    mov     edx,0                   ;clear edx for division
                    inc     ecx                     ;update ecx
                    div     var1                    ;divide by 10hex
                    add     edx,30h                 ;convert remainder into ascii
                    push    dx                      ;push value onto stack (byte value)
                    cmp     eax,0                   ;is the quotient = 0?
                    jnz     HexToDecCont1           ;if not, repeat the proccess


                    cmp     IsNegative,1            ;is the value negative?
                    jnz     HexToDecCont2           ;if not jump
                    mov     byte ptr [esi],'-'      ;otherwise, put - char into first elelment
                    inc     edi                     ;update edi


HexToDecCont2:

                    ;****************************************************************
                    ;Now that everything is on the stack, lets pop them off into the 
                    ;Answer array.  We will be checking to make sure we don't write
                    ;past the end of the array by using the equate MAX_SIGS
                    ;****************************************************************
                    cmp     edi,MAX_SIGS            ;does the answer contain the max elements?
                    jz      FullAnswer              ;if so, jump out
                    pop     dx                      ;pop off first element
                    mov     [esi][edi],dl           ;move element into answer
                    inc     edi                     ;update number of items in answer
                    
                    loopd   HexToDecCont2           ;loop back, decramenting ecx

                    jmp     HexToDecDone            ;jump over FullAnswer



FullAnswer:

                    ;****************************************************************
                    ;If we are here it is because the max amount of characters have 
                    ;been added to the Answer array and we need to fix the stack before
                    ;returning to the calling procedure
                    ;****************************************************************
                    add     esp,ecx                 ;subtract what's left from the stack


HexToDecDone:
                    ;****************************************************************
                    ;Update Variables and return
                    ;****************************************************************
                    mov     LengthOfAns,edi
                    ret


HexToDec            ENDP
    

                    END     Main