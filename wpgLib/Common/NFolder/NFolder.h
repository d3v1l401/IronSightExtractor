#pragma once
#ifndef _UTILS_NF_H_
#define _UTILS_NF_H_

#include <direct.h>  
#include <io.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <iostream>
#include <list>

namespace wpgLib {
	namespace Common {
		static bool folderExists(const char* folderName) {
			if (_access(folderName, 0) == -1)
				return false;

			DWORD attr = GetFileAttributes((LPCSTR)folderName);
			if (!(attr & FILE_ATTRIBUTE_DIRECTORY))
				return false;

			return true;
		}

		static bool createFolder(std::string folderName) {
			std::list<std::string> folderLevels;
			char* c_str = (char*)folderName.c_str();

			char* strPtr = &c_str[strlen(c_str) - 1];

			do {
				if (folderExists(c_str))
					break;

				do {
					strPtr--;
				} while ((*strPtr != '\\') && (*strPtr != '/') && (strPtr >= c_str));
				folderLevels.push_front(std::string(strPtr + 1));
				strPtr[1] = 0;
			} while (strPtr >= c_str);

			if (_chdir(c_str))
				return true;

			for (std::list<std::string>::iterator it = folderLevels.begin(); it != folderLevels.end(); it++) {
				if (CreateDirectory(it->c_str(), NULL) == 0)
					return true;

				_chdir(it->c_str());
			}

			return false;
		}
	}
}

#endif