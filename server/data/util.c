#include <string.h>

// Check if a message is a pong response
int check_is_electricity_data(char *message) {
  if (!message) {
      return 0;
  }
  
  return (strstr(message, "\"type\":\"data\"") != NULL || 
          strstr(message, "\"type\": \"data\"") != NULL);
}