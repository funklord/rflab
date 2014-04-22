#include "bytecoder.h"
#include "helper.h"
#include "manchester.h"

#ifdef CONFIG_BYTECODER_BIGLEN
void bc_encode_len(uint8_t *buf, bc_len_t len, bc_len_t encoded_len, bc_len_t offset, uint_fast8_t bytes, bool big_endian)
{
	uint_fast8_t i;
	for(i=0;i<bytes;i++) {
		if(big_endian)
			buf[offset+i] = READ_BYTE(encoded_len, bytes-i-1);
		else
			buf[offset+i] = READ_BYTE(encoded_len, i);
	}
}

bc_len_t bc_decode_len(uint8_t *buf, bc_len_t len, bc_len_t offset, uint_fast8_t bytes, bool big_endian)
{
	bytecodec_len_t out=0;
	uint_fast8_t i;
	for(i=0;i<bytes;i++) {
		if(big_endian)
			out |= buf[offset+i] << (8*(i-1));
		else
			out |= buf[offset+i] << (8*i);
	}
	return(out);
}
#else
void bc_encode_len(uint8_t *buf, bc_len_t len, bc_len_t encoded_len, bc_len_t offset)
{
	buf[offset] = encoded_len;
}


bc_len_t bc_decode_len(uint8_t *buf, bc_len_t len, bc_len_t offset)
{
	return(buf[offset]);
}
#endif


void lshift_bytes(uint8_t *buf, bc_len_t len, bc_len_t offset)
{
	bc_len_t i;
	for(i=0;i<len-offset;i++)
		buf[i] = buf[i+offset];
}


void lshift_bits(uint8_t *buf, bc_len_t len, uint_fast8_t offset)
{
	bc_len_t i;
	for(i=0;i<len;i++) {
		buf[i] >> offset;
		if(i<(len-1))
			buf[i] |= (buf[i] & (pow(2,offset)-1)) << (8-offset);
	}
}


//void codec_chain_add(bytecodec_chain_t *codec_chain, bytecodec_id_t,

int bc_encode(bytecodec_t *codec, uint8_t *buf, bc_len_t len)
{
	switch(codec_chain->codec[i]->id) {
	case BYTECODEC_ABORT:
		return(0);
		case FIXED_LENGTH:
			return(codec->opt[0]);
			break;
	case ENCODED_LENGTH:
#ifdef CONFIG_BYTECODER_BIGLEN
		bc_encode_len(buf, len, current_len, codec_chain->codec[i]->opt[0], codec_chain->codec[i]->opt[1], codec_chain->codec[i]->opt[2]);
#else
		bc_encode_len(buf, len, current_len, codec_chain->codec[i]->opt[0]);
#endif
		break;
	case BYTECODEC_MANCHESTER_GE_THOMAS:
		break;
	}
}


int bc_encode_chain(bytecodec_chain_t *codec_chain, uint8_t *buf, bc_len_t len)
{
	bc_len_t current_len=len;
	uint_fast8_t i;
	for(i=0;i<codec_chain->amount;i++)
		bc_encode(codec_chain->codec[i], buf, len);
	return(0);
}

int bytedecoder(bytecodec_chain_t codec_chain, uint8_t *buf, bc_len_t len)
{
	uint_fast8_t i;
	for(i=0;i<codec_chain->amount;i++) {

	}
	return(0);
}