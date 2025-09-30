#ifndef HTTP_UTIL_H
#define HTTP_UTIL_H

char *build_req(char *response, int status_code);

char *ws_connection_req(char *domain, char *PORT, char *ws_req_key);

char *ws_connection_upgrade_res(char *ws_accept_key);

#endif