#include "wsmessage.h"


// IN: a 32-bit masking key, memory to mask, length of memory segment to mask
// PRE: big endian architecture, valid memory length
// POST: masked memory
// OUT: None
void wsmessage__mask_flip(const uint32_t masking_key, unsigned char * const payload,  const K1MAXUINT_T len)
{
	// mask payload data of specified length as per RFC 6455 5.3 Client to Server Masking
	union key_union_t {
		uint32_t main 	; // left to right value
		uint8_t sub[4] 	; // 0-right, 4-left
	} ukey 			;

	ukey.main = masking_key ;
	int j 			;

	for (K1MAXUINT_T i = 0; i < len; ++i)
	{
		j = i%4 			;  // server reads bytes from right to left
		payload[i] ^= ukey.sub[j]	;
	}
}
