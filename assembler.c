#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>



char* itoa(int val, int base){
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
	
}


char* toBinary(int n, int len)
{
    char* binary = (char*)malloc(sizeof(char) * len);
    int k = 0;
    for (unsigned i = (1 << (len - 1)); i > 0; i = i / 2) {
        binary[k++] = (n & i) ? '1' : '0';
    }
    binary[k] = '\0';
    return binary;
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
        // ADD     //SUB      //LUI     //ORI     //lw     //sw
        "000000" ,"000000" ,"001111" ,"001101", "100011","101011"  
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

struct key_value
{
    int key;
    char* value;
}List;


int main()
{
    FILE* fp;
    FILE* fo;
    char line[100];
    char *token = NULL;
    char *op1, *op2, *op3, *word,*la,*label,*space;
    char ch;
    char dest[128][33] = {{0}};
    char word_dest[128][11]= {{0}};
    char* dest_pt;  // pointer to the result string
    char* word_pt;
    int word_c =0;
    int program[1000];
    int counter = 0; // holds the address of the machine code instruction

    // passed test case rtype.asm 
    // passed test case directive2.asm
    // passed test case data2.asm 
   char array [1000];
   int inst =0;
   int word_count_inside =0;
   int label_count_inside =0;
   int space_count_inside =0;
   int words_count_global = 0;
   int label_count_global= 0;
   char label_name[100][15];
   char label_address[100][15];
   
   fp = fopen("forwback.asm", "r");
     // First Pass    
    if (fp != NULL)
    {    rewind(fp);
         while(fgets(line,sizeof line,fp)!= NULL)
        {
        token=strtok(line,"\n\t\r ");
           if (token[strlen(token)-1]==':')          //Label
                { token[strlen(token)-1]='\0'; 
               if(label_count_inside == 1)
                {   // breaking the counter when new label encounters next line 
                    // add the address for both [.word and .space - (n)*4 bytes +(m)*40 bytes ]
                    
                     int l_add_count_words = word_count_inside*4;
                     int l_add_count_space = space_count_inside*40;  // 40 bytes 
                     int address_sum = l_add_count_space + l_add_count_words;
                     strcpy(label_address[label_count_global-1],itoa(address_sum,16)); 
                     word_count_inside = 0;
                     label_count_inside =0;
                     space_count_inside = 0;
                }
        
                printf("\nlabel:%s\n",token);
                strcpy(label_name[label_count_global],token);
                label_count_global++;
                label_count_inside++;
                }
            if (strcmp(token,".word")== 0)          //Word
                {
                la = strtok(NULL, "\t\r ");
                printf("\nword found: : %s\n", token);
                words_count_global++;
                word_count_inside++;

                }
            if (strcmp(token,".space")== 0)          //Space
                {
                la = strtok(NULL, "\t\r ");
                printf("\nspace found: : %s\n", token);
                space_count_inside++;
                }

            if (strcmp(token,".text")==0 )  //go till the .text segment
            {
               int l_add_count_words = word_count_inside*4;
               int l_add_count_space = space_count_inside*40;  // 40 bytes 
               int address_sum = l_add_count_space + l_add_count_words;
               strcpy(label_address[label_count_global-1],itoa(address_sum,16));
               break;

            }

        }   
         fclose(fp); 
    }
    
    

    // label table multi char array
    for (int i =0 ;i < label_count_global; i++)
    {
        printf("\nlabels:%s\n",label_name[i]);
        printf("\nlabel_address:%s\n",label_address[i]);

    }

///// need to implement 

    fp = fopen("forwback.asm", "r");
     // second Pass    
    if (fp != NULL)
    {    rewind(fp);
         while(fgets(line,sizeof line,fp)!= NULL)
        {
            token=strtok(line,"\n\t\r ");           // skip till .text 
            
            if (token[strlen(token)-1]==':')
            {
               // printf("pass2 label %s",token);

            }

           // if (strcmp(token,".text")==0 )
            //    break;

        }
        fclose(fp); 
    }



    fp = fopen("forwback.asm", "r");
    if (fp != NULL)
    {   // third and final pass before writing bin table. 
        rewind(fp);
        while (fgets(line, sizeof line, fp) != NULL)
        {
            token = strtok(line, "\n\t "); // get the instruction mnemonic or label
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

                if (strcmp(token, ".space") == 0) //----------------- ADD dummy data to .data section -------------------------------
                {
                    printf("\nAdd 40 bytes of dummy data since space is found.!!\n");
                    space = strtok(NULL, "\t\r ");
                    printf("space found allocate 4byte : %s\n",space);
                    //int wd = atoi(word)
                    for (int i=0; i<10;i++)
                    { // adding 40bytes of dummy data, note each data is 4 bytes long so 4*10 = 40 bytes   
                     strcat(word_dest[word_c],itoa(i+1,10));
                     word_c ++;
                    }

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
                    strcat(dest[inst],opcode[0]);  // sub -opcode
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
                    strcat(dest[inst],opcode[1]);  // ADD -opcode
                    strcat(dest[inst],reg[rs]);   // rs 5 bit
                    strcat(dest[inst],reg[rt]);   // rt 5 bits
                    strcat(dest[inst],reg[rd]);   // rd 5 bits
                    strcat(dest[inst],sham_code[0]);   //shamt
                    strcat(dest[inst],funct_code[1]);   // function code
                    inst ++;
                }

                if (strcmp(token, "la") == 0) //----------------- la -------------------------------
                {
                    printf("\nla Start!!\n");
                    op1 = strtok(NULL, "$,\n\r ");
                    printf("rt register: %s\n", op1);
                    int rt = atoi(op1);
                    printf("rt in int: %d\n", rt);
                    la= strtok(NULL, ",\n\r ");
                    // check the label_name in array 
                    int address_convert =0;
                    for (int i =0 ;i < label_count_global; i++)
                        {
                          if(strcmp(label_name[i],la)==0)  //
                          {
                            address_convert =atoi(label_address[i]);                      
                          }

                        }
                     printf("label address:0x%08x\n",address_convert);
                     char* binary = toBinary(address_convert, 32);
                     printf("The binary representation: %s", binary);

                     int len = strlen(binary);
                     int len1 = len/2;
                     int len2 = len - len1; // Compensate for possible odd length
                     char *lui_upper = malloc(len1+1); // one for the null terminator
                     memcpy(lui_upper, binary, len1);
                     lui_upper[len1] = '\0';
                     char *ori_upper = malloc(len2+1); // one for the null terminator
                     memcpy(ori_upper, binary+len1, len2);
                     ori_upper[len2] = '\0';
                     printf("\n%s\n",ori_upper);
                     printf("\n%s\n",lui_upper);

                     if(strlen(lui_upper)!=0)
                     {
                        //printf("entered");
                        strcat(dest[inst],opcode[2]);   // LUI opcode 
                        strcat(dest[inst],sham_code[0]);   // 
                        strcat(dest[inst],reg[rt]);
                        strcat(dest[inst],lui_upper);
                        inst ++;
                     }

                     if(strlen(ori_upper)!=0)
                     {
                        //printf("entered");
                        strcat(dest[inst],opcode[3]);   // ORI opcode 
                        strcat(dest[inst],reg[rt]);  // rs
                        strcat(dest[inst],reg[rt]);  // rt
                        strcat(dest[inst],ori_upper);
                        inst ++;
                     }
                     
                }

                if (strcmp(token, "lw") == 0) //----------------- lw -------------------------------
                {
                    printf("\nlw Start!!\n");
                    op1 = strtok(NULL, "$,\n\r ");
                    printf("Operator 1(rt): %s\n", op1);
                    int rt = atoi(op1);
                    printf("rt in int: %d\n", rt);
                    op2 = strtok(NULL, ",(\n\r ");
                    printf("Operator 2(offset): %s\n", op2);
                    int offset = atoi(op2);
                    printf("offset in int: %d\n", offset);
                    op3 = strtok(NULL, "$),\n\r ");
                    printf("Operator 3(rs): %s\n", op3);
                    int rs = atoi(op3);
                    printf("rs in int: %d\n", rs);
                    char* binary = toBinary(offset, 16);
                    strcat(dest[inst],opcode[4]);  // lw -opcode
                    strcat(dest[inst],reg[rs]);   // rs 5 bit
                    strcat(dest[inst],reg[rt]);   // rt 5 bits
                    strcat(dest[inst],binary);   // rd 5 bits
                    inst ++;
                }

                if (strcmp(token, "sw") == 0) //----------------- lw -------------------------------
                {
                    printf("\nsw Start!!\n");
                    op1 = strtok(NULL, "$,\n\r ");
                    printf("Operator 1(rt): %s\n", op1);
                    int rt = atoi(op1);
                    printf("rt in int: %d\n", rt);
                    op2 = strtok(NULL, ",(\n\r ");
                    printf("Operator 2(offset): %s\n", op2);
                    int offset = atoi(op2);
                    printf("offset in int: %d\n", offset);
                    op3 = strtok(NULL, "$),\n\r ");
                    printf("Operator 3(rs): %s\n", op3);
                    int rs = atoi(op3);
                    printf("rs in int: %d\n", rs);
                    char* binary = toBinary(offset, 16);
                    strcat(dest[inst],opcode[5]);  // lw -opcode
                    strcat(dest[inst],reg[rs]);   // rs 5 bit
                    strcat(dest[inst],reg[rt]);   // rt 5 bits
                    strcat(dest[inst],binary);   // rd 5 bits
                    inst ++;
                }

                //printf("instr num:%d",inst);    
                token = strtok(NULL, ",\n\t");


            }
        }
    
    fclose(fp);

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

    fo = fopen("forwback.bin","wb");
    if (fo == NULL){
        printf("Error opening output file");
    }

   // dump .data section 
   size_t elts_written_data = fwrite(value_data, sizeof(value_data), 1, fo);
   // dump .text section 
   size_t elts_written_text = fwrite(value, sizeof(value), 1, fo);
    
    // bin file dumping check 

    if(elts_written_text == 0){
      printf(" .text section is missing,binary file won't have starting address @ox00000200 ");
      }
    else if(elts_written_data == 0)
    {
      printf(" .data section is missing,binary file .data section starting address will be initialzied as '0', no data no assembler decoding ");
    }
    else 
    {
        printf("bin file with both .data and .text section generated successfully");
    }
    

    //fwrite(array, sizeof(array), 1, fo);
  

  
    fclose(fo);


    printf("\nDone!!\n");

    return 0;
}
