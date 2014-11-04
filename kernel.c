void printString(char*);
void readString(char*);
int mod(int, int);
int div(int, int);
void writeInt(int);
void readInt(int*);
/*Begin edits DCC 20140929*/
void readSector(char*, int);
void readFile(char*, char*, int);
/*End edits DCC 20140929*/
void handleInterrupt21(int,int,int,int);

void main()
{
  char line[80];
  int x;

/*Begin edits DCC 20140929*/
  /* PART A 
  char buffer[512];
  makeInterrupt21();
  interrupt(33,2,&buffer,30,0);
  interrupt(33,2,buffer,0,0);
  */
  /* PART B */
  char buffer[13312];
  int size;
  makeInterrupt21();
  interrupt(33,3,"msg\0",buffer,&size);
  interrupt(33,0,buffer,0,0);
  //while(1);  
  /*interrupt(33,0,"Hello world\r\n\0",0,0);
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
  */
/*End edits DCC 20140929*/  
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

/*Begin edits DCC 20140929*/
void readSector(char* dir, int sector)
{
   /*Begin edits TTH 10/17/14 */  
   int ah;     /*read a sector (2 for read, 3 for write)*/
   int al;     /*number of sectors to read*/
   int ch;         /*track number*/
   int cl;         /*relative sector number*/
   int dh;         /*head number*/
   int dl;     /*device number (for floppy, 0)*/
   int ax;
   int cx;
   int dx;

   ah = 2;
   al = 1;
   dl = 0;
  
   /* End edits TTH 10/17/14*/
 
   /*sector passed is absolute
   interrupt requires relative
   conversion for floppy disk below
   */
   cl = (mod(sector, 18) + 1);
   dh = mod(div(sector, 18), 2);
   ch = div(sector, 36);
   
   /*derive interrupt values*/
   ax = (ah*256) + al;
   cx = (ch*256) + cl;
   dx = (dh*256) + dl;
   
   /*call interrupt*/
   interrupt(19, ax, dir, cx, dx);
}
void readFile(char* fname, char* buffer, int* size) 
{ 
  /* Begin edits TTH 10/17/14*/
   int isFound;
   int isMatch;
   int sectorNo;
   int i; int j;
   char dir[512];

   isFound = 0;
   i = 0;
   /* End edits TTH 10/17/14 */
   /*Load directory sector into 512-byte char array*/
   /*Disk directory sits at sector 2*/
   readSector(dir, 2);
   /*Try to match file name. If not found, return*/
   /*interrupt(33,0,dir,0,0);*/
   /*interrupt(33,0,fname,0,0);*/
   while (isFound == 0)
   {
      isMatch = 1; j = 0; // was semi
      while (j < 6)
      {
         if (fname[j] != dir[j+(32*i)]) isMatch = 0;
         j = j+1;
      }
      if (isMatch == 1) isFound = 1;
      i = i+1;
   }
   /*interrupt(33,0,"I'm out of here\0",0,0);*/
   if (isFound == 0) return;
   /*interrupt(33,0,"pass\0",0,0);*/
   /*Using sector numbers in directory, load file, sector by sector, into buffer*/
   i=(i-1);/*+6;
   i = 3;*/
   j = 6;
   while ((dir[(i*32)+j]) != 0) {
      /*interrupt(33,0,"ML\0",0,0);*/
      sectorNo = dir[(i*32)+j];
      readSector(buffer, sectorNo);
      /*TODO resize buffer (add 512 to buffer address)*/
      buffer = buffer + 512;
      /*Write sector count back*/
      *size = *size + 1;
      //writeInt(*size);
      j = j+1;
   }
   
}

/*End edits DCC 20140929*/

void handleInterrupt21(int ax, int bx, int cx, int dx)
{
  switch(ax){
  case 0: printString(bx); break;
  /*Begin edits DCC 20140929*/
  case 2: readSector(bx, cx); break;
  case 3: readFile(bx, cx, dx); break;
  /*End edits DCC 20140929*/
  case 1: readString(bx); break;
  case 14: writeInt(bx); break;
  case 15: readInt(bx); break;
  }
}
