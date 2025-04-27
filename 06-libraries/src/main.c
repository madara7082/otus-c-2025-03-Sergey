#include "main.h"

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        printf("USAGE: %s <city>\n", argv[0]);
        return EXIT_FAILURE;
    }

    struct MemoryStruct chunk = {0};
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();
    if (!curl) 
    {
        printf("CURL init error\n");
        return EXIT_FAILURE;       
    }

    char* city_escaped = curl_easy_escape(curl, argv[1], 0);    // перекодируем город для URL, lenght=0, функция сама определит размер
    if (!city_escaped)
    {
        printf("City to URL error\n");
        return EXIT_FAILURE;         
    }

    char url[URL_REQ_SYM_MAX];
    snprintf(url, sizeof(url), "http://wttr.in/%s?format=j1", city_escaped);    // помещаем город в запрос к API
    curl_free(city_escaped);
    
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    CURLcode res;
    res = curl_easy_perform(curl);
    if(res != CURLE_OK) 
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        return EXIT_FAILURE; 
    }

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if (http_code != HTTP_OK)
    {
        printf("Error, HTTP status code: %ld\n", http_code);
        return EXIT_FAILURE; 
    }

    cJSON *json = cJSON_Parse(chunk.memory);                                                // парсим все данные
    cJSON *current_condition = cJSON_GetObjectItemCaseSensitive(json, "current_condition"); // получаем значение поля "current_condition"
    cJSON *current = cJSON_GetArrayItem(current_condition, 0);                              // получаем указатель на поля "current_condition"
    cJSON *temp_c = cJSON_GetObjectItemCaseSensitive(current, "temp_C");                    // получаем указатель на поле температуры
    cJSON *winddir16Point = cJSON_GetObjectItemCaseSensitive(current, "winddir16Point");
    cJSON *winddirDegree = cJSON_GetObjectItemCaseSensitive(current, "winddirDegree");
    cJSON *windspeedKmph = cJSON_GetObjectItemCaseSensitive(current, "windspeedKmph");
    cJSON *weatherDescFields = cJSON_GetObjectItemCaseSensitive(current, "weatherDesc");    // получаем значение поля "weatherDesc"
    cJSON *desc = cJSON_GetArrayItem(weatherDescFields, 0);                                 // получаем указатель на поля "weatherDesc"
    cJSON *weatherDesc = cJSON_GetObjectItemCaseSensitive(desc, "value");                   // получаем указатель на поле описания погоды

    printf("Current weather in %s: \n", argv[1]);
    printf("%s, %s°C\n", weatherDesc->valuestring, temp_c->valuestring);
    printf("Wind %s, %s°, %s km/h \n", winddir16Point->valuestring, winddirDegree->valuestring, windspeedKmph->valuestring);

    cJSON_Delete(json);
    free(chunk.memory);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return EXIT_SUCCESS;
}

size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
   
    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(!ptr) 
    {
      printf("Not enough memory (realloc returned NULL)\n");
      return EXIT_FAILURE;
    }
   
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
   
    return realsize;   
}
