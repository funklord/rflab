#include <stdint.h>
#include "config.h"

#ifdef CONFIG_MANCHESTER_ENC_NIBBLE_LOOKUP
const uint8_t manchester_enc_nibble_lookup[16];
#endif

#ifdef CONFIG_MANCHESTER_ENC_BYTE_LOOKUP
const uint16_t manchester_enc_byte_lookup[256];
#endif

