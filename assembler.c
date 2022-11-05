/*
 MIPS Assembler
Project source code: https://github.com/sabareeswarans11/mips_assembler
-----------------------------------------------------------------------------------
Input : Run the code with argument asm file Example(./assembler.c data2.asm)
Output: It will create the bin file with the data2.bin name

---------Testcases Achieved----------------------------------------------------------
1)data2.asm
2)directive2.asm
3)rtype.asm
4)parsing.asm
5)forwback.asm
6)test.asm // handout

----------------------------------------------------------------------------------------
Supported Instructions
 // ADD     //SUB      //LUI     //ORI     //LW     //SW     //BNE    //BEQ     //JUMP

Supported Directory
.data        .text          .word       .space
----------------------------------------------------------------------------------------
                                  Bin file
---------------------------------------------------------------------------------------
--------------.data(512 bytes)-------------------.text(512 bytes)----------------------
----------------------------------------------------------------------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

static const char *const reg[] = {
    // $0     $1        $2       $3     $4     $5      $6       $7     $8       $9     $10
    "00000", "00001", "00010", "00011", "00100", "00101", "00110", "00111", "01000", "01001", "01010",
    // $11     $12       $13     $14     $15    $16     $17     $18     $19     $20    $21
    "01011", "01100", "01101", "01110", "01111", "10000", "10001", "10010", "10011", "10100", "10101",
    // $22     $23       $24    $25     $26     $27     $28     $29     $30     $31
    "10110", "10111", "11000", "11001", "11010", "11011", "11100", "11101", "11110", "11111"};

static const char *const opcode[] = {
    // ADD     //SUB      //LUI     //ORI     //lw     //sw     //bne    //beq     //jump
    "000000", "000000", "001111", "001101", "100011", "101011", "000101", "000100", "000010"};

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

struct key_value
{
    int key;
    char *value;
} List;

char *string2int(int value, int base_digit)
{
    static char buffer[32] = {0};
    int i = 30;
    for (; value && i; --i, value /= base_digit)
        buffer[i] = "0123456789abcdef"[value % base_digit];
    return &buffer[i + 1];
}

char *stringtoBinary(int number, int length)
{
    char *binary = (char *)malloc(sizeof(char) * length);
    int k = 0;
    for (unsigned j = (1 << (length - 1)); j > 0; j = j / 2)
    {
        binary[k++] = (number & j) ? '1' : '0';
    }
    binary[k] = '\0';
    return binary;
}

int main(int argc, char *argv[])
{
    FILE *fp;
    FILE *fo;
    char line[100];
    char *token = NULL;
    char *op1, *op2, *op3, *word, *la, *label, *space;
    char ch;
    char dest[128][33] = {{0}};
    char word_dest[128][11] = {{0}};
    char *dest_pt;
    char *word_pt;
    int word_c = 0;
    int inst = 0;
    int word_count_inside = 0;
    int label_count_inside = 0;
    int space_count_inside = 0;
    int words_count_global = 0;
    int label_count_global = 0;
    char label_name[100][15];
    char label_address[100][15];
    char *writeFile; // Opening the file to write
    size_t bin_written_data;
    size_t bin_written_text;

    ///////////////////////////////////First- Pass///////////////////////////////////////////////////////////////

    if ((fp = fopen(argv[1], "r")) == NULL)
    {
        printf("Argument Error:Input file missing.\npattern:./assembler inputfile.asm\n");
        return -1;
    }

    else if ((fp = fopen(argv[1], "r")) != NULL)
    {
        rewind(fp);
        while (fgets(line, sizeof line, fp) != NULL)
        {
            token = strtok(line, "\n\t\r ");
            if (token[strlen(token) - 1] == ':') // Label
            {
                token[strlen(token) - 1] = '\0';
                if (label_count_inside == 1)
                { // breaking the counter when new label encounters next line
                  // add the address for both [.word and .space - (n)*4 bytes +(m)*40 bytes ]

                    int l_add_count_words = word_count_inside * 4;
                    int l_add_count_space = space_count_inside * 40; // 40 bytes
                    int address_sum = l_add_count_space + l_add_count_words;
                    strcpy(label_address[label_count_global - 1], string2int(address_sum, 16));
                    word_count_inside = 0;
                    label_count_inside = 0;
                    space_count_inside = 0;
                }

                strcpy(label_name[label_count_global], token);
                label_count_global++;
                label_count_inside++;
            }
            if (strcmp(token, ".word") == 0) // Word
            {
                la = strtok(NULL, "\t\r ");
                words_count_global++;
                word_count_inside++;
            }
            if (strcmp(token, ".space") == 0) // Space
            {
                la = strtok(NULL, "\t\r ");
                space_count_inside++;
            }

            if (strcmp(token, ".text") == 0) // go till the .text segment
            {
                int l_add_count_words = word_count_inside * 4;
                int l_add_count_space = space_count_inside * 40; // 40 bytes
                int address_sum = l_add_count_space + l_add_count_words;
                strcpy(label_address[label_count_global - 1], string2int(address_sum, 16));
                break;
            }
        }
        fclose(fp);
    }

    int label_count_inside_text = 0;
    int line_num_text = 0;
    char label_name_inside_text[100][15];
    char label_address_inside_text[100][15];
    ////////////////////////////////Second-Pass///////////////////////////////////////////////////////////
    fp = fopen(argv[1], "r");
    if (fp != NULL)
    {
        while (fgets(line, sizeof line, fp) != NULL)
        {
            token = strtok(line, "\n\t "); // skip till .text
            if (strcmp(token, ".text") == 0)
            {
                break;
            }
        }
        int pc_address = 512;

        while (fgets(line, sizeof line, fp) != NULL)
        {
            token = strtok(line, "\n\t\r ");

            while (token)
            {
                if (token[strlen(token) - 1] == ':')
                {
                    token[strlen(token) - 1] = '\0';
                    strcpy(label_address_inside_text[line_num_text], string2int(pc_address, 10));
                    strcpy(label_name_inside_text[label_count_inside_text], token);
                    label_count_inside_text++;
                    pc_address -= 4;
                    line_num_text++;
                    token[strlen(token) - 1] = '\0';
                }
                token = strtok(NULL, "\t\r ");
            }
            pc_address += 4;
        }
        fclose(fp);
    }

    ///////////////////////////////////Third -Pass//////////////////////////////////////////////////////////////
    ///////////////////final pass before writing bin table.////////////////////////////////////////////////////
    fp = fopen(argv[1], "r");
    if (fp != NULL)
    { // third and f
        rewind(fp);
        while (fgets(line, sizeof line, fp) != NULL)
        {
            token = strtok(line, "\n\t "); // get the instruction mnemonic or label
            while (token)
            {
                if (strcmp(token, ".word") == 0) //----------------- ADD word data to .data section -------------------------------
                {
                    word = strtok(NULL, "\t\r ");
                    strcat(word_dest[word_c], word);
                    word_c++;
                }

                if (strcmp(token, ".space") == 0) //----------------- ADD dummy data to .data section -------------------------------
                {
                    space = strtok(NULL, "\t\r ");
                    for (int i = 0; i < 10; i++)
                    {
                        // adding 40bytes of dummy data, note each data is 4 bytes long so 4*10 = 40 bytes
                        strcat(word_dest[word_c], string2int(i + 1, 10));
                        word_c++;
                    }
                }

                if (strcmp(token, "add") == 0) //----------------- ADD -------------------------------
                {
                    op1 = strtok(NULL, "$,\n\r ");
                    int rd = atoi(op1);
                    op2 = strtok(NULL, "$,\n\r ");
                    int rs = atoi(op2);
                    op3 = strtok(NULL, "$,\n\r ");
                    int rt = atoi(op3);
                    strcat(dest[inst], opcode[0]);     // add -opcode
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
                    strcat(dest[inst], opcode[1]);     // sub -opcode
                    strcat(dest[inst], reg[rs]);       // rs 5 bit
                    strcat(dest[inst], reg[rt]);       // rt 5 bits
                    strcat(dest[inst], reg[rd]);       // rd 5 bits
                    strcat(dest[inst], sham_code[0]);  // shamt
                    strcat(dest[inst], funct_code[1]); // function code
                    inst++;
                }

                if (strcmp(token, "la") == 0) //----------------- la -------------------------------
                {
                    op1 = strtok(NULL, "$,\n\r ");
                    int rt = atoi(op1);
                    la = strtok(NULL, ",\n\r ");
                    // check the label_name in array
                    int address_convert = 0;
                    for (int i = 0; i < label_count_global; i++)
                    {
                        if (strcmp(label_name[i], la) == 0) //
                        {
                            address_convert = atoi(label_address[i]);
                        }
                    }
                    char *binary = stringtoBinary(address_convert, 32);
                    int len = strlen(binary);
                    int len1 = len / 2;
                    int len2 = len - len1;
                    char *lui_upper = malloc(len1 + 1);
                    memcpy(lui_upper, binary, len1);
                    lui_upper[len1] = '\0';
                    char *ori_upper = malloc(len2 + 1);
                    memcpy(ori_upper, binary + len1, len2);
                    ori_upper[len2] = '\0';

                    if (strlen(lui_upper) != 0)
                    {
                        strcat(dest[inst], opcode[2]);    // LUI opcode
                        strcat(dest[inst], sham_code[0]); //
                        strcat(dest[inst], reg[rt]);
                        strcat(dest[inst], lui_upper);
                        inst++;
                    }

                    if (strlen(ori_upper) != 0)
                    {
                        strcat(dest[inst], opcode[3]); // ORI opcode
                        strcat(dest[inst], reg[rt]);   // rs
                        strcat(dest[inst], reg[rt]);   // rt
                        strcat(dest[inst], ori_upper);
                        inst++;
                    }
                }

                if (strcmp(token, "lw") == 0) //----------------- lw -------------------------------
                {
                    op1 = strtok(NULL, "$,\n\r ");
                    int rt = atoi(op1);
                    op2 = strtok(NULL, ",(\n\r ");
                    int offset = atoi(op2);
                    op3 = strtok(NULL, "$),\n\r ");
                    int rs = atoi(op3);
                    char *binary = stringtoBinary(offset, 16);
                    strcat(dest[inst], opcode[4]); // lw -opcode
                    strcat(dest[inst], reg[rs]);   // rs 5 bit
                    strcat(dest[inst], reg[rt]);   // rt 5 bits
                    strcat(dest[inst], binary);    // rd 5 bits
                    inst++;
                }

                if (strcmp(token, "sw") == 0) //----------------- sw -------------------------------
                {
                    op1 = strtok(NULL, "$,\n\r ");
                    int rt = atoi(op1);
                    op2 = strtok(NULL, ",(\n\r ");
                    int offset = atoi(op2);
                    op3 = strtok(NULL, "$),\n\r ");
                    int rs = atoi(op3);
                    char *binary = stringtoBinary(offset, 16);
                    strcat(dest[inst], opcode[5]); // lw -opcode
                    strcat(dest[inst], reg[rs]);   // rs 5 bit
                    strcat(dest[inst], reg[rt]);   // rt 5 bits
                    strcat(dest[inst], binary);    // rd 5 bits
                    inst++;
                }

                if (strcmp(token, "bne") == 0) //----------------- bne -------------------------------
                {
                    op1 = strtok(NULL, "$,\n\r ");
                    int rs = atoi(op1);
                    op2 = strtok(NULL, "$,\n\r ");
                    int rt = atoi(op2);
                    op3 = strtok(NULL, ",\n\r ");
                    int branch_targ_address = 0;
                    for (int i = 0; i < label_count_inside_text; i++)
                    {
                        if (strcmp(label_name_inside_text[i], op3) == 0)
                        {
                            branch_targ_address = atoi(label_address_inside_text[i]);
                        }
                    }
                    char *binary = stringtoBinary(branch_targ_address, 16);
                    strcat(dest[inst], opcode[6]); // bne -opcode
                    strcat(dest[inst], reg[rs]);   // rs 5 bit
                    strcat(dest[inst], reg[rt]);   // rt 5 bits
                    strcat(dest[inst], binary);    //  offset
                    inst++;
                }

                if (strcmp(token, "beq") == 0) //----------------- beq-------------------------------
                {
                    op1 = strtok(NULL, "$,\n\r ");
                    int rs = atoi(op1);
                    op2 = strtok(NULL, "$,\n\r ");
                    int rt = atoi(op2);
                    op3 = strtok(NULL, ",\n\r ");
                    int branch_targ_address = 0;
                    for (int i = 0; i < label_count_inside_text; i++)
                    {
                        if (strcmp(label_name_inside_text[i], op3) == 0) //
                        {
                            branch_targ_address = atoi(label_address_inside_text[i]);
                        }
                    }
                    char *binary = stringtoBinary(branch_targ_address, 16);
                    strcat(dest[inst], opcode[7]); // bne -opcode
                    strcat(dest[inst], reg[rs]);   // rs 5 bit
                    strcat(dest[inst], reg[rt]);   // rt 5 bits
                    strcat(dest[inst], binary);    //  offset
                    inst++;
                }

                if (strcmp(token, "j") == 0) //----------------- j-------------------------------
                {
                    op1 = strtok(NULL, "\n\r ");
                    int jump_targ_address = 0;
                    for (int i = 0; i < label_count_inside_text; i++)
                    {
                        if (strcmp(label_name_inside_text[i], op3) == 0) //
                        {
                            jump_targ_address = atoi(label_address_inside_text[i]);
                        }
                    }
                    char *binary = stringtoBinary(jump_targ_address, 26);
                    strcat(dest[inst], opcode[8]); // bne -opcode
                    strcat(dest[inst], binary);    //  offset
                    inst++;
                }

                if (strcmp(token, "ori") == 0) //----------------- ori-------------------------------
                {
                    op1 = strtok(NULL, "$,\n\r ");
                    int rt = atoi(op1);
                    op2 = strtok(NULL, "$,\n\r ");
                    int rs = atoi(op2);
                    op3 = strtok(NULL, "\n\r ");
                    int address = atoi(op3);
                    char *binary = stringtoBinary(address, 16);
                    strcat(dest[inst], opcode[3]); // ori -opcode
                    strcat(dest[inst], reg[rs]);   // rs 5 bit
                    strcat(dest[inst], reg[rt]);   // rt 5 bits
                    strcat(dest[inst], binary);    //  offset
                    inst++;
                }
                token = strtok(NULL, ",\n\t");
            }
        }

        fclose(fp);
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
    //////////////////////////////Write -Binary/////////////////////////////////////////////////////////////////

    char *testvar = argv[1];
    const char param[] = ".";
    writeFile = strtok(testvar, param);
    strcat(writeFile, ".bin");

    fo = fopen(writeFile, "wb");
    if (fo == NULL)
    {
        printf("Error opening output file");
    }

    else
    {
        // dump .data section
        bin_written_data = fwrite(value_data, sizeof(value_data), 1, fo);
        // dump .text section
        bin_written_text = fwrite(value, sizeof(value), 1, fo);
    }

    // bin file dumping check
    if ((bin_written_text == 0) && (bin_written_data == 0))
    {
        printf("\nError:BinFile not generated\n");
    }
    else
    {
        printf("\nDone BinFile Generated Successfully!\n");
    }
    fclose(fo);
    return 0;
}
