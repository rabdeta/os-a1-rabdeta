#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

int
match(char *name, char *pattern)
{
  if (*pattern == '\0')
    return *name == '\0';

  if (*pattern == '*') {
    return match(name, pattern + 1) ||
           (*name && match(name + 1, pattern));
  }

  if (*name == *pattern)
    return match(name + 1, pattern + 1);

  return 0;
}

void
  find(char *path, char *target, int type)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    close(fd);
    return;
  }

  if (st.type != T_DIR) {
    close(fd);
    return;
  }

  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
    close(fd);
    return;
  }

  strcpy(buf, path);
  p = buf + strlen(buf);
  *p++ = '/';

  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0)
      continue;

    if (strcmp(de.name, ".") == 0 ||
        strcmp(de.name, "..") == 0)
      continue;

    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;

    if (stat(buf, &st) < 0)
      continue;

char name[DIRSIZ + 1];
memmove(name, de.name, DIRSIZ);
name[DIRSIZ] = '\0';

if (match(name, target)) {
  if (type == 0 ||
      (type == 1 && (st.type == T_FILE || st.type == T_DEVICE)) ||
      (type == 2 && st.type == T_DIR)) {
    printf("%s\n", buf);
  }
}

if (st.type == T_DIR)
  find(buf, target, type);
  }

  close(fd);
}

int
main(int argc, char *argv[])
{
    if (argc != 3 && argc != 5) {
        fprintf(2, "usage: find <dir> <pattern> [-type f|d]\n");
        exit(1);
    }

    int type = 0;

    if (argc == 5) {
        if (strcmp(argv[3], "-type") != 0) {
            fprintf(2, "usage: find <dir> <pattern> [-type f|d]\n");
            exit(1);
        }

        if (strcmp(argv[4], "f") == 0) {
            type = 1;
        } else if (strcmp(argv[4], "d") == 0) {
            type = 2;
        } else {
            fprintf(2, "Invalid type. Use f or d.\n");
            exit(1);
        }
    }

    find(argv[1], argv[2], type);
    exit(0);
}
