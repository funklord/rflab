#include "helper.h"
#include <stdio.h>

char *int_to_binary_string(int num, uint_fast8_t len)
{
	static char str[33];
	int i;
	for(i=0;i<len;i++)
		str[i] = READ_BIT(num, len-i-1) ? '1' : '0';
	str[len]='\0';
	return(str);
}

char *int_to_binary_string_l2r(int num, uint_fast8_t len)
{
	static char str[33];
	int i;
	for(i=0;i<len;i++)
		str[i] = READ_BIT(num, len-i-1) ? '1' : '0';
	str[len]='\0';
	return(str);
}

char *int_to_binary_level_string_l2r(int num, uint_fast8_t len)
{
	static char str[33];
	int i;
	for(i=0;i<len;i++)
		str[i] = READ_BIT(num, len-i-1) ? '-' : '_';
	str[len]='\0';
	return(str);
}

void print_levels_of_binary_code(uint8_t *buf, int len)
{
	int i;
	for(i=0;i<len;i++)
		printf("%-8d", i);
	printf("\n");
	for(i=0;i<len;i++)
		printf("%s", int_to_binary_level_string_l2r(buf[i], 8));
	printf("\n");
	for(i=0;i<len;i++)
		printf("%s", int_to_binary_string_l2r(buf[i], 8));
	printf("\n");
}
