
#include "cJSON.h"

#include "../include/config_parser.h"

workspace *convert_to_KDL(cJSON *json, const char *command) {

  // top level object values

  cJSON *workspace_json = cJSON_GetObjectItem(json, command);
  if (workspace_json == NULL) {
    printf("Error finding frontend object\n");
    return NULL;
  }

  workspace *ws = malloc(sizeof(workspace));

  cJSON *driver = cJSON_GetObjectItem(workspace_json, "driver");
  if (driver != NULL && cJSON_IsString(driver)) {
    ws->driver = strdup(driver->valuestring);
  };

  cJSON *root = cJSON_GetObjectItem(workspace_json, "root");
  if (root != NULL && cJSON_IsString(root)) {
    ws->root = strdup(root->valuestring);
  };

  cJSON *window = cJSON_GetObjectItem(workspace_json, "window");
  if (window != NULL && cJSON_IsString(window)) {
    ws->window = strdup(window->valuestring);
  };

  // Parse single command (for exec driver)
  cJSON *command_item = cJSON_GetObjectItem(workspace_json, "command");
  if (command_item != NULL && cJSON_IsString(command_item)) {
    ws->command = strdup(command_item->valuestring);
  }

  // Parse commands array (for composite driver)
  cJSON *commands = cJSON_GetObjectItem(workspace_json, "commands");
  if (commands != NULL && cJSON_IsArray(commands)) {
    ws->command_count = cJSON_GetArraySize(commands);
    ws->commands = malloc(ws->command_count * sizeof(char *));
    
    for (int i = 0; i < ws->command_count; i++) {
      cJSON *cmd = cJSON_GetArrayItem(commands, i);
      if (cmd != NULL && cJSON_IsString(cmd)) {
        ws->commands[i] = strdup(cmd->valuestring);
      } else {
        ws->commands[i] = NULL;
      }
    }
  }

  printf("=== Parsed Workspace ===\n");
  printf("  driver: %s\n", ws->driver ? ws->driver : "(null)");
  printf("  root: %s\n", ws->root ? ws->root : "(null)");
  printf("  window: %s\n", ws->window ? ws->window : "(null)");
  if (ws->command) {
    printf("  command: %s\n", ws->command);
  }
  if (ws->commands && ws->command_count > 0) {
    printf("  commands (%d):\n", ws->command_count);
    for (int i = 0; i < ws->command_count; i++) {
      printf("    - %s\n", ws->commands[i] ? ws->commands[i] : "(null)");
    }
  }
  printf("========================\n");

  // nested object values

  cJSON *objects = cJSON_GetObjectItem(workspace_json, "layout");
  if (objects != NULL && cJSON_IsObject(objects)) {
    // Allocate layout structure
    ws->layout = malloc(sizeof(layout));
    cJSON *tabs = cJSON_GetObjectItem(objects, "tabs");
    if (tabs != NULL && cJSON_IsArray(tabs)) {
      ws->layout->tab_count = cJSON_GetArraySize(tabs);
      ws->layout->tabs = malloc(ws->layout->tab_count * sizeof(tab));

      for (int i = 0; i < ws->layout->tab_count; i++) {
        cJSON *tab = cJSON_GetArrayItem(tabs, i);
        if (tab != NULL && cJSON_IsObject(tab)) {
          // Get tab name
          cJSON *name = cJSON_GetObjectItem(tab, "name");
          if (name != NULL && cJSON_IsString(name)) {
            ws->layout->tabs[i].name = strdup(name->valuestring);
            printf("  tab: %s\n", name->valuestring);
          }

          // Get panes
          cJSON *panes = cJSON_GetObjectItem(tab, "panes");
          if (panes != NULL && cJSON_IsArray(panes)) {
            ws->layout->tabs[i].pane_count = cJSON_GetArraySize(panes);
            ws->layout->tabs[i].panes =
                malloc(ws->layout->tabs[i].pane_count * sizeof(char *));

            for (int j = 0; j < ws->layout->tabs[i].pane_count; j++) {
              cJSON *pane = cJSON_GetArrayItem(panes, j);
              if (pane != NULL && cJSON_IsString(pane)) {
                ws->layout->tabs[i].panes[j] = strdup(pane->valuestring);
                printf("    pane: %s\n", pane->valuestring);
              }
            }
          }
        }
      }
    }
  }

  printf("\n");
  // print the workspace
  printf("workspace: driver=%s, root=%s, window=%s\n",
         ws->driver ? ws->driver : "(null)", ws->root ? ws->root : "(null)",
         ws->window ? ws->window : "(null)");

  if (ws->layout != NULL) {
    printf("  layout: %d tabs\n", ws->layout->tab_count);
    for (int i = 0; i < ws->layout->tab_count; i++) {
      printf("    tab[%d]: %s (%d panes)\n", i,
             ws->layout->tabs[i].name ? ws->layout->tabs[i].name : "(null)",
             ws->layout->tabs[i].pane_count);
      for (int j = 0; j < ws->layout->tabs[i].pane_count; j++) {
        printf("      pane[%d]: %s\n", j,
               ws->layout->tabs[i].panes[j] ? ws->layout->tabs[i].panes[j]
                                            : "(null)");
      }
    }
  }

  return ws;
}
