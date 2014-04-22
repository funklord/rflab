#ifndef BYTECODER_H
#define BYTECODER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef CONFIG_BYTECODER_BIGLEN
typedef bc_len_t int;
#else
typedef bc_len_t uint_fast8_t;
#endif

typedef enum {
	BYTECODEC_ABORT,
	BYTECODEC_RISING_EDGE,  //!< bit feature
	BYTECODEC_FALLING_EDGE, //!< bit feature
	BYTECODEC_START_BIT,    //!< bit feature
	BYTECODEC_SYNC_WORD,
	BYTECODEC_PREAMBLE,
	BYTECODEC_LTRIM, //!< needed if fixed length?
	BYTECODEC_FIXED_LENGTH,
	BYTECODEC_ENCODED_LENGTH,
	BYTECODEC_MANCHESTER_GE_THOMAS,
	BYTECODEC_MANCHESTER_IEEE802_3,
	BYTECODEC_DIFFERENTIAL_MANCHESTER_T0,
	BYTECODEC_DIFFERENTIAL_MANCHESTER_T1,
	BYTECODEC_BMC,
	BYTECODEC_CRC8,
	BYTECODEC_CRC16
} bytecodec_id_t;


typedef struct {
	bytecodec_id_t id;
	bc_len_t opt[3];
} bytecodec_t;


typedef struct {
	bytecodec_t codec[];
	int amount;
	bool enc_dest_buf;    //!< calculate if secondary buffer is needed, or if work can be done in place
	bool dec_dest_buf;    //!< calculate if secondary buffer is needed, or if work can be done in place
	bc_len_t enc_buf_len; //!< calculate the total buffer length required for encoding
	bc_len_t dec_buf_len; //!< calculate the total buffer length required for decoding
} bytecodec_chain_t;

#endif