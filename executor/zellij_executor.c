#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/config_parser.h"

void *zellij_executor(workspace *ws, char *command) {

  if (ws == NULL || ws->driver == NULL || strcmp(ws->driver, "zellij") != 0) {
    return NULL;
  }

  // Create temporary KDL file
  char kdl_path[256];
  snprintf(kdl_path, sizeof(kdl_path), "/tmp/zellij_%s.kdl", command);
  
  FILE *kdl_file = fopen(kdl_path, "w");
  if (!kdl_file) {
    return NULL;
  }

  // Write KDL layout - simplified structure
  fprintf(kdl_file, "layout {\n");
  
  if (ws->root) {
    const char *home = getenv("HOME");
    if (home && strstr(ws->root, "~/") == ws->root) {
      fprintf(kdl_file, "    cwd \"%s%s\";\n", home, ws->root + 1);
    } else {
      fprintf(kdl_file, "    cwd \"%s\";\n", ws->root);
    }
  }

  if (ws->layout && ws->layout->tabs) {
    // For simplicity, just put all panes in one tab with split
    fprintf(kdl_file, "    tab name=\"Workspace\" {\n");
    
    for (int i = 0; i < ws->layout->tab_count; i++) {
      tab *t = &ws->layout->tabs[i];
      if (t->panes && t->pane_count > 0) {
        for (int j = 0; j < t->pane_count; j++) {
          // Parse command and args
          char *cmd = strdup(t->panes[j]);
          char *space = strchr(cmd, ' ');
          if (space) {
            *space = '\0';
            char *args = space + 1;
            while (*args && *args == ' ') args++;
            fprintf(kdl_file, "        pane command=\"%s\"", cmd);
            if (*args) {
              fprintf(kdl_file, " args=\"%s\"", args);
            }
          } else {
            fprintf(kdl_file, "        pane command=\"%s\"", cmd);
          }
          fprintf(kdl_file, ";\n");
          free(cmd);
        }
      }
    }
    
    fprintf(kdl_file, "    }\n");
  }

  fprintf(kdl_file, "}\n");
  fclose(kdl_file);

  // Execute zellij with the layout
  char exec_cmd[512];
  if (strcmp(ws->window, "new") == 0) {
    snprintf(exec_cmd, sizeof(exec_cmd), "TERM=xterm-256color zellij --layout \"%s\"", kdl_path);
  } else {
    snprintf(exec_cmd, sizeof(exec_cmd), "TERM=xterm-256color zellij options --default-layout \"%s\"", kdl_path);
  }

  system(exec_cmd);
  
  // Clean up
  // unlink(kdl_path); // Keep for debugging
  
  return NULL;
}