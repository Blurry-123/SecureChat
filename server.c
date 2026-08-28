#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_CLIENTS 50
#define MAX_USERNAME 50
#define MAX_KEY 100
#define MAX_FRAME_SIZE (1024 * 1024 + 8192)
#define MAX_FILE_SIZE (1024 * 1024)
#define MAX_MESSAGE_SIZE 4096

typedef struct
{
    SOCKET socket;
    char username[MAX_USERNAME];
    char key[MAX_KEY];
    int active;
} Client;

Client clients[MAX_CLIENTS];

CRITICAL_SECTION clients_lock;


int send_all(SOCKET socket, const char *data, int length)
{
    int total = 0;

    while (total < length)
    {
        int n = send(
            socket,
            data + total,
            length - total,
            0
        );

        if (n == SOCKET_ERROR)
        {
            return 0;
        }

        total += n;
    }

    return 1;
}


int recv_all(SOCKET socket, char *buffer, int length)
{
    int total = 0;

    while (total < length)
    {
        int n = recv(
            socket,
            buffer + total,
            length - total,
            0
        );

        if (n <= 0)
        {
            return 0;
        }

        total += n;
    }

    return 1;
}


int send_frame(
    SOCKET socket,
    const char *data,
    int length)
{
    uint32_t network_length;

    network_length =
        htonl((uint32_t)length);

    if (!send_all(
            socket,
            (char *)&network_length,
            sizeof(network_length)))
    {
        return 0;
    }

    return send_all(
        socket,
        data,
        length
    );
}


int recv_frame(
    SOCKET socket,
    char **data,
    int *length)
{
    uint32_t network_length;

    if (!recv_all(
            socket,
            (char *)&network_length,
            sizeof(network_length)))
    {
        return 0;
    }

    *length =
        (int)ntohl(network_length);

    if (*length <= 0 ||
        *length > MAX_FRAME_SIZE)
    {
        return 0;
    }

    *data =
        malloc((size_t)*length + 1);

    if (*data == NULL)
    {
        return 0;
    }

    if (!recv_all(
            socket,
            *data,
            *length))
    {
        free(*data);
        *data = NULL;

        return 0;
    }

    (*data)[*length] = '\0';

    return 1;
}


void xor_crypt(
    char *data,
    int length,
    const char *key)
{
    int key_length =
        (int)strlen(key);

    if (key_length == 0)
    {
        return;
    }

    for (int i = 0; i < length; i++)
    {
        data[i] ^=
            key[i % key_length];
    }
}


int send_encrypted(
    SOCKET socket,
    const char *key,
    const char *data,
    int length)
{
    char *copy;

    copy =
        malloc((size_t)length);

    if (copy == NULL)
    {
        return 0;
    }

    memcpy(
        copy,
        data,
        length
    );

    xor_crypt(
        copy,
        length,
        key
    );

    int result =
        send_frame(
            socket,
            copy,
            length
        );

    free(copy);

    return result;
}


int recv_encrypted(
    SOCKET socket,
    const char *key,
    char **data,
    int *length)
{
    if (!recv_frame(
            socket,
            data,
            length))
    {
        return 0;
    }

    xor_crypt(
        *data,
        *length,
        key
    );

    return 1;
}


int find_client(
    const char *username)
{
    for (int i = 0;
         i < MAX_CLIENTS;
         i++)
    {
        if (clients[i].active &&
            strcmp(
                clients[i].username,
                username) == 0)
        {
            return i;
        }
    }

    return -1;
}


void remove_client(int index)
{
    EnterCriticalSection(
        &clients_lock
    );

    if (clients[index].active)
    {
        printf(
            "Disconnected: %s\n",
            clients[index].username
        );

        closesocket(
            clients[index].socket
        );

        clients[index].active = 0;

        clients[index].username[0] =
            '\0';

        clients[index].key[0] =
            '\0';

        clients[index].socket =
            INVALID_SOCKET;
    }

    LeaveCriticalSection(
        &clients_lock
    );
}


void send_error(
    int index,
    const char *message)
{
    send_encrypted(
        clients[index].socket,
        clients[index].key,
        message,
        (int)strlen(message)
    );
}


/* =========================================================
   SEND MESSAGE
   Command:
   SEND TO username message
   ========================================================= */

void handle_send(
    int sender_index,
    char *command)
{
    char target[MAX_USERNAME];
    char *message;
    char *output;

    message =
        malloc(MAX_MESSAGE_SIZE);

    if (message == NULL)
    {
        send_error(
            sender_index,
            "ERROR memory allocation failed"
        );

        return;
    }

    output =
        malloc(MAX_MESSAGE_SIZE);

    if (output == NULL)
    {
        free(message);

        send_error(
            sender_index,
            "ERROR memory allocation failed"
        );

        return;
    }

    memset(
        target,
        0,
        sizeof(target)
    );

    memset(
        message,
        0,
        MAX_MESSAGE_SIZE
    );

    if (sscanf(
            command,
            "SEND TO %49s %4095[^\n]",
            target,
            message) != 2)
    {
        free(message);
        free(output);

        send_error(
            sender_index,
            "ERROR invalid command format"
        );

        return;
    }

    EnterCriticalSection(
        &clients_lock
    );

    int receiver_index =
        find_client(target);

    if (receiver_index == -1)
    {
        LeaveCriticalSection(
            &clients_lock
        );

        free(message);
        free(output);

        send_error(
            sender_index,
            "ERROR user is not online"
        );

        return;
    }

    int length =
        snprintf(
            output,
            MAX_MESSAGE_SIZE,
            "FROM %s: %s",
            clients[sender_index].username,
            message
        );

    if (length < 0 ||
        length >= MAX_MESSAGE_SIZE)
    {
        LeaveCriticalSection(
            &clients_lock
        );

        free(message);
        free(output);

        send_error(
            sender_index,
            "ERROR message too long"
        );

        return;
    }

    int result =
        send_encrypted(
            clients[receiver_index].socket,
            clients[receiver_index].key,
            output,
            length
        );

    LeaveCriticalSection(
        &clients_lock
    );

    free(message);
    free(output);

    if (!result)
    {
        send_error(
            sender_index,
            "ERROR could not send message"
        );

        return;
    }

    send_encrypted(
        clients[sender_index].socket,
        clients[sender_index].key,
        "OK MESSAGE SENT",
        15
    );
}


/* =========================================================
   LIST
   ========================================================= */

void handle_list(int index)
{
    char *output;

    output =
        malloc(MAX_MESSAGE_SIZE);

    if (output == NULL)
    {
        send_error(
            index,
            "ERROR memory allocation failed"
        );

        return;
    }

    strcpy(
        output,
        "ONLINE "
    );

    int first = 1;

    EnterCriticalSection(
        &clients_lock
    );

    for (int i = 0;
         i < MAX_CLIENTS;
         i++)
    {
        if (clients[i].active)
        {
            if (!first)
            {
                strcat(
                    output,
                    ", "
                );
            }

            strcat(
                output,
                clients[i].username
            );

            first = 0;
        }
    }

    LeaveCriticalSection(
        &clients_lock
    );

    send_encrypted(
        clients[index].socket,
        clients[index].key,
        output,
        (int)strlen(output)
    );

    free(output);
}


/* =========================================================
   FILE TRANSFER
   ========================================================= */

void handle_file(
    int sender_index,
    char *data,
    int total_length)
{
    char *newline;

    newline =
        strchr(data, '\n');

    if (newline == NULL)
    {
        send_error(
            sender_index,
            "ERROR invalid file format"
        );

        return;
    }

    int header_length =
        (int)(newline - data);

    char header[1024];

    if (header_length >=
        (int)sizeof(header))
    {
        send_error(
            sender_index,
            "ERROR file header too long"
        );

        return;
    }

    memcpy(
        header,
        data,
        header_length
    );

    header[header_length] =
        '\0';

    char target[MAX_USERNAME];
    char filename[256];
    int file_size;

    if (sscanf(
            header,
            "SENDFILE TO %49s %255s %d",
            target,
            filename,
            &file_size) != 3)
    {
        send_error(
            sender_index,
            "ERROR invalid file command"
        );

        return;
    }

    if (file_size < 0 ||
        file_size > MAX_FILE_SIZE)
    {
        send_error(
            sender_index,
            "ERROR file too large"
        );

        return;
    }

    int payload_size =
        total_length -
        header_length -
        1;

    if (payload_size != file_size)
    {
        send_error(
            sender_index,
            "ERROR invalid file size"
        );

        return;
    }

    const char *extension =
        strrchr(filename, '.');

    if (extension == NULL ||
        _stricmp(
            extension,
            ".txt") != 0)
    {
        send_error(
            sender_index,
            "ERROR only .txt files are supported"
        );

        return;
    }

    EnterCriticalSection(
        &clients_lock
    );

    int receiver_index =
        find_client(target);

    if (receiver_index == -1)
    {
        LeaveCriticalSection(
            &clients_lock
        );

        send_error(
            sender_index,
            "ERROR user is not online"
        );

        return;
    }

    char receiver_header[1024];

    int receiver_header_length =
        snprintf(
            receiver_header,
            sizeof(receiver_header),
            "RECVFILE FROM %s %s %d\n",
            clients[sender_index].username,
            filename,
            file_size
        );

    if (receiver_header_length < 0 ||
        receiver_header_length >=
            (int)sizeof(receiver_header))
    {
        LeaveCriticalSection(
            &clients_lock
        );

        send_error(
            sender_index,
            "ERROR file header too large"
        );

        return;
    }

    int new_length =
        receiver_header_length +
        file_size;

    char *new_data =
        malloc((size_t)new_length);

    if (new_data == NULL)
    {
        LeaveCriticalSection(
            &clients_lock
        );

        send_error(
            sender_index,
            "ERROR server memory failure"
        );

        return;
    }

    memcpy(
        new_data,
        receiver_header,
        receiver_header_length
    );

    if (file_size > 0)
    {
        memcpy(
            new_data +
                receiver_header_length,
            newline + 1,
            file_size
        );
    }

    int result =
        send_encrypted(
            clients[receiver_index].socket,
            clients[receiver_index].key,
            new_data,
            new_length
        );

    free(new_data);

    LeaveCriticalSection(
        &clients_lock
    );

    if (!result)
    {
        send_error(
            sender_index,
            "ERROR could not send file"
        );

        return;
    }

    send_encrypted(
        clients[sender_index].socket,
        clients[sender_index].key,
        "OK FILE SENT",
        12
    );
}


/* =========================================================
   CLIENT THREAD
   ========================================================= */

DWORD WINAPI client_thread(
    LPVOID argument)
{
    int index =
        *(int *)argument;

    free(argument);

    SOCKET socket =
        clients[index].socket;


    /* =====================================================
       RECEIVE REGISTRATION
       ===================================================== */

    char *registration = NULL;
    int registration_length;

    printf(
        "Waiting for registration...\n"
    );

    if (!recv_frame(
            socket,
            &registration,
            &registration_length))
    {
        printf(
            "Registration receive failed. Error: %d\n",
            WSAGetLastError()
        );

        remove_client(index);

        return 0;
    }

    printf(
        "Registration packet received: %s\n",
        registration
    );


    /* =====================================================
       CHECK REGISTRATION
       ===================================================== */

    char command[20];
    char username[MAX_USERNAME];
    char key[MAX_KEY];

    memset(
        command,
        0,
        sizeof(command)
    );

    memset(
        username,
        0,
        sizeof(username)
    );

    memset(
        key,
        0,
        sizeof(key)
    );

    if (sscanf(
            registration,
            "%19s %49s %99s",
            command,
            username,
            key) != 3 ||
        strcmp(
            command,
            "REGISTER") != 0)
    {
        send_frame(
            socket,
            "ERROR invalid registration",
            26
        );

        free(registration);

        remove_client(index);

        return 0;
    }

    free(registration);

    if (strlen(username) == 0 ||
        strlen(key) == 0)
    {
        send_frame(
            socket,
            "ERROR invalid registration",
            26
        );

        remove_client(index);

        return 0;
    }


    /* =====================================================
       CHECK DUPLICATE USERNAME
       ===================================================== */

    EnterCriticalSection(
        &clients_lock
    );

    if (find_client(username) != -1)
    {
        LeaveCriticalSection(
            &clients_lock
        );

        send_encrypted(
            socket,
            key,
             "ERROR username already exists",
             (int)strlen("ERROR username already exists")
        );

        remove_client(index);

        return 0;
    }

    strcpy(
        clients[index].username,
        username
    );

    strcpy(
        clients[index].key,
        key
    );

    LeaveCriticalSection(
        &clients_lock
    );


    /* =====================================================
       SEND REGISTRATION RESPONSE
       ===================================================== */

    char welcome[128];

    snprintf(
        welcome,
        sizeof(welcome),
        "OK REGISTERED %s",
        username
    );

    printf(
        "Sending registration response to %s...\n",
        username
    );

    if (!send_encrypted(
            socket,
            key,
            welcome,
            (int)strlen(welcome)))
    {
        printf(
            "Failed to send registration response to %s. Error: %d\n",
            username,
            WSAGetLastError()
        );

        remove_client(index);

        return 0;
    }

    printf(
        "Registered: %s\n",
        username
    );


    /* =====================================================
       COMMAND LOOP
       ===================================================== */

    while (1)
    {
        char *data = NULL;
        int length;

        if (!recv_encrypted(
                socket,
                clients[index].key,
                &data,
                &length))
        {
            free(data);

            printf(
                "Connection lost: %s. Error: %d\n",
                clients[index].username,
                WSAGetLastError()
            );

            break;
        }

        printf(
            "Command from %s: %s\n",
            clients[index].username,
            data
        );

        if (strncmp(
                data,
                "SENDFILE TO ",
                12) == 0)
        {
            handle_file(
                index,
                data,
                length
            );
        }
        else if (strncmp(
                     data,
                     "SEND TO ",
                     8) == 0)
        {
            handle_send(
                index,
                data
            );
        }
        else if (strcmp(
                     data,
                     "LIST") == 0)
        {
            handle_list(index);
        }
        else if (strcmp(
                     data,
                     "QUIT") == 0)
        {
            char goodbye[128];

            snprintf(
                goodbye,
                sizeof(goodbye),
                "GOODBYE %s",
                clients[index].username
            );

            send_encrypted(
                socket,
                clients[index].key,
                goodbye,
                (int)strlen(goodbye)
            );

            free(data);

            break;
        }
        else
        {
            send_error(
                index,
                "ERROR unknown command"
            );
        }

        free(data);
    }

    remove_client(index);

    return 0;
}


/* =========================================================
   MAIN
   ========================================================= */

int main(
    int argc,
    char *argv[])
{
    if (argc != 2)
    {
        printf(
            "Usage: server.exe <port>\n"
        );

        printf(
            "Example: server.exe 8080\n"
        );

        return 1;
    }

    int port =
        atoi(argv[1]);

    if (port <= 0 ||
        port > 65535)
    {
        printf(
            "Invalid port number.\n"
        );

        return 1;
    }


    /* =====================================================
       WINSOCK
       ===================================================== */

    WSADATA wsa;

    if (WSAStartup(
            MAKEWORD(2, 2),
            &wsa) != 0)
    {
        printf(
            "WSAStartup failed.\n"
        );

        return 1;
    }

    InitializeCriticalSection(
        &clients_lock
    );


    /* =====================================================
       INITIALIZE CLIENT ARRAY
       ===================================================== */

    for (int i = 0;
         i < MAX_CLIENTS;
         i++)
    {
        clients[i].active = 0;

        clients[i].socket =
            INVALID_SOCKET;

        clients[i].username[0] =
            '\0';

        clients[i].key[0] =
            '\0';
    }


    /* =====================================================
       CREATE SERVER SOCKET
       ===================================================== */

    SOCKET server_socket =
        socket(
            AF_INET,
            SOCK_STREAM,
            IPPROTO_TCP
        );

    if (server_socket ==
        INVALID_SOCKET)
    {
        printf(
            "Socket creation failed.\n"
        );

        DeleteCriticalSection(
            &clients_lock
        );

        WSACleanup();

        return 1;
    }


    /* =====================================================
       REUSE ADDRESS
       ===================================================== */

    int option = 1;

    setsockopt(
        server_socket,
        SOL_SOCKET,
        SO_REUSEADDR,
        (char *)&option,
        sizeof(option)
    );

struct sockaddr_in server_address;

    memset(
        &server_address,
        0,
        sizeof(server_address)
    );

    server_address.sin_family =
        AF_INET;

    /*
       INADDR_ANY allows connections
       through Tailscale as well.
    */

    server_address.sin_addr.s_addr =
        INADDR_ANY;

    server_address.sin_port =
        htons((u_short)port);


    /* =====================================================
       BIND
       ===================================================== */

    if (bind(
            server_socket,
            (struct sockaddr *)&server_address,
            sizeof(server_address))
        == SOCKET_ERROR)
    {
        printf(
            "Bind failed. Error: %d\n",
            WSAGetLastError()
        );

        closesocket(
            server_socket
        );

        DeleteCriticalSection(
            &clients_lock
        );

        WSACleanup();

        return 1;
    }


    /* =====================================================
       LISTEN
       ===================================================== */

    if (listen(
            server_socket,
            MAX_CLIENTS)
        == SOCKET_ERROR)
    {
        printf(
            "Listen failed. Error: %d\n",
            WSAGetLastError()
        );

        closesocket(
            server_socket
        );

        DeleteCriticalSection(
            &clients_lock
        );

        WSACleanup();

        return 1;
    }


    /* =====================================================
       SERVER STARTED
       ===================================================== */

    printf(
        "==============================\n"
    );

    printf(
        "       Secure Chat Server\n"
    );

    printf(
        "==============================\n"
    );

    printf(
        "Server running on port %d\n",
        port
    );

    printf(
        "Waiting for clients...\n"
    );


    /* =====================================================
       ACCEPT CLIENTS
       ===================================================== */

    while (1)
    {
        struct sockaddr_in client_address;

        int address_length =
            sizeof(client_address);

        SOCKET client_socket =
            accept(
                server_socket,
                (struct sockaddr *)&client_address,
                &address_length
            );

        if (client_socket ==
            INVALID_SOCKET)
        {
            printf(
                "Accept failed. Error: %d\n",
                WSAGetLastError()
            );

            continue;
        }

        EnterCriticalSection(
            &clients_lock
        );

        int index = -1;

        for (int i = 0;
             i < MAX_CLIENTS;
             i++)
        {
            if (!clients[i].active)
            {
                index = i;
                break;
            }
        }

        if (index == -1)
        {
            LeaveCriticalSection(
                &clients_lock
            );

            send_frame(
                client_socket,
                "ERROR server full",
                18
            );

            closesocket(
                client_socket
            );

            continue;
        }

        clients[index].socket =
            client_socket;

        clients[index].active = 1;

        LeaveCriticalSection(
            &clients_lock
        );


        /* =================================================
           CREATE CLIENT THREAD
           ================================================= */

        int *argument =
            malloc(sizeof(int));

        if (argument == NULL)
        {
            closesocket(
                client_socket
            );

            EnterCriticalSection(
                &clients_lock
            );

            clients[index].active = 0;

            clients[index].socket =
                INVALID_SOCKET;

            LeaveCriticalSection(
                &clients_lock
            );

            continue;
        }

        *argument = index;

        HANDLE thread =
            CreateThread(
                NULL,
                0,
                client_thread,
                argument,
                0,
                NULL
            );

        if (thread == NULL)
        {
            free(argument);

            remove_client(index);
        }
        else
        {
            CloseHandle(thread);
        }
    }


    /* =====================================================
       CLEANUP
       ===================================================== */

    closesocket(
        server_socket
    );

    DeleteCriticalSection(
        &clients_lock
    );

    WSACleanup();

    return 0;
}
   