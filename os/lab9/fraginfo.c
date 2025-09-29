#include <sys/stat.h>
#include <sys/statvfs.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
   struct stat sfile;
   struct statvfs sdisk;

   if (argc != 2) {
      fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   if (lstat(argv[1], &sfile) == -1) {
      perror("lstat");
      exit(EXIT_FAILURE);
   }

   if (statvfs(argv[1], &sdisk) == -1) {
      perror("statvfs");
      exit(EXIT_FAILURE);
   }

   printf("Filesystem block size:    %jd\n",
         (intmax_t) sdisk.f_bsize);


   printf("File size:                %jd bytes\n",
         (intmax_t) sfile.st_size);

   int wasted = sdisk.f_bsize - sfile.st_size % sdisk.f_bsize;
   printf("Internal fragmentation:   %d bytes\n", wasted);

   exit(EXIT_SUCCESS);
}
