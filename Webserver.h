#include <processthreadsapi.h>
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#define WebServerOpen 0
#define WebServerClosed 1

#define MaxRequestSize 16384
#define MaxHttpPathSize 128

#define ContentType_HTML "Text/html"
#define ContentType_CSS "Text/css"
#define ContentType_JSON "application/json"
#define ContentType_JS "application/javascript"

#define MaxFileSize 64000
#define DefaultFileSize 16000
#define SmallFileSize 4000

#define Connection_Close "close"
#define Connection_Keep "keep-alive"

typedef struct recvData recvData;
typedef struct httpData httpData;
typedef struct httpRequestData httpRequestData;
typedef struct httpResponseData httpResponseData;
typedef struct HttpPassData HttpPassData;

struct recvData
{
    char *buff;
    SOCKET socket;
};

struct HttpPassData
{
    httpResponseData *out;
    httpRequestData *in;
};

struct httpData
{
    char method[20];
    char path[128];
    char contentType[40];
    int contentLength;
    char *data;
    char host[64];
    char connection[30];
};
struct httpRequestData
{
    char *RequestString;
    httpData HttpData;
};

struct httpResponseData
{
    int statusCode;
    httpData HttpData;
};

int GetNextToken(int startIdx, char *string, char *out);
httpResponseData WhenGET(httpRequestData RequestData);
httpResponseData WhenPOST(httpRequestData RequestData);
httpResponseData WhenHEAD(httpRequestData RequestData);
httpResponseData WhenOPTIONS(httpRequestData RequestData);
int SendFile(HttpPassData in, FILE *file);
int GetFileTypeFromPath(char *out, char *path);
