
#include "cJSON.h"

#include "../include/config_parser.h"

workspace *convert_to_KDL(cJSON *json, const char *command) {

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

  printf("=== Parsed Workspace ===\n");
  printf("  driver: %s\n", ws->driver ? ws->driver : "(null)");
  printf("  root: %s\n", ws->root ? ws->root : "(null)");
  printf("  window: %s\n", ws->window ? ws->window : "(null)");
  printf("========================\n");

  return ws;
}
