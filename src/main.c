#include <fcntl.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/common.h"
#include "../include/file.h"
#include "../include/parse.h"

void print_usage(char *argv[]) {

  printf("Usage: %s -n -f <datbase path>\n", argv[0]);
  printf("\t -n - create a new database file\n");
  printf("\t -f - (required) path to a database file\n");
  return;
}

int main(int argc, char *argv[]) {

  int c;
  char *db_path = NULL;
  bool new_file = false;

  while ((c = getopt(argc, argv, "nf:")) != -1) {
    switch (c) {
    case 'n':
      new_file = true;
      break;

    case 'f':
      db_path = optarg;
      break;

    case '?':
      printf("Unknown param -%c\n", c);
      break;

    default:
      return -1;
    }
  }

  if (db_path == NULL) {
    printf("Database path is a required argument\n");
    print_usage(argv);
    return 0;
  }

  int dbfd;
  struct db_header_t *dbhdr = NULL;

  if (new_file) {
    dbfd = db_create_file(db_path);
    if (dbfd == STATUS_ERROR) {
      printf("Could not create the database file\n");
      return -1;
    }

    if (create_db_header(dbfd, &dbhdr) == STATUS_ERROR) {
      printf("Failed to create a database header\n");
      close(dbfd);
      return -1;
    };

  } else {
    dbfd = db_open_file(db_path);
    if (dbfd == STATUS_ERROR) {
      printf("Could not open the database file\n");
      return -1;
    }
    if (validate_db_header(dbfd, &dbhdr) == STATUS_ERROR) {
      close(dbfd);
      printf("Invalid database file header \n");
      return -1;
    }
  }

  printf("new file?: %d\n", new_file);
  printf("db path:  %s\n", db_path);

  struct employee_t *employees = NULL;

  if (read_employees(dbfd, dbhdr, &employees) == STATUS_ERROR) {
    close(dbfd);
    free(employees);
    printf("Could not read employees from database file \n");
    return -1;
  };

  output_file(dbfd, dbhdr);

  return 0;
}
