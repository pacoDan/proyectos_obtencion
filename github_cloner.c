#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>

// Función para escribir datos recibidos de CURL en un buffer
size_t write_callback(void *ptr, size_t size, size_t nmemb, char *data) {
    strcat(data, ptr);
    return size * nmemb;
}

// Función para clonar un repositorio y sus ramas
void clone_repo(const char *user, const char *repo) {
    char command[512];
    snprintf(command, sizeof(command), "git clone --mirror https://github.com/%s/%s.git", user, repo);
    system(command);
}

// Función para listar las ramas y clonar cada una
void clone_branches(const char *user, const char *repo) {
    char api_url[256];
    snprintf(api_url, sizeof(api_url), "https://api.github.com/repos/%s/%s/branches", user, repo);
    
    CURL *curl;
    CURLcode res;
    char data[100000] = {0};

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, api_url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    json_object *parsed_json = json_tokener_parse(data);
    int arraylen = json_object_array_length(parsed_json);
    for(int i = 0; i < arraylen; i++) {
        json_object *branch = json_object_array_get_idx(parsed_json, i);
        json_object *name;
        json_object_object_get_ex(branch, "name", &name);

        char branch_name[100];
        snprintf(branch_name, sizeof(branch_name), "%s", json_object_get_string(name));
        
        char clone_command[512];
        snprintf(clone_command, sizeof(clone_command), "git clone --branch %s https://github.com/%s/%s.git %s_%s", branch_name, user, repo, repo, branch_name);
        system(clone_command);
    }
}

int main() {
    const char *user = "pacodan";
    char api_url[256];
    snprintf(api_url, sizeof(api_url), "https://api.github.com/users/%s/repos", user);

    CURL *curl;
    CURLcode res;
    char data[100000] = {0};

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, api_url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    json_object *parsed_json = json_tokener_parse(data);
    int arraylen = json_object_array_length(parsed_json);
    for(int i = 0; i < arraylen; i++) {
        json_object *repo = json_object_array_get_idx(parsed_json, i);
        json_object *name;
        json_object_object_get_ex(repo, "name", &name);

        char repo_name[100];
        snprintf(repo_name, sizeof(repo_name), "%s", json_object_get_string(name));

        printf("Cloning repository: %s\n", repo_name);
        clone_repo(user, repo_name);
        clone_branches(user, repo_name);
    }

    return 0;
}
