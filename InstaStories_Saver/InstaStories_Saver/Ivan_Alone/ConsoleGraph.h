#ifndef IVAN_ALONE_CONSOLEGRAPH_H_
#define IVAN_ALONE_CONSOLEGRAPH_H_

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#ifdef _WIN32
	#include <windows.h>
	#include <conio.h>
	#include <regex>
	#define sleep(time) Sleep(time*1000)
#else
	#include <unistd.h>
	#include <termios.h>
	#define sleep(time) sleep(time)
#endif

using namespace std;

#define BINARY_T vector<bool>
#define HEXIMAL_T vector<int>

namespace Ivan_Alone {

	class ConsoleGraph {
		private:
			bool useStarsAsWinBuilders = false;
			int* colors = new int[2];
			int slide = 0;
			bool win32CacheTest = false;
			string win32Cache = "";
			bool graphEnabled = false;
			bool progressBarLockVar = false;

			static string winversion;
			vector<string> args;

			void init(bool useStarsAsWinBuilders, bool disableNoGraph, string window);
			void initLinuxColors();
			void exeW(const char* aim);
			void configureWindowSize();

			string getSlideSpaces();
			string colorCut(int color);
			void graphLine(bool isFull = false, bool isEmpty = false, int start_marker = -1);
			void graphWrite(string text, bool isCenter = false);
			string normalize(string input, unsigned int mod = 8, char block = '0');
			BINARY_T invert(BINARY_T abin);

			HEXIMAL_T dechex(int dec);
			HEXIMAL_T parsehex(string hex_str);
			int hexdec(HEXIMAL_T hex);
			string hexstr(HEXIMAL_T hex);

			BINARY_T decbin(int dec);
			BINARY_T parsebin(string bin);
			int bindec(BINARY_T bin);
			string binstr(BINARY_T bin);

			bool contains(string in, vector<string> find);
			int convert2bytes(unsigned char byte1, unsigned char byte2);
			int wingetch();
		public:
			class System {
				public:
					static bool isWin32();
					static string getWin32Version();
			};
			ConsoleGraph();
			ConsoleGraph(string __do_not_configure_window);
			ConsoleGraph(bool useStarsAsWinBuilders);
			ConsoleGraph(bool useStarsAsWinBuilders, bool disableNoGraph);
			void setInitArgs(int argc, char** argv);
			void graphTitle(string title = "");
			void graphSetSlide(int slide);
			void graphClear();
			bool graphColor(int bg, int txt, bool store = false);
			bool graphColorReset();
			string graphReadLn(string text = "");
			string graphReadPassword(string text = "");
			void graphDottedLine();
			void graphFilledLine();
			void graphEmptyLine();
			void graphStartingLine();
			void graphEndingLine();
			void graphWriteToCenterLine(string text);
			bool graphDrawPicFile(int bg_color, int fg_color, string filename, int show_timer = 0, vector<string> args = {});
			bool graphDrawPic(int bg_color, int fg_color, string data, int show_timer = 0, vector<string> args = {});
			void graphProgressBarCreate();
			void graphProgressBarUpdate(int current, int count);
			void graphProgressBarClose();
			void graphWriteToLine(string text);
			void graphFinish();
			void graphPause();
			static string toString(int a);
			static string ltrim(string in);
			static string rtrim(string in);
			static string trim(string in);
			static string file_get_contents(string filename);
	};
}

#endif
