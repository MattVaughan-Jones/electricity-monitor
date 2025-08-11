#ifndef http_H
#define http_H

char *build_req(char *response, const int len);

char *ws_connection_req(char *domain, char *PORT, char *key);

#endif