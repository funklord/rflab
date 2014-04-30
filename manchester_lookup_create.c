#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "manchester.h"
#if defined(CONFIG_MANCHESTER_ENC_NIBBLE_LOOKUP) || defined(CONFIG_MANCHESTER_ENC_BYTE_LOOKUP)
#error need SW routines to generate lookup table
#endif

int main(void)
{
	int i;
	FILE *fh, *fc;
	fh = fopen("manchester_lookup.h", "w");
	fc = fopen("manchester_lookup.c", "w");
	fprintf(fh, "#include <stdint.h>\n#include \"config.h\"\n\n");
	fprintf(fc, "#include \"manchester_lookup.h\"\n\n");

	fprintf(fh, "#ifdef CONFIG_MANCHESTER_ENC_NIBBLE_LOOKUP\nconst uint8_t manchester_enc_nibble_lookup[16];\n#endif\n\n");
	fprintf(fc, "#ifdef CONFIG_MANCHESTER_ENC_NIBBLE_LOOKUP\nconst uint8_t manchester_enc_nibble_lookup[16] = {\n");
	for(i=0;i<16;i++) {
		fprintf(fc, "0x%02X,\n", (uint8_t)manchester_encode_nibble(i));
	}
	fprintf(fc, "};\n#endif\n");

	fprintf(fh, "#ifdef CONFIG_MANCHESTER_ENC_BYTE_LOOKUP\nconst uint16_t manchester_enc_byte_lookup[256];\n#endif\n\n");
	fprintf(fc, "#ifdef CONFIG_MANCHESTER_ENC_BYTE_LOOKUP\nconst uint16_t manchester_enc_byte_lookup[256] = {\n");
	for(i=0;i<256;i++) {
		fprintf(fc, "0x%04X,\n", (unsigned int)manchester_encode_byte(i));
	}
	fprintf(fc, "};\n#endif\n");

	fclose(fh);
	fclose(fc);
	return(0);
}
