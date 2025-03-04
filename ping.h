#ifndef PING_H
#define PING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <winsock2.h>
#include <unistd.h>
#include <ipexport.h>
#include <icmpapi.h>
#include <stdint.h>

int input_address();
int validate_address(char *str);
HANDLE prepare_request();
int do_request(char *address, HANDLE fl);
void format_error_message(const char *msg);

#endif
