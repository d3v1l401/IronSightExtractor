#include <iostream>
#include <conio.h>
#include <WPG\WPG.h>
#include "Common\Colors\Colors.h"
#pragma comment(lib, "wpgLib.lib")

using namespace std;
using namespace AdvancedConsole;

void main(int argc, char* argv[]) {
	cout << Color(AC_WHITE, AC_BACKGROUND) << Color(AC_BLACK) << "[ IronSight Extractor ]" << Color(AC_WHITE) << Color(AC_BLACK, AC_BACKGROUND) << "\n\n";
	cout << "Made by " << Color(AC_RED) << "d3v" << Color(AC_WHITE) << "il401 (" << Color(AC_CYAN) << "http://d3vsite.org" << Color(AC_WHITE) << ")\n";
	cout << "This project is Open Source under " << Color(AC_GREEN) << "GNU GPLv3 License" << Color(AC_WHITE) << ", if you paid for this, you got scammed.\n";

	cout << "Usage: isex.exe <" << Color(AC_GREEN) << "name.wpg" << Color(AC_WHITE) << ">\n\n";

	if (argc == 2) {
		std::string fileName(argv[1]);
		if (fileName.length() > 0) {
			wpgLib::WPG* _wpgFile = new wpgLib::WPG(fileName);
			if (_wpgFile->IsOpen()) {
				cout << "[+] File: " << Color(AC_CYAN) << fileName << Color(AC_WHITE) << "\n";
				cout << " |-[ Magic: " << Color(AC_GREEN) << _wpgFile->GetHeader()->Magic << Color(AC_WHITE) << " ]\n";
				cout << " |-[ Version: " << Color(AC_GREEN) << _wpgFile->GetHeader()->Version << Color(AC_WHITE) << " ]\n";
				cout << " |-[ Files Count: " << Color(AC_GREEN) << _wpgFile->GetHeader()->FileCount << Color(AC_WHITE) << " ]\n |\n";
				cout << " |-[+] Content \n";
				
				_wpgFile->SaveFiles();
			} else 
				cout << Color(AC_RED) << "ERROR:" << Color(AC_WHITE) << " Could not open " << Color(AC_CYAN) << fileName << Color(AC_WHITE) << "\n";
			

			_wpgFile->~WPG();
		}
	}

	_getch();
}