#pragma once
#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#include <seed.h>
#include <modes.h>
#pragma comment(lib, "cryptlib.lib")

namespace wpgLib {

	class Crypto {
	private:
		char* _key = nullptr;
		char* _vec = nullptr;

		CryptoPP::CBC_Mode<CryptoPP::SEED>::Encryption* _encryptor;
		CryptoPP::CBC_Mode<CryptoPP::SEED>::Decryption* _decryptor;

		static uint32_t _defKey[4];
		static uint32_t _defVec[4];

		char* Pad(char* _data, int _size, int& _outSize) {
			int len = _size + CryptoPP::SEED::BLOCKSIZE - (_size % CryptoPP::SEED::BLOCKSIZE);
			auto _out = new char[len] {0};
			memcpy(_out, _data, _size);
			_outSize = len;
			return _out;
		}

		char* Unpad(char* _data, int _size, int& _outSize) {
			// TODO
			return nullptr; 
		}

	public:

		void SetKey(char* _key, char* _vec) {
			this->_encryptor = new CryptoPP::CBC_Mode<CryptoPP::SEED>::Encryption((CryptoPP::byte*)_key, CryptoPP::SEED::DEFAULT_KEYLENGTH, (CryptoPP::byte*)_vec);
			this->_decryptor = new CryptoPP::CBC_Mode<CryptoPP::SEED>::Decryption((CryptoPP::byte*)_key, CryptoPP::SEED::DEFAULT_KEYLENGTH, (CryptoPP::byte*)_vec);
		}

		Crypto() :
			_key((char*)Crypto::_defKey), _vec((char*)Crypto::_defVec) {
			this->SetKey(_key, _vec);
		}

		Crypto(char* _key, char* _vec):
			_key(_key), _vec(_vec) {
			this->SetKey(_key, _vec);
		}

		~Crypto() {
			if (this->_key != nullptr) 
				free(this->_key);
			if (this->_vec != nullptr)
				free(this->_vec);
		}

		void Encrypt(char* _input, int _size, char** _output, int& _sizeOut) {
			_sizeOut = 0;
			if (_input != nullptr && _size > 0 && _output != nullptr) {
			
				int _paddedSize = 0;
				_input = this->Pad(_input, _size, _paddedSize);

				*_output = new char[_paddedSize] {0};
				this->_encryptor->ProcessData((CryptoPP::byte*)*_output, (CryptoPP::byte*)_input, _paddedSize);
				_sizeOut = _paddedSize;
				free(_input);
			}
		}

		void Decrypt(char* _input, int _size, char** _output, int& _sizeOut) {
			_sizeOut = 0;
			if (_input != nullptr && _size > 0 && _output != nullptr) {
				int _paddedSize = 0;
				_input = this->Pad(_input, _size, _paddedSize);

				*_output = new char[_paddedSize] {0};
				this->_decryptor->ProcessData((CryptoPP::byte*)*_output, (CryptoPP::byte*)_input, _paddedSize);
				_sizeOut = _paddedSize;
				free(_input);
			}
		}
	};

}

#endif