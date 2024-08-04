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
// Open bin file for writing
int ascii_to_hex(char c);
   int getVal(char c);

/* FEOF example */

int main()
{
    FILE* fidBin;
    FILE* fidOpCodes;
    FILE* fidPatchedBin;
       
    int USER_MEM_SPACE_SIZE_IN_BYTES = 2048;  
    int REGISTER_SIZE = 8;
    int NUM_TECHNICAL_REGISTER_BOTTOM = 2;

    int num_opcodes;

    unsigned char bufferBin[USER_MEM_SPACE_SIZE_IN_BYTES];
    unsigned char bufferOp[USER_MEM_SPACE_SIZE_IN_BYTES];
    int idx = 0;
    char c;
    int val;

    fidBin = fopen ("basic_prog2.bin" , "r");
    fidOpCodes = fopen ("patches.txt" , "r");
    fidPatchedBin = fopen ("patched.bin" , "w");

    if (fidBin == NULL) perror ("Error opening file");
    else
    {

        fread((void *)bufferBin,1,USER_MEM_SPACE_SIZE_IN_BYTES,fidBin);

        idx = 0;
        while((c = fgetc(fidOpCodes)) != EOF)
        {
            bufferOp[idx++] = getVal(c) * 16 + getVal(fgetc(fidOpCodes));
            c = fgetc(fidOpCodes); // \r
            c = fgetc(fidOpCodes); // \n
        }
        num_opcodes = idx;
        printf("Num opcodes: %d\n",num_opcodes);

        idx=0;
        int i = 0;
        int registerOffset = REGISTER_SIZE;
        while(i < num_opcodes)
        {
            
            
            for (int j = 0; j < REGISTER_SIZE-1; j++)
            {
                bufferBin[USER_MEM_SPACE_SIZE_IN_BYTES - 
                        (NUM_TECHNICAL_REGISTER_BOTTOM * REGISTER_SIZE)-registerOffset + j] = bufferOp[i]; 
                        i++;
                        if (i == num_opcodes)
                            break;
            }
            registerOffset += REGISTER_SIZE;
        }

        for(int i = 0; i < USER_MEM_SPACE_SIZE_IN_BYTES; i++)
        printf("bufferBin[0x%02X] = 0x%02X\n",i,bufferBin[i]);

        fwrite((void *)bufferBin,1,USER_MEM_SPACE_SIZE_IN_BYTES,fidPatchedBin);

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

