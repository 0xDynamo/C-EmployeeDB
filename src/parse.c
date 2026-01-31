#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/common.h"
#include "../include/include/parse.h"

void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees) {}

int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees,
                 char *addstring) {}

int read_employees(int fd, struct dbheader_t *dbhdr,
                   struct employee_t **employeesOut) {}

int output_file(int fd, struct dbheader_t *dbhdr,
                struct employee_t *employees) {
  if (fd < 0) {
    printf("Error: Received a bad FD from the user\n");
    return STATUS_ERROR;
  }
  dbhdr->magic = htonl(dbhdr->magic);       // ✅ 32-bit
  dbhdr->version = htons(dbhdr->version);   // ← Fix: 16-bit
  dbhdr->count = htons(dbhdr->count);       // ← Fix: 16-bit
  dbhdr->filesize = htonl(dbhdr->filesize); // ✅ 32-bit
  off_t position = 0;                       // New position in  bytes
  if (lseek(fd, position, SEEK_SET) == -1) {
    perror("lseek");
    return STATUS_ERROR;
  };

  ssize_t written = write(fd, dbhdr, sizeof(struct dbheader_t));
  if (written != sizeof(struct dbheader_t)) {
    perror("write");
    return STATUS_ERROR;
  }
  return STATUS_SUCCESS;
}

int validate_db_header(int fd, struct dbheader_t **headerOut) {
  if (fd < 0) {
    printf("Error: Received a bad FD from the user\n");
    return STATUS_ERROR;
  }
  struct dbheader_t *dbheader = calloc(1, sizeof(struct dbheader_t));
  if (dbheader == NULL) {
    perror("calloc");
    printf("Error: DB header Memory allocation failed..\n");
    return STATUS_ERROR;
  }
  if (read(fd, dbheader, sizeof(struct dbheader_t)) !=
      sizeof(struct dbheader_t)) {
    perror("read");
    free(dbheader);
    return STATUS_ERROR;
  };

  // handling different CPU architectures
  dbheader->magic = ntohl(dbheader->magic);       // ✅ 32-bit
  dbheader->version = ntohs(dbheader->version);   // ← Fix: 16-bit
  dbheader->count = ntohs(dbheader->count);       // ← Fix: 16-bit
  dbheader->filesize = ntohl(dbheader->filesize); // ✅ 32-bit

  if (dbheader->magic != HEADER_MAGIC) {
    printf("Improper header magic\n");
    free(dbheader);
    return -1;
  }
  if (dbheader->version != 1) {
    printf("Improper header version\n");
    free(dbheader);
    return -1;
  }

  struct stat dbstat = {0};
  fstat(fd, &dbstat);
  if (dbheader->filesize != dbstat.st_size) {
    printf("Corrupted database");
    free(dbheader);
    return -1;
  };
  *headerOut = dbheader;
  return STATUS_SUCCESS;
}

int create_db_header(
    struct dbheader_t **headerOut) { // ← Just remove "int fd, "
  struct dbheader_t *dbheader = calloc(1, sizeof(struct dbheader_t));
  if (dbheader == NULL) {
    perror("calloc");
    printf("Error: DB header Memory allocation failed..\n");
    return STATUS_ERROR;
  }
  dbheader->version = 0x1;
  dbheader->count = 0;
  dbheader->magic = HEADER_MAGIC;
  dbheader->filesize = sizeof(struct dbheader_t);
  *headerOut = dbheader;
  return STATUS_SUCCESS;
}
