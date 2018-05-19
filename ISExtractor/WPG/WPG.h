#pragma once
#ifndef WPG_H_
#define WPG_H_

#pragma warning(disable: 4996)
#include <iostream>
#include <string>
#include <fstream>
#include <gzip.h>
#include <zlib.h>
#include <direct.h>  
#include <io.h>
#include "..\Crypto\Crypto.h"
#include "..\Common\Colors\Colors.h"
#include <Windows.h>

#define DEFAULT_MAGIC "RPKG"


using namespace std;
using namespace AdvancedConsole;

class WPG {

public:

	const enum FLAG_TYPE {
		TYPE_COMPRESSED	 = 1,
		TYPE_ENCRYPTED	 = 2,
		TYPE_BOTH        = TYPE_ENCRYPTED + TYPE_COMPRESSED,
		TYPE_UNKNOWN
	};

	typedef struct FileEntry {
		std::string	Name;
		uint32_t	Offset;
		uint32_t	Size;
		uint32_t	Flags;

		char*		_buffer;
		bool		_threwFlag;
	};

	typedef struct Header {
		std::string				Magic;
		uint32_t				Version;
		std::string				Path;
		uint32_t				FileCount;
		std::vector<FileEntry>	Files;
	};

	Header*			_header		= nullptr;
	std::fstream	_handler;
	Crypto*			_crypto		= new Crypto();

	const bool IsOpen() {
		return this->_handler.is_open();
	}

	const Header* GetHeader() {
		return this->_header;
	}

	const std::vector<FileEntry> GetFiles() {
		if (this->_header != nullptr)
			this->_header->Files;
		
	}

	bool AddFile(FileEntry* _entry) {
		if (this->_header != nullptr) {
			this->_header->Files.push_back(*_entry);
			this->_header->FileCount += 1;
		}
	}

	WPG(std::string filePath) {
		
		this->_handler.open(filePath, std::ios::binary | std::ios::in);

		if (!this->IsOpen())
			return;

		this->_handler.seekg(0, std::ios::beg);
		if (!this->readMeta())
			return;
		if (!this->readFiles())
			return;
		this->_handler.close();
	}

	std::string remoteSpecials(std::string& stringr) {
		std::string lel;
		for (int i = 0; i < stringr.size(); ++i)
			if (((stringr[i] >= 'a' && stringr[i] <= 'z') || (stringr[i] >= 'A' && stringr[i] <= 'Z') || (stringr[i] >= '0' && stringr[i] <= '9')) || stringr[i] == '.')
				lel += stringr[i];

		return lel;
	}

	bool SaveFiles() {
		CreateDirectoryA(_header->Path.c_str(), NULL);
		for (auto i = _header->Files.begin(); i != _header->Files.end(); i++) {

			auto path = (*i).Name.substr(0, (*i).Name.find_last_of("\\/"));
			this->createFolder(path);
			auto name = (*i).Name.substr((*i).Name.find_last_of("\\/") + 1);

			cout << Color(AC_WHITE) << "    |- [ Name: " << Color(AC_GREEN) << (*i).Name << Color(AC_WHITE) << "";
			if ((*i)._threwFlag)
				cout << Color(AC_RED) << "*" << Color(AC_WHITE) << "\n";

			switch ((*i).Flags) {
			case TYPE_COMPRESSED:
				cout << Color(AC_WHITE) << "    |- [ State: " << Color(AC_CYAN) << (*i).Flags << Color(AC_WHITE) << " ( " << Color(AC_YELLOW) << "Compressed" << Color(AC_WHITE) << " )\n";
				break;
			case TYPE_ENCRYPTED:
				cout << Color(AC_WHITE) << "    |- [ State: " << Color(AC_CYAN) << (*i).Flags << Color(AC_WHITE) << " ( " << Color(AC_YELLOW) << "Encrypted" << Color(AC_WHITE) << " )\n";
				break;
			case TYPE_BOTH:
				cout << Color(AC_WHITE) << "    |- [ State: " << Color(AC_CYAN) << (*i).Flags << Color(AC_WHITE) << " ( " << Color(AC_YELLOW) << "Encrypted & Compressed" << Color(AC_WHITE) << " )\n";
				break;
			default:
				cout << Color(AC_WHITE) << "    |- [ State: " << Color(AC_RED) << "UNKNOWN - PLEASE REPORT" << Color(AC_WHITE) << "\n";
				break;
			}
			cout << Color(AC_WHITE) << "    |- [ Size: " << Color(AC_MAGENTA) << (*i).Size << Color(AC_WHITE) << " bytes \n";
			cout << Color(AC_WHITE) << "   [-]\n";

			try {
				std::fstream out(name, std::ios::out | std::ios::binary);
				out.write((*i)._buffer, (*i).Size);
				out.close();
			} catch (std::exception e) {
				cout << Color(AC_RED) << "ERROR: " << Color(AC_WHITE) << e.what() << Color(AC_WHITE) << "\n";
			}
		}
		cout << Color(AC_WHITE) << "[-]\n";
		return true;
	}

	~WPG() {
		if (this->_header != nullptr) {
			this->_header->Files.clear();

			free(this->_header);
		}

		if (this->IsOpen())
			this->_handler.close();
	}

private:



	bool folderExists(const char* folderName) {
		if (_access(folderName, 0) == -1) {
			//File not found
			return false;
		}

		DWORD attr = GetFileAttributes((LPCSTR)folderName);
		if (!(attr & FILE_ATTRIBUTE_DIRECTORY)) {
			// File is not a directory
			return false;
		}

		return true;
	}

	bool createFolder(std::string folderName) {
		list<std::string> folderLevels;
		char* c_str = (char*)folderName.c_str();

		// Point to end of the string
		char* strPtr = &c_str[strlen(c_str) - 1];

		// Create a list of the folders which do not currently exist
		do {
			if (folderExists(c_str)) {
				break;
			}
			// Break off the last folder name, store in folderLevels list
			do {
				strPtr--;
			} while ((*strPtr != '\\') && (*strPtr != '/') && (strPtr >= c_str));
			folderLevels.push_front(string(strPtr + 1));
			strPtr[1] = 0;
		} while (strPtr >= c_str);

		if (_chdir(c_str)) {
			return true;
		}

		// Create the folders iteratively
		for (list<std::string>::iterator it = folderLevels.begin(); it != folderLevels.end(); it++) {
			if (CreateDirectory(it->c_str(), NULL) == 0) {
				return true;
			}
			_chdir(it->c_str());
		}

		return false;
	}

	inline bool replace(std::string& str, const std::string& from, const std::string& to) {
		size_t start_pos = str.find(from);
		if (start_pos == std::string::npos)
			return false;
		str.replace(start_pos, from.length(), to);
		return true;
	}

	CryptoPP::ZlibDecompressor* _unzipper = new CryptoPP::ZlibDecompressor();

	void decompress(char* _in, int _inSize, char** _out, int& _outSize) {
		
		free(this->_unzipper);
		_unzipper = new CryptoPP::ZlibDecompressor();
		this->_unzipper->PutMessageEnd((CryptoPP::byte*)_in, _inSize);

		_outSize = this->_unzipper->MaxRetrievable();
		*_out = new char[_outSize]{ 0 };
		this->_unzipper->Get((CryptoPP::byte*)*_out, _outSize);
	}

	bool readFiles() {
		if (this->IsOpen() && this->_header != nullptr) {

			for (int i = 0; i < this->_header->FileCount; i++) {
				FileEntry _entry;

				char* _name = new char[129]{ 0 };
				this->_handler.read(reinterpret_cast<char*>(_name), 128);
				auto _len = strlen(_name);
				if (_len > 0) {

					_entry.Name.assign(_name);
					free(_name);

					this->_handler.read(reinterpret_cast<char*>(&_entry.Offset),	sizeof(uint32_t));
					this->_handler.read(reinterpret_cast<char*>(&_entry.Size),		sizeof(uint32_t));
					this->_handler.read(reinterpret_cast<char*>(&_entry.Flags),	sizeof(uint32_t));

					if (_entry.Size > 4) {
						auto _curOff = this->_handler.tellg();
						this->_handler.seekg(_entry.Offset + 4, std::ios::beg);
						_entry._buffer = new char[(_entry.Size) + 1]{ 0 };
						this->_handler.read(_entry._buffer, _entry.Size - 4);
						this->_handler.seekg(_curOff, std::ios::beg);


						char* nData = nullptr;
						int nDataSize = 0;

						int DataSize = (char *)_entry._buffer[1] - (char *)*_entry._buffer;
						if (DataSize <= 4) {
							_entry._threwFlag = true;
						}

						switch (_entry.Flags) {
						case TYPE_COMPRESSED:
							this->decompress(_entry._buffer, _entry.Size, &nData, nDataSize);
							_entry._buffer = nData;
							_entry.Size = nDataSize;

							break;

						case TYPE_ENCRYPTED:

							free(this->_crypto);
							this->_crypto = new Crypto();
							this->_crypto->Decrypt(_entry._buffer, _entry.Size, &nData, nDataSize);

							free(_entry._buffer);
							_entry._buffer = nData + 4;
							_entry.Size -= 4;

							break;
						case TYPE_BOTH:
							free(this->_crypto);
							this->_crypto = new Crypto();
							this->_crypto->Decrypt(_entry._buffer, _entry.Size, &nData, nDataSize);
							if (nDataSize > 0) {
								free(_entry._buffer);

								uint32_t* dataOutSize = new uint32_t{ 0 };
								memcpy(dataOutSize, nData, 4);

								if (_entry.Size > 0x6400000) {
									cout << Color(AC_YELLOW) << "WARNING:" << Color(AC_WHITE) << " Wrong size!" << Color(AC_WHITE) << "\n";
									break;
								}

								this->decompress(nData + 4, _entry.Size - 4, &_entry._buffer, (int&)_entry.Size);
								if (_entry.Size != *dataOutSize) {
									cout << Color(AC_YELLOW) << "WARNING:" << Color(AC_WHITE) << " post-process differential size, ignoring!" << Color(AC_WHITE) << "\n";
								}

								free(dataOutSize);

								break;

							} else
								cout << Color(AC_RED) << "ERROR:" << Color(AC_WHITE) << " Decryption failed." << Color(AC_WHITE) << "\n";

							break;
						default:

							cout << Color(AC_RED) << "ERROR:" << Color(AC_WHITE) << " Unknown state flag, you should report this to me!" << Color(AC_WHITE) << "\n";

							break;
						}
						

					} else
						cout << Color(AC_RED) << "ERROR:" << Color(AC_WHITE) << " Something went wrong..." << Color(AC_WHITE) << "\n";

				}
				_header->Files.push_back(_entry);

			}

		}

		return false;
	}

	bool readMeta() {
		if (this->IsOpen()) {
			if (this->_header != nullptr) {
				this->_header->Files.clear();

				free(this->_header);
			}
			
			this->_header = new Header();

			char* _magic = new char[5]{ 0 };
			this->_handler.read(_magic, 4);

			if (!strcmp(_magic, DEFAULT_MAGIC)) {
				this->_header->Magic.assign(_magic);
				free(_magic);

				this->_handler.read(reinterpret_cast<char*>(&this->_header->Version), sizeof(uint32_t));
				
				char* _path = new char[129]{ 0 };
				this->_handler.read(reinterpret_cast<char*>(_path), 128);
				auto _len = strlen(_path);
				if (_len > 0) {
					this->_header->Path.assign(_path);
					free(_path);

					this->_handler.read(reinterpret_cast<char*>(&this->_header->FileCount), sizeof(uint32_t));
					if (this->_header->FileCount > 0)
						return true;
					else
						return false;
				}

				free(_path);
				return false;
			}

			free(_magic);
			return false;
		}

		return false;
	}
};

#endif