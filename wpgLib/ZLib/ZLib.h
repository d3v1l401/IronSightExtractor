#pragma once
#ifndef _ZLIB_H_
#define _ZLIB_H_

#include <zlib.h>
#include <smartptr.h>
#pragma comment(lib, "cryptlib.lib")

namespace wpgLib {

	class ZLib {
	private:

		std::shared_ptr<CryptoPP::ZlibDecompressor> _unzipper;
		std::shared_ptr<CryptoPP::ZlibCompressor>	_zipper;

	public:

		ZLib(): _unzipper(new CryptoPP::ZlibDecompressor()), _zipper(new CryptoPP::ZlibCompressor()) {}
		~ZLib() {
			_unzipper.~shared_ptr();
			_zipper.~shared_ptr();
		}

		void ZLib::Compress(char* _input, int _inSize, char** _output, int& _outSize) {
			this->_zipper->PutMessageEnd((CryptoPP::byte*)_input, _inSize);

			_outSize = this->_zipper->MaxRetrievable();
			*_output = new char[_outSize] { 0 };
			this->_zipper->Get((CryptoPP::byte*)*_output, _outSize);
		}

		void ZLib::Decompress(char* _input, int _inSize, char** _output, int& _outSize) {
			this->_unzipper->PutMessageEnd((CryptoPP::byte*)_input, _inSize);

			_outSize = this->_unzipper->MaxRetrievable();
			*_output = new char[_outSize] { 0 };
			this->_unzipper->Get((CryptoPP::byte*)*_output, _outSize);
		}

	};

}

#endif