#include "ping.h"

int main()
{
    return input_address();
}

int input_address()
{
    const char filename[] = "ping_log.log";
    logger_launch(filename);
    char symbol;
    char address[16];
    switch (scanf_s("%15s", address, sizeof(address)))
    {
    case 1:
        do
        {
            symbol = getchar();
        } while (symbol != '\n' && symbol != EOF);
        log_do(INFO, "Success reading IP: %s", filename);
        break;
    default:
        DWORD err = GetLastError();
        LPSTR err_msg = NULL;
        message_error_print("Invalid IP-address input", err_msg, err);
        log_do(ERR, "Invalid IP-address input: %s", filename);
        return -1;
    }
    char *str = address;
    int flag = validate_address(str);
    switch (flag)
    {
    case 1:
    {
        log_do(INFO, "Success validation IP: %s", filename);
        HANDLE icmp_file_to_ping = prepare_request();
        switch ((uintptr_t)icmp_file_to_ping)
        {
        case (uintptr_t)INVALID_HANDLE_VALUE:
            DWORD err = GetLastError();
            LPSTR err_msg = NULL;
            message_error_print("ERR in preparing request", err_msg, err);
            log_do(ERR, "Preparing request ERR: %s", filename);
            return -1;
        default:
            do_request(address, icmp_file_to_ping, filename);
            IcmpCloseHandle(icmp_file_to_ping);
            return 0;
        }
        break;
    }
    default:
        DWORD err = GetLastError();
        LPSTR err_msg = NULL;
        message_error_print("Incorrect IP-address:", err_msg, err);
        log_do(ERR, "Incorrect IP-address: %s", filename);
        return -1;
    }
    // printf("[DEBUG] Validation result for '%s': %d\n", address, flag);
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
    // printf("[DEBUG] ICMP Handle: %p\n", icmp_file_to_ping);
    return icmp_file_to_ping;
}

int do_request(char *address, HANDLE fl, const char *filename)
{
    char test_message[] = "Testing PING";

    DWORD size_of_reply = sizeof(ICMP_ECHO_REPLY) + (sizeof(test_message) + 1);
    LPVOID message_buff = malloc(size_of_reply);
    switch ((bool)message_buff)
    {
    case 0:
        DWORD err = GetLastError();
        LPSTR err_msg = NULL;
        message_error_print("Memory allocation ERR", err_msg, err);
        log_do(ERR, "Memory allocation ERR: %s", filename);
        return -1;
    default:
        break;
    }
    IPAddr destination_address = inet_addr(address);
    switch ((int)destination_address)
    {
    case -1:
        DWORD err = GetLastError();
        LPSTR err_msg = NULL;
        message_error_print("Invalid address format", err_msg, err);
        log_do(ERR, "Invalid address format: %s", filename);
        free(message_buff);
        return -1;
    default:
        break;
    }
    printf("Sending ping to %s...\n", address);
    fflush(stdout);
    int packet_sent = 0;
    do
    {
        DWORD reply_value = IcmpSendEcho(fl, destination_address, test_message, sizeof(test_message), NULL, message_buff,
                                         size_of_reply, 1000);
        switch (reply_value)
        {
        case 0:
            DWORD err = GetLastError();
            LPSTR err_msg = NULL;
            message_error_print("Request time out", err_msg, err);
            log_do(ERR, "Request time out: %s", filename);
            break;

        default:
        {
            PICMP_ECHO_REPLY reply = (PICMP_ECHO_REPLY)message_buff;
            switch (reply->Status)
            {
            case IP_SUCCESS:
                break;
            default:
                printf("ICMP-ERR with status: %ld\n", reply->Status);
                log_do(ERR, "ICMP-ERR: %s", filename);
                break;
            }
            printf("Reply from %s: bytes=%d time=%ldms TTL=%d\n", inet_ntoa(*(struct in_addr *)&reply->Address), reply->DataSize, reply->RoundTripTime, reply->Options.Ttl);
            packet_sent += 1;
            break;
        }
        }
        Sleep(1000);
    } while (packet_sent < 4);
    log_do(INFO, "Success sending ping: %s", filename);
    logger_close(filename);
    free(message_buff);
    return 0;
}

void message_error_print(const char *msg, LPSTR err_msg, DWORD err_id)
{
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        err_id,
        MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
        (LPSTR)&err_msg,
        0,
        NULL);
    printf("%s: %s", msg, err_msg);
    LocalFree(err_msg);
}