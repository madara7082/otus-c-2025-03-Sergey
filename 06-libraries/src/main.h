#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

#define URL_REQ_SYM_MAX 256
#define HTTP_OK         200

struct MemoryStruct
{
    char* memory;
    size_t size;
};

size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp);

#endif /* __MAIN_H */