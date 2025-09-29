#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>

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

   printf("ID of containing device:  %jx\n",
           (uintmax_t) sfile.st_dev);

   printf("Filesystem block size:    %jd\n",
         (intmax_t) sdisk.f_bsize);

   printf("File type:                ");
   switch (sfile.st_mode & S_IFMT) {
      case S_IFBLK:  printf("block device\n");            break;
      case S_IFCHR:  printf("character device\n");        break;
      case S_IFDIR:  printf("directory\n");               break;
      case S_IFIFO:  printf("FIFO/pipe\n");               break;
      case S_IFLNK:  printf("symlink\n");                 break;
      case S_IFREG:  printf("regular file\n");            break;
      case S_IFSOCK: printf("socket\n");                  break;
      default:       printf("unknown?\n");                break;
   }

   printf("I-node number:            %ju\n",
         (uintmax_t) sfile.st_ino);

   printf("Mode:                     %jo (octal)\n",
         (uintmax_t) sfile.st_mode);

   printf("Link count:               %ju\n",
         (uintmax_t) sfile.st_nlink);
   printf("Ownership:                UID=%ju   GID=%ju\n",
         (uintmax_t) sfile.st_uid,
         (uintmax_t) sfile.st_gid);

   printf("File size:                %jd bytes\n",
         (intmax_t) sfile.st_size);
   printf("Unit blocks allocated:    %jd  (1 unit = 512 B)\n",
         (intmax_t) sfile.st_blocks);

   exit(EXIT_SUCCESS);
}
