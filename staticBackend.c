#include "Webserver.h"
#include <stdio.h>

httpResponseData WhenGET(httpRequestData RequestData)
{
    printf("\njj\n");
    httpResponseData out;
    HttpPassData PassData =
        {&out,
         &RequestData};
    out.HttpData.data = calloc(DefaultFileSize, 1);
    strcpy(out.HttpData.contentType, ContentType_HTML);
    if (strcmp(RequestData.HttpData.path, "") == 0)
    {
        strcpy(RequestData.HttpData.path, "index.html");
    }
    FILE *load = fopen(RequestData.HttpData.path, "r");
    if (load == NULL)
    {
        printf("\nfuck\n");
        out.statusCode = 404;
        goto ret;
    }
    strcpy(out.HttpData.connection, Connection_Close);

    char Ftype[8];
    GetFileTypeFromPath(Ftype, RequestData.HttpData.path);
    if (strcmp(Ftype, "css") == 0)
    {
        strcpy(out.HttpData.contentType, ContentType_CSS);
    }
    else if (strcmp(Ftype, "js") == 0)
    {
        strcpy(out.HttpData.contentType, ContentType_JS);
    }
    printf("file type  %s", Ftype);
    SendFile(PassData, load);
ret:
    return out;
}

httpResponseData WhenPOST(httpRequestData RequestData)
{
    printf("\n\nPOST\n\n");
    httpResponseData ht;
    ht.HttpData.data = calloc(100, 1);
    strcpy(ht.HttpData.data, "hello");
    ht.statusCode = 204;
    return ht;
}
httpResponseData WhenHEAD(httpRequestData RequestData) {}
httpResponseData WhenOPTIONS(httpRequestData RequestData) {}
