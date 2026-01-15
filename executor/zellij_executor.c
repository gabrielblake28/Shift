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

  // Write KDL layout
  fprintf(kdl_file, "layout {\n");
  if (ws->root) {
    fprintf(kdl_file, "    cwd \"%s\";\n", ws->root);
  }

  if (ws->layout && ws->layout->tabs) {
    for (int i = 0; i < ws->layout->tab_count; i++) {
      tab *t = &ws->layout->tabs[i];
      fprintf(kdl_file, "    tab name=\"%s\"%s {\n", 
              t->name ? t->name : "Tab", 
              i == 0 ? " focus=true" : "");
      
      if (t->panes && t->pane_count > 0) {
        for (int j = 0; j < t->pane_count; j++) {
          // Parse command and args
          char *cmd = strdup(t->panes[j]);
          char *space = strchr(cmd, ' ');
          if (space) {
            *space = '\0';
            // Find the end of the args string (remove trailing spaces)
            char *args = space + 1;
            while (*args && *args == ' ') args++;
            
            // Build args array by splitting on spaces
            char *arg_start = args;
            int arg_count = 0;
            fprintf(kdl_file, "        pane command=\"%s\"", cmd);
            
            if (*args) {
              fprintf(kdl_file, " args=\"%s\"", args);
            }
          } else {
            fprintf(kdl_file, "        pane command=\"%s\"", cmd);
          }
          
          if (ws->root) {
            fprintf(kdl_file, " cwd=\"%s\"", ws->root);
          }
          
          fprintf(kdl_file, ";\n");
          free(cmd);
        }
      }
      
      fprintf(kdl_file, "    }\n");
    }
  }

  fprintf(kdl_file, "}\n");
  fclose(kdl_file);

  // Execute zellij with the layout
  char exec_cmd[512];
  if (strcmp(ws->window, "new") == 0) {
    snprintf(exec_cmd, sizeof(exec_cmd), "zellij --layout \"%s\"", kdl_path);
  } else {
    snprintf(exec_cmd, sizeof(exec_cmd), "zellij options --default-layout \"%s\"", kdl_path);
  }

  system(exec_cmd);
  
  // Clean up
  // unlink(kdl_path); // Commented for debugging
  
  return NULL;
}