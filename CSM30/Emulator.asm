;************************************************************
;  Jared Fowler
;  CS M30 Fall 2013 
;  Project 2
;
;  CPU Emulator.
;  -Contains 6 registers each 8 bits (R0-R5)
;  -1K of Ram
;  -All memory addresses are 2 bytes long and are stored in big endian
;  -CISC (Complex instruction set computing)
;  -Upon reset CPU begins execution at location 0
;
;************************************************************
       
       
        .586
        .MODEL flat, stdcall

include Win32API.asm

        .STACK 4096

        .DATA

;---------------------
; EQUATES 
;---------------------

MAX_FILE_NAME_LEN   EQU     256
MAX_RAM             EQU     1024

_CR                 EQU     0Dh
_LF                 EQU     0Ah

ADDFUN              EQU     11h
SUBFUN              EQU     22h
XORFUN              EQU     44h
LOADFUN             EQU     05h
LOADRFUN            EQU     55h
STOREFUN            EQU     06h
STORRFUN            EQU     66h
OUTFUN              EQU     0CCh
JNZFUN              EQU     0AAh
HALTFUN             EQU     0FFh

;---------------------
;variables
;---------------------

errorFileOpen       byte    "ERROR:  Unable to open input file", _CR, _LF

filename            byte    "c:\machine.bin", 0

ProgramBuffer       byte    MAX_RAM dup (0)        ;max size of RAM 1K

RetCode             dword   0

BytesWritten        dword   0
BytesRead           dword   0
FileHandle          dword   0
FileSize            dword   0

hStdOut             dword   0
hStdIn              dword   0

Reg                 byte    6 dup(0)                ;6 Registers Reg0,Reg1,Reg2,Reg3,Reg4,Reg5

toWrite             byte    0                       ;will be utilized when writing to the screen


        .CODE

Main    Proc

        ;*********************************
        ; Get Handle to Standard output
        ;*********************************
        invoke  GetStdHandle, STD_OUTPUT_HANDLE
        mov     hStdOut,eax

        ;*********************************
        ; Get Handle to Standard input
        ;*********************************
        invoke  GetStdHandle, STD_INPUT_HANDLE
        mov     hStdIn,eax

        ;*********************************
        ; Open existing file for Reading
        ;*********************************
        invoke  CreateFileA, offset filename, GENERIC_READ, FILE_SHARE_NONE,\
                             0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0
        cmp     eax,-1                  ;was open successful?
        je      OpenError               ;No....Display error and Exit
        mov     FileHandle,eax          ;Yes...then save file handle

        ;********************************************
        ; Determine the size of the file (in bytes)
        ;********************************************
        invoke  GetFileSize, FileHandle, 0
        mov     FileSize, eax

        ;****************************************
        ; Read the entire file into emulator RAM
        ;****************************************
        invoke  ReadFile, FileHandle, offset ProgramBuffer, FileSize, offset BytesRead, 0
        cmp     eax,0                   ;was it successful?
        je      Finish                  ;No...then Exit

        ;*********************************
        ; Close the file
        ;*********************************
        invoke  CloseHandle, FileHandle


;**************************************************************************************************
;IMPORTANT!!!!!!!   Some permenantly set registers
;**************************************************************************************************		
        mov     edi,offset ProgramBuffer    ;edi will always reference to ProgramBuffer
        mov     esi,offset Reg              ;esi will always reference to the 6 Registers (8 bit)
        mov     ebp,0                       ;ebp will always keep track of the offset in PrgramBuffer


;**************************************************************************************************
;Continue:  MAIN
;
;This function will read the next byte from ProgramBuffer.  It will then compare the value with
;all 10 of the possible functions until a successful comparison is made.  This function acts as
;a loop in a sense, but without using the ecx register.  This function does end with a jmp Finish,
;but will most likely crash if it doesn't eventually read a halt command (FFh)
;
;**************************************************************************************************	

CONTINUE:
        movzx   eax,byte ptr [edi][ebp] ;move next byte from ProgramBuffer into al
        inc     ebp                     ;update offset

        cmp     al,ADDFUN               ;command for function ADDITION?
        jz      ADDITION                ;if so, jump

        cmp     al,SUBFUN               ;command for function SUBTRACTION?
        jz      SUBTRACTION             ;if so, jump

        cmp     al,XORFUN               ;command for function EXOR?
        jz      EXOR                    ;if so, jump
        
        cmp     al,LOADFUN              ;command for function LOAD?
        jz      LOAD                    ;if so, jump

        cmp     al,LOADRFUN             ;command for function LOADR?
        jz      LOADR                   ;if so, jump

        cmp     al,STOREFUN             ;command for function STORE?
        jz      STORE                   ;if so, jump

        cmp     al,STORRFUN             ;command for function STORR?
        jz      STORR                   ;if so, jump

        cmp     al,OUTFUN               ;command for function PRINT?
        jz      PRINT                   ;if so, jump

        cmp     al,JNZFUN               ;command for function JUMPNOTZERO?
        jz      JUMPNOTZERO             ;if so, jump

        cmp     al,HALTFUN              ;command for function HALT?
        jz      HALT                    ;if so, jump
        

        jmp     Finish                  ;goTo the end of the code


;**************************************************************************************************
;ADDITION : ADD FUNCTION  (11h)
;                           R1 = R1 + R2
;
;This function will read the next two bytes in ProgramBuffer.  The first byte will represent the
;registor number of R1, and the second byte will represent the registor number of R2
;Note: R1,R2 are stand alone variables, and do not neccesarily mean Reg1,Reg2
;
;**************************************************************************************************

ADDITION:
        movzx   eax,byte ptr [edi][ebp]         ;byte to determine first register
        inc     ebp                             ;update current offset
        movzx   ebx,byte ptr [edi][ebp]         ;byte to determine second register
        inc     ebp                             ;update current offset
        movzx   edx,byte ptr [esi][ebx]         ;move value of R2 into dl
        add     byte ptr [esi][eax],dl          ;R1 = R1 + R2
        jmp     CONTINUE                        ;Back to Emulator
        

;**************************************************************************************************
;SUBTRACTION : SUB FUNCTION  (22h)
;                           R1 = R1 - R2
;
;This function will read the next two bytes in ProgramBuffer.  The first byte will represent the
;registor number of R1, and the second byte will represent the registor number of R2
;Note: R1,R2 are stand alone variables, and do not neccesarily mean Reg1,Reg2
;
;**************************************************************************************************

SUBTRACTION:
        movzx   eax,byte ptr [edi][ebp]         ;byte to determine first register
        inc     ebp                             ;update current offset
        movzx   ebx,byte ptr [edi][ebp]         ;byte to determine second register
        inc     ebp                             ;update current offset
        movzx   edx,byte ptr [esi][ebx]         ;move value of R2 into dl
        sub     byte ptr [esi][eax],dl          ;R1 = R1 - R2
        jmp     CONTINUE                        ;Back to Emulator


;**************************************************************************************************
;EXOR : XOR (Exclusive Or) FUNCTION  (44h)
;                           R1 = R1 XOR R2
;
;This function will read the next two bytes in ProgramBuffer.  The first byte will represent the
;registor number of R1, and the second byte will represent the registor number of R2
;Note: R1,R2 are stand alone variables, and do not neccesarily mean Reg1,Reg2
;
;**************************************************************************************************

EXOR:
        movzx   eax,byte ptr [edi][ebp]         ;byte to determine first register
        inc     ebp                             ;update current offset
        movzx   ebx,byte ptr [edi][ebp]         ;byte to determine second register
        inc     ebp                             ;update current offset
        movzx   edx,byte ptr [esi][ebx]         ;move value of R2 into dl
        xor     byte ptr [esi][eax],dl          ;R1 = R1 xor R2
        jmp     CONTINUE                        ;Back to Emulator


;**************************************************************************************************
;LOAD : LEA (Load Effective Address) FUNCTION  (05h)
;							Load R1 with Value at adress
;
;This function will read a total of 3 bytes in ProgramBuffer.  The first byte will represent the
;registor number of R1, and the next two will represent a 16bit memory address, located within
;ProgramBuffer.  This adress will be read one bit at a time b/c it is stored in memory as Big Endian
;
;**************************************************************************************************

LOAD:
        mov     ebx,0                           ;clear ebx
        movzx   eax,byte ptr [edi][ebp]         ;byte to determine first register
        inc     ebp                             ;update current offset
        mov     bh,byte ptr [edi][ebp]          ;put first part of address into bh
        inc     ebp                             ;update current offset
        mov     bl,byte ptr [edi][ebp]          ;put second part of address into bl
        inc     ebp                             ;update current offset
        movzx   ecx,byte ptr [edi][ebx]         ;get byte value from 16 bit address and put into cl
        mov     byte ptr [esi][eax],cl          ;put value into the correct register
        jmp     CONTINUE                        ;back to emulator


;**************************************************************************************************
;LOADR : LEA (Load Effective Address) FUNCTION  (55h)
;                           Load R1 with Value at (Adress + R1)
;
;This function will read a total of 3 bytes in ProgramBuffer.  The first byte will represent the
;registor number of R1, and the next two will represent a 16bit memory address, located within
;ProgramBuffer.  This adress will be read one bit at a time b/c it is stored in memory as Big Endian
;
;**************************************************************************************************

LOADR:
        mov     ebx,0                           ;clear ebx
        movzx   eax,byte ptr [edi][ebp]         ;byte to determine first register
        inc     ebp                             ;update current offset
        mov     bh,byte ptr [edi][ebp]          ;put first part of address into bh
        inc     ebp                             ;update current offset
        mov     bl,byte ptr [edi][ebp]          ;put second part of address into bl
        inc     ebp                             ;update current offset
        movzx   edx,byte ptr [esi][eax]         ;put value of R1 into dl
        add     ebx,edx                         ;(Adress + R1)
        movzx   ecx,byte ptr [edi][ebx]         ;get byte value from 16 bit address and put into cl. This does not inc offset
        mov     byte ptr [esi][eax],cl          ;put value into the correct register
        jmp     CONTINUE                        ;back to emulator


;**************************************************************************************************
;STORE :  (06h)
;                           Write Value in R0 to Address
;
;This function only takes in 2 bytes from ProgramBuffer. The 2 bytes represent a memory address to where
;the value contained in Reg 0, (this being the actual emulator register), is written to.  
;This adress will be read one bit at a time b/c it is stored in memory as Big Endian
;
;**************************************************************************************************

STORE:
        mov     eax,0                           ;clear eax
        mov     ah,byte ptr [edi][ebp]          ;put first part of address into ah
        inc     ebp                             ;update current offset
        mov     al,byte ptr [edi][ebp]          ;put second part of address into al
        inc     ebp                             ;update current offset
        mov     bl,byte ptr [esi]               ;copy value in R0 to bl
        mov     byte ptr [edi][eax],bl          ;Write value in R0 to address
        jmp     CONTINUE                        ;back to emulator


;**************************************************************************************************
;STORR :  (66h)
;                           Write Value in R0 to (Address + R1)
;
;This function will take in 3 bytes from ProgramBuffer. The first byte will represent the 
;registor number of R1, and the next two will represent a 16bit memory address, located within
;ProgramBuffer.  This adress will be read one bit at a time b/c it is stored in memory as Big Endian
;Note that R0 will be the literal register, while R1 represents a variable register that will be determined.
;
;**************************************************************************************************

STORR:
        mov     ebx,0                           ;clear ebx
        movzx   eax,byte ptr [edi][ebp]         ;byte to determine first register
        inc     ebp                             ;update offset
        mov     bh,byte ptr [edi][ebp]          ;put first part of address into bh
        inc     ebp                             ;update offset
        mov     bl,byte ptr [edi][ebp]          ;put second part of address into bl
        inc     ebp                             ;update offset
        movzx   edx,byte ptr [esi]              ;store value of R0 into dl
        movzx   ecx,byte ptr [esi][eax]         ;put value of R1 into cl
        add     ebx,ecx                         ;update to new position
        mov     byte ptr [edi][ebx],dl          ;Write value in R0 to (Address + R1)
        jmp     CONTINUE                        ;back to emulator


;**************************************************************************************************
;PRINT :  OUT  (CCh)
;                           Send value in R1 to screen
;
;Function reads in one byte which will represent the register number of R1.  This value is then
;printed to the screen through Win32API
;
;**************************************************************************************************

PRINT:
        movzx   eax,byte ptr [edi][ebp]         ;byte to determine which register
        inc     ebp                             ;update offset
        movzx   ebx,byte ptr [esi][eax]         ;put value contained in R1 into bl
        mov     toWrite,bl                      ;now put this value into variable toWrite
        invoke  WriteConsoleA, hStdOut, OFFSET toWrite, SIZEOF toWrite, OFFSET BytesWritten, 0
        jmp     CONTINUE                        ;go back to emulator


;**************************************************************************************************
;JUMPNOTZERO :  JNZ  (AAh)
;                           If value in R1 isn't zero then next instruction is at Adress
;
;This function takes in a total of 3 bytes from ProgramBuffer. The first byte will represent the 
;registor number of R1, and the next two will represent a 16bit memory address, located within
;ProgramBuffer.  This adress will be read one bit at a time b/c it is stored in memory as Big Endian. 
;This function might dramatically update the current offset
;
;**************************************************************************************************

JUMPNOTZERO:
        movzx   eax,byte ptr [edi][ebp]         ;byte to determine which register
        inc     ebp                             ;update offset
        movzx   edx,byte ptr [esi][eax]         ;put value of R1 into dl
        cmp     dl,0                            ;is value of R1 == 0 ?

        jz      CONTINUE                        ;if so, go back to emulator
                                                ;else, continue below
        mov     ebx,0                           ;clear ebx
        mov     bh,byte ptr [edi][ebp]          ;put first part of address into bh
        inc     ebp                             ;update offset
        mov     bl,byte ptr [edi][ebp]          ;put second part of address into bl
        mov     ebp,ebx                         ;put the new offset into ebp
        jmp     CONTINUE                        ;back to emulator


;**************************************************************************************************
;HALT :   (FFh)
;                           CPU Halts - Terminate the Program
;
;This function doesn't read anything from ProgramBuffer.  It just ends the program!
;
;**************************************************************************************************

HALT:
        jmp     Finish



OpenError:
        invoke  WriteConsoleA, hStdOut, OFFSET errorFileOpen, SIZEOF errorFileOpen, OFFSET BytesWritten, 0
        jmp finish

Finish:
        ;Terminate Program
        invoke  ExitProcess, RetCode

Main    endp

        END Main