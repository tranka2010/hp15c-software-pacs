// Basic utility to take a compiled program in hex (bytes)
// and write the program(s) into user memory
//
// Pepin Torres P.E. 08/04/2024
//
// Inputs:
// patch.txt - this file has the compiled program one byte per line
// factory_reset_memory_dump_0A0Ah.bin - this file is a clean memory dump from a fresh reset (assuming image with CHKSUM 0A0Ah)
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int getVal(char c);
void remove_char_from_string(char c, char *src, char *dst);
void replace_char_from_string(char from, char to, char *str);

int main()
{
    FILE* fidBin;
    FILE* fidOpCodes;
    FILE* fidPatchedBin;
       
    int USER_MEM_SPACE_SIZE_IN_BYTES = 2048;  
    int REGISTER_SIZE = 8;
    int NUM_TECHNICAL_REGISTER_BOTTOM = 2;

    int num_opcodes;

    char bufferBin[USER_MEM_SPACE_SIZE_IN_BYTES];
    char bufferOpCodeText[USER_MEM_SPACE_SIZE_IN_BYTES];
    char bufferOpCodeTextParsed[USER_MEM_SPACE_SIZE_IN_BYTES];
    char bufferOpCodeBinary[USER_MEM_SPACE_SIZE_IN_BYTES];

    int idx;
    int val;
    int currOpCode;
    int registerOffset = REGISTER_SIZE;

    char CARRIAGE_RETURN = 0xa;
    char NEWLINE = 0xd;
    char SPACE = 0x20;

    fidBin = fopen ("preconditioned_image_for_patching.bin" , "r");
    fidOpCodes = fopen ("../engineering_basics/audio_eq_cookbook.hex" , "r");
    fidPatchedBin = fopen ("../engineering_basics/audio_eq_cookbook.bin" , "w");

    if (fidBin == NULL) perror ("Error opening file");
    else
    {
        // Read user image and program opcodes as ascii text
        fread((void *)bufferBin,1,USER_MEM_SPACE_SIZE_IN_BYTES,fidBin);
        fread((void *)bufferOpCodeText,1,USER_MEM_SPACE_SIZE_IN_BYTES,fidOpCodes);

        // Remove all non-alpha numerical ascii characters and store in a buffer
        replace_char_from_string(CARRIAGE_RETURN, SPACE, bufferOpCodeText);
        replace_char_from_string(NEWLINE, SPACE, bufferOpCodeText);
        remove_char_from_string(SPACE, bufferOpCodeText,bufferOpCodeTextParsed);

        // Convert ASCII hex values into numbers (basically atoi)
        idx=0;
        for (int i = 0; i < strlen(bufferOpCodeTextParsed); i=i+2)
        {
            bufferOpCodeBinary[idx] = getVal(bufferOpCodeTextParsed[i]) * 16 + 
                                      getVal(bufferOpCodeTextParsed[i+1]);
            idx++;
        }

        // Number of instructions
        num_opcodes = idx;
        printf("Num opcodes: %d\n",num_opcodes);

        // Copy program binary data into user image starting from
        // the 3rd 8-byte register from the bottom and work the way up
        // right to left
        // 
        //  Top of user image
        //        :
        //  Reg 8, Bytes 1-8  | Reg 7, Bytes 1-8
        //  Reg 6, Bytes 1-8  | Reg 5, Bytes 1-8
        //  Reg 4, Bytes 1-8  | Reg 3, Bytes 1-8 <-first 7 opcodes here
        //  Reg 2, Bytes 1-8  | Reg 1, Bytes 1-8
        // Bottom of user image
        // Note: the 8th byte of each register from Reg 3 and up need to be 0x00
        // so each register only has 7 bytes of actual program data
        while(currOpCode < num_opcodes)
        {      
            for (int currRegByte = 0; currRegByte < REGISTER_SIZE-1; currRegByte++)
            {
                bufferBin[USER_MEM_SPACE_SIZE_IN_BYTES - 
                        (NUM_TECHNICAL_REGISTER_BOTTOM * REGISTER_SIZE)-registerOffset + currRegByte] = 
                        bufferOpCodeBinary[currOpCode]; 
                currOpCode++;
                if (currOpCode == num_opcodes) break;
            }
            registerOffset += REGISTER_SIZE;
        }

        // Write patched user image to file
        fwrite((void *)bufferBin,1,USER_MEM_SPACE_SIZE_IN_BYTES,fidPatchedBin);

        // Clean up
        fclose (fidBin);
        fclose (fidOpCodes);
        fclose (fidPatchedBin);
    }
    return 0;
}

   int getVal(char c)
   {
       int rtVal = 0;

       if(c >= '0' && c <= '9')
       {
           rtVal = c - '0';
       }
       else
       {
           rtVal = c - 'a' + 10;
       }

       return rtVal;
   }


void remove_char_from_string(char c, char *src, char *dst)
{
    int i=0;
    int len = strlen(src)+1;
    int idx = 0;
    for(i=0; i<len; i++)
    {
        if(src[i] != c)
        {
            // Move all the char following the char "c" by one to the left.
            strncpy(&dst[idx],&src[i],1);
            idx++;
        }
    }
}

void replace_char_from_string(char from, char to, char *str)
{
    int i = 0;
    int len = strlen(str)+1;

    for(i=0; i<len; i++)
    {
        if(str[i] == from)
        {
            str[i] = to;
        }
    }
}