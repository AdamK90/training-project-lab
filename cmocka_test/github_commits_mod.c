/*
 * Copyright (c) 2009-2016 Petri Lehtinen <petri@digip.org>
 *
 * Jansson is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>

#include "github_commits_mod.h"

static int newline_offset(const char *text)
{
    const char *newline = strchr(text, '\n');
    if(!newline)
        return strlen(text);
    else
        return (int)(newline - text);
}

static size_t write_response(void *ptr, size_t size, size_t nmemb, void *stream)
{
    struct write_result *result = (struct write_result *)stream;

    if(result->pos + size * nmemb >= BUFFER_SIZE - 1)
    {
        fprintf(stderr, "error: too small buffer\n");
        return 0;
    }

    memcpy(result->data + result->pos, ptr, size * nmemb);
    result->pos += size * nmemb;

    return size * nmemb;
}

static char *request(const char *url)
{
    CURL *curl = NULL;
    CURLcode status;
    struct curl_slist *headers = NULL;
    char *data = NULL;
    long code;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(!curl)
        goto error;

    data = malloc(BUFFER_SIZE);
    if(!data)
        goto error;

    struct write_result write_result = {
        .data = data,
        .pos = 0
    };

    curl_easy_setopt(curl, CURLOPT_URL, url);

    /* GitHub commits API v3 requires a User-Agent header */
    headers = curl_slist_append(headers, "User-Agent: Jansson-Tutorial");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_response);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &write_result);

    status = curl_easy_perform(curl);
    if(status != 0)
    {
        fprintf(stderr, "error: unable to request data from %s:\n", url);
        fprintf(stderr, "%s\n", curl_easy_strerror(status));
        goto error;
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    if(code != 200)
    {
        fprintf(stderr, "error: server responded with code %ld\n", code);
        goto error;
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    curl_global_cleanup();

    /* zero-terminate the result */
    data[write_result.pos] = '\0';

    return data;

error:
    if(data)
        free(data);
    if(curl)
        curl_easy_cleanup(curl);
    if(headers)
        curl_slist_free_all(headers);
    curl_global_cleanup();
    return NULL;
}

/*char * get_result(const char *user, const char *repository) {
    size_t i;
    char *text;
    char url[URL_SIZE];

    if (!user || !repository) {
        return NULL;
    }

    snprintf(url, URL_SIZE, URL_FORMAT, user, repository);

    text = request(url);
    return text;
}*/

commit_data get_first(const char* user, const char* repository, int *error) {
    json_t *root;
    json_error_t err;

    commit_data commit_d;
    commit_d.sha = NULL;
    commit_d.message = NULL;

    char *text = get_result(user, repository);

    root = json_loads(text, 0, &err);
    free(text);
    if (!root) {
        *error = 1;
    }
    if (!json_is_array(root)) {
        *error = 2;
        json_decref(root);
        return commit_d;
    }

    json_t *data, *sha, *commit, *message;

    data = json_array_get(root, 0);
    if(!json_is_object(data)) {
        *error = 3;
        json_decref(root);
        return commit_d;
    }
    sha = json_object_get(data, "sha");
    if(!json_is_string(sha))
    {
       *error = 4;
       json_decref(root);
       return commit_d;
    }
    commit = json_object_get(data, "commit");
    if(!json_is_object(commit))
    {
       *error = 5;
       json_decref(root);
       return commit_d;
    }
    message = json_object_get(commit, "message");
    if(!json_is_string(message))
    {
       *error = 6;
       json_decref(root);
       return commit_d;
    }

    commit_d.message = json_string_value(message);
    commit_d.sha = json_string_value(sha);
    *error = 0;
    return commit_d;
}