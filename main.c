#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <processthreadsapi.h>
#include <winsock2.h>
#include <windows.h>
#include <time.h>
#include <math.h>
#include "Webserver.h"
#define WebServerOpen 0
#define WebServerClosed 1
#define MaxRequestSize 16384
// things to do make the api

int threads = 0;
int keepOpen;

httpResponseData callMethodFromBackend(char method[], httpRequestData hi)
{
    if (strcmp(method, "GET") == 0)
    {
        return WhenGET(hi);
    }
    else if (strcmp(method, "POST") == 0)
    {
        printf("\nPOSt\n");
        return WhenPOST(hi);
    }
    else if (strcmp(method, "HEAD") == 0)
    {
        return WhenHEAD(hi);
    }
    else if (strcmp(method, "OPTIONS") == 0)
    {
        return WhenOPTIONS(hi);
    }
    // segfalut provention
    else
    {
        httpResponseData hi;
        strcpy(hi.HttpData.data, "hello");
        return hi;
    }
}

int inttostr(int i, char *out)
{
    int preI = 0;
    for (int k = 1; k <= (int)ceil(log10(i)); k++)
    {
        int tento = (int)powl(10, (int)ceil(log10(i)) - (k - 1));
        preI = i % (tento / 10);
        printf("\n%d %d %d\n", (i % tento - preI), tento, i % tento);
        out[k - 1] = (i % tento - preI) / (tento / 10) + 48;
        out[k] = '\000';
    }
    return 0;
}

int strtoint(char *string)
{
    int out = 0;
    for (int i = 0; i < strlen(string); i++)
    {
        out += ((string[strlen(string) - (i + 1)] - 48) * (int)powl(10, i));
    }
    return out;
}

int printWquatesASNTM(char *buf)
{
    for (int k = 0; k < 30; k++)
    {
        printf("%c", (buf[k] == 0) ? 34 : buf[k]);
    }
}

int GetNextToken(int startIdx, char *string, char *out)
{
    char output[128];
    int i = startIdx;
    for (i = i; string[i] != ' ' && i < strlen(string); i++)
    {
        output[i - startIdx] = string[i];
        output[i - (startIdx - 1)] = '\000';
    }
    strcpy(out, output);
    i++;
    return i;
}

httpData httpPasers(char *httpRequest)
{
    httpData output;
    char fristline[128];
    for (int i = 0; httpRequest[i] != '\n' && i < 128; i++)
    {
        fristline[i] = httpRequest[i];
    }
    fristline[strlen(fristline)] = '\000';
    int i;
    for (i = 0; httpRequest[i] != ' ' && i < 20; i++)
    {
    }
    i++;
    for (i = i; httpRequest[i] != ' ' && i < 80; i++)
    {
        // printf("\n %c %d", httpRequest[i], i - strlen(output.method));
        // printf("\n %S\n", output.path);
        // printWquatesASNTM(output.path);

        output.path[i - strlen(output.method) - 1] = httpRequest[i];
        output.path[(i - strlen(output.method) - 1) + 1] = '\000';
    }
    output.path[strlen(output.path)] = '\000';
    i = 0;
    char line[256];
    while (httpRequest[i] != '\000' || i == 0)
    {
        int preveisI = i;
        for (i = i; httpRequest[i] != '\n' && i < MaxRequestSize; i++)
        {
            line[i - preveisI] = httpRequest[i];
            line[(i + 1) - preveisI] = '\000';
            // printf("\n%c\n\n ", httpRequest[i]);
            // printWquatesASNTM(line);
            // printf("\n%s\n", line);
        }
        char hi[256];
        int nextT;
        nextT = GetNextToken(0, line, hi);
        printf("\nfrist token %s", hi);
        printf("\nrequest %s\n", line);
        printf("%d\n", strcmp(hi, "Host:"));
        if (strcmp(hi, "Host:") == 0)
        {
            char value[128];
            GetNextToken(nextT, line, value);
            printf("\nkey %s\n value   %s \n", hi, value);
            strcpy(output.host, value);
        }
        if (strcmp(hi, "Content-Type:") == 0)
        {
            char value[128];
            GetNextToken(nextT, line, value);
            printf("\nkey %s\n value   %s \n", hi, value);
            strcpy(output.contentType, value);
        }
        if (strcmp(hi, "Content-Length:") == 0)
        {
            char value[128];
            GetNextToken(nextT, line, value);
            printf("\nkey %s\n value   %d \n", hi, strtoint(value));
            output.contentLength = strtoint(value);
        }
        if (strcmp(hi, "GET") == 0 || strcmp(hi, "POST") == 0 || strcmp(hi, "HEAD") == 0 || strcmp(hi, "OPTIONS") == 0)
        {
            strcpy(output.method, hi);
        }
        i++;
    }
    printf("\npath %s", output.path);
    printf("\nmethod %s\n", output.method);
    printWquatesASNTM(output.method);
    return output;
}

long unsigned printshit(void *lpParam)
{
    recvData hi = *(recvData *)lpParam;
    while (true)
    {
        char fristline[256];
        char cuff[MaxFileSize] = "HTTP/1.1 200 OK\r\nContent-Type: Text/html\r\nContent-Length: 81\r\nConnection: keep-alive\r\n\r\n";
        char kii[MaxRequestSize] = "";

        int error = recv(hi.socket, kii, MaxRequestSize, 0);
        if (error <= 0)
        {
            printf("\nsocket Closed");
            free(lpParam);
            --threads;
            return 0;
        }
        if (error > 0)
        {
            printf("\n\n%d\n\n", error);
            char method[20] = "";
            httpData Data = httpPasers(kii);
            strcpy(method, Data.method);

            httpRequestData RequestData =
                {
                    kii,
                    Data,
                };
            httpResponseData Response = callMethodFromBackend(method, RequestData);
            if (Response.HttpData.data == NULL)
            {
                printf("hi");
            }
            printWquatesASNTM(Response.HttpData.data);

            // format the Response
            char j[2][32];
            inttostr(Response.statusCode, j[0]);
            inttostr(Response.HttpData.contentLength, j[1]);
            strcpy(cuff, "");
            strcat(cuff, "http/1.1 ");
            strcat(cuff, j[0]);
            strcat(cuff, "\r\n");
            strcat(cuff, "Content-Type: ");
            strcat(cuff, Response.HttpData.contentType);
            strcat(cuff, "\r\n");
            strcat(cuff, "Content-Length: ");
            strcat(cuff, j[1]);
            strcat(cuff, "\r\n");
            strcat(cuff, "Connection: ");
            strcat(cuff, Response.HttpData.connection);
            strcat(cuff, "\r\n");
            strcat(cuff, "\r\n");
            strcat(cuff, Response.HttpData.data);

            if (strcmp(method, "STOP") == 0)
            {
                keepOpen = WebServerClosed;
            }
            if (strcmp(method, "THD") == 0)
            {
                printf("\nThreads %d\n", threads);
            }
            // strcat(cuff, dataToSend);
            printf("\n\nserveropen %d", keepOpen);
            printf("\nmethod %s\n\n", method);
            printf("\nhi %s %ld\n\n\n", fristline, Response.HttpData.data);
            printf(kii);
            if (strcmp(method, "GET") == 0 || strcmp(method, "POST") == 0) // checks if method is POST/GET
            {
                send(hi.socket, cuff, strlen(cuff), 0);
                printf("\n\n\nRequested\n%s\n\n", cuff);
            }
            free(Response.HttpData.data);
        }
        Sleep(100);
    }
    return 0;
}

long unsigned accseptAny(void *lpParam)
{
    while (true)
    {
        SOCKET connection;
        SOCKET hi = *((SOCKET *)lpParam);

        connection = accept(hi, NULL, NULL);
        if (connection == INVALID_SOCKET)
        {
            printf("socket fucked %d\n", WSAGetLastError());
        }
        else
        {
            printf("it worked\n");
            char post[512];
            char buff[512];
            long unsigned tid;
            LPTHREAD_START_ROUTINE k = &printshit;
            recvData *hi = malloc(sizeof(recvData));
            (*hi).socket = connection;
            (*hi).buff = malloc(256);
            threads++;
            CreateThread(NULL, 0, k, (LPVOID)hi, 0, &tid);
            printf(buff);
        }
        if (keepOpen == 1)
        {
            printf("server closed %d", keepOpen);
            return 0;
        }
    }
}

int main(int argc, char const *argv[])
{
    keepOpen = WebServerOpen;
    WSADATA WSAmyDATA;
    printf("%d", WSAStartup(0x000A, &WSAmyDATA));
    int pastTime = time(NULL);
    SOCKET mysocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mysocket == INVALID_SOCKET)
    {
        printf("\nfuck\n");
    }
    printf("SERVER ACTIVED made by VED");

    Sleep(1500);

    SOCKADDR mysockaddr;
    mysockaddr.sa_family = AF_INET;
    // port is 0x20FF
    mysockaddr.sa_data[0] = 0x00;
    mysockaddr.sa_data[1] = 0x50;
    // ip is 127.0.0.1
    mysockaddr.sa_data[2] = 127;
    mysockaddr.sa_data[3] = 0;
    mysockaddr.sa_data[4] = 0;
    mysockaddr.sa_data[5] = 1;
    // pading
    for (int i = 6; i < 14; i++)
    {
        mysockaddr.sa_data[i] = 0;
    }
    if (bind(mysocket, &mysockaddr, 16) != 0)
    {
        printf("\nbind fuck %d\n", WSAGetLastError());
    }
    if (listen(mysocket, SOMAXCONN) != 0)
    {
        printf("fuck %d\n", WSAGetLastError());
    }
    SOCKET *SocketToPass = malloc(sizeof(SOCKET));
    *SocketToPass = mysocket;
    CreateThread(NULL, 0, &accseptAny, SocketToPass, 0, NULL);
    Sleep(180000);
    return 0;
}
