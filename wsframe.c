//wsframe.c
//author: krunch3r (https://github.com/krunch3r76/)
//license: GPL 3
// INTERNAL
#include "wsmessage.h"
#include <stddef.h> // NULL
#include <stdio.h> //debug


// inputs					process						output
//
//						set opcodes				WSFRAMHEADER
// msg	{unmasked}				set payload length			msg {masked}
// msglen					record msg len
// maskingkey {optional}			point to message
//											set masking key
//											mask msg
void wsmessage__frame_msg(WSFRAMEHEADER *f, char const *msg, const K1MAXINT_T len, uint32_t * const maskingkey)
{
	// populate a frame header object (wsmessage.h) for sending a websocket message bytewise
	union {
		// uint8_t b[2]				; // bytes 3 and 4 unused assuming 4 byte int (struct)
		struct temp_t {
			unsigned OPCODE: 	4	;
			unsigned RSV: 		3	;
			unsigned FIN: 		1	;

			unsigned paylen: 	7	;
			unsigned MASK: 		1	;
		} temp					;
	} t_u						;
	t_u.temp.FIN=1					;
	t_u.temp.RSV=0					;
	t_u.temp.OPCODE=1				;
	t_u.temp.MASK=
		( maskingkey == NULL ) ? 0 : 1	;


	f->msg_len = len						; // remember framed message length
	if (len <126) {
		t_u.temp.paylen=(unsigned int)len 			; // write payload length field value and compute end of payload length field
		f->payload_len_end = f->d + 2				; //	when len <126
	} else {
		if (len <= (0xffff)) { 
			t_u.temp.paylen=126 				; //	when len < 0xffff
			f->d[2] = *((uint8_t *)&len+1)			;
			f->d[3] = *((uint8_t *)&len+0)			;
			f->payload_len_end=&f->d[4] 			;
		} else { 
			t_u.temp.paylen=127 				; //	when len >= 0xffff
			int i, j					;
			// i 2..9 , j 7 .. 0
			for (i=2,j=7; j >=0; --j, ++i)
			{
				f->d[i] = *((uint8_t *)&len+j)		;
			}
			f->payload_len_end = &f->d[10]			;
		}
	}

	*(uint16_t*)f->d=*(uint16_t*)&t_u				; // write bits as initialized
	f->msg = (uint8_t*)msg						; // remember where the message begins in memory
	if(maskingkey) { // note this implied the mask bit has been set

		*(uint32_t *)f->payload_len_end=*maskingkey	; // write masking key unto header
		f->mk = (uint32_t*)f->payload_len_end		; // there is no point to storing the masking key here? TODO review
        	// printf("[%s:%s]:wrote masking key %u(%8x) to header\n", __func__, __FILE__, *maskingkey, *maskingkey);

    	wsmessage__mask_flip(*f->mk, f->msg, f->msg_len)	; // flip message
    	// printf("%s:%s:reversed %u to %u\n", __func__, __FILE__, original.data, target.data);
    } // else {
      //  f->mk = NULL; // ensure this is NULL
      // }
}







