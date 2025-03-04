#include "ping.h"

int main()
{
    return input_address();
}

int input_address()
{
    char symbol;
    char address[16];
    switch (scanf_s("%15s", address, sizeof(address)))
    {
    case 1:
        do
        {
            symbol = getchar();
        } while (symbol != '\n' && symbol != EOF);
        break;
    default:
        format_error_message("Invalid IP-address input");
        return -1;
    }
    char *str = address;
    int flag = validate_address(str);
    switch (flag)
    {
    case 1:
    {
        HANDLE icmp_file_to_ping = prepare_request();
        switch ((uintptr_t)icmp_file_to_ping)
        {
        case (uintptr_t)INVALID_HANDLE_VALUE:
            format_error_message("Error in preparing request\n");
            break;
        default:
            do_request(address, icmp_file_to_ping);
            IcmpCloseHandle(icmp_file_to_ping);
            return 0;
        }
        break;
    }
    default:
        format_error_message("Incorrect IP-address");
        return -1;
    }
    //printf("[DEBUG] Validation result for '%s': %d\n", address, flag);
    fflush(stdout);
    return 0;
}

int validate_address(char *str)
{
    int flag = 1;
    int len = strlen(str);
    int decision = (len >= 7 && len <= 15);

    switch (decision)
    {
    case 0:
        flag = 0;
        break;
    default:
        break;
    }

    int count_dot = 0;
    int digit_count = 0;
    int current_octet = 0;
    int i = 0;

    do
    {
        switch (str[i])
        {
        case '.':
            switch (digit_count == 0 || current_octet > 255)
            {
            case 1:
                flag = 0;
                break;
            default:
                count_dot++;
                digit_count = 0;
                current_octet = 0;
                break;
            }
            break;

        default:
            switch (isdigit(str[i]))
            {
            case 0:
                flag = 0;
                break;
            default:
                digit_count++;
                current_octet = current_octet * 10 + (str[i] - '0');
                switch (digit_count > 3 || current_octet > 255)
                {
                case 1:
                    flag = 0;
                    break;
                default:
                    break;
                }
                break;
            }
            break;
        }
        i++;
    } while (str[i] != '\0' && flag != 0);

    switch (count_dot != 3 || digit_count == 0 || current_octet > 255)
    {
    case 1:
        flag = 0;
        break;
    default:
        break;
    }

    return flag;
}

HANDLE prepare_request()
{
    HANDLE icmp_file_to_ping = IcmpCreateFile();
    //printf("[DEBUG] ICMP Handle: %p\n", icmp_file_to_ping);
    return icmp_file_to_ping;
}

int do_request(char *address, HANDLE fl)
{
    char test_message[] = "Testing PING";

    DWORD size_of_reply = sizeof(ICMP_ECHO_REPLY) + sizeof(test_message);
    LPVOID message_buff = malloc(size_of_reply);
    switch ((bool)message_buff)
    {
    case 0:
        format_error_message("Memory allocation error");
        return -1;
    default:
        break;
    }
    IPAddr destination_address = inet_addr(address);
    switch ((int)destination_address)
    {
    case -1:
        format_error_message("Invalid address format");
        free(message_buff);
        return -1;
    default:
        break;
    }
    printf("Sending ping to %s...\n", address);
    fflush(stdout);
    DWORD reply_value = IcmpSendEcho(fl, destination_address, test_message, sizeof(test_message), NULL, message_buff,
                                     size_of_reply, 3000);
    switch (reply_value)
    {
    case 0:
        format_error_message("Request time out");
        free(message_buff);
        break;

    default:
    {
        PICMP_ECHO_REPLY reply = (PICMP_ECHO_REPLY)message_buff;
        printf("Reply from %s: bytes=%d time=%ldms TTL=%ld\n", inet_ntoa(*(struct in_addr *)&reply->Address), reply->DataSize, reply->RoundTripTime, reply->Status);
        free(message_buff);
        break;
    }
    break;
    }

    return 0;
}

void format_error_message(const char *msg)
{
    printf("%s", msg);
    fflush(stdout);
}
