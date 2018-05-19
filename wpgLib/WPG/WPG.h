#pragma once
#ifndef WPG_H_
#define WPG_H_

#include <iostream>
#include <string>
#include <fstream>
#include "..\Crypto\Crypto.h"
#include "..\ZLib\ZLib.h"
#include <Windows.h>
#include "..\Common\NFolder\NFolder.h"

#define DEFAULT_MAGIC "RPKG"

using namespace std;

namespace wpgLib {


	class WPG {
	private:

		std::fstream	_handler;
		Crypto*			_crypto = new Crypto();
		ZLib*			_zlib = new ZLib();

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

	private:

		Header*			_header = nullptr;

	public:

		const bool IsOpen() {
			return this->_handler.is_open();
		}

		const Header* GetHeader() {
			return this->_header;
		}

		const FileEntry* GetFile(std::string _name) {
			for (auto i = this->_header->Files.begin(); i != this->_header->Files.end(); i++) {
				if (!i->Name.compare(_name))
					return &(*i);
			}

			return nullptr;
		}

		const std::vector<FileEntry> GetFiles() {
			if (this->_header != nullptr)
				this->_header->Files;
		}

		bool AddFile(FileEntry* _entry) {
			if (this->_header != nullptr) {
				this->_header->Files.push_back(*_entry);
				this->_header->FileCount++;
				return true;
			}

			return false;
		}

		bool RemoveFile(FileEntry* _entry) {
			if (this->_header != nullptr) {
				for (auto i = this->_header->Files.begin(); i != this->_header->Files.end(); i++) {
					if (!(*i).Name.compare(_entry->Name)) {
						i = this->_header->Files.erase(i);
						this->_header->FileCount--;
						return true;
					}
				}
			}

			return false;
		}

		bool ChangeVersion(uint32_t _nVersion) {
			if (this->_header != nullptr) {
				this->_header->Version = _nVersion;
				return true;
			}

			return false;
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

		bool SaveFile(std::string _name) {
			if (this->_header != nullptr) {
				for (auto i = this->_header->Files.begin(); i != this->_header->Files.end(); i++) {
					if (!(*i).Name.compare(_name)) {
						auto path = (*i).Name.substr(0, (*i).Name.find_last_of("\\/"));
						Common::createFolder(path);
						auto name = (*i).Name.substr((*i).Name.find_last_of("\\/") + 1);

						std::fstream out(name, std::ios::out | std::ios::binary);
						out.write((*i)._buffer, (*i).Size);
						out.close();

						return true;
					}
				}
			}

			return false;
		}

		bool SaveFiles() {
			CreateDirectoryA(_header->Path.c_str(), NULL);
			for (auto i = _header->Files.begin(); i != _header->Files.end(); i++) {

				auto path = (*i).Name.substr(0, (*i).Name.find_last_of("\\/"));
				Common::createFolder(path);
				auto name = (*i).Name.substr((*i).Name.find_last_of("\\/") + 1);

				std::fstream out(name, std::ios::out | std::ios::binary);
				out.write((*i)._buffer, (*i).Size);
				out.close();
			
			}
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
						this->_handler.read(reinterpret_cast<char*>(&_entry.Flags),		sizeof(uint32_t));

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
								this->_zlib->Decompress(_entry._buffer, _entry.Size, &nData, nDataSize);
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
										break;
									}

									this->_zlib->Decompress(nData + 4, _entry.Size - 4, &_entry._buffer, (int&)_entry.Size);
									if (_entry.Size != *dataOutSize) {
										break;
									}

									free(dataOutSize);

									break;

								} else

								break;
							default:
								break;
							}
						} else
							return false;
					}
					_header->Files.push_back(_entry);
				}
			}

			return true;
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

}

#endif