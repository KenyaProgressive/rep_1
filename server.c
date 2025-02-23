#include "server.h"

void send_ping(
    /* [in] */ handle_t IDL_handle,
    /* [string][in] */ unsigned char *address,
    /* [out] */ boolean *result)
{
    ;
}

void start_server() {
    RPC_STATUS status;
    status = RpcServerUseProtoseqEp((unsigned char *)"ncacn_ip_tcp", RPC_C_PROTSEQ_MAX_REQS_DEFAULT, (unsigned char *)"8888", NULL);
    switch (status) {
        case RPC_S_OK:
            break;
        default:
            perror(status);
            break;
    }

}