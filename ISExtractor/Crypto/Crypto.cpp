#include "Crypto.h"

/*
keyFrag = 0x8F4FE388F17917083794F3E98905D40A;
keyFrag2 = 0xA7668D26811AA835FAD9BA6F1251636;

keyFrag[0] = 0x8F4FE388;
keyFrag[1] = 0xF1791708;
keyFrag[2] = 0x3794F3E9;
keyFrag[3] = 0x8905D40A;
keyFrag2[0] = 0xA7668D26;
keyFrag2[1] = 0x811AA835;
keyFrag2[2] = 0xFAD9BA6F;
keyFrag2[3] = 0x1251636;

*/
uint32_t Crypto::_defKey[4]	= { 0x8F4FE388, 0xF1791708, 0x3794F3E9, 0x8905D40A };
uint32_t Crypto::_defKey2[4]	= { 0xA7668D26, 0x811AA835, 0xFAD9BA6F, 0x1251636 };
