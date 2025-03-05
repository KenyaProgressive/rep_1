#ifndef PING_H
#define PING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <winsock2.h>
#include <ipexport.h>
#include <icmpapi.h>
#include <stdint.h>
#include "logger.h"

int input_address();
int validate_address(char *str);
HANDLE prepare_request();
int do_request(char *address, HANDLE fl, const char *filename);
void message_error_print(const char *msg, LPSTR err_msg, DWORD err_id);
void print_statistics(int ps, int pr, DWORD mt, DWORD mxt, DWORD ttm, const char *filename);

#endif
