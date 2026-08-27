#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
#include <winsock2.h>

#include "crypto.h"

#define MAX_FRAME_SIZE (1024 * 1024 + 8192)
#define MAX_FILE_SIZE (1024 * 1024)
#define MAX_USERNAME 50
#define MAX_KEY 100

SOCKET client_socket;

char username[MAX_USERNAME];
char key[MAX_KEY];

volatile int running = 1;


/* SEND ALL */

int send_all(SOCKET socket, const char *data, int length)
{
    int total = 0;

    while (total < length)
    {
        int n = send(socket, data + total, length - total, 0);

        if (n == SOCKET_ERROR)
            return 0;

        total += n;
    }

    return 1;
}


/* RECEIVE ALL */

int recv_all(SOCKET socket, char *buffer, int length)
{
    int total = 0;

    while (total < length)
    {
        int n = recv(socket, buffer + total, length - total, 0);

        if (n <= 0)
            return 0;

        total += n;
    }

    return 1;
}


/* SEND LENGTH-PREFIXED FRAME */

int send_frame(SOCKET socket, const char *data, int length)
{
    uint32_t network_length = htonl((uint32_t)length);

    if (!send_all(socket,
                  (char *)&network_length,
                  sizeof(network_length)))
    {
        return 0;
    }

    return send_all(socket, data, length);
}


/* RECEIVE LENGTH-PREFIXED FRAME */

int recv_frame(SOCKET socket, char **data, int *length)
{
    uint32_t network_length;

    if (!recv_all(socket,
                  (char *)&network_length,
                  sizeof(network_length)))
    {
        return 0;
    }

    *length = ntohl(network_length);

    if (*length <= 0 || *length > MAX_FRAME_SIZE)
    {
        return 0;
    }

    *data = (char *)malloc(*length + 1);

    if (*data == NULL)
        return 0;

    if (!recv_all(socket, *data, *length))
    {
        free(*data);
        *data = NULL;
        return 0;
    }

    (*data)[*length] = '\0';

    return 1;
}


/* ENCRYPTED SEND */

int send_encrypted(const char *data, int length)
{
    char *copy = (char *)malloc(length);

    if (copy == NULL)
        return 0;

    memcpy(copy, data, length);

    xor_crypt(copy, length, key);

    int result = send_frame(client_socket, copy, length);

    free(copy);

    return result;
}


/* ENCRYPTED RECEIVE */

int recv_encrypted(char **data, int *length)
{
    if (!recv_frame(client_socket, data, length))
    {
        return 0;
    }

    xor_crypt(*data, *length, key);

    return 1;
}


/* GET FILE NAME FROM PATH */

void get_filename(const char *path, char *filename)
{
    const char *p1 = strrchr(path, '\\');
    const char *p2 = strrchr(path, '/');

    const char *last = p1;

    if (p2 != NULL && (last == NULL || p2 > last))
        last = p2;

    if (last != NULL)
        strcpy(filename, last + 1);
    else
        strcpy(filename, path);
}


/* RECEIVE FILE */

void handle_received_file(char *data, int length)
{
    char *newline = strchr(data, '\n');

    if (newline == NULL)
    {
        printf("ERROR invalid file packet\n");
        return;
    }

    int header_length = (int)(newline - data);

    char header[1024];

    if (header_length >= (int)sizeof(header))
    {
        printf("ERROR file header too long\n");
        return;
    }

    memcpy(header, data, header_length);

    header[header_length] = '\0';

    char sender[MAX_USERNAME];
    char filename[256];
    int file_size;

    if (sscanf(header,
               "RECVFILE FROM %49s %255s %d",
               sender,
               filename,
               &file_size) != 3)
    {
        printf("ERROR invalid file header\n");
        return;
    }

    int payload_size = length - header_length - 1;

    if (file_size < 0 ||
        file_size > MAX_FILE_SIZE ||
        payload_size != file_size)
    {
        printf("ERROR invalid file size\n");
        return;
    }

    char output_filename[512];

    snprintf(output_filename,
             sizeof(output_filename),
             "received_%s",
             filename);

    FILE *file = fopen(output_filename, "wb");

    if (file == NULL)
    {
        printf("ERROR could not create received file\n");
        return;
    }

    fwrite(newline + 1,
           1,
           file_size,
           file);

    fclose(file);

    printf("\nRECVFILE FROM %s: %s (%d bytes)\n",
           sender,
           filename,
           file_size);

    printf("[content saved to ./%s]\n",
           output_filename);
}


/* RECEIVER THREAD */

DWORD WINAPI receiver_thread(LPVOID parameter)
{
    (void)parameter;

    while (running)
    {
        char *data = NULL;
        int length;

        if (!recv_encrypted(&data, &length))
        {
            if (running)
                printf("\nServer disconnected.\n");

            running = 0;
            break;
        }

        if (strncmp(data, "RECVFILE ", 9) == 0)
        {
            handle_received_file(data, length);
        }
        else
        {
            printf("\nserver$ %s\n", data);
        }

        free(data);

        if (running)
        {
            printf("%s$ ", username);
            fflush(stdout);
        }
    }

    return 0;
}


/* SEND FILE */

void send_file(char *input)
{
    char target[MAX_USERNAME];
    char path[512];

    if (sscanf(input,
               "SENDFILE TO %49s %511[^\n]",
               target,
               path) != 2)
    {
        printf("ERROR invalid file command\n");
        return;
    }

    FILE *file = fopen(path, "rb");

    if (file == NULL)
    {
        printf("server$ ERROR file not found: %s\n", path);
        return;
    }

    fseek(file, 0, SEEK_END);

    long size = ftell(file);

    fseek(file, 0, SEEK_SET);

    if (size < 0 || size > MAX_FILE_SIZE)
    {
        printf("server$ ERROR file too large\n");

        fclose(file);

        return;
    }

    char filename[256];

    get_filename(path, filename);

    const char *extension = strrchr(filename, '.');

    if (extension == NULL ||
        _stricmp(extension, ".txt") != 0)
    {
        printf("server$ ERROR only .txt files are supported\n");

        fclose(file);

        return;
    }

    char *content = (char *)malloc((size_t)size);

    if (content == NULL)
    {
        printf("ERROR memory allocation failed\n");

        fclose(file);

        return;
    }

    size_t read_count = fread(content,
                              1,
                              (size_t)size,
                              file);

    fclose(file);

    if (read_count != (size_t)size)
    {
        free(content);

        printf("ERROR could not read file\n");

        return;
    }

    char header[1024];

    int header_length = snprintf(
        header,
        sizeof(header),
        "SENDFILE TO %s %s %ld\n",
        target,
        filename,
        size
    );

    int total_length = header_length + (int)size;

    char *packet = (char *)malloc(total_length);

    if (packet == NULL)
    {
        free(content);

        printf("ERROR memory allocation failed\n");

        return;
    }

    memcpy(packet,
           header,
           header_length);

    memcpy(packet + header_length,
           content,
           (size_t)size);

    free(content);

    if (!send_encrypted(packet, total_length))
    {
        printf("ERROR sending file\n");
    }
    else
    {
        printf("File sent to %s\n", target);
    }

    free(packet);
}


/* MAIN */

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: client.exe <server_ip> <port>\n");
        return 1;
    }

    printf("==============================\n");
    printf(" Secure Chat Client\n");
    printf("==============================\n");

    printf("Username: ");
    scanf("%49s", username);

    printf("Key: ");
    scanf("%99s", key);

    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup failed.\n");
        return 1;
    }

    client_socket = socket(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP
    );

    if (client_socket == INVALID_SOCKET)
    {
        printf("Socket creation failed.\n");

        WSACleanup();

        return 1;
    }

    struct sockaddr_in server_address;

    memset(&server_address,
           0,
           sizeof(server_address));

    server_address.sin_family = AF_INET;

    server_address.sin_port =
        htons((u_short)atoi(argv[2]));


    server_address.sin_addr.s_addr =
        inet_addr(argv[1]);

    if (server_address.sin_addr.s_addr == INADDR_NONE)
    {
        printf("Invalid server IP address.\n");

        closesocket(client_socket);

        WSACleanup();

        return 1;
    }

    if (connect(
            client_socket,
            (struct sockaddr *)&server_address,
            sizeof(server_address))
        == SOCKET_ERROR)
    {
        printf("Connection failed.\n");

        closesocket(client_socket);

        WSACleanup();

        return 1;
    }

    printf("Connected to server.\n");

    char registration[512];

    snprintf(
        registration,
        sizeof(registration),
        "REGISTER %s %s",
        username,
        key
    );

    if (!send_frame(
            client_socket,
            registration,
            (int)strlen(registration)))
    {
        printf("Registration failed.\n");

        closesocket(client_socket);

        WSACleanup();

        return 1;
    }

    char *response = NULL;
    int response_length;

    if (!recv_encrypted(
            &response,
            &response_length))
    {
        printf("Could not receive registration response.\n");
        closesocket(client_socket);

        WSACleanup();

        return 1;
    }

    printf("server$ %s\n", response);

    int registered =
        strncmp(response,
                "OK REGISTERED",
                13) == 0;

    free(response);

    if (!registered)
    {
        closesocket(client_socket);

        WSACleanup();

        return 1;
    }

    /*
       CREATE RECEIVER THREAD
    */

    HANDLE receiver =
        CreateThread(
            NULL,
            0,
            receiver_thread,
            NULL,
            0,
            NULL
        );

    if (receiver == NULL)
    {
        printf("Could not create receiver thread.\n");

        closesocket(client_socket);

        WSACleanup();

        return 1;
    }

    printf("\nConnected successfully!\n");

    printf("\nAvailable commands:\n");
    printf("SEND TO <user> <message>\n");
    printf("SENDFILE TO <user> <filename>\n");
    printf("LIST\n");
    printf("QUIT\n\n");

    /* COMMAND LOOP */

    while (running)
    {
        char input[MAX_FRAME_SIZE];

        printf("%s$ ", username);

        fflush(stdout);

        if (fgets(
                input,
                sizeof(input),
                stdin) == NULL)
        {
            break;
        }

        input[
            strcspn(input, "\r\n")
        ] = '\0';

        if (strlen(input) == 0)
            continue;

        if (strncmp(
                input,
                "SENDFILE TO ",
                12) == 0)
        {
            send_file(input);

            continue;
        }

        if (!send_encrypted(
                input,
                (int)strlen(input)))
        {
            printf("Send failed.\n");
            break;
        }

        if (strcmp(input, "QUIT") == 0)
        {
            Sleep(300);

            running = 0;

            break;
        }
    }

    running = 0;

    shutdown(
        client_socket,
        SD_BOTH
    );

    closesocket(client_socket);

    WaitForSingleObject(
        receiver,
        1000
    );

    CloseHandle(receiver);

    WSACleanup();

    return 0;
}