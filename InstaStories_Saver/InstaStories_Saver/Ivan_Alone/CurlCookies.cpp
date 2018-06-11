#include "CurlCookies.h"
#include <fstream>

namespace Ivan_Alone {

	CurlCookies::CurlCookies(string cookie_path) {
		this->cookies_file = cookie_path;
		this->reload();
	}

	void CurlCookies::reload() {
		vector<vector<string>> file;
		vector<string> line;
		string text = "";
		ifstream cookies(this->cookies_file);
		char ch = 0;
		cookies.unsetf(ios::skipws);
		while (cookies >> ch) {
			if (ch == '\n') {
				line.push_back(text);
				file.push_back(line);
				line = vector<string>();
				text = "";
			} else if (ch == '\t') {
				line.push_back(text);
				text = "";
			} else {
				text += ch;
			}
		}
		this->cookies = this->extractCookies(file);
	}

	map<string, map<string, map<string, string>>> CurlCookies::extractCookies(vector<vector<string>> file) {
		this->domains.clear();
		map<string, map<string, map<string, string>>> j;
		for (const auto& vc : file) {
			if (((vector<string>)vc).size() == 7) {
				map<string, string> cookie;
				int address = 0;
				string domain;
				string _cookie;
				for (const auto& tokens : (vector<string>)vc) {
					switch (address) {
						case 0:
							domain = tokens;
							break;
						case 1:
							cookie["flag"] = tokens;
							break;
						case 2:
							cookie["path"] = tokens;
							break;
						case 3:
							cookie["secure"] = tokens;
							break;
						case 4:
							cookie["expiration"] = tokens;
							break;
						case 5:
							_cookie = tokens;
							break;
						case 6:
							cookie["value"] = tokens;
							break;
					}
					address++;
				}
				bool flag = true;
				for (const auto& dom : this->domains) {
					if (dom == domain) {
						flag = false;
						break;
					}
				}
				if (flag) this->domains.push_back(domain);
				j[domain][_cookie] = cookie;

			}
		}
		return j;
	}

	string CurlCookies::getValidValue(string key) {
		for (const auto& domain : this->domains) {
			map<string, map<string, string>> s = this->cookies.at(domain);
			if (s.count(key) != 0) {
				return s[key]["value"];
			}
		}
		return "";
	}
}
