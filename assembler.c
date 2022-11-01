#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

//const uintptr_t mem_text_segment = 0x0200;

void binary(unsigned n)
{
    unsigned i;
    printf("Binary representation:");
    for (i = 1 << 5; i > 0; i = i / 2)
        (n & i) ? printf("1") : printf("0");
}


 static const char * const reg[] = {
        // $0     $1        $2       $3     $4     $5      $6       $7     $8       $9     $10 
        "00000", "00001", "00010","00011","00100","00101","00110","00111","01000","01001","01010", 
       // $11     $12       $13     $14     $15    $16     $17     $18     $19     $20    $21 
        "01011", "01100", "01101","01110","01111","10000","10001","10010","10011","10100","10101",
       // $22     $23       $24    $25     $26     $27     $28     $29     $30     $31   
        "10110", "10111", "11000","11001","11010","11011","11100","11101","11110","11111"
   };

 static const char * const opcode[] = {
       // ADD
        "000000"  
    };

 static const char * const funct_code[] = {
       // 6 bit 
       // ADD    // SUB
        "100000","100010"  
    };

static const char * const sham_code[] = {
       // 5 bit 
       // ADD
        "00000"
    };

typedef struct bin1 {
    uint32_t machine_code_data[128];
} bin1; 


typedef struct bin2 {
    int32_t* machine_code_text;
} bin2; 

int main()
{
    FILE* fp;
    FILE* fo;
   
    char line[100];
    char *token = NULL;
    char *op1, *op2, *op3, *word,*label;
    char ch;
    char dest[128][33] = {{0}};
    char word_dest[128][11]= {{0}};
    char* dest_pt;  // pointer to the result string
    char* word_pt;
    int program[1000];
    int counter = 0; // holds the address of the machine code instruction
    int inst =0;
    int word_c =0;
    // passed test case rtype.asm 
    // passed test case directive.asm

    fp = fopen("directive.asm", "r");
    if (fp != NULL)
    {
        // second pass 
        while (fgets(line, sizeof line, fp) != NULL)
        {
            token = strtok(line, ",\n\t "); // get the instruction mnemonic or label
            while (token)
            {  
                if (strcmp(token, ".word") == 0) //----------------- ADD word data to .data section -------------------------------
                {
                    printf("\nAdd word!!\n");
                    word = strtok(NULL, "\t\r ");
                    printf("word found allocate 4byte : %s\n", word);
                    //int wd = atoi(word)
                    strcat(word_dest[word_c],word);
                    word_c ++;

                }

                if (strcmp(token, "add") == 0) //----------------- ADD -------------------------------
                {
                    printf("\nAdd Start!!\n");
                    op1 = strtok(NULL, "$,\n\r ");
                    printf("Operator 1: %s\n", op1);
                    int rd = atoi(op1);
                    printf("rd in int: %d\n", rd);
                    op2 = strtok(NULL, "$,\n\r ");
                    printf("Operator 2: %s\n", op2);
                    int rs = atoi(op2);
                    printf("rs in int: %d\n", rs);
                    op3 = strtok(NULL, "$,\n\r ");
                    printf("Operator 3: %s\n", op3);
                    int rt = atoi(op3);
                    printf("rt in int: %d\n", rt);
                    strcat(dest[inst],opcode[0]);  // ADD -opcode
                    strcat(dest[inst],reg[rs]);   // rs 5 bit
                    strcat(dest[inst],reg[rt]);   // rt 5 bits
                    strcat(dest[inst],reg[rd]);   // rd 5 bits
                    strcat(dest[inst],sham_code[0]);   //shamt
                    strcat(dest[inst],funct_code[0]);   // function code
                    inst ++;
                }

                if (strcmp(token, "sub") == 0) //----------------- SUB -------------------------------
                {
                    printf("\nsub Start!!\n");
                    op1 = strtok(NULL, "$,\n\r ");
                    printf("Operator 1: %s\n", op1);
                    int rd = atoi(op1);
                    printf("rd in int: %d\n", rd);
                    op2 = strtok(NULL, "$,\n\r ");
                    printf("Operator 2: %s\n", op2);
                    int rs = atoi(op2);
                    printf("rs in int: %d\n", rs);
                    op3 = strtok(NULL, "$,\n\r ");
                    printf("Operator 3: %s\n", op3);
                    int rt = atoi(op3);
                    printf("rt in int: %d\n", rt);
                    strcat(dest[inst],opcode[0]);  // ADD -opcode
                    strcat(dest[inst],reg[rs]);   // rs 5 bit
                    strcat(dest[inst],reg[rt]);   // rt 5 bits
                    strcat(dest[inst],reg[rd]);   // rd 5 bits
                    strcat(dest[inst],sham_code[0]);   //shamt
                    strcat(dest[inst],funct_code[1]);   // function code
                    inst ++;
                }



                //printf("instr num:%d",inst);    
                token = strtok(NULL, ",\n\t");


            }
        }
                    
    }


    /// .data decoded result 
    printf("\nword array 0:%s\n",word_dest[0]);
    printf("\nword array 0:%s\n",word_dest[1]);
    printf("\nword array 0:%s\n",word_dest[2]);
    printf("\nword array 0:%s\n",word_dest[3]);

    /// .text decoded result 
    printf("\narray 0:%s\n",dest[0]);
    printf("\narray 1:%s\n",dest[1]);
    printf("\narray 3:%s\n",dest[2]);
    printf("\narray 4:%s\n",dest[3]);

    bin1 b1;
    bin2 b2;
    // .text segment starts at 0x0200  and dynamically memory allocated maximum 128 * 4 bytes = 512 bytes 
    b2.machine_code_text = (int32_t *)malloc(inst * sizeof(int32_t));  
    if (b2.machine_code_text == NULL) {
        printf("Memory not allocated.\n");
         }
    
    
    else{
        for(int i= 0; i < word_c ;i++)
            {
             b1.machine_code_data[i] = strtol(word_dest[i],&word_pt,10);
             printf("\nint value-word:%d\n",b1.machine_code_data[i]);
            }


        for(int i= 0; i < inst ;i++)
            {   
            b2.machine_code_text[i] =strtol(dest[i],&dest_pt,2);
            printf("\nint value:%d\n",b2.machine_code_text[i]);
             }  
        }

   
    /// @brief .data section byte order reversed and write it in big-endian format irrespective of os  
    unsigned char value_data[128][4]={0};  // unallocated will be zero
    int count_word=0;
    for(int i= 0; i < word_c  ;i++)
    {
        value_data[count_word][0] = (uint32_t)b1.machine_code_data[i] >> 24;
        value_data[count_word][1] = (uint32_t)b1.machine_code_data[i] >> 16;
        value_data[count_word][2] = (uint32_t)b1.machine_code_data[i] >> 8;
        value_data[count_word][3] = (uint32_t)b1.machine_code_data[i];
        count_word ++;
    }


   /// @brief .text section byte order reversed and write it in big-endian format irrespective of os  
   
   unsigned char value[inst][4];
   int count_inst=0;
   for(int i= 0; i < inst  ;i++)
    {
        
        value[count_inst][0] = (uint32_t)b2.machine_code_text[i] >> 24;
        value[count_inst][1] = (uint32_t)b2.machine_code_text[i] >> 16;
        value[count_inst][2] = (uint32_t)b2.machine_code_text[i] >> 8;
        value[count_inst][3] = (uint32_t)b2.machine_code_text[i];
        count_inst ++ ;

    }

    fo = fopen("directive.bin","wb");
    if (fo == NULL){
        printf("Error opening output file");
    }

   // dump .data section 
   size_t elts_written_data = fwrite(value_data, sizeof(value_data), 1, fo);
   // dump .text section 
   size_t elts_written_text = fwrite(value, sizeof(value), 1, fo);
    
    
    if (elts_written_text == 0){
      printf("binary file not written");
      }


    //fwrite(array, sizeof(array), 1, fo);
  

    fclose(fp);
    fclose(fo);

    printf("\nDone!!\n");

    return 0;
}
