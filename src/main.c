
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/config_parser.h"

int main(int argc, char *argv[]) {

  // parse the command line arguments
  if (argc < 2) {
    printf("Usage: shift <command>\n");
    return 1;
  }
  char *command = argv[1];

  // parse the config file
  workspace *ws = parse_config(command);
  if (ws == NULL) {
    return 1;
  }

  // run the command
  printf("Running command: %s\n", command);

  return 0;
}
