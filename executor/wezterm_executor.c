#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/config_parser.h"

// Find the wezterm binary in various locations
static char *find_wezterm_binary(void) {
  static char wezterm_path[1024];
  
  // Try common locations first
  const char *paths[] = {
    "wezterm",  // Try PATH first
    "/Applications/WezTerm.app/Contents/MacOS/wezterm",
    "/usr/local/bin/wezterm",
    "/opt/homebrew/bin/wezterm",
    NULL
  };
  
  for (int i = 0; paths[i] != NULL; i++) {
    if (strcmp(paths[i], "wezterm") == 0) {
      // Check if wezterm is in PATH
      if (system("which wezterm >/dev/null 2>&1") == 0) {
        return "wezterm";
      }
    } else if (access(paths[i], X_OK) == 0) {
      strncpy(wezterm_path, paths[i], sizeof(wezterm_path) - 1);
      wezterm_path[sizeof(wezterm_path) - 1] = '\0';
      return wezterm_path;
    }
  }
  
  // Try to find running wezterm process and derive path
  FILE *fp = popen("ps aux | grep wezterm-gui | grep -v grep | head -1 | awk '{print $11}'", "r");
  if (fp) {
    char line[1024];
    if (fgets(line, sizeof(line), fp) != NULL) {
      // Remove newline
      line[strcspn(line, "\n")] = 0;
      if (strlen(line) > 0) {
        // Replace "wezterm-gui" with "wezterm" in the path
        char *gui_pos = strstr(line, "wezterm-gui");
        if (gui_pos) {
          strncpy(wezterm_path, line, gui_pos - line);
          wezterm_path[gui_pos - line] = '\0';
          strcat(wezterm_path, "wezterm");
          if (access(wezterm_path, X_OK) == 0) {
            pclose(fp);
            return wezterm_path;
          }
        }
      }
    }
    pclose(fp);
  }
  
  return NULL;
}

void *wezterm_executor(workspace *ws, char *command) {

  if (ws == NULL || ws->driver == NULL || strcmp(ws->driver, "wezterm") != 0) {
    return NULL;
  }

  // Find wezterm binary
  char *wezterm_bin = find_wezterm_binary();
  if (wezterm_bin == NULL) {
    fprintf(stderr, "Error: wezterm binary not found. Please ensure WezTerm is installed.\n");
    return NULL;
  }
  
  printf("Using wezterm: %s\n", wezterm_bin);

  // Expand ~ in root path
  char expanded_root[512];
  if (ws->root && strstr(ws->root, "~/") == ws->root) {
    const char *home = getenv("HOME");
    if (home) {
      snprintf(expanded_root, sizeof(expanded_root), "%s%s", home, ws->root + 1);
    } else {
      strncpy(expanded_root, ws->root, sizeof(expanded_root));
    }
  } else {
    strncpy(expanded_root, ws->root ? ws->root : ".", sizeof(expanded_root));
  }

  if (ws->layout && ws->layout->tabs) {
    int first_tab = 1;
    
    for (int i = 0; i < ws->layout->tab_count; i++) {
      tab *t = &ws->layout->tabs[i];
      
      for (int j = 0; j < t->pane_count; j++) {
        char spawn_cmd[2048];
        
        // Spawn creates a new tab by default when WEZTERM_PANE is set
        snprintf(spawn_cmd, sizeof(spawn_cmd),
                 "\"%s\" cli spawn --cwd %s -- %s",
                 wezterm_bin, expanded_root, t->panes[j]);
        
        printf("Executing: %s\n", spawn_cmd);
        int result = system(spawn_cmd);
        if (result != 0) {
          fprintf(stderr, "Warning: command failed with exit code %d\n", result);
        }
        usleep(100000); // 100ms delay between spawns
      }
    }

    printf("Created WezTerm tabs for '%s'\n", command);
  }

  return NULL;
}
