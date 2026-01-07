#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *name;
  char **panes;
  int pane_count;
} tab;

typedef struct {
  tab *tabs;
  int tab_count;
} layout;

typedef struct {
  char *driver;
  char *root;
  char *window;
  char *command;   // for "exec" driver
  char **commands; // for "composite" driver
  int command_count;
  layout *layout;
} workspace;

// Function declarations
workspace *parse_config(const char *command);
void free_workspace(workspace *ws);

#endif
