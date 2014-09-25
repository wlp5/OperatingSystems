BlackDOS Operating System
Document Revision: 2014-09-20

Team Members:
     Daniel Crowder
     Timothy Hopkins
     William Peters

3460:426 Operating Systems

Documentation: This document details steps taken to complete assignments provided in 3460:426 Operating Systems with Dr. Tim O'Neil. All files mentioned are included in a submitted .zip file which will be submitted with this document to Springboard.

Notes with each lab are given in reverse chronological order.

Lab 03: BlackDOS Function Calls
     Lab Source:http://www.cs.uakron.edu/~toneil/teaching/cs426/labs/lab%203%20blackdos%20functions/lab%203%20blackdos%20function%20calls.pdf
	 Download revised version of kernel.asm
	1. Add Integer I/O Functions to kernel.c
	2. writeInt()
		Create helper functions: mod and div
			int mod(int a, int b)
			{
			  int x = a;
			  while (x >= b) x = x - b;
			  return x;
			}

			int div(int a, int b)
			{
			  int q = 0;
			  while(q * b <= a) q++;
			  return (q - 1);
			}
		Use 6-character array for 16-bit integer ('\0' as final character)
			void writeInt(int x)
			{
			  char number[6], *d;
			  int q = x, r;  d = number + 5;
			  *d = 0; 
			  d--;
			  while(q > 0)
				{
				  r = mod(q,10);
				  q = div(q,10);
				  *d = r + 48; 
				  d--;
				}
			  d++;
			  printString(d);
			}
	3. readInt()
		Read number as character string
		Convert ASCII characters left-to-right as numeric value
		Store converted, weighted values as running sum
		Store sum at an address (passed by-reference to function)
			void readInt(int* x)
			{
			  char number[6];
			  int i = 0;
			  *x = 0;
			  readString(number);
			  for(; number[i] != 0; i++)
			  {
				*x = (*x) * 10;
				*x = (*x) + number[i] - '0';
			  } 
			  printString("\r\n\0");
			}
	4. To test: Compile kernel.c using compileOS.sh script
	5. Create Interrupt 33
		Rename kernel.asm as lib.asm
		Download revised kernel.asm (includes makeInterrupt21() and interrupt21ServiceRoutine())
		Create function in C Code: void handleInterrupt21(int ax, int bx, int cx, int dx)
		Replace all prototypes with handleInterrupt21()
	6. Add Interrupt Function calls
		Define values for interrupt calls:
			Print String: 0
			Read String: 1
			Write Integer: 14
			Read Integer 15
		Define any other value as an error
		void handleInterrupt21(int ax, int bx, int cx, int dx)
		{
		  switch(ax){
		  case 0: printString(bx); break;
		  case 1: readString(bx); break;
		  case 14: writeInt(bx); break;
		  case 15: readInt(bx); break;
		  }
		}
	7. Replace main() functionality
		void main()
		{
		  char line[80];
		  int x;


		  makeInterrupt21();
		  interrupt(33,0,"Hello world\r\n\0",0,0);
		  interrupt(33,0,"\r\n\0",0,0);
		  interrupt(33,0,"Enter a line: \0",0,0);
		  interrupt(33,1,line,0,0);
		  interrupt(33,0,"\r\nYou wrote: \0",0,0);
		  interrupt(33,0,line,0,0);
		  interrupt(33,0,"\r\n\0",0,0);
		  interrupt(33,0,"Enter a number: \0",0,0);
		  interrupt(33,15,&x,0,0);
		  interrupt(33,0,"You entered \0",0,0);
		  interrupt(33,14,x,0,0);
		  interrupt(33,0,"\r\n\0",0,0);
		  /*
		  writeInt(0);
		  printString("Enter a line: \0");
		  readString(line);
		  printString("\r\nYou typed: \0");
		  printString(line);
		  printString("\r\n\0");
		  printString("Enter a number: \0");
		  readInt(&x);
		  printString("You Entered \0");
		  writeInt(x);
		  printString("\r\n\0");
		  */
		  while(1);
		}
	8. To test: Compile using compileOS.sh provided in .zip file
		dd if=/dev/zero of=floppya.img bs=512 count=2880
		dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc
		bcc -ansi -c -o kernel.o kernel.c
		as86 kernel.asm -o kernel_asm.o
		ld86 -o kernel -d kernel.o kernel_asm.o
		dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3

Lab 02: Interrupt-Driven I/O
    Lab Source:http://www.cs.uakron.edu/~toneil/teaching/cs426/labs/lab%202%20io/lab%202%20io.pdf
    Download kernel.asm, Create kernel.c
	1. Write Basic I/O Interrupts into code.
		Code:
		void printString(char*);
		void readString(char*);

		void main()
		{
		  char line[80];
		  printString("Hello World \r\n\0");
		  printString("Enter a line: \0");
		  readString(line);
		  printString("\r\nYou typed: \0");
		  printString(line);
		  printString("\r\n\0");
		  while(1);
		}

		void printString(char* ch)
		{
		  /* char* ch;*/
		  int i = 0;
		  while (ch[i]!= '\0'){
			char al = ch[i];
			char ah = 14;
			int ax = ah * 256 + al;
			interrupt(16, ax, 0, 0, 0);
			i = i + 1;
		  }

		 }
		void readString(char* ch)
		{
		  int i;
		  char al;
		  char ah;
		  int ax;
		  i = 0;
		  do
			{
			  ch[i] = interrupt(22,0,0,0,0);
			  if(ch[i] == '\b')
			{
			  printString("\b \b");
			  if(i > 0)
				i--;
			}
			  else
			{
			  al = ch[i];
			  ah = 14;
			  ax = ah * 256 + al;
			  interrupt(16,ax,0,0,0);
			  i++;
			}
			}while(ch[i-1] != 13);
		  ch[i - 1] = '\0';
		}
	2. Compile kernel with bcc 
	   bcc -ansi -c -o kernel.o kernel.c
	3. Link with kernel.asm file 
	   as86 kernel.asm -o kernel_asm.o
	   ld86 -o kernel -d kernel.o kernel_asm.o
	4. Copy kernel machine code to floppya.img
	   dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
	5. Run Bochs to Test. "Hello World" shot print
	6. Append all command-line above in lab to compileOS.sh text file
	7. Make compileOS.sh an executable
	   chmod +x compileOS.sh

Lab 01: Introduction
     Lab Source: http://www.cs.uakron.edu/~toneil/teaching/cs426/labs/lab%201%20intro/lab%201%20intro.pdf
     Download bootload.asm, osxterm.txt, test.img
        Source: www.cs.uakron.edu/~toneil/teaching/cs426/labs/index.html
     1. Rename test.img to floppya.img
     2. Test Bochs
       Open command prompt
       Type bochs -f osxterm.txt 
       "Bochs Works" appears, confirming functionality
     3. Assemble bootloader
        nasm bootload.asm
     4. Create image file of floppy disk with only zeros
        dd if=/dev/zero of=floppya.img bs=512 count=2880
     5. Create image file with bootloader
        dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc
     6. Create all commands in lab into shell script file.
        Title: compileOS.sh
