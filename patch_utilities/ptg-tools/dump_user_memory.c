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
#include <math.h>

int getVal(char c);
void remove_char_from_string(char c, char *src, char *dst);
void replace_char_from_string(char from, char to, char *str);
char *str_replace(char *orig, char *rep, char *with);
double convertToFloat(unsigned char *rowBuffer, int isDisplay);

int main(int argc, char **argv)
{
    FILE* fidBin;
    FILE* fidOpCodes;
    FILE* fidPatchedBin;
       
    int USER_MEM_SPACE_SIZE_IN_BYTES = 2048;  
    int REGISTER_SIZE = 8;
    int NUM_TECHNICAL_REGISTER_BOTTOM = 2;

    int MAX_BYTES_HP15C_CE = 672;
    int MAX_BYTES_HP15C_CE_EXPANDED = 999;

    int numBytes;

    unsigned char bufferBin[USER_MEM_SPACE_SIZE_IN_BYTES];
    char bufferBytesAsText[USER_MEM_SPACE_SIZE_IN_BYTES];
    char bufferBytesAsTextParsed[USER_MEM_SPACE_SIZE_IN_BYTES];
    char bufferBytesBinary[USER_MEM_SPACE_SIZE_IN_BYTES];

    int idx;
    int val;
    int currByte = 0;
    int registerOffset = REGISTER_SIZE;

    char CARRIAGE_RETURN = 0xa;
    char NEWLINE = 0xd;
    char SPACE = 0x20;

    //printf("%d\n",argc);
    if (argc == 2)
    {
        printf("Input: %s %s\n",argv[0], argv[1]);
        fidBin = fopen(argv[1] , "r");
    }
    else    
    {
        printf("Only one argument is allowed.  Namely, the HP15C-CE image file retrieved via \"VoyagerSave -d dump.mem\" \n");
        return -1;
    }


    if (fidBin == NULL) perror ("Error opening file");
    else
    {
        // Read user image and program opcodes as ascii text
        fread((void *)bufferBin,1,USER_MEM_SPACE_SIZE_IN_BYTES,fidBin);
        int rowNum = 0;
        unsigned char rowBuffer[8];
        for(int k = 0; k < USER_MEM_SPACE_SIZE_IN_BYTES; k+=8)
        {
            printf("0x%04x: ",k);
            for (int j = 0; j<8; j++)
            {
                printf("%02x ",bufferBin[k+j]);
                rowBuffer[j] = bufferBin[k+j];
            }
            printf("\n");
            if (rowNum == 14)
            {
                printf("y-register: %g\n",convertToFloat(&rowBuffer[0],0));
            }
            if (rowNum == 15)
            {
                printf("z-register: %g\n",convertToFloat(&rowBuffer[0],0));
            }
            if (rowNum == 16)
            {
                printf("t-register: %g\n",convertToFloat(&rowBuffer[0],0));
            }
            if (rowNum == 18)
            {
                printf("x-register: %g\n",convertToFloat(&rowBuffer[0],1));
            }
            if (rowNum == 30)
            {
                printf("R0-register: %g\n",convertToFloat(&rowBuffer[0],0));
            }
            if (rowNum == 31)
            {
                printf("R1-register: %g\n",convertToFloat(&rowBuffer[0],0));
            }            
            rowNum++;
        }
        printf("\n");
        // Clean up
        
        fclose (fidBin);

    }
    return 0;
}

double convertToFloat(unsigned char *rowBuffer, int isDisplay)
{
    double BCD = 0;

    if (!isDisplay)
    {      
        BCD+=(double)(rowBuffer[6]&0x0f) *1.0;
        BCD+=(double)(rowBuffer[5]/16)   *0.1;
        BCD+=(double)(rowBuffer[5]&0x0f) *0.01;
        BCD+=(double)(rowBuffer[4]/16)   *0.001; 
        BCD+=(double)(rowBuffer[4]&0x0f) *0.0001;
        BCD+=(double)(rowBuffer[3]/16)   *0.00001; 
        BCD+=(double)(rowBuffer[3]&0x0f) *0.000001;
        BCD+=(double)(rowBuffer[2]/16)   *0.0000001;
        BCD+=(double)(rowBuffer[2]&0x0f) *0.00000001;
        BCD+=(double)(rowBuffer[1]/16)   *0.000000001;

        if ((double)(rowBuffer[1]&0x0f) == 9.0)
        {
            BCD*=pow(10,(double)((rowBuffer[0]/16)*10 + (double)(rowBuffer[0]&0x0f))-100.0);
        }   
        else
        {          
            BCD*=pow(10,(double)((rowBuffer[0]/16)*10 + (double)(rowBuffer[0]&0x0f)));
        }

        if (rowBuffer[6]/16 == 0x9)
        {
            BCD *= -1.0;
        }               
    }
    else
    {
        if ((unsigned int)(rowBuffer[6]&0x0f) != 0xf)
           BCD+=((double)(rowBuffer[6]&0x0f) * 1000000); 
        printf("%g ",BCD);
        if ((unsigned int)(rowBuffer[5]/16) != 0xf)
           BCD+=((double)(rowBuffer[5]/16) * 100000);
        printf("%g ",BCD);
        if ((unsigned int)(rowBuffer[5]&0x0f) != 0xf)
           BCD+=((double)(rowBuffer[5]&0x0f) * 10000);
        printf("%g ",BCD);
        if ((unsigned int)(rowBuffer[4]/16) != 0xf)
           BCD+=((double)(rowBuffer[4]/16) * 1000); 
        printf("%g ",BCD);
        if ((unsigned int)(rowBuffer[4]&0x0f) != 0xf)
           BCD+=((double)(rowBuffer[4]&0x0f) * 100);
        printf("%g ",BCD);
        if ((unsigned int)(rowBuffer[3]/16) != 0xf)
           BCD+=((double)(rowBuffer[3]/16) * 10);
        printf("%g ",BCD);
        if ((unsigned int)(rowBuffer[3]&0x0f) != 0xf)
           BCD+=((double)(rowBuffer[3]&0x0f) * 1);
        printf("%g ",BCD);
        
        double SIGN = (double)(rowBuffer[2]/16  == 0 ? +1 : -1);
        
        double EXP =((double)(rowBuffer[2]&0x0f) * 10.0 + (double)rowBuffer[1]/16);

        BCD *= pow(10,SIGN*EXP);
        BCD *= pow(10,(double)(rowBuffer[6]/16)-7);

 
    }


    return BCD;


}