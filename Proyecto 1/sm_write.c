// cliente.c




#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <fcntl.h>          
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
 
#define SMOBJ_NAME  "/sharedMemory"

int main(void)
{
   int fd;
   char buf[] = "Hello,this is writting process";
   char *ptr;
   
   fd = shm_open (SMOBJ_NAME,  O_RDWR  , 00200); /* open shared memory object*/
   if(fd == -1)
   {
       printf("Error file descriptor %s\n", strerror(errno));
	   exit(1);
   }
   
   ptr = mmap(NULL, sizeof(buf), PROT_WRITE, MAP_SHARED, fd, 0);
   if(ptr == MAP_FAILED)
   {
      printf("Map failed in write process: %s\n", strerror(errno));
      exit(1);
   }
   
   memcpy(ptr,buf, sizeof(buf));
   printf("%d \n", (int)sizeof(buf));
   close(fd);
   
   return 0;
}
