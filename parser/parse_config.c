
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../formatter/format_kdl.h"
#include "../include/config_parser.h"

// take in a string that will represent the command a user wants to run
// and parse out the config file to find the correct object
// what should it return?
// what should it return?

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
