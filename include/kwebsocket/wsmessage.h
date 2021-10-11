#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#ifdef _WIN32
#endif

#ifndef K1MAXUINT_T
#define K1MAXUINT_T 	uint32_t
#define K1MAXINT_T	int32_t
#endif

typedef struct {
	uint8_t		 	d[14]				; // maximum length assuming largest possible payload length
	void			*payload_len_end	;
	uint32_t	 	*mk					;
	uint8_t			*msg				;
	K1MAXUINT_T 	msg_len				;
} WSFRAMEHEADER						; // non-contiguousness resolved with payload and msg pointers


typedef struct parseframe_err_t {
        unsigned payload_over_buffer: 1;
    unsigned ping: 1;
        unsigned closed_connection_frame: 1;
        unsigned nothing_to_parse: 1;
        unsigned payload_size_not_implemented: 1;
} parseframe_err;


int
	wsmessage__parseframe			(uint8_t * beg, uint8_t * end, uint8_t * *payload, uint8_t * *frame_end, uint32_t * maskingkey);

void
    wsmessage__frame_msg			(WSFRAMEHEADER *f, char const *msg, const K1MAXINT_T len, uint32_t * const maskingkey);

void
	wsmessage__mask_flip			(const uint32_t masking_key, unsigned char* const payload, const K1MAXUINT_T len);


#ifdef __cplusplus
}
#endif
