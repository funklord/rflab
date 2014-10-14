//! Manchester encoder/decoder

//! @file manchester.c
//! @author Nabeel Sowan (nabeel.sowan@vibes.se)
//!
//! Encoding has lookup table support, with either 16 entries (16b) or 256 entries (512b)
//! Hardware enc/dec support on Si1024 (8051)
//! Also included are algorithms for differential manchester and Biphase Mark Code


#include "manchester.h"
#if defined(CONFIG_MANCHESTER_ENC_NIBBLE_LOOKUP) || defined(CONFIG_MANCHESTER_ENC_BYTE_LOOKUP)
#include "manchester_lookup.h"
#endif


#ifdef CONFIG_MANCHESTER
#ifdef CONFIG_MANCHESTER_ENC
#ifdef CONFIG_MANCHESTER_ENC_NIBBLE
//! manchester encode a nibble according to G.E. Thomas convention

//! invert output for IEEE802.3 convention
uint_fast8_t manchester_encode_nibble(uint_fast8_t nibble)
{
#ifdef CONFIG_MANCHESTER_ENC_NIBBLE_LOOKUP
	extern const uint8_t manchester_enc_nibble_lookup[16];
	return(manchester_enc_nibble_lookup[nibble]);
#else
	uint_fast8_t out=0, i;
	for(i=0;i<4;i++) {
		if(READ_BIT(nibble, i)) {
			SET_BIT(out, i << 1);       // 01
		} else {
			SET_BIT(out, (i << 1) + 1); // 10
		}
	}
	return(out);
#endif
}
#endif


#ifdef CONFIG_MANCHESTER_ENC_BYTE
//! manchester encode a byte according to G.E. Thomas convention

//! invert output for IEEE802.3 convention
uint_fast16_t manchester_encode_byte(uint_fast8_t byte)
{
#if defined(CONFIG_MANCHESTER_ENC_HW) && defined(__Si102x__)
	uint_fast16_t out=0;
	SFRPAGE_STORE();
	SFRPAGE_02();
	ENC0CN=0x00;
	ENC0L=in;
	ENC0CN=0x20;
	while(!(ENC0CN&0x80));
	out = ENC0L | (ENC0M<<8);
	SFRPAGE_RESTORE();
	return(out);
#else
#ifdef CONFIG_MANCHESTER_ENC_BYTE_LOOKUP
	extern const uint16_t manchester_enc_byte_lookup[256];
	return(manchester_enc_byte_lookup[byte]);
#elif defined(CONFIG_MANCHESTER_ENC_NIBBLE_LOOKUP)
	extern const uint8_t manchester_enc_nibble_lookup[16];
	return(manchester_enc_nibble_lookup[byte & 0x0f] | (manchester_enc_nibble_lookup[byte >> 4]<<8));
#else
	uint_fast16_t out=0;
	uint_fast8_t i;
	for(i=0;i<8;i++) {
		if(READ_BIT(byte, i)) {
			SET_BIT(out, i << 1);       // 01
		} else {
			SET_BIT(out, (i << 1) + 1); // 10
		}
	}
	return(out);
#endif
#endif
}
#endif


//! manchester encode an array according to G.E. Thomas convention

//! invert output for IEEE802.3 convention
//! the buffer is encoded in place, therefore the buffer needs to be len * 2
//! @param buf input/output data (needs to be len * 2)
//! @param len length of input data
//! @todo implement DMA based HW encoding on Si102x
void manchester_encode_buf(uint8_t *buf, int len)
{
	int i;
#if defined(CONFIG_MANCHESTER_ENC_BYTE)
	for(i=len-1;i>=0;i--) {
		uint_fast16_t tmp;
		tmp = manchester_encode_byte(buf[i]);
		buf[(i<<1)+1] = tmp>>8;
		buf[(i<<1)] = tmp & 0x00ff;
	}
#elif defined(CONFIG_MANCHESTER_ENC_NIBBLE)
	for(i=len-1;i>=0;i--) {
		buf[(i<<1)+1] = manchester_encode_nibble(buf[i]>>4);
		buf[(i<<1)] = manchester_encode_nibble(buf[i]);
	}
#else
#error need byte encoder
#endif
}
#endif //CONFIG_MANCHESTER_ENC


#ifdef CONFIG_MANCHESTER_DEC
#ifdef CONFIG_MANCHESTER_DEC_NIBBLE
//! manchester decode a byte to nibble according to G.E. Thomas convention

//! invert input for IEEE802.3 convention
//! @return nibble
int_fast8_t manchester_decode_nibble(uint_fast8_t in)
{
	uint_fast8_t out=0, i;
	for(i=0;i<4;i++) {
		if(!(READ_BIT(in, i<<1) ^ READ_BIT(in, (i<<1)+1))) //error detection
			return(-1);
		if(READ_BIT(in, i << 1))
			SET_BIT(out, i);
	}
	return(out);
}
#endif


#ifdef CONFIG_MANCHESTER_DEC_BYTE
//! manchester decode 16 bits to a byte according to G.E. Thomas convention

//! invert input for IEEE802.3 convention
//! @return byte or error = -1
int_fast16_t manchester_decode_byte(uint_fast16_t in)
{
#if defined(CONFIG_MANCHESTER_DEC_HW) && defined(__Si102x__)
	SFRPAGE_STORE();
	SFRPAGE_02();
	ENC0CN=0x00;
	ENC0L= in & 0x00ff;
	ENC0M= in >> 8;
	ENC0CN=0x10;
	while(!(ENC0CN&0x80));
	if(ENC0CN&0x40) { //check for error
		SFRPAGE_RESTORE();
		return(-1);
	}
	in = ENC0L;
	SFRPAGE_RESTORE();
	return(in);
#else
	uint_fast8_t out=0, i;
	for(i=0;i<8;i++) {
		if(!(READ_BIT(in, i<<1) ^ READ_BIT(in, (i<<1)+1))) //error detection
			return(-1);
		if(READ_BIT(in, i << 1))
			SET_BIT(out, i);
	}
	return(out);
#endif
}
#endif


//! manchester decode an array according to G.E. Thomas convention

//! invert input for IEEE802.3 convention
//! the buffer is decoded in place
//! @retval 0 the whole buffer decoded ok
//! @return retval>0 amount of bytes decoded before an error occurred
//! @todo implement DMA based HW decoding on Si102x
int manchester_decode_buf(uint8_t *buf, int len)
{
	int i;
#if defined(CONFIG_MANCHESTER_DEC_BYTE)
	for(i=0;i<len;i+=2) {
		int_fast16_t tmp = manchester_decode_byte(buf[i] | (buf[i+1]<<8));
		if(tmp == -1)
			return((i>>1)-1);
		buf[i>>1] = tmp;
	}
#elif defined(CONFIG_MANCHESTER_DEC_NIBBLE)
	for(i=0;i<len;i++) {
		if(i & 1)
			buf[i>>1] |= manchester_decode_nibble(buf[i]) << 4;
		else
			buf[i>>1] = manchester_decode_nibble(buf[i]);
	}
#else
#error need byte decoder
#endif
	return(0);
}
#endif //CONFIG_MANCHESTER_ENC
#endif //CONFIG_MANCHESTER


#if defined(CONFIG_MANCHESTER_ERROR_DETECTOR) && (defined(CONFIG_MANCHESTER) || defined(CONFIG_DIFF_MANCHESTER))
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
#endif //CONFIG_MANCHESTER_ERROR_DETECTOR


#ifdef CONFIG_DIFF_MANCHESTER
#ifdef CONFIG_DIFF_MANCHESTER_ENC
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
#endif //CONFIG_DIFF_MANCHESTER_ENC


#ifdef CONFIG_DIFF_MANCHESTER_DEC
//! decode a differential manchester sequence where transition=0

//! @param prev last bit of previous sequence
uint_fast8_t differential_manchester_decode_nibble(bool prev, uint_fast8_t in)
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
#endif //CONFIG_DIFF_MANCHESTER_DEC
#endif //CONFIG_DIFF_MANCHESTER


#ifdef CONFIG_BMC
#ifdef CONFIG_BMC_ENC
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
#endif //CONFIG_BMC_ENC


#ifdef CONFIG_BMC_ERROR_DETECTOR
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
#endif //CONFIG_BMC_ERROR_DETECTOR


#ifdef CONFIG_BMC_DEC
//! decode a bmc sequence byte into nibble
uint_fast8_t bmc_decode_nibble(uint_fast8_t in)
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
#endif //CONFIG_BMC_DEC
#endif //CONFIG_BMC


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
