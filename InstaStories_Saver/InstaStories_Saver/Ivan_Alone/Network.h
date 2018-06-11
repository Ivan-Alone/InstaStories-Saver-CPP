#ifndef NETWORK_H_
#define NETWORK_H_

#include <iostream>
#include <map>
#include <vector>
#include <string>
#ifdef _WIN32
	#include "../curl/curl.h"
#else
	#include "curl/curl.h"
#endif

using namespace std;

namespace Ivan_Alone {

	class Network {
		private:
			string  cookie_path;
			string  proxy_data;
			static string downloadedResponse;
			string  http_build_query(map<string, string> par_array);
			curl_slist* compileHeader(map<string, string> header, vector<string> remove_array);
			static size_t write_callback(char* data, size_t size, size_t nmemb, void* stream);
		public:
			Network(string cookie_path);
			Network(string cookie_path, string proxy_data);

			string GetQuery(string url);
			string GetQuery(string url, map<string, string> header_plus);

			string PostQuery(string url);
			string PostQuery(string url, map<string, string> par_array);
			string PostQuery(string url, map<string, string> par_array, map<string, string> header_plus);

			string Request(map<CURLoption, string> curl_opt_array);
			string Request(map<CURLoption, string> curl_opt_array, map<string, string> par_array);
	};

}

#endif
