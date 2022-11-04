/*
CIS 600 Computer Architecture Fall 2022 Project 1 - MIPS Assembler
Authors: Mohamed Gani Mohamed Sulthan- 2811619, Sabareeswaran Shanmugam - 2796495

Input : Run the code with argument asm file Example(./assembler.c data2.asm)
Output: It will create the bin file with the data2.bin name

Testcases Achieved:
data2.asm
directive2.asm
rtype.asm
parsing.asm
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

void binary(unsigned n)
{
    unsigned i;
    printf("Binary representation:");
    for (i = 1 << 5; i > 0; i = i / 2)
        (n & i) ? printf("1") : printf("0");
}

static const char *const reg[] = {
    // $0     $1        $2       $3     $4     $5      $6       $7     $8       $9     $10
    "00000", "00001", "00010", "00011", "00100", "00101", "00110", "00111", "01000", "01001", "01010",
    // $11     $12       $13     $14     $15    $16     $17     $18     $19     $20    $21
    "01011", "01100", "01101", "01110", "01111", "10000", "10001", "10010", "10011", "10100", "10101",
    // $22     $23       $24    $25     $26     $27     $28     $29     $30     $31
    "10110", "10111", "11000", "11001", "11010", "11011", "11100", "11101", "11110", "11111"};

static const char *const opcode[] = {
    // ADD
    "000000"};

static const char *const funct_code[] = {
    // 6 bit
    // ADD    // SUB
    "100000", "100010"};

static const char *const sham_code[] = {
    // 5 bit
    // ADD
    "00000"};

typedef struct bin1
{
    uint32_t machine_code_data[128];
} bin1;

typedef struct bin2
{
    int32_t *machine_code_text;
} bin2;

int main(int argc, char *argv[])
{
    FILE *fp;
    FILE *fo;

    char line[100];
    char *token = NULL;
    char *op1, *op2, *op3, *word, *label;
    char ch;
    char dest[128][33] = {{0}};
    char word_dest[128][11] = {{0}};
    char *dest_pt; // pointer to the result string
    char *word_pt;
    int program[1000];
    int counter = 0; // holds the address of the machine code instruction
    int inst = 0;
    int word_c = 0;
    char *writeFile; // Opening the file to write

    printf("Total number of argument passed: %d\n", argc);

    // open source file in read mode
    if ((fp = fopen(argv[1], "r")) == NULL)
    {
        printf("Error...\nCannot open file: %s\n", argv[1]);
        printf("Specify the file name.\n");
        return -1;
    }
    else if ((fp = fopen(argv[1], "r")) != NULL)
    {
        char *testvar = argv[1];
        printf("First argument %s\n", testvar);

        // Creating token to write output file
        const char param[] = ".";
        writeFile = strtok(testvar, param);

        //  second pass
        while (fgets(line, sizeof line, fp) != NULL)
        {
            token = strtok(line, ",\n\t "); // get the instruction mnemonic or label
            while (token)
            {
                if (strcmp(token, ".word") == 0) //----------------- ADD word data to .data section -------------------------------
                {
                    word = strtok(NULL, "\t\r ");
                    strcat(word_dest[word_c], word);
                    word_c++;
                }

                if (strcmp(token, "add") == 0) //----------------- ADD -------------------------------
                {
                    op1 = strtok(NULL, "$,\n\r ");
                    int rd = atoi(op1);

                    op2 = strtok(NULL, "$,\n\r ");
                    int rs = atoi(op2);

                    op3 = strtok(NULL, "$,\n\r ");
                    int rt = atoi(op3);

                    strcat(dest[inst], opcode[0]);     // ADD -opcode
                    strcat(dest[inst], reg[rs]);       // rs 5 bit
                    strcat(dest[inst], reg[rt]);       // rt 5 bits
                    strcat(dest[inst], reg[rd]);       // rd 5 bits
                    strcat(dest[inst], sham_code[0]);  // shamt
                    strcat(dest[inst], funct_code[0]); // function code
                    inst++;
                }

                if (strcmp(token, "sub") == 0) //----------------- SUB -------------------------------
                {
                    op1 = strtok(NULL, "$,\n\r ");
                    int rd = atoi(op1);

                    op2 = strtok(NULL, "$,\n\r ");
                    int rs = atoi(op2);

                    op3 = strtok(NULL, "$,\n\r ");
                    int rt = atoi(op3);

                    strcat(dest[inst], opcode[0]);     // ADD -opcode
                    strcat(dest[inst], reg[rs]);       // rs 5 bit
                    strcat(dest[inst], reg[rt]);       // rt 5 bits
                    strcat(dest[inst], reg[rd]);       // rd 5 bits
                    strcat(dest[inst], sham_code[0]);  // shamt
                    strcat(dest[inst], funct_code[1]); // function code
                    inst++;
                }
                token = strtok(NULL, ",\n\t");
            }
        }
    }

    bin1 b1;
    bin2 b2;
    // .text segment starts at 0x0200  and dynamically memory allocated maximum 128 * 4 bytes = 512 bytes
    b2.machine_code_text = (int32_t *)malloc(inst * sizeof(int32_t));
    if (b2.machine_code_text == NULL)
    {
        printf("Memory not allocated.\n");
    }

    else
    {
        for (int i = 0; i < word_c; i++)
        {
            b1.machine_code_data[i] = strtol(word_dest[i], &word_pt, 10);
        }

        for (int i = 0; i < inst; i++)
        {
            b2.machine_code_text[i] = strtol(dest[i], &dest_pt, 2);
        }
    }

    /// @brief .data section byte order reversed and write it in big-endian format irrespective of os
    unsigned char value_data[128][4] = {0}; // unallocated will be zero
    int count_word = 0;
    for (int i = 0; i < word_c; i++)
    {
        value_data[count_word][0] = (uint32_t)b1.machine_code_data[i] >> 24;
        value_data[count_word][1] = (uint32_t)b1.machine_code_data[i] >> 16;
        value_data[count_word][2] = (uint32_t)b1.machine_code_data[i] >> 8;
        value_data[count_word][3] = (uint32_t)b1.machine_code_data[i];
        count_word++;
    }

    /// @brief .text section byte order reversed and write it in big-endian format irrespective of os
    unsigned char value[inst][4];
    int count_inst = 0;
    for (int i = 0; i < inst; i++)
    {

        value[count_inst][0] = (uint32_t)b2.machine_code_text[i] >> 24;
        value[count_inst][1] = (uint32_t)b2.machine_code_text[i] >> 16;
        value[count_inst][2] = (uint32_t)b2.machine_code_text[i] >> 8;
        value[count_inst][3] = (uint32_t)b2.machine_code_text[i];
        count_inst++;
    }
    
    // Opening a file to write the output
    strcat(writeFile, ".bin");
    printf("Outfile Created: %s", writeFile);
    fo = fopen(writeFile, "wb");
    if (fo == NULL)
    {
        printf("Error opening output file");
    }

    // dump .data section
    size_t elts_written_data = fwrite(value_data, sizeof(value_data), 1, fo);
    // dump .text section
    size_t elts_written_text = fwrite(value, sizeof(value), 1, fo);

    if (elts_written_text == 0)
    {
        printf("binary file not written");
    }

    fclose(fp);
    fclose(fo);

    printf("\nDone!!\n");

    return 0;
}
