#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/config_parser.h"
#include "wezterm_executor.h"

void *composite_executor(workspace *ws, char *command) {
  if (ws == NULL || ws->driver == NULL || strcmp(ws->driver, "composite") != 0) {
    return NULL;
  }

  if (ws->commands == NULL || ws->command_count == 0) {
    fprintf(stderr, "Error: composite workspace '%s' has no commands to execute\n", command);
    return NULL;
  }

  printf("Executing composite workspace '%s' with %d command(s)...\n", command, ws->command_count);

  for (int i = 0; i < ws->command_count; i++) {
    char *sub_command = ws->commands[i];
    printf("\n[%d/%d] Executing workspace: %s\n", i + 1, ws->command_count, sub_command);

    workspace *sub_ws = parse_config(sub_command);
    if (sub_ws == NULL) {
      fprintf(stderr, "Error: Failed to load workspace '%s'\n", sub_command);
      continue;
    }

    if (sub_ws->driver == NULL) {
      fprintf(stderr, "Error: Workspace '%s' has no driver specified\n", sub_command);
      free_workspace(sub_ws);
      continue;
    }

    if (strcmp(sub_ws->driver, "wezterm") == 0) {
      wezterm_executor(sub_ws, sub_command);
    } else if (strcmp(sub_ws->driver, "composite") == 0) {
      fprintf(stderr, "Warning: Nested composite workspaces are not supported (skipping '%s')\n", sub_command);
    } else {
      fprintf(stderr, "Unsupported driver '%s' in workspace '%s'\n", sub_ws->driver, sub_command);
    }

    free_workspace(sub_ws);
  }

  printf("\nComposite workspace '%s' completed.\n", command);
  return NULL;
}
