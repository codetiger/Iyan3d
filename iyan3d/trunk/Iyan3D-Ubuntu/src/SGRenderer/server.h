
#ifndef SERVER_H_
#define SERVER_H_

#include "common.h"
#include <cstring>

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
	size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
	return written;
}

bool downloadFile(const char* url, const char* filePath) {
	FILE *file = fopen(filePath, "wb");

	curl_global_init(CURL_GLOBAL_ALL);
	CURL *curl = curl_easy_init();

	if(curl && file) {
		CURLcode res;
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

		res = curl_easy_perform(curl);
		if(res != CURLE_OK) {
			return false;
		}

		fclose(file);
		curl_easy_strerror(res);
		curl_easy_cleanup(curl);
		return true;
	}
	return false;
}

char* url_encode(const char* data) {
	curl_global_init(CURL_GLOBAL_ALL);
	CURL *curl = curl_easy_init();
	return curl_easy_escape(curl, data, strlen(data));
}

bool uploadFile(const char* url, const char* filePath) {
	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;
	struct curl_slist *headerlist = NULL;
	static const char buf[] = "Expect:";

	curl_global_init(CURL_GLOBAL_ALL);

	curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "video", CURLFORM_FILE, filePath, CURLFORM_END);
	curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "filename", CURLFORM_COPYCONTENTS, filePath, CURLFORM_END);
	curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "submit", CURLFORM_COPYCONTENTS, "send", CURLFORM_END);

	CURL *curl = curl_easy_init();
	headerlist = curl_slist_append(headerlist, buf);
	if(curl) {
		CURLcode res;
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

		res = curl_easy_perform(curl);
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",

		curl_easy_strerror(res));
		curl_easy_cleanup(curl);
		curl_formfree(formpost);
		curl_slist_free_all (headerlist);

		return true;
	}
	return false;
}

#endif
