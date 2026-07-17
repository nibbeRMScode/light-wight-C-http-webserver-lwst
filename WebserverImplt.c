#include "Webserver.h"
#include <stdio.h>

int SendFile(HttpPassData in, FILE *file)
{
    httpResponseData *out = in.out;
    httpRequestData *RequestData = in.in;
    char buf[128];
    char *bigBuff = calloc(MaxFileSize, 1);
    int loadedSize = 0;
    while (fgets(buf, 128, file) != NULL)
    {
        printf("\nfile %s", buf);
        strcat(bigBuff, buf);
    }
    loadedSize = strlen(bigBuff);
    printf("\n%d %s %d\n\n", loadedSize, (*RequestData).HttpData.path, (*out).statusCode);
    (*out).HttpData.contentLength = loadedSize;
    strcpy((*out).HttpData.data, bigBuff);
    free(bigBuff);
    return 0;
}

int GetFileTypeFromPath(char *out, char *path)
{
    char mid[15] = "";
    for (int i = strlen(path) - 1; path[i] != '.'; i--)
    {
        // everything this gives is fliped
        // printf("\n%c", RequestData.HttpData.path[i]);
        mid[strlen(path) - i - 1] = path[i];
        mid[strlen(path) - i] = '\000';
    }
    for (int i = strlen(mid) - 1; i >= 0; i--)
    {
        out[(strlen(mid) - 1) - i] = mid[i];
        out[(strlen(mid) - 1) - (i - 1)] = '\000';
    }
}
