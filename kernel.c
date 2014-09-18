void printString(char*);
void readString(char*);
int mod(int, int);
int div(int, int);
void writeInt(int);
void readInt(int*);
void handleInterrupt21(int,int,int,int);

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

void handleInterrupt21(int ax, int bx, int cx, int dx)
{
  switch(ax){
  case 0: printString(bx); break;
  case 1: readString(bx); break;
  case 14: writeInt(bx); break;
  case 15: readInt(bx); break;
  }
}
