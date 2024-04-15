#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
 
#include "shared_memory.c"

int main(void)
{
    char buf[] = "Hello,this is writting process";
    printf("%d \n", (int)sizeof(buf));

    write_buf(buf);
   
   return 0;
}
