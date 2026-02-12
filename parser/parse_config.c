
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../formatter/format_kdl.h"
#include "../include/config_parser.h"

workspace *parse_config(const char *command) {
  // pull in the config file
  FILE *config_file = fopen("config/shift_config.json", "r");
  if (config_file == NULL) {
    printf("Error opening config file\n");
    return NULL;
  }

  printf("config file: %s\n", "config/shift_config.json");
  // read the whole file into memory
  fseek(config_file, 0, SEEK_END);
  long file_size = ftell(config_file);
  rewind(config_file);

  char *file_contents = malloc(file_size + 1);
  fread(file_contents, 1, file_size, config_file);
  fclose(config_file);

  // parse the JSON
  cJSON *json = cJSON_Parse(file_contents);
  free(file_contents);

  if (json == NULL) {
    printf("Error parsing JSON\n");
    return NULL;
  }

  workspace *ws = convert_to_KDL(json, command);

  return ws;
}

void free_workspace(workspace *ws) {
  if (ws == NULL) {
    return;
  }

  // Free driver string
  if (ws->driver) {
    free(ws->driver);
  }

  // Free root string
  if (ws->root) {
    free(ws->root);
  }

  // Free window string
  if (ws->window) {
    free(ws->window);
  }

  // Free command string (for exec driver)
  if (ws->command) {
    free(ws->command);
  }

  // Free commands array (for composite driver)
  if (ws->commands) {
    for (int i = 0; i < ws->command_count; i++) {
      if (ws->commands[i]) {
        free(ws->commands[i]);
      }
    }
    free(ws->commands);
  }

  // Free layout structure
  if (ws->layout) {
    if (ws->layout->tabs) {
      for (int i = 0; i < ws->layout->tab_count; i++) {
        tab *t = &ws->layout->tabs[i];
        if (t->name) {
          free(t->name);
        }
        if (t->panes) {
          for (int j = 0; j < t->pane_count; j++) {
            if (t->panes[j]) {
              free(t->panes[j]);
            }
          }
          free(t->panes);
        }
      }
      free(ws->layout->tabs);
    }
    free(ws->layout);
  }

  // Free the workspace struct itself
  free(ws);
}
