#ifndef IVAN_ALONE_CURLCOOKIES_H_
#define IVAN_ALONE_CURLCOOKIES_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;

namespace Ivan_Alone {

	class CurlCookies {
		private:
			map<string, map<string, map<string, string>>> cookies;
			string cookies_file;
			vector<string> domains;
			map<string, map<string, map<string, string>>> extractCookies(vector<vector<string>> file);
		public:
			CurlCookies(string cookie_path);
			void reload();
			string getValidValue(string key);
	};

}

#endif
