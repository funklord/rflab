#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "manchester.h"
#include "helper.h"


int test_manchester_code(uint8_t *in, int len)
{
	int e=0;
	uint8_t *tmp;
	if(!(tmp = malloc(len * 2))) {
		printf("Error: malloc failed\n");
	}
	memcpy(tmp, in, len);
	manchester_encode_buf(tmp, len);
	if(manchester_check_buf(tmp, len<<1)) {
		manchester_decode_buf(tmp, len<<1);
		e = memcmp(in, tmp, len);
		printf("decoding manchester %s\n", e ? "failed" : "ok");
		if(e)
			return(-2);
	} else {
		printf("invalid manchester data\n");
		return(-1);
	}
	return(0);
}


int test_differential_manchester_code(bool prev, uint8_t *in, int len)
{
	int e=0;
	uint8_t *tmp;
	if(!(tmp = malloc(len * 2))) {
		printf("Error: malloc failed\n");
	}
	differential_manchester_encode_buf(tmp, prev, in, len);
	if(manchester_check_buf(tmp, len<<1)) {
		differential_manchester_decode_buf(prev, tmp, len<<1);
		e = memcmp(in, tmp, len);
		printf("decoding differential manchester %s\n", e ? "failed" : "ok");
		if(e)
			return(-2);
	} else {
		printf("invalid manchester data\n");
		return(-1);
	}
	return(0);
}


int test_bmc_code(bool prev, uint8_t *in, int len)
{
	int e=0;
	uint8_t *tmp;
	if(!(tmp = malloc(len * 2))) {
		printf("Error: malloc failed\n");
	}
	bmc_encode_buf(tmp, prev, in, len);
	if(bmc_check_buf(prev, tmp, len<<1)) {
		bmc_decode_buf(tmp, len<<1);
		e = memcmp(in, tmp, len);
		printf("decoding bmc %s\n", e ? "failed" : "ok");
		if(e)
			return(-2);
	} else {
		printf("invalid bmc data\n");
		return(-1);
	}
	return(0);
}


int main(void)
{
	int i;
	//test byte encoding/decoding
	for(i=0;i<4;i++) {
		int j, r, r2, byte=0;
		for(j=0;j<4;j++) {
			if(GET_BIT(i, 0))
				SET_BIT(byte, j<<1);
			if(GET_BIT(i, 1))
				SET_BIT(byte, (j<<1)+1);
		}
		printf("Testing %d:                                                %s\n", i, int_to_binary_string(byte,4));
		r = manchester_check_byte(byte);
		printf("manchester_check_byte: %s\n", r ? "valid" : "invalid manchester data");
		//if(r) {
			r = manchester_encode_nibble(byte&0x0f);
			r2 = manchester_decode_nibble(r);
			printf("manchester_encode_nibble:                %s", int_to_binary_string(r, 8));
			printf(" decode: %s %s\n", int_to_binary_string(r2,4), (r2 == (byte>>4)) ? "ok" : "fail");

			r = differential_manchester_encode_nibble(0,byte);
			r2 = differential_manchester_decode_byte(0,r);
			printf("differential_manchester_encode_nibble 0: %s", int_to_binary_string(r, 8));
			printf(" decode: %s %s\n", int_to_binary_string(r2,4), (r2 == (byte>>4)) ? "ok" : "fail");
			r = differential_manchester_encode_nibble(1,byte);
			r2 = differential_manchester_decode_byte(1,r);
			printf("differential_manchester_encode_nibble 1: %s", int_to_binary_string(r,8));
			printf(" decode: %s %s\n", int_to_binary_string(r2,4), (r2 == (byte>>4)) ? "ok" : "fail");

			r2 = find_transitions_in_byte(1,r);
			printf("find transitions: %s\n", int_to_binary_string(r2,8));

			r = bmc_encode_nibble(0,byte);
			r2 = bmc_decode_byte(r);
			printf("bmc_encode_nibble 0:                     %s", int_to_binary_string(r,8));
			printf(" decode: %s %s\n", int_to_binary_string(r2,4), (r2 == (byte>>4)) ? "ok" : "fail");
			r = bmc_encode_nibble(1,byte);
			r2 = bmc_decode_byte(r);
			printf("bmc_encode_nibble 1:                     %s", int_to_binary_string(r,8));
			printf(" decode: %s %s\n", int_to_binary_string(r2,4), (r2 == (byte>>4)) ? "ok" : "fail");
		//}

	}
	//test random array encoding
#define TEST_ARRAY_LEN 32
	uint8_t test_array[TEST_ARRAY_LEN];
	printf("Testing random array\n");
	for(i=0;i<TEST_ARRAY_LEN;i++)
		test_array[i] = rand();
	print_levels_of_binary_code(test_array, TEST_ARRAY_LEN);
	test_manchester_code(test_array, TEST_ARRAY_LEN);
	test_differential_manchester_code(0, test_array, TEST_ARRAY_LEN);
	test_bmc_code(0, test_array, TEST_ARRAY_LEN);
	return(0);
}
