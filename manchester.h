//! Manchester encoder/decoder

//! @file manchester.h
//! @author Nabeel Sowan (nabeel.sowan@vibes.se)
//!
//! Encoding has lookup table support, with either 16 entries (16b) or 256 entries (512b)
//! Hardware enc/dec support on Si1024 (8051)
//! Also included are algorithms for differential manchester and Biphase Mark Code

#ifndef MANCHESTER_H
#define MANCHESTER_H

#include <stdbool.h>
#include <stdint.h>
#include "config.h"
#include "helper.h"


//bit macros

//! encode a manchester sequence according to G.E. Thomas convention

//! invert output for IEEE802.3 convention
#define manchester_encode_bit(b) ((b)?1:2))


//! encode a differential manchester sequence where transition=0

//! @param prev last bit of previous sequence
#define differential_manchester_encode_bit(prev, b) ((prev)?((b)?1:2):((b)?2:1))


//! check for valid manchester sequence

//! this should be done before manchester decoding
//! @retval 1 valid
//! @retval 0 invalid
#define manchester_check_valid_sequence(b) (READ_BIT((b),0) ^ READ_BIT((b),1))


//! decode a manchester sequence according to G.E. Thomas convention

//! invert output for IEEE802.3 convention
#define manchester_decode_bit(b) (READ_BIT((b),0))


//! decode a differential manchester sequence where transition=0

//! @param prev last bit of previous manchester sequence
//! @param b first bit of current manchester sequence
#define differential_manchester_decode_bit(prev, b) (prev ^ b)

#define bmc_encode_bit(prev, b) ((prev)?((b)?2:0):((b)?1:3))
#define bmc_check_bit(prev, b) (prev ^ READ_BIT((b), 0))
#define bmc_decode_bit(b) (READ_BIT((b),0) ^ READ_BIT((b),1))

//manchester
#ifdef CONFIG_MANCHESTER
#ifdef CONFIG_MANCHESTER_ENC
#ifdef CONFIG_MANCHESTER_ENC_NIBBLE
uint_fast8_t manchester_encode_nibble(uint_fast8_t nibble);
#endif
#ifdef CONFIG_MANCHESTER_ENC_BYTE
uint_fast16_t manchester_encode_byte(uint_fast8_t byte);
#endif
void manchester_encode_buf(uint8_t *buf, int len);
#endif
#ifdef CONFIG_MANCHESTER_DEC
#ifdef CONFIG_MANCHESTER_DEC_NIBBLE
int_fast8_t manchester_decode_nibble(uint_fast8_t in);
#endif
#ifdef CONFIG_MANCHESTER_DEC_BYTE
int_fast16_t manchester_decode_byte(uint_fast16_t in);
#endif
int manchester_decode_buf(uint8_t *buf, int len);
#endif
#endif

#if defined(CONFIG_MANCHESTER_ERROR_DETECTOR) && (defined(CONFIG_MANCHESTER) || defined(CONFIG_DIFF_MANCHESTER))
bool manchester_check_byte(uint_fast8_t in);
bool manchester_check_buf(uint8_t *buf, int len);
#endif

//differential manchester
#ifdef CONFIG_DIFF_MANCHESTER
#ifdef CONFIG_DIFF_MANCHESTER_ENC
uint_fast8_t differential_manchester_encode_nibble(bool prev, uint_fast8_t nibble);
void differential_manchester_encode_buf(uint8_t *dest, bool prev, const uint8_t *buf, int len);
#endif
#ifdef CONFIG_DIFF_MANCHESTER_DEC
uint_fast8_t differential_manchester_decode_nibble(bool prev, uint_fast8_t in);
void differential_manchester_decode_buf(bool prev, uint8_t *buf, int len);
#endif
#endif

//bmc
#ifdef CONFIG_BMC
#ifdef CONFIG_BMC_ENC
uint_fast8_t bmc_encode_nibble(bool prev, uint_fast8_t nibble);
void bmc_encode_buf(uint8_t *dest, bool prev, const uint8_t *buf, int len);
#endif
#ifdef CONFIG_BMC_ERROR_DETECTOR
bool bmc_check_byte(bool prev, uint_fast8_t in);
bool bmc_check_buf(bool prev, uint8_t *buf, int len);
#endif
#ifdef CONFIG_BMC_DEC
uint_fast8_t bmc_decode_nibble(uint_fast8_t in);
void bmc_decode_buf(uint8_t *buf, int len);
#endif
#endif

uint_fast8_t find_transitions_in_byte(bool prev, uint_fast8_t in);

#endif
