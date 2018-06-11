#include "ConsoleGraph.h"

#ifdef _WIN32
	#pragma warning(disable : 4996)
#endif

#define system(prog) if(system(prog));

namespace Ivan_Alone {
	bool ConsoleGraph::System::isWin32() {
		return System::getWin32Version() != "";
	}

	string ConsoleGraph::winversion = "-1";

	string ConsoleGraph::System::getWin32Version() {
		#ifdef _WIN32
			if (winversion == "-1") {
				char cmd[256];
				char buf[BUFSIZ];
				FILE *ptr;
				if ((ptr = _popen("ver", "r")) != NULL) {
					int j = 0;
					while (fgets(buf, BUFSIZ, ptr) != NULL) {
						for (int i = 0; i < strlen(buf); i++) {
							cmd[j] = buf[i];
							j++;
						}
					}
					(void)_pclose(ptr);
				}
				if (::string(cmd).find('.') == string::npos) {
					winversion = toString((DWORD)(LOBYTE(LOWORD(GetVersion())))) + "." + toString((DWORD)(HIBYTE(LOWORD(GetVersion()))));
				} else {
					std::tr1::cmatch res;
					std::tr1::regex rx("[0-9]{1,2}\\.[0-9]{1,2}\\.[0-9]{4,5}");
					std::tr1::regex_search(cmd, res, rx);

					winversion = res[0];
					for (int i = winversion.length() - 1; i >= 0; i--) {
						bool flag = winversion[i] == '.';
						winversion.erase(i, 1);
						if (flag) break;
					}
				}
			}

			return winversion;
		#else
			return "";
		#endif
	}

	ConsoleGraph::ConsoleGraph() {
		init(false, false, "");
	}

	ConsoleGraph::ConsoleGraph(string __do_not_configure_window) {
		init(false, false, __do_not_configure_window);
	}

	ConsoleGraph::ConsoleGraph(bool useStarsAsWinBuilders) {
		init(useStarsAsWinBuilders, false, "");
	}

	ConsoleGraph::ConsoleGraph(bool useStarsAsWinBuilders, bool disableNoGraph) {
		init(useStarsAsWinBuilders, disableNoGraph, "");
	}

	void ConsoleGraph::init(bool useStarsAsWinBuilders, bool disableNoGraph, string window) {
		cout.clear();
		this->colors[0] = 0x0;
		this->colors[1] = 0x7;
		initLinuxColors();
		this->graphEnabled = !disableNoGraph;

		if (window == "__do_not_configure_window") {
			this->useStarsAsWinBuilders = (System::isWin32() && System::getWin32Version() != "10.0") ? true : false;
			return;
		}

		this->useStarsAsWinBuilders = (System::isWin32() && System::getWin32Version() != "10.0") ? true : useStarsAsWinBuilders;
		configureWindowSize();
		graphClear();
	}

	void ConsoleGraph::setInitArgs(int argc, char** argv) {
		this->args.clear();
		this->graphEnabled = true;
		for (int i = 0; i < argc; i++) {
			string arg(argv[i]);
			if (arg == "--use-stars") {
				this->useStarsAsWinBuilders = true;
			}
			if (arg == "--no-graph") {
				this->graphEnabled = false;
			}
			this->args.push_back(arg);
		}
	}

	void ConsoleGraph::graphClear() {
		system(System::isWin32() ? "cls" : "reset");
	}

	string ConsoleGraph::graphReadLn(string text) {
		if (this->progressBarLockVar) return "";

        if (this->graphEnabled) graphLine();
        cout << (text == "" ? "" : text + " ") + getSlideSpaces() + "> ";
        char buffer[256];
        cin >> buffer;
        cout << (char)(0x0D);
        string read;
        read = buffer;
        return read;
    }

	string ConsoleGraph::graphReadPassword(string text) {
		if (this->progressBarLockVar) return "";

        if (this->graphEnabled) graphLine();
        cout << (text == "" ? "" : text + " ") + getSlideSpaces() + "> ";
        char buffer[256];
        cin >> buffer;
        cout << (char)(0x0D);
        string read;
        read = buffer;
        string mask = "";
        string mask_value = "";
        mask_value += (char)0xE2;
        mask_value += (char)0x96;
        mask_value += (char)0x92;
        if (System::isWin32()) {
            if (System::getWin32Version() == "10.0") {
                for (unsigned int i = 0; i < 117-this->slide-2-(text==""?0:(text.length())+1); i++) {
                    cout << (char)(0x08);
                    if (i < read.length()) {
                    	mask += mask_value;
                    }
                }
            }
        } else {
        	cout << "\033[1A";
            cout << "\033[" << 3 + text.length() + 2 + this->slide << "C";
            for (unsigned i = 0; i < read.length(); i++) {
                mask += mask_value;
            }
        }
        if (!System::isWin32() || System::getWin32Version() == "10.0") {
            cout << mask;
            cout << (char)(0x0A);
            cout << (char)(0x0D);
        }
        return read;
    }

	bool ConsoleGraph::graphColor(int bg, int txt, bool store) {
        bg = max(0, min(15, bg));
        txt = max(0, min(15, txt));
        if (store) this->colors = new int[2]{bg, txt};
        if (System::isWin32()) {
            system(("color "+hexstr(dechex(bg))+hexstr(dechex(txt))).c_str());
            return true;
        } else {
            initLinuxColors();
            system(("setterm --background "+colorCut(bg)+" --foreground "+colorCut(txt)).c_str());
            return true;
        }
    }

	bool ConsoleGraph::graphColorReset() {
        return graphColor(this->colors[0], this->colors[1]);
    }

    void ConsoleGraph::graphDottedLine() {
		if (this->progressBarLockVar) return;

		if (!this->graphEnabled) {
			cout << "= = = =";
            cout << (char)(0x0A);
            cout << (char)(0x0D);
			return;
		}
        string gen = "";
        for (int i = 0; i < 114; i++) {
            gen += (i%2) ? " " : (this->useStarsAsWinBuilders?"*":"=");
        }
        graphLine();
        graphWrite(gen, true);
    }

    void ConsoleGraph::graphFilledLine() {
		if (this->progressBarLockVar) return;

		if (!this->graphEnabled) {
			cout << "=======";
            cout << (char)(0x0A);
            cout << (char)(0x0D);
			return;
		}
        graphLine(true);
    }

    void ConsoleGraph::graphEmptyLine() {
		if (this->progressBarLockVar) return;

		if (!this->graphEnabled) {
			cout << "       ";
            cout << (char)(0x0A);
            cout << (char)(0x0D);
			return;
		}
        graphLine(false, true);
    }

    void ConsoleGraph::graphStartingLine() {
		if (this->progressBarLockVar) return;

		if (!this->graphEnabled) {
			cout << "=======";
            cout << (char)(0x0A);
            cout << (char)(0x0D);
			return;
		}
        graphLine(true, false, 0);
    }

    string ConsoleGraph::ltrim(string in) {
    	while (in[0] == ' ' || in[0] == '\t' || in[0] == '\n') {
    		in = in.erase(0,1);
    	}
    	return in;
    }

    string ConsoleGraph::rtrim(string in) {
    	while (in[in.length()-1] == ' ' || in[in.length()-1] == '\t' || in[in.length() - 1] == '\n') {
    		in = in.erase(in.length()-1,1);
    	}
    	return in;
    }

    string ConsoleGraph::trim(string in) {
    	return ltrim(rtrim(in));
    }

    void ConsoleGraph::graphEndingLine() {
		if (this->progressBarLockVar) return;

		if (!this->graphEnabled) {
			cout << "=======";
            cout << (char)(0x0A);
            cout << (char)(0x0D);
			return;
		}
        graphLine(true, false, 1);
    }

    void ConsoleGraph::graphWriteToCenterLine(string text) {
		if (this->progressBarLockVar) return;

		if (!this->graphEnabled) {
			cout << text;
            cout << (char)(0x0A);
            cout << (char)(0x0D);
			return;
		}
        graphLine();
        text = trim(text);
        string str = "";
        for (int i = 0; i < min(114, (int)(text.length())); i++) {
            str += text[i];
        }
        float d = (((float)str.length())/2.0);
        int r = round(d);
        int s = r < d ? r+1 : r;
        for (int i = 0; i < 57-s; i++) {
            str = " "+str;
        }
        graphWrite(str, true);
    }

    string ConsoleGraph::file_get_contents(string filename) {
		string words = "";
		ifstream file(filename);
		if (!file.good()) return "";
		char ch = 0;
		file.unsetf(ios::skipws);
		while (file >> ch) {
			words += ch;
		}
    	return words;
    }

	bool ConsoleGraph::contains(string in, vector<string> find) {
		for (unsigned int i = 0; i < find.size(); i++) {
			if (in.find(find.at(i)) != string::npos) {
				return true;
			}
		}
		return false;
	}

    bool ConsoleGraph::graphDrawPicFile(int bg_color, int fg_color, string filename, int show_timer, vector<string> args) {
		if (this->progressBarLockVar) return false;

        return graphDrawPic(bg_color, fg_color, file_get_contents(filename), show_timer, args);
    }

    bool ConsoleGraph::graphDrawPic(int bg_color, int fg_color, string data, int show_timer, vector<string> args) {
		if (this->progressBarLockVar) return false;

    	if (!this->graphEnabled) {
			return false;
		}

        if (data.substr(0, 7) != "ACONPIC") {
            return false;
        }

        bool restore = true;
        bool clear = true;
        for (unsigned int i = 0; i < args.size(); i++) {
        	if (args.at(i) == "restore") {
        		restore = false;
        	}
        	if (args.at(i) == "clear") {
        		clear = false;
        	}
        }

        graphClear();
        graphColor(bg_color, fg_color);

        bool isWin10 = System::getWin32Version() == "10.0";

        vector<vector<char>> modes;
        if ((!System::isWin32() || isWin10) && !this->useStarsAsWinBuilders) {
            modes = {
            		{' '},
					{(char)0xE2, (char)0x96, (char)0x84},
					{(char)0xE2, (char)0x96, (char)0x80},
					{(char)0xE2, (char)0x96, (char)0x88}
            };
        } else {
            modes = {
            		{' '},
            		{','},
            		{'\''},
            		{'#'}
            };
        }

        int x_size = convert2bytes((unsigned char)data[7], (unsigned char)data[8]);
        int y_size = convert2bytes((unsigned char)data[9], (unsigned char)data[10]);

        float ox_test = ((float)x_size)/4.0;
        int ox_test_f = (int)(ox_test);
        int x_bytes = ox_test_f + (ox_test_f < ox_test ? 1 : 0);

        if (x_bytes*y_size != (int)data.length()-11) {
            return false;
        }

        int start = 11;
        string image;
        for (int i = 0; i < y_size; i++) {
            string line = data.substr(start, x_bytes);
            int x = 0;
            bool _exit = false;
            for (unsigned int s = 0; s < line.length() && !_exit; s++) {
                string chain = normalize(binstr(decbin((unsigned char)line[s])));
                for (int px = 0; px < 4; px++) {
                    if (x >= x_size) {
                    	_exit = true;
                    	break;
                    }
                    vector<char> lst = modes.at(bindec(parsebin(chain.substr(px*2, 2))));
                    for (unsigned int sym = 0; sym < lst.size(); sym++) {
                    	image += (lst[sym]);
                    }
                    x++;
                }
            }

            if (!System::isWin32() || isWin10) {
            	image += '\n';
            }
            start += x_bytes;
        }

        cout.flush();
        cout.write(image.c_str(), image.length()-(!System::isWin32() || isWin10 ? 2 : 1));
        cout.flush();

        if (show_timer <= 0) {
            graphPause();
        } else {
            sleep(show_timer);
        }

        if (clear) graphClear();
        if (restore) graphColorReset();

    	return true;
    }

	void ConsoleGraph::graphProgressBarCreate() {
		if (this->progressBarLockVar) return;
		this->progressBarLockVar = true;

		if (!this->graphEnabled) {
			return;
		}
        graphLine();
        cout << (char)(0x08);
    }

	void ConsoleGraph::graphProgressBarUpdate(int current, int count) {
		if (!this->progressBarLockVar) return;

        string sym = this->useStarsAsWinBuilders ? "#" : "▓";
        string cur_txt;

        if (current > count) {
            cur_txt = toString(count);
        } else {
            cur_txt = toString(current);
            for (unsigned int i = 0; i < toString(count).length()-toString(current).length(); i++) {
                cur_txt = " "+cur_txt;
            }
        }

        string counter = " " + cur_txt + " / "  + toString(count) + "  ";
        int len = counter.length();

		if (!this->graphEnabled) {
			cout << counter;
			for (int i = 0; i < len; i++)
				cout << (char)(0x08);
			return;
		}

        float _100_perc = 114-len;

        int blocks = round((((float)current)/(count == 0 ? -1.0 : (float)count))*_100_perc);

        for (int i = 0; i < blocks; i++) {
            counter += sym;
        }

        int d = counter.length()-(this->useStarsAsWinBuilders?0:2*blocks);

        for (int i = 0; i < d; i++) {
        	counter += (char)(0x08);
        }
        cout.flush();
        cout << counter;

    }

	void ConsoleGraph::graphProgressBarClose() {
		if (!this->progressBarLockVar) return;
		this->progressBarLockVar = false;

		cout << (char)(0x0A) << (char)(0x0D);
    }

	void ConsoleGraph::graphWriteToLine (string text) {
		if (this->progressBarLockVar) return;

		if (!this->graphEnabled) {
			cout << getSlideSpaces() << text << (char)(0x0A) << (char)(0x0D);
			return;
		}
        graphLine();
        graphWrite(text);
		cout.flush();
    }

    void ConsoleGraph::graphFinish() {
		cout.flush();
        graphPause();
        exit(0);
    }

	void ConsoleGraph::graphPause() {
		if (this->progressBarLockVar) return;
		this->wingetch();
    }

	void ConsoleGraph::initLinuxColors() {
		if (!System::isWin32()) {
			vector<float*> color_reloc = {
				new float[3]{0x00, 0x00, 0x00},
				new float[3]{0x00, 0x00, 0x80},
				new float[3]{0x00, 0x80, 0x00},
				new float[3]{0x00, 0x80, 0x80},
				new float[3]{0x80, 0x00, 0x00},
				new float[3]{0x80, 0x00, 0x80},
				new float[3]{0x80, 0x80, 0x00},
				new float[3]{0xFF, 0xFF, 0xFF}
			};
			for (unsigned int id = 0; id < color_reloc.size(); id++) {
				float* color = color_reloc.at(id);
				system(("tput initc "+toString(id)+" "+toString(round(color[0]*(1000.0/255.0)))+" "+toString(round(color[1]*(1000.0/255.0)))+" "+toString(round(color[2]*(1000.0/255.0)))).c_str());
			}
		}
	}
	string ConsoleGraph::toString(int a) {
		#ifdef _WIN32
			return std::to_string(a);
		#else
			return std::to_string(a);
		#endif
	}

	void ConsoleGraph::graphSetSlide(int slide) {
		this->slide = slide;
	}

	void ConsoleGraph::graphTitle(string title) {
        title = title==""?"Console Graph Class App":title;
        if (System::isWin32()) {
            system(("title "+title).c_str());
        } else {
        	cout<< "\033]2;" << title << "\007";
        }
    }

    string ConsoleGraph::normalize(string input, unsigned int mod, char block) {
        while(input.length() < mod) {
            input = block + input;
        }
        return input;
    }

    BINARY_T ConsoleGraph::invert(BINARY_T abin) {
    	BINARY_T _new;
    	for (int i = abin.size()-1; i >= 0; i--) {
    		_new.push_back(abin.at(i));
    	}
    	return _new;
    }

	string ConsoleGraph::binstr(BINARY_T bin) {
		string b = "";
    	for (int i = bin.size()-1; i >= 0; i--) {
    		b += bin.at(i) ? "1" : "0";
    	}
    	return (b==""?"0":b);
	}

	BINARY_T ConsoleGraph::parsebin(string bin_str) {
		BINARY_T bin;
		unsigned int i = 0;
		string ref = "";
		while ((bin_str[i] == '0' || bin_str[i] == '1') && i < bin_str.length()) {
			ref += bin_str[i];
			i++;
		}
		for (unsigned int j = 0; j < ref.length(); j++) {
			bin.push_back(ref[j] == '1');
		}
		return invert(bin);
	}

	BINARY_T ConsoleGraph::decbin(int dec) {
		BINARY_T t;
		while (dec > 1) {
			t.push_back((bool)(dec % 2));
			dec = dec / 2;
		}
		t.push_back((bool)(dec));
		return t;
	}

	int ConsoleGraph::bindec(BINARY_T bin) {
		int timer = 0;
		for (unsigned int i = 0; i < bin.size(); i++) {
			timer += bin.at(i) *  pow(2, i);
		}
		return timer;
	}

	int ConsoleGraph::hexdec(HEXIMAL_T hex) {
		int timer = 0;
		for (unsigned int i = 0; i < hex.size(); i++) {
			timer += hex.at(i) *  pow((float)16, i);
		}
		return timer;
	}

	HEXIMAL_T ConsoleGraph::dechex(int dec) {
		HEXIMAL_T hex;
		while (dec > 15) {
			hex.push_back(dec % 16);
			dec = dec / 16;
		}
		hex.push_back(dec % 16);
		return hex;
	}

	HEXIMAL_T ConsoleGraph::parsehex(string hex_str) {
		HEXIMAL_T hex;
		string reform = "";
		unsigned int i = 0;
		while (((hex_str[i] >= '0' && hex_str[i] <= '9') || (hex_str[i] >= 'A' && hex_str[i] <= 'F') || (hex_str[i] >= 'a' && hex_str[i] <= 'f')) && i < hex_str.length()) {
			if (hex_str[i] >= 'a' && hex_str[i] <= 'f') {
				reform += hex_str[i]-32;
			} else {
				reform += hex_str[i];
			}
			i++;
		}
		for (int j = reform.length()-1; j >= 0; j--) {
			if (reform[j] >= 'A' && reform[j] <= 'F') {
				hex.push_back(reform[j] - 'A' + 10);
			} else {
				hex.push_back(reform[j] - '0');
			}
		}
		return hex;
	}

	string ConsoleGraph::hexstr(HEXIMAL_T hex) {
		string str = "";
    	for (int i = hex.size()-1; i >= 0; i--) {
    		if (hex.at(i) < 10) {
    			str += 48+hex.at(i);
    		} else {
    			str += 55+hex.at(i);
    		}
    	}
		return str;
	}

	string ConsoleGraph::colorCut(int color) {
		return toString(color > 7 ? color & 7 : color);
	}

	void ConsoleGraph::configureWindowSize() {
		if (System::isWin32()) {
			system("mode con:cols=120 lines=30");
		} else {
			cout << "\e[8;30;120t";
		}
	}

	void ConsoleGraph::graphLine(bool isFull, bool isEmpty, int start_marker) {
		if (System::isWin32()) {
			cout << ' ';
		} else {
			cout << (char)0x1B << "[1C";
		}
		cout << (this->useStarsAsWinBuilders ? "*" : (start_marker == -1 ? (isFull ? "╠" : "║") : (start_marker == 0 ? "╔" : "╚")));
		for(int i = 0; i < 116; i++)
			cout << (isFull ? (this->useStarsAsWinBuilders ? "*" : "═") : " ");
		cout << (this->useStarsAsWinBuilders ? "*" : (start_marker == -1 ? (isFull ? "╣" : "║" ) : (start_marker == 0 ? "╗" : "╝")));
		if (isFull) {
			cout << (char)0x0A << (char)0x0D;
			return;
		}
		if (!isEmpty) {
			for(int i = 0; i < 116; i++)
				cout << (char)0x08;
		} else {
			cout << (char)0x0A << (char)0x0D;
		}
		cout.flush();
	}

	void ConsoleGraph::graphWrite(string text, bool isCenter) {
        text = (isCenter ? "" : getSlideSpaces()) + text;
        string str = "";
        for (int i = 0; i < min(114, (int)text.length()); i++) {
            str += text[i];
        }
        cout << str << (char)(0x0A) << (char)(0x0D);
	}

	string ConsoleGraph::getSlideSpaces() {
        string str = "";
        for (int i = 0; i < this->slide; i++) str += " ";
        return str;
    }

	int ConsoleGraph::wingetch() {
		#ifdef _WIN32
			return _getch();
		#else
			int ch;
			struct termios oldt, newt;
			tcgetattr( STDIN_FILENO, &oldt );
			newt = oldt;
			newt.c_lflag &= ~( ICANON | ECHO );
			tcsetattr( STDIN_FILENO, TCSANOW, &newt );
			ch = getchar();
			tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
			return ch;
		#endif
	}

	int ConsoleGraph::convert2bytes(unsigned char byte1, unsigned char byte2) {
		return bindec(parsebin(normalize(binstr(decbin((int)byte1)))+normalize(binstr(decbin((int)byte2)))));
	}
}
