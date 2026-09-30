#include <stdio.h>
#include <stdlib.h>
/*
Fibonacci
*/
void main(void)
{
  int I, PRI = 0, SEG = 1, SIG;
  printf("\t %d \t %d", PRI, SEG);
  for (I=3; I <= 50; I++)
  {
      SIG = SIG + SEG;
      PRI = SEG;
      SEG = SIG;
      printf("\t %d", SIG);
  }
}
