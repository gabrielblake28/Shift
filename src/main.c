
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/config_parser.h"
#include "../executor/zellij_executor.h"
#include "../executor/tmux_executor.h"

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

  // execute based on driver
  if (ws->driver && strcmp(ws->driver, "zellij") == 0) {
    zellij_executor(ws, command);
  } else if (ws->driver && strcmp(ws->driver, "tmux") == 0) {
    tmux_executor(ws, command);
  } else {
    printf("Unsupported driver: %s\n", ws->driver ? ws->driver : "(null)");
  }

  return 0;
}
