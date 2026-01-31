#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "/home/dynamo/workspace/C-learning/capstone/include/common.h"
#include "/home/dynamo/workspace/C-learning/capstone/include/file.h"

int create_db_file(char *filename) {
  if (!strstr(filename, ".db")) {
    printf("Error: filename must have .db extension\n");
    return STATUS_ERROR;
  }

  if (access(filename, F_OK) == 0) {
    printf("Error: Database %s already exists\n", filename);
    return STATUS_ERROR;
  }

  int fd = open(filename, O_RDWR | O_CREAT, 0644);
  if (fd == -1) {
    perror("open");
    return STATUS_ERROR;
  }

  printf("Creating Database: %s\n", filename);
  return fd;
}

int open_db_file(char *filename) {
  if (strstr(filename, ".py") || strstr(filename, ".sh")) {
    printf("GET AWAY FROM HERE HACKER!\n");
    return -1;
  }

  int fd = open(filename, O_RDWR);
  if (fd == -1) {
    perror("open");
    return STATUS_ERROR;
  }

  printf("Opening Database: %s\n", filename);
  return fd;
}