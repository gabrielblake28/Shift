#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/config_parser.h"

void *tmux_executor(workspace *ws, char *command) {

  if (ws == NULL || ws->driver == NULL || strcmp(ws->driver, "tmux") != 0) {
    return NULL;
  }

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

  char session_name[128];
  snprintf(session_name, sizeof(session_name), "shift_%s", command);

  if (ws->layout && ws->layout->tabs) {
    // Kill any existing session with same name
    char kill_cmd[256];
    snprintf(kill_cmd, sizeof(kill_cmd), "tmux kill-session -t %s 2>/dev/null", session_name);
    system(kill_cmd);
    
    // Collect all commands from all tabs
    char commands[32][256]; // Support up to 32 commands
    int command_count = 0;
    
    for (int i = 0; i < ws->layout->tab_count && command_count < 32; i++) {
      tab *t = &ws->layout->tabs[i];
      for (int j = 0; j < t->pane_count && command_count < 32; j++) {
        strncpy(commands[command_count], t->panes[j], sizeof(commands[command_count]) - 1);
        commands[command_count][sizeof(commands[command_count]) - 1] = '\0';
        command_count++;
      }
    }

    if (command_count > 0) {
      // Create new tmux session without running command immediately
      char create_cmd[1024];
      snprintf(create_cmd, sizeof(create_cmd), 
               "tmux new-session -d -s %s -c %s", 
               session_name, expanded_root);
      system(create_cmd);
      
      // Small delay to ensure session is ready
      usleep(100000); // 100ms

      // Send command to first pane
      char send_cmd[1024];
      snprintf(send_cmd, sizeof(send_cmd), 
               "tmux send-keys -t %s:0.0 '%s' Enter", 
               session_name, commands[0]);
      system(send_cmd);

      // Add remaining panes and send commands
      for (int i = 1; i < command_count; i++) {
        char split_cmd[1024];
        snprintf(split_cmd, sizeof(split_cmd), 
                 "tmux split-window -t %s -h -c %s", 
                 session_name, expanded_root);
        system(split_cmd);
        
        usleep(50000); // 50ms delay
        
        char send_cmd_pane[1024];
        snprintf(send_cmd_pane, sizeof(send_cmd_pane), 
                 "tmux send-keys -t %s:0.%d '%s' Enter", 
                 session_name, i, commands[i]);
        system(send_cmd_pane);
      }

      // Balance the layout to show all panes side by side
      char balance_cmd[256];
      snprintf(balance_cmd, sizeof(balance_cmd), 
               "tmux select-layout -t %s even-horizontal", session_name);
      system(balance_cmd);

      printf("Created tmux session '%s' with %d panes\n", session_name, command_count);
      printf("Run 'tmux attach-session -t %s' to connect\n", session_name);
    }
  }

  return NULL;
}