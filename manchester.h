
#ifndef MANCHESTER_H
#define MANCHESTER_H

#include <stdbool.h>
#include <stdint.h>
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
uint_fast8_t manchester_encode_nibble(uint_fast8_t nibble);
void manchester_encode_buf(uint8_t *buf, int len);

bool manchester_check_byte(uint_fast8_t in);
bool manchester_check_buf(uint8_t *buf, int len);

uint_fast8_t manchester_decode_byte(uint_fast8_t in);
void manchester_decode_buf(uint8_t *buf, int len);

//differential manchester
uint_fast8_t differential_manchester_encode_nibble(bool prev, uint_fast8_t nibble);
void differential_manchester_encode_buf(uint8_t *dest, bool prev, const uint8_t *buf, int len);

uint_fast8_t differential_manchester_decode_byte(bool prev, uint_fast8_t in);
void differential_manchester_decode_buf(bool prev, uint8_t *buf, int len);

//bmc
uint_fast8_t bmc_encode_nibble(bool prev, uint_fast8_t nibble);
void bmc_encode_buf(uint8_t *dest, bool prev, const uint8_t *buf, int len);

bool bmc_check_byte(bool prev, uint_fast8_t in);
bool bmc_check_buf(bool prev, uint8_t *buf, int len);

uint_fast8_t bmc_decode_byte(uint_fast8_t in);
void bmc_decode_buf(uint8_t *buf, int len);

uint_fast8_t find_transitions_in_byte(bool prev, uint_fast8_t in);

#endif
