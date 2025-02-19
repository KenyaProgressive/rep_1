#include "ping.h"

int main()
{
    input_address();
    return 0;
}

// rpc icmp

int input_address()
{
    char address[16];
    switch (scanf("%s", address)) {
        case 1:
            break;
        default:
            perror("Invalid IP-address input");
            return 1;
    }
    address[16] = '\0';
    char *str = address;
    int flag = validate_address(str);
    switch (flag)
    {
    case 1:
        printf("YES");
        break;
    default:
        printf("Incorrect IP-address");
        break;
    }

    return 0;
}

int validate_address(char *str)
{
    int flag = 1;
    int decision = strlen(str) >= 7 && strlen(str) <= 15;
    switch (decision)
    {
    case 1:
        break;
    default:
        flag = 0;
        break;
    }
    int count_dot = 0; // dots counter
    int num = 0;
    do
    {
        switch (*str)
        {
        case '.':
            count_dot += 1;
            break;
        default:
            num = isdigit(*str);
            switch(num) {
                case 0:
                    flag = 0;
                    break;
                default:
                    break;
            break;
            }
        }
        str++;
    } while (*str != '\0');

    return flag;
}
