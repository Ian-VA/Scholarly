#include "curl/curl.h"
#include "curl/easy.h"
#include <iostream>
#include <string>
#include <cstdio>
#include <algorithm>
#include <stdio.h>
#include <vector>
#include "libxml/tree.h"
#include "libxml/HTMLparser.h"
#define CURL_STATICLIB
#pragma comment (lib, "Normaliz.lib")
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Wldap32.lib")
#pragma comment (lib, "advapi32.lib")
#pragma comment(lib, "libcurl_a.lib")



static std::size_t write(void* buffer, std::size_t size, std::size_t nmemb, void* param) // callback function
{
	std::string& text = *static_cast<std::string*>(param);
	std::size_t totalsize = size * nmemb;
	text.append(static_cast<char*>(buffer), totalsize); 

	return totalsize;
}

void parser(xmlNode *a_node)
{
	xmlNode* cur_node = NULL; // binary tree
	if (NULL == a_node){
		return;
	}

	for (cur_node = a_node; cur_node; cur_node = cur_node->next){
		if (cur_node->type == XML_ELEMENT_NODE){
			std::cout << "Node Type: Text, Node Name: " + cur_node->name;
		} else if (cur_node->type == XML_TEXT_NODE) {
			std::string content = static_cast<char*>(cur_node->content);
			std::cout << "Node Type: Text" + "Node Content: " + content + "Content Length: " + content.size();
		}
		parser(cur_node->children); // recursion
	}

	return;
}

int main()
{
	
	CURL* curl; // initialization
	htmlDocPtr doc;
	xmlNode* roo_element = NULL; 
	std::string input;
	std::string result;
	CURLcode response;
	std::string url;

	std::cout << "Keywords (Note that more keywords means less accurate articles): ";
	getline(std::cin, input);

	if (input.find(" ")) {
		std::replace(input.begin(), input.end(), ' ', '+');
	}

	url = "https://scholar.google.com/scholar?hl=en&as_sdt=0%2C5&q=" + input + "&btnG=";


	curl_global_init(CURL_GLOBAL_ALL);

	curl = curl_easy_init(); 
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
		
		response = curl_easy_perform(curl);
		if (response != CURLE_OK) { 
			std::cout << curl_easy_strerror(response);
		}
		else {
			std::cout << response << std::endl;
		}
		curl_easy_cleanup(curl); 


	}
	else { 
		throw "Error";
	}

	doc = htmlReadFile(result.c_str(), NULL, HTML_PARSE_NOBLANKS | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET);
	curl_global_cleanup(); 

	roo_element = xmlDocGetRootElement(doc);

	if (roo_element == NULL)
	{
		std::cout << "empty document" << std::endl;
		xmlFreeDoc(doc);
		return 0;
	} 

	std::cout << result;
	std::cout << "Root node: " + roo_element->name;
	parser(roo_element);

	xmlFreeDoc(doc);
	xmlCleanupParser();   
}
