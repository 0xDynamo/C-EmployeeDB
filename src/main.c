// #include <bits/getopt_core.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
  // if (strcmp(argv[1], "-h") == 0) {
  printf("Usage: %s [-h]\n", argv[0]);

  printf("\n");
  printf("OPTIONS\n");
  printf("\n");
  printf("[*] required options\n");
  printf("\n");
  printf("\t[*][-f] <database file>\t\t | open existing database file\n");
  printf("\t   [-n] <database name>.db\t | create new database \n");
  printf("\n");
  return;
};

int main(int argc, char *argv[]) {
  if (argc < 2) {
    print_usage(argv);
    return -1;
  }
  char *filepath = NULL;
  char *filename = NULL;

  int dbfd = -1;
  struct db_header_t *dbheader = NULL;

  int c = 0;

  while ((c = getopt(argc, argv, "hn:f:")) != -1) {
    switch (c) {
    case 'h':
      print_usage(argv);
      return 0;
      break;
    case 'n':
      filename = optarg;
      break;
    case 'f':
      filepath = optarg;
      break;
    case '?':
      printf("Unknown option -%c\n", optopt);
      print_usage(argv);
    default:
      return -1;
    }
  }

  // Check that only one was provided
  if (filepath && filename) {
    printf("Error: Cannot use -f and -n together\n");
    return -1;
  }

  // Now call the functions after validation
  if (filename) {
    dbfd = create_db_file(filename);
    if (dbfd == -1) {
      return -1;
    }
    if (create_db_header(&dbheader) == STATUS_ERROR) {
      printf("Error: Failed to create Database Header\n");
      return -1;
    }
  } else if (filepath) {
    dbfd = open_db_file(filepath); // ← Store it!
    if (dbfd == -1) {
      return -1;
    }
    if (validate_db_header(dbfd, &dbheader) == STATUS_ERROR) {
      printf("Error: Failed to validate database header!\n");
      close(dbfd);
      return -1;
    }
  }

  output_file(dbfd, dbheader, NULL);
  return 0;
}
