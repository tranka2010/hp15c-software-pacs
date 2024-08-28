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
char *str_replace(char *orig, char *rep, char *with);

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

    char bufferBin[USER_MEM_SPACE_SIZE_IN_BYTES];
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
    if (argc == 3)
    {
        printf("Input: %s %s\n",argv[0], argv[1]);
        fidOpCodes = fopen(argv[1] , "r");
        fidBin = fopen(argv[2] , "r");
        fidPatchedBin = fopen(str_replace(argv[1],".hex",".bin"), "w");
    }
    if (argc == 1)
    {
        printf("Only two argument is allowed.  Namely, the .hex file containing the values copied into an HP15C-CE image file and a template .bin file.\n");
        return -1;
    }


    if (fidBin == NULL) perror ("Error opening file");
    else
    {
        // Read user image and program opcodes as ascii text
        fread((void *)bufferBin,1,USER_MEM_SPACE_SIZE_IN_BYTES,fidBin);
        fread((void *)bufferBytesAsText,1,USER_MEM_SPACE_SIZE_IN_BYTES,fidOpCodes);

        // Remove all non-alpha numerical ascii characters and store in a buffer
        replace_char_from_string(CARRIAGE_RETURN, SPACE, bufferBytesAsText);
        replace_char_from_string(NEWLINE, SPACE, bufferBytesAsText);
        remove_char_from_string(SPACE, bufferBytesAsText,bufferBytesAsTextParsed);

        // Convert ASCII hex values into numbers (basically atoi)
        idx=0;
        for (int i = 0; i < strlen(bufferBytesAsTextParsed); i=i+2)
        {
            bufferBytesBinary[idx] = getVal(bufferBytesAsTextParsed[i]) * 16 + 
                                      getVal(bufferBytesAsTextParsed[i+1]);
            idx++;
        }

        // Number of bytes
        numBytes = idx;
        printf("Num bytes: %d of 672 (%.2f%%)\n",numBytes, 100.0*(float)numBytes/(float)MAX_BYTES_HP15C_CE);

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
        while(currByte < numBytes)
        {      
            for (int currRegByte = 0; currRegByte < REGISTER_SIZE-1; currRegByte++)
            {
                bufferBin[USER_MEM_SPACE_SIZE_IN_BYTES - 
                        (NUM_TECHNICAL_REGISTER_BOTTOM * REGISTER_SIZE)-registerOffset + currRegByte] = 
                        bufferBytesBinary[currByte]; 
                currByte++;
                if (currByte == numBytes) break;
            }
            registerOffset += REGISTER_SIZE;
        }

        // Write patched user image to file
        fwrite((void *)bufferBin,1,USER_MEM_SPACE_SIZE_IN_BYTES,fidPatchedBin);
        printf("Output: %s\n", str_replace(argv[1],".hex",".bin"));

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
char *str_replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep (the string to remove)
    int len_with; // length of with (the string to replace rep with)
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}