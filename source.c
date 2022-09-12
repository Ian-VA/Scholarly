#include "curl/curl.h"
#include "curl/easy.h"
#include <stdio.h>
#include <htmlstreamparser.h>
#define CURL_STATICLIB
#pragma comment (lib, "Normaliz.lib")
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Wldap32.lib")
#pragma comment (lib, "advapi32.lib")
#pragma warning(disable : 4996) // for vs studio to tolerate strcat




static size_t write(void* buffer, size_t size, size_t nmemb, void* hsp)
{
	size_t realsize = size * nmemb, p;
	for (p = 0; p < realsize; p++) {
		html_parser_char_parse(hsp, ((char*)buffer)[p]);
		if (html_parser_cmp_tag(hsp, "a", 1)){
			if (html_parser_cmp_attr(hsp, "href=", 4)){
				if (html_parser_is_in(hsp, HTML_VALUE_ENDED)) {
					html_parser_val(hsp)[html_parser_val_length(hsp)] = '\0';
					printf("%s\n", html_parser_val(hsp));
				}
			}
		}
	}
	return realsize;
}


int main(int argc, char* argv[])
{
	// declaration
	CURL* curl;
	CURLcode response;
	char tag[1], attr[4], val[128], userinput[100], userinput2[100];
	HTMLSTREAMPARSER* hsp;
	char print[] = "Keywords (Be specific!): ";

	printf_s("%s", print);
	scanf_s("%s", userinput);

	for (int i = 0; i < strlen(userinput); i++)
	{
		if (userinput[i] == ' ') {
			userinput[i] = '+';
		}
	}

	for (int i = 0; i < strlen(userinput2); i++)
	{
		if (userinput[i] == ' ') {
			userinput[i] = '%';
		}
	}
	
	char url[] = "https://scholar.google.com/scholar?hl=en&as_sdt=0%2C5&q=";
	strcat(url, userinput);
	strcat(url, "&btng=");


	curl_global_init(CURL_GLOBAL_ALL);
	hsp = html_parser_init();


	curl = curl_easy_init(); // initialization
	if (curl) {

		html_parser_set_tag_to_lower(hsp, 1);
		html_parser_set_attr_to_lower(hsp, 1);
		html_parser_set_tag_buffer(hsp, tag, sizeof(tag));
		html_parser_set_attr_buffer(hsp, attr, sizeof(attr));
		html_parser_set_val_buffer(hsp, val, sizeof(val) - 1);

		curl_easy_setopt(curl, CURLOPT_URL, url); // specifications
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, hsp);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);


		// curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		response = curl_easy_perform(curl);

		curl_easy_cleanup(curl); // cleanup


	} else { // if Curl doesn't work 
		return 0;
	}

	curl_easy_perform(curl);

	curl_global_cleanup(); // cleanup
	html_parser_cleanup(hsp);

	
}
