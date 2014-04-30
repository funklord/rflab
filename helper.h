#ifndef HELPER_H
#define HELPER_H

#include <stdint.h>
#include <stdbool.h>

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

#define LOW_NIBBLE(w) ((uint8_t) ((w) & 0x0f))
#define HIGH_NIBBLE(w) ((uint8_t) ((w) >> 4))
#define LOW_BYTE(w) ((uint8_t) ((w) & 0xff))
#define HIGH_BYTE(w) ((uint8_t) ((w) >> 8))

#define READ_BYTE(p, n) (((p) >> (8 * (n))) & 0xff)

#define BIT(n) (1u << (n))
//in place
#define BIT_SET(p,n) ((p) & (BIT(n)))
#define BIT_CLEAR(p,n) (!BITSET((p), (n)))
//query
#define BITS_SET(p,b) (((p) & (b)) == (b))
#define BITS_CLEAR(p,b) (((p) & (b)) == 0)
//accessing variables
#define SET_BITS(p,b) ((p) |= (b))
#define CLR_BITS(p,b) ((p) &= (~(b)))
#define SET_BIT(p,n) (SET_BITS(p, (BIT(n))))
#define CLR_BIT(p,n) (CLEARBITS((p), (BIT((n)))))
#define FLIP_BIT(p,m) ((p) ^= (m))
#define GET_BIT(p,n) ((p) & (BIT(n)))
#define READ_BIT(p,n) (((p) >> (n)) & 0x01)
#define WRITE_BIT(p,n,value) ((value)?SET_BIT((p),(n)):CLR_BIT((p),(n)))

#define LOOP_UNTIL_SET(p) while(!(p));
#define LOOP_UNTIL_CLR(p) while(p);
#define LOOP_UNTIL_BIT_SET(p, n) while(!((p) & BIT(n)));
#define LOOP_UNTIL_BIT_CLR(p, n) while((p) & BIT(n));


#if defined(__Si102x__)

#define SFRPAGE_LEGACY() SFRPAGE=LEGACY_PAGE
#define SFRPAGE_02() SFRPAGE=0x02
#ifdef CONFIG_SFRPAGE_RESTORE
#define CONFIG_SFRPAGE_TMPVAR sfrpage_tmp
#define SFRPAGE_STORE() uint8_t CONFIG_SFRPAGE_TMPVAR=SFRPAGE
#define SFRPAGE_RESTORE() SFRPAGE=CONFIG_SFRPAGE_TMPVAR;
#elif CONFIG_SFRPAGE_RETURN_TO_LEGACY
#define SFRPAGE_STORE() (0)
#define SFRPAGE_RESTORE() SFRPAGE_LEGACY()
#else
#define SFRPAGE_STORE() (0)
#define SFRPAGE_RESTORE() (0)
#endif

#endif


char *int_to_binary_string(int num, uint_fast8_t len);
char *int_to_binary_string_l2r(int num, uint_fast8_t len);
char *int_to_binary_level_string_l2r(int num, uint_fast8_t len);
void print_levels_of_binary_code(uint8_t *buf, int len);

#endif
