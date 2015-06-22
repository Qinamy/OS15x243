#include <stdio.h>
#include <syscall.h>

int
main (int argc, char **argv)
{
   int i;
  printf("argc=%d",argc);
  for (i = 0; i < argc; i++)
  { printf ("argv[%d]=%s ",i, argv[i]);
    printf ("\n");}
  if(!strcmp(argv[1],”halt”))
halt();
else if (!strcmp(argv[1],”open”))
opne(argv[2]);
else if (!strcmp(argv[1],”exit”))

exit(0);
  return EXIT_SUCCESS;

}
