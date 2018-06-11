#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <map>
#include "nlohmann/json.hpp"
#include "curl/curl.h"
#include "Ivan_Alone/Network.h"
#include "Ivan_Alone/CurlCookies.h"
#include "Ivan_Alone/ConsoleGraph.h"

using json = nlohmann::json;
using namespace std;
using namespace Ivan_Alone;

#define ifor(var, from, to) for(int var = from; var < to; var++)

Network* net;
CurlCookies* cookies;
ConsoleGraph* console = nullptr;
json config;

string save_path;
string __temp;
string __instagram;
string __csrftoken;

string loading_sprite;
string developer_sprite;

map<string, string> getInstagramHeaders(map<string, string> plus = map<string, string>()) {
	map<string, string> elite = {
		{"Referer", "https://www.instagram.com/"},
		{"X-CSRFToken", cookies != nullptr ? cookies->getValidValue("csrftoken") : ""},
		{"X-Instagram-AJAX", "1"},
		{"Content-Type", "application/x-www-form-urlencoded"},
		{"X-Requested-With", "XMLHttpRequest"}
	};

	for (const auto& kvp : plus) {
		elite[(string)kvp.first] = (string)plus.at(kvp.first);
	}

	return elite;
}

bool FileExists(const char* fname) {
  return ::ifstream(fname, ios::in).good();
}

bool contains(string where, vector<string> words) {
	for (unsigned int i = 0; i < words.size(); i++) {
		if (where.find(words.at(i)) != string::npos) {
			return true;
		}
	}

	return false;
}

string jsonString(json str) {	
	string s = str.dump();
	s = s.erase(s.length() - 1, 1);
	s = s.erase(0, 1);
	return s;
}

bool mkdir(string dir) {
	#ifdef _WIN32
		if (contains(dir, vector<string>{"\"", "?", ">", "<", "|", ":", "*"})) {
			return false;
		}
		for (unsigned int i = 0; i < dir.length(); i++) {
			if (dir[i] == '/') dir[i] = '\\';
		}
		dir = "mkdir \"" + dir + "\" > nul 2>&1";
	#else
		dir = "mkdir -p \"" + dir + "\"";
	#endif
	return system(dir.c_str()) == 0;
}

string toString(json a) {
	return a;
}

void errorNote(std::exception* err, int err_pos = 0) {
	string txt = "An error occurred while executing the program, the execution will be stopped!\n    Error: ";
	string e_net = "\n    (Network error happens or Instagram API was changed. Check your network & proxy settings!)";
	switch (err_pos) {
		case 1:
			txt += "Login error";
			break;
		case 2:
			txt += "Checkpoint choice error ()";
			break;
		case 3:
			txt += "Security code sending error";
			break;
		case 4:
			txt += "Feed receiving error";
			break;
		case 5:
			txt += "Stories receiving error";
			break;
		case 6:
			txt += "Current user's stories receiving error";
			break;
		case 7:
			txt += "Current user's pinned stories (full list) receiving error";
			break;
		case 8:
			txt += "Current user's pinned stories (stack) receiving error";
			break;
		case 9:
			txt += "Pinned watch error";
			break;
		case 10:
			txt += "Story watch error";
			break;
		case 11:
			txt += "Empty story file from pinned list";
			break;
		case 12:
			txt += "Empty story file from stories list";
			break;
	}
	txt += e_net;

	console->graphColor(0xC, 0xF);
	string cache = "";
	for (unsigned int i = 0; i < txt.length(); i++) {
		if (txt[i] == '\n') {
			console->graphWriteToLine(cache);
			cache = "";
		} else {
			cache += txt[i];
		}
	}
	console->graphWriteToLine(cache);
	console->graphWriteToLine("");

	if (err != nullptr) {
		console->graphWriteToLine("    Error additional information: ");
		string err_info(err->what());
		string line = "";
		for (unsigned int i = 0; i < err_info.length(); i++) {
			if (i != 0 && i % (114 - 8) == 0) {
				console->graphWriteToLine("        " + ConsoleGraph::trim(line));
				line = "";
			}
			line += err_info[i];
		}
		console->graphWriteToLine("        " + ConsoleGraph::trim(line));
	}
	if (!ConsoleGraph::System::isWin32()) {
		console->graphColor(0xF, 0x0);
	}
	throw 0;
}

void errorNote(int err_pos = 0) {
	errorNote(nullptr, err_pos);
}

string date(time_t tim) {
	tm *ltm = localtime(&tim);
	return ConsoleGraph::toString(1900 + ltm->tm_year) + "." +
			(ltm->tm_mon < 9 ? "0" : "") + ConsoleGraph::toString(1 + ltm->tm_mon) + "." +
			(ltm->tm_mday < 10 ? "0" : "") + ConsoleGraph::toString(ltm->tm_mday) + " - " +
			(ltm->tm_hour < 10 ? "0" : "") + ConsoleGraph::toString(ltm->tm_hour) + "." +
			(ltm->tm_min < 10 ? "0" : "") + ConsoleGraph::toString(ltm->tm_min) + "." +
			(ltm->tm_sec < 10 ? "0" : "") + ConsoleGraph::toString(ltm->tm_sec);
}

void downloadStoriesByLink(bool incognito, string directory, string username, json stories, bool isPinned) {
	console->graphProgressBarCreate();
	console->graphProgressBarUpdate(0, stories.size());
	cookies->reload();

	for (unsigned int id = 0; id < stories.size(); id++) {
		json story = stories.at(id);
		time_t time_public = (time_t)(int)story["taken_at_timestamp"];
		if (story["__typename"] == "GraphStoryImage" || story["__typename"] == "GraphStoryVideo") {
			json img_path = story[story["__typename"] == "GraphStoryVideo" ? "video_resources" : "display_resources"];
			json image_maxres = img_path[img_path.size()-1];

			string filename = directory+"/"+username+" at "+date(time_public)+(story["__typename"] == "GraphStoryVideo" ? ".mp4" : ".jpg");

			if (!FileExists(filename.c_str())) {
				json status;
				if (!incognito) {
					time_t timestamp = time(NULL);
					try {
						string s = net->PostQuery("https://www.instagram.com/stories/reel/seen", map<string, string>({
							{"reelMediaId", story["id"]},
							{"reelMediaOwnerId", story["owner"]["id"]},
							{"reelId", story["owner"]["id"]},
							{"reelMediaTakenAt", console->toString(time_public)},
							{"viewSeenAt", console->toString(timestamp)}
						}), getInstagramHeaders(map<string, string>({
							{"Referer", "https://www.instagram.com/stories/"+username+"/"}
						})));
						status = json::parse(s);
					} catch (std::exception& err) {
						errorNote(&err, isPinned ? 9 : 10);
					} catch (...) {
						errorNote(isPinned ? 9 : 10);
					}
				}

				if (incognito ? true : status["status"] == "ok") {

					string file = net->GetQuery(jsonString(image_maxres["src"]), getInstagramHeaders());
					if (file == "") {
						errorNote(isPinned ? 11 : 12);
					}

					ofstream output(filename, std::ios::binary);

					output << file;

					output.close();
				}
			}
		}
		console->graphProgressBarUpdate(id+1, stories.size());
	}
	console->graphProgressBarClose();
}

bool in_array(char* arg, int argc, char** argv) {
	for (int i = 0; i < argc; i++) {
		if (::string(arg) == ::string(argv[i])) {
			return true;
		}
	}
	return false;
}

void load() {
	cout << "Loading..." << endl;

	string conf_path = "config.json";

	try {
		config = json::parse(ConsoleGraph::file_get_contents(conf_path));
	} catch (...) {
	}

	save_path = "";
	if (config["stories_folder"] != nullptr) {
		__instagram = jsonString(config["stories_folder"]);
	} else {
		__instagram = "Instagram";
	}
	__instagram = save_path+__instagram;
	if (config["temp_folder"] != nullptr) {
		__temp = jsonString(config["temp_folder"]);
	}
	else {
		__temp = "temp";
	}
	__temp = save_path+__temp;
	string __cookie_path = config["cookies_storage"] != nullptr ? config["cookies_storage"] : "curl_cookies.lcf";__cookie_path = __temp+"/"+__cookie_path;
	mkdir(__temp);

	net = new Network(__cookie_path, config["proxy"] != nullptr ? config["proxy"] : "");
	cookies = new CurlCookies(__cookie_path);

	if (cookies->getValidValue("csrftoken") == "") {
		net->PostQuery("https://www.instagram.com", map<string, string>(), getInstagramHeaders());
		cookies->reload();
	}

	__csrftoken = cookies->getValidValue("csrftoken");

	loading_sprite = ConsoleGraph::file_get_contents(save_path+toString(config["loading_sprite_1"] != nullptr ? config["loading_sprite_1"] : ""));
	developer_sprite = ConsoleGraph::file_get_contents(save_path+toString(config["loading_sprite_2"] != nullptr ? config["loading_sprite_2"] : ""));

	cout << "Loading done!" << endl;

	console = new ConsoleGraph();
}

int main(int argc, char** argv) {
	#ifdef _WIN32
		SetConsoleOutputCP(CP_UTF8);
		setvbuf(stdout, nullptr, _IOFBF, 1000);
	#endif

	load();

	console->setInitArgs(argc, argv);
	console->graphTitle("InstaStories Saver");

	console->graphColor(0xF, 0x0, true);

	if (!in_array((char*)"--no-bootsprites", argc, argv)) {
		console->graphDrawPic(0xF, 0xD, loading_sprite, 2);
		console->graphDrawPic(0x0, 0xC, developer_sprite, 1);
	}

	console->graphStartingLine();
	console->graphEmptyLine();
	console->graphWriteToCenterLine("Instagram Stories Saver [Ivan_Alone]");
	console->graphEmptyLine();
	console->graphDottedLine();

	string user = "";

	try {
		if (cookies->getValidValue("ds_user_id") != "") {

			user = cookies->getValidValue("ds_user_id");

		} else {
			while (true) {
				console->graphEmptyLine();
				console->graphWriteToLine("Enter your login & password from Instagram: ");
				console->graphEmptyLine();
				console->graphWriteToLine("Login: ");
				string login = console->graphReadLn();
				console->graphEmptyLine();
				console->graphWriteToLine("Password: ");
				string pass = console->graphReadPassword();
				console->graphEmptyLine();

				if (login != "" && pass != "") {

					json auth_json;
					try {
						auth_json = json::parse(net->PostQuery("https://www.instagram.com/accounts/login/ajax/", map<string, string>({
							{"username", login},
							{"password", pass}
						}), getInstagramHeaders()));
					} catch (std::exception& err) {
						errorNote(&err, 1);
					} catch (...) {
						errorNote(1);
					}

					cookies->reload();
					if (auth_json["authenticated"] == true) {
						user = cookies->getValidValue("ds_user_id");
						console->graphWriteToLine("Logged in in " + date(time(NULL)));
						console->graphEmptyLine();
						console->graphDottedLine();
						break;
					} else if (auth_json["message"] == "checkpoint_required") {
						string checkpoint_url = auth_json["checkpoint_url"];
						json checkout_data_dcd;
						try {
							checkout_data_dcd = json::parse(net->PostQuery("https://www.instagram.com"+checkpoint_url, map<string, string>({
								{"choice", "1"}
							}), getInstagramHeaders()));
						} catch (std::exception& err) {
							errorNote(&err, 2);
						} catch (...) {
							errorNote(2);
						}
						string mail = checkout_data_dcd["fields"]["contact_point"];
						console->graphWriteToLine("Attention: security code was sent to "+mail);

						json sequrity_data;
						int EC_TEST = 0;

						while (sequrity_data["status"] != "ok") {
							if (EC_TEST != 0) {
								console->graphWriteToLine("Error: wrong security code, repeat it!");
								console->graphEmptyLine();
							}
							console->graphWriteToLine("Enter your code: ");

							try {
								sequrity_data = json::parse(net->PostQuery("https://www.instagram.com"+checkpoint_url, map<string, string>({
										{"security_code", console->graphReadLn()}
								}), getInstagramHeaders()));
							} catch (std::exception& err) {
								errorNote(&err, 3);
							} catch (...) {
								errorNote(3);
							}

							EC_TEST = 1;
							console->graphEmptyLine();
						}

						cookies->reload();
						user = cookies->getValidValue("ds_user_id");
						console->graphWriteToLine("Logged in in " + date(time(NULL)));

						console->graphEmptyLine();
						console->graphDottedLine();

						break;
					} else {
						console->graphWriteToLine("Login or password is incorrect, repeat input please!");
					}
				} else {
					console->graphWriteToLine("Login or password is empty, repeat input please!");
				}
				console->graphEmptyLine();
				console->graphDottedLine();
			}
			__csrftoken = cookies->getValidValue("csrftoken");
		}

		console->graphEmptyLine();

		if (user != "") {
			bool incognito = config["incognito"] == nullptr ? false : (bool)config["incognito"];
			if (incognito) {
				console->graphWriteToCenterLine("!!! Incognito mode is activated !!!");
				console->graphEmptyLine();
				console->graphEmptyLine();
			}

			json feed;
			try {
				feed = json::parse(net->GetQuery("https://www.instagram.com/?__a=1", getInstagramHeaders()));
			}	catch (std::exception& err) {
				errorNote(&err, 4);
			} catch (...) {
				errorNote(4);
			}
			string usern = feed["graphql"]["user"]["username"] == nullptr ? "" : feed["graphql"]["user"]["username"];
			console->graphWriteToLine("Grabbing subscribes from "+usern+"'s feed...");
			console->graphEmptyLine();

			json stories;
			try {
				stories = json::parse(net->GetQuery("https://www.instagram.com/graphql/query/?query_id=17890626976041463&variables={}", getInstagramHeaders()));
			} catch (std::exception& err) {
				errorNote(&err, 5);
			} catch (...) {
				errorNote(5);
			}
			stories = stories["data"]["user"]["feed_reels_tray"]["edge_reels_tray_to_reel"]["edges"];
			console->graphWriteToLine("Subscribes grabbed, going to downloading...");
			console->graphEmptyLine();
			console->graphDottedLine();
			console->graphEmptyLine();


			if (stories == nullptr || stories.size() < 1) {
				console->graphWriteToLine("Nothing to download, no stories in your feed!");
				console->graphEmptyLine();
			} else {
				for (unsigned int dss = 0; dss < stories.size(); dss++) {
					json __user = stories.at(dss); {
						json _user       = __user["node"];
						string id         = _user["id"];
						json user_info  = _user["user"];

						string owner = user_info["username"];

						console->graphWriteToLine("Reading & downloading Stories by "+owner+"...");
						console->graphEmptyLine();

						json user_stories;
						try {
							user_stories = json::parse(net->GetQuery("https://www.instagram.com/graphql/query/?query_id=17873473675158481&variables={\"reel_ids\":[\""+id+"\"],\"precomposed_overlay\":false}", getInstagramHeaders()));
						} catch (std::exception& err) {
							errorNote(&err, 6);
						} catch (...) {
							errorNote(6);
						}
						mkdir(__instagram);

						string directory = __instagram+"/";
						directory += jsonString(user_info["username"]);

						mkdir(directory);

						downloadStoriesByLink(incognito, directory, owner, user_stories["data"]["reels_media"][0]["items"], false);
						console->graphEmptyLine();

						console->graphWriteToLine("Trying to find & download Pinned Stories by "+owner+"...");
						console->graphEmptyLine();
						json user_highlight;
						try {
							user_highlight = json::parse(net->GetQuery("https://www.instagram.com/graphql/query/?query_hash=9ca88e465c3f866a76f7adee3871bdd8&variables={\"user_id\":\""+id+"\",\"include_highlight_reels\":true}", getInstagramHeaders()));
						} catch (std::exception& err) {
							errorNote(&err, 7);
						} catch (...) {
							errorNote(7);
						}
						if (user_highlight["data"]["user"]["edge_highlight_reels"]["edges"] != nullptr && user_highlight["data"]["user"]["edge_highlight_reels"]["edges"].size() != 0) {
							string stories_spack = "";
							for (unsigned int p = 0; p < user_highlight["data"]["user"]["edge_highlight_reels"]["edges"].size(); p++) {
								json pack = user_highlight["data"]["user"]["edge_highlight_reels"]["edges"][p]["node"];
								if (pack != nullptr && pack["__typename"] == "GraphHighlightReel" && pack["cover_media"] != nullptr) {
									stories_spack += jsonString(pack["id"]);
									stories_spack += "\",\"";
								}
							}
							if (stories_spack != "") stories_spack = stories_spack.substr(0, stories_spack.length()-3);
							json packs_array;
							try {
								packs_array = json::parse(net->GetQuery("https://www.instagram.com/graphql/query/?query_hash=45246d3fe16ccc6577e0bd297a5db1ab&variables={\"highlight_reel_ids\":[\""+stories_spack+"\"],\"precomposed_overlay\":false}", getInstagramHeaders()));
							} catch (std::exception& err) {
								errorNote(&err, 8);
							} catch (...) {
								errorNote(8);
							}
							json items;
							int sck = 0;
							for (unsigned int blockId = 0; blockId < packs_array["data"]["reels_media"].size(); blockId++) {
								for (unsigned int itemId = 0; itemId < packs_array["data"]["reels_media"][blockId]["items"].size(); itemId++) {
									if (packs_array["data"]["reels_media"][blockId]["items"][itemId] != nullptr) {
										items[sck] = packs_array["data"]["reels_media"][blockId]["items"][itemId];
										sck++;
									}
								}
							}
							downloadStoriesByLink(incognito, directory, owner, items, true);

						} else {
							console->graphWriteToLine("Pinned are empty!");
						}
						console->graphEmptyLine();
						console->graphEmptyLine();
					}
				}
			}

			console->graphDottedLine();
			console->graphEmptyLine();
			console->graphWriteToLine("Downloading of Stories done!");
		}
	} catch (...) {

	}

    console->graphEmptyLine();
    console->graphEndingLine();

    if (!in_array((char*)"--no-exit-pause", argc, argv)) {
        console->graphFinish();
    } else {
        delete console;
    }


	return 0;
}
