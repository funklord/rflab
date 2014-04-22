#include "manchester.h"


//! manchester encode an array according to G.E. Thomas convention

//! invert output for IEEE802.3 convention
uint_fast8_t manchester_encode_nibble(uint_fast8_t nibble)
{
	uint_fast8_t out=0, i;
	for(i=0;i<4;i++) {
		if(READ_BIT(nibble, i)) {
			SET_BIT(out, i << 1);       // 01
		} else {
			SET_BIT(out, (i << 1) + 1); // 10
		}
	}
	return(out);
}


//! manchester encode an array according to G.E. Thomas convention
//! invert output for IEEE802.3 convention
//! the buffer is encoded in place, therefore the buffer needs to be len * 2

//! @param buf input/output data (needs to be len * 2)
//! @param len length of input data
void manchester_encode_buf(uint8_t *buf, int len)
{
	int i;
	for(i=len-1;i>=0;i--) {
		buf[(i<<1)+1] = manchester_encode_nibble(buf[i]>>4);
		buf[(i<<1)] = manchester_encode_nibble(buf[i]);
	}
}


//! check if byte contains valid manchester code

//! should be done before decoding
//! use for all types of manchester code (not BMC)
//! @retval 1=valid
bool manchester_check_byte(uint_fast8_t in)
{
	uint_fast8_t i;
	for(i=0;i<8;i+=2) {
		if(!(READ_BIT(in, i) ^ READ_BIT(in, i+1)))
			return(0);
	}
	return(1);
}


//! check if array contains valid manchester code

//! should be done before decoding
//! use for all types of manchester code (not BMC)
//! @retval 1=valid
bool manchester_check_buf(uint8_t *buf, int len)
{
	int i;
	for(i=0;i<len;i++) {
		if(!manchester_check_byte(buf[i]))
			return(0);
	}
	return(1);
}


//! manchester decode a byte to nibble according to G.E. Thomas convention

//! invert output for IEEE802.3 convention
//! @return nibble
uint_fast8_t manchester_decode_byte(uint_fast8_t in)
{
	uint_fast8_t out=0, i;
	for(i=0;i<4;i++) {
		if(READ_BIT(in, i << 1))
			SET_BIT(out, i);
	}
	return(out);
}


//! manchester decode an array according to G.E. Thomas convention

//! invert output for IEEE802.3 convention
//! the buffer is decoded in place
void manchester_decode_buf(uint8_t *buf, int len)
{
	int i;
	for(i=0;i<len;i++) {
		if(i & 1)
			buf[i>>1] |= manchester_decode_byte(buf[i]) << 4;
		else
			buf[i>>1] = manchester_decode_byte(buf[i]);
	}
}


//! encode a differential manchester sequence where transition=0

//! @param prev last bit of previous sequence
uint_fast8_t differential_manchester_encode_nibble(bool prev, uint_fast8_t nibble)
{
	uint_fast8_t out=0, i;
	for(i=0;i<8;i+=2) {
		if(READ_BIT(nibble, i>>1))// {
			prev = !prev; //transition on 0
		if(prev)
			SET_BIT(out, i + 1); // 10
		else
			SET_BIT(out, i);     // 01
	}
	return(out);
}


//! encode a differential manchester sequence where transition=0

//! Tip: if buf = dest + buf_length you can reuse the same buffer
//! @param dest destination buffer (needs to be len * 2)
//! @param prev last bit of previous sequence
//! @param buf input data
//! @param len length of buf
void differential_manchester_encode_buf(uint8_t *dest, bool prev, const uint8_t *buf, int len)
{
	int i;
	for(i=0;i<len;i++) {
		dest[i<<1] = differential_manchester_encode_nibble(prev, buf[i]);
		dest[(i<<1)+1] = differential_manchester_encode_nibble(dest[i<<1] >> 7, HIGH_NIBBLE(buf[i]));
		prev = dest[(i<<1)+1] >> 7;
	}
}


//! decode a differential manchester sequence where transition=0

//! @param prev last bit of previous sequence
uint_fast8_t differential_manchester_decode_byte(bool prev, uint_fast8_t in)
{
	uint_fast8_t out=0, i;
	for(i=0;i<4;i++) {
		if(!(prev ^ READ_BIT(in, i << 1))) {
			SET_BIT(out, i);
			prev = !prev;
		}
	}
	return(out);
}


//! decode a differential manchester sequence where transition=0

//! the buffer is decoded in place
//! @param prev last bit of previous sequence
//! @param buf input data
//! @param len length of buf
void differential_manchester_decode_buf(bool prev, uint8_t *buf, int len)
{
	int i;
	for(i=0;i<len;i++) {
		bool tmp = buf[i] >> 7;
		if(i & 1)
			buf[i>>1] |= differential_manchester_decode_byte(prev, buf[i]) << 4;
		else
			buf[i>>1] = differential_manchester_decode_byte(prev, buf[i]);
		prev = tmp;
	}
}


//! encode bmc sequence nibble

//! @param prev last bit of previous sequence
uint_fast8_t bmc_encode_nibble(bool prev, uint_fast8_t nibble)
{
	uint_fast8_t out=0, i;
	for(i=0;i<8;i+=2) {
		if(READ_BIT(nibble, i>>1)) {
			if(prev)
				SET_BIT(out, i + 1); // 10
			else
				SET_BIT(out, i);       // 01
		} else {
			if(!prev) {
				SET_BIT(out, i);
				SET_BIT(out, i + 1); // 11
			}
			prev = !prev;
		}
	}
	return(out);
}


//! encode bmc sequence array

//! Tip: if buf = dest + buf_length you can reuse the same buffer
//! @param dest destination buffer (needs to be len * 2)
//! @param prev last bit of previous sequence
//! @param buf input data
//! @param len length of buf
void bmc_encode_buf(uint8_t *dest, bool prev, const uint8_t *buf, int len)
{
	int i;
	for(i=0;i<len;i++) {
		dest[i<<1] = bmc_encode_nibble(prev, buf[i]);
		dest[(i<<1)+1] = bmc_encode_nibble(dest[i<<1] >> 7, buf[i]>>4);
		prev = dest[(i<<1)+1] >> 7;
	}
}


//! check if byte contains valid bmc

//! should be done before decoding
//! @param prev last bit of previous sequence
//! @retval 1=valid
bool bmc_check_byte(bool prev, uint_fast8_t in)
{
	uint_fast8_t i;
	for(i=0;i<8;i+=2) {
		if(!(prev ^ READ_BIT(in, i)))
			return(0);
		prev = READ_BIT(in, i+1);
	}
	return(1);
}


//! check if buffer contains valid bmc

//! should be done before decoding
//! @param prev last bit of previous sequence
//! @retval 1=valid
bool bmc_check_buf(bool prev, uint8_t *buf, int len)
{
	int i;
	for(i=0;i<len;i++) {
		if(!bmc_check_byte(prev, buf[i]))
			return(0);
		prev = buf[i] >> 7;
	}
	return(1);
}


//! decode a bmc sequence byte into nibble
uint_fast8_t bmc_decode_byte(uint_fast8_t in)
{
	uint_fast8_t out=0, i;
	for(i=0;i<8;i+=2) {
		if((READ_BIT(in, i) ^ READ_BIT(in, i+1)))
			SET_BIT(out, i>>1);
	}
	return(out);
}


//! decode a bmc sequence buffer

//! the buffer is decoded in place
//! @param buf input data
//! @param len length of input data
void bmc_decode_buf(uint8_t *buf, int len)
{
	int i;
	for(i=0;i<len;i++) {
		if(i & 1)
			buf[i>>1] |= bmc_decode_byte(buf[i]) << 4;
		else
			buf[i>>1] = bmc_decode_byte(buf[i]);
	}
}


//! decode a sequence of bits into transitions where transition=1

//! @param prev last bit of previous sequence
uint_fast8_t find_transitions_in_byte(bool prev, uint_fast8_t in)
{
	uint_fast8_t out=0, i;
	for(i=0;i<8;i++) {
		if(prev != READ_BIT(in, i)) {
			SET_BIT(out, i);
			prev = !prev;
		}
	}
	return(out);
}
