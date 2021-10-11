//<INCLUDES>
#include <stdint.h>
#include <stdio.h> //review
#include "wsmessage.h"

//</INCLUDES>

//<MACROS>

#define ERRCAST(_iErr_) (*(parseframe_err *)&_iErr_)
//</MACROS>

//<HELPERFUNCTIONS>
/*
static int d_parseframe_testbit(uint8_t word, int bit_to_test)
{
	word >>= bit_to_test;
	word &= 1;
	return word;
}

static void d_parseframe_printbits(uint8_t word)
{
//	int i, testbit(int word, int bit_to_test);
	int i;
	for (i = 7; i >=0; i--) printf("%1d", d_parseframe_testbit(word, i));
	printf("\n");
}
*/
//</HELPERFUNCTIONS>

//<PUBLICFUNCTIONS>




// input					process									output
//
// beg						[!close frame]								payload
// end						parse length								frame_end
//		 				compute range
//
int wsmessage__parseframe
		( uint8_t *beg		, uint8_t *end
		, uint8_t **payload	, uint8_t **frame_end
		, uint32_t * maskingkey
		)
	// mark off the end of a websocket message if seen
{

	int iErr = 0;
	typedef union t_u_type {
		uint8_t b[4]				; // bytes 3 and 4 unused assuming 4 byte int (struct)
		struct temp_t {
			unsigned OPCODE: 	4	;
			unsigned RSV: 		3	;
			unsigned FIN: 		1	;

			unsigned paylen: 	7	;
			unsigned MASK: 		1	;
		} 	bf				;
	} top_u						;

	if (end - beg < 1) {
		frame_end = NULL;
		return iErr;
	}
	top_u *t = (top_u *)beg;

    if (t->bf.OPCODE == 0x9) {
        ERRCAST(iErr).ping=1;
    }

    if (t->bf.OPCODE == 0x8) {
		frame_end = NULL;
		ERRCAST(iErr).closed_connection_frame = 1;
		return iErr;
	}

	uint8_t pl1 = t->bf.paylen;
	if (pl1 < 126) {
		*payload = &beg[2];
		if (t->bf.MASK==1) { (*payload)+=4; }
		*frame_end = (*payload + pl1);
		if ( *frame_end <= end) {
			// OK
		} else {
			*frame_end=NULL;
			return iErr;
		}
	} else if (pl1 == 126) {
		if (end - beg >4) { // [value readable]
			uint16_t pl2;
			// encode in NBO
			*((uint8_t *)&pl2+0) = *(uint8_t*)&beg[3];
			*((uint8_t *)&pl2+1) = *(uint8_t*)&beg[2];
			*payload = &beg[4]; // 2 + 2
			if (t->bf.MASK==1) { (*payload)+=4; }

			*frame_end = (*payload + pl2);
			if ( *frame_end <= end) {
				// OK
			} else { 
				// computed frame_end exceeds capacity
				ERRCAST(iErr).payload_over_buffer = 1;
				*frame_end = NULL;
			}
		} else { // [!value readable]
			*frame_end=NULL;	   
		}
	} else {
		// TODO *******************untested!
		K1MAXUINT_T pl3;
		// encode in NBO
		*((uint8_t *)&pl3+0) = *(uint8_t*)&beg[9];
		*((uint8_t *)&pl3+1) = *(uint8_t*)&beg[8];
		*((uint8_t *)&pl3+2) = *(uint8_t*)&beg[7];
		*((uint8_t *)&pl3+3) = *(uint8_t*)&beg[6];
		*((uint8_t *)&pl3+4) = *(uint8_t*)&beg[5];
		*((uint8_t *)&pl3+5) = *(uint8_t*)&beg[4];
		*((uint8_t *)&pl3+6) = *(uint8_t*)&beg[3];
		*((uint8_t *)&pl3+7) = *(uint8_t*)&beg[2];
		*payload = &beg[10]; // 2+ 8
		if (t->bf.MASK==1) { (*payload)+=4; }

		*frame_end = (*payload + pl3);
		if ( *frame_end > end) {
			ERRCAST(iErr).payload_over_buffer = 1;
			*frame_end = NULL;
		}
		//		*frame_end = 0;
		ERRCAST(iErr).payload_size_not_implemented = 1; // ERROR BECAUSE THIS HAS NOT BEEN TESTED
		return iErr;
	}

	// read masking key relative from payload if appropriate
	// save for version 3
	if (*frame_end && t->bf.MASK==1)  {

		//printf("\n[[[%s:%s]]] DEBUG MASK SET\n", __func__, __FILE__);
		uint8_t *p_payload = *payload;
		uint32_t *p32_payload = (uint32_t *)p_payload;
        	*maskingkey = p32_payload[-1];
        	// printf("[[[%s:%s]]]:read masking key %u (%8x) from header\n", __func__, __FILE__, *maskingkey, *maskingkey);

		wsmessage__mask_flip(*maskingkey, *payload, (K1MAXUINT_T )(*frame_end - *payload));
    }
	return iErr;
}

//</PUBLICFUNCTIONS>



