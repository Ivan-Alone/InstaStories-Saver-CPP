#include "Network.h"

#include <stdio.h>
#include <iterator>
using namespace std;

namespace Ivan_Alone {

	Network::Network(string cookie_path) {
		this->cookie_path = cookie_path;
		this->proxy_data = "";
	}

	Network::Network(string cookie_path, string proxy_data) {
		this->cookie_path = cookie_path;
		this->proxy_data = proxy_data;
	}

	string Network::downloadedResponse = "";

	string Network::http_build_query(map<string, string> par_array) {
		string par = "";
        for (const auto& kvp : par_array) {
        	par += (string)kvp.first + "=" + par_array.at((string)kvp.first) + "&";
        }
		return par.substr(0, par.length() != 0 ? par.length()-1 : 0);
	}

	curl_slist* Network::compileHeader(map<string, string> header, vector<string> remove_array) {
		struct curl_slist* headers = NULL;
        for (const auto& kvp : remove_array) {
    	    headers = curl_slist_append(headers, ((string)(kvp+":")).c_str());
        }
        for (const auto& kvp : header) {
    	    headers = curl_slist_append(headers, ((string)(kvp.first+": " + header.at(kvp.first))).c_str());
        }
		return headers;
	}

	size_t Network::write_callback(char* data, size_t size, size_t nmemb, void* stream) {
		downloadedResponse.append(data, size*nmemb);
        return size*nmemb;
	}

	string Network::GetQuery(string url) {
		return Network::GetQuery(url, map<string, string>());
	}

	string Network::GetQuery(string url, map<string, string> header_plus) {
		map<CURLoption, string> options = {{CURLOPT_URL, url}};

		return Network::Request(options, header_plus);
	}

	string Network::PostQuery(string url) {
		return Network::PostQuery(url, map<string, string>());
	}

	string Network::PostQuery(string url, map<string, string> par_array) {
		return Network::PostQuery(url, par_array, map<string, string>());
	}

	string Network::PostQuery(string url, map<string, string> par_array, map<string, string> header_plus) {
		map<CURLoption, string> options = {
				{CURLOPT_URL, url},
				{CURLOPT_POST, "1"},
				{CURLOPT_POSTFIELDS, (char*)http_build_query(par_array).c_str()}
			};

		return Network::Request(options, header_plus);
	}

	string Network::Request(map<CURLoption, string> curl_opt_array) {
		return Network::Request(curl_opt_array, map<string, string>());
	}

	string Network::Request(map<CURLoption, string> curl_opt_array, map<string, string> par_array) {
		CURL* curl = curl_easy_init();
		if(curl) {

			map<string, string> header = {
				{"User-Agent", "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:60.0) Gecko/20100101 Firefox/60.0"},
				{"Accept", "*/*"},
				{"Accept-Language", "ru-RU,ru;q=0.8,en-US;q=0.5,en;q=0.3"},
				{"Connection", "keep-alive"}
			};

			for (const auto& kvp : par_array) {
				header[(string)kvp.first] = (string)par_array.at(kvp.first);
			}

			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, this->compileHeader(header, vector<string>{"Accept"}));
			curl_easy_setopt(curl, CURLOPT_COOKIEJAR, (char*)this->cookie_path.c_str());
			curl_easy_setopt(curl, CURLOPT_COOKIEFILE, (char*)this->cookie_path.c_str());

	        for (const auto& kvp : curl_opt_array) {
	        	if ((int)kvp.first == CURLOPT_POST) {
        			bool flag = false;
        			if ("1" == (string)curl_opt_array.at(kvp.first)) {
        				flag = true;
        			}
        			curl_easy_setopt(curl, CURLOPT_POST, flag);
	        	} else {
        			curl_easy_setopt(curl, (CURLoption)kvp.first, (char*)(curl_opt_array.at(kvp.first).c_str()));
	        	}
	        }

			if (this->proxy_data != "") {
				curl_easy_setopt(curl, CURLOPT_PROXY, this->proxy_data.c_str());
			}

	        downloadedResponse = "";

	        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

	        CURLcode res = curl_easy_perform(curl);
	        curl_easy_cleanup(curl);

	        if (CURLE_OK == res) {
				return downloadedResponse;
	        }
		}
		return "";
	}
}
