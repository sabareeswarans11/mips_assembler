# mips_assembler

check the bin file o/p in hex editor 

sample is down below ,

<img width="774" alt="Screen Shot 2022-11-01 at 12 34 37 PM" src="https://user-images.githubusercontent.com/94094997/199288761-9f27a6cb-ad3d-4ade-b607-6678c7edcde6.png">

Note :

. data section ( each word is 4 bytes , 4*128 = 512 bytes maximum size and starts at 0x0000000
. text section ( each  instruction is 4 bytes , 4*128 = 512 bytes maximum size and starts at 0x00000200

Note if the data section memory is assigned to '0' means no .word , .space encountered while parsing asm file.

for instance, 
example.asm,

  .data
  .word 0
  .word 1
  .word 2
  .text
Respective example.bin  (viewed in hex editor) 
memory location    Decoded result 
0x00000000           00 00 00 00 
0x00000004           00 00 00 01
0x00000008           00 00 00 02
0x0000000C           00 00 00 00
.                    00 00 00 00
.                    00 00 00 00
.                    00 00 00 00
.                    00 00 00 00
.                    00 00 00 00
.                    00 00 00 00
0x00000200                          -------------> .text ( holds MIPS instructions) , to make starting address of text section , i have created bin[128][4]                                                      so which holds 512 bytes and next bin (.text section is dynamically allocated memory )
                                                    bin file size wont exceed  1024 bytes .
  

