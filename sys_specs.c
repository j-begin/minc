#include <stdio.h>
#include <stdlib.h>

static const char* const STATIC_TEXT = 
	"#ifndef MINC_SYS_SPECS_GUARD\n"
	"#define MINC_SYS_SPECS_GUARD\n"
	"typedef float f32;\n";

int main(void) {
	FILE* sys_specs_h = NULL;
	const long C_CHAR_SIZE = sizeof(char) * 8;
	const long C_SHORT_SIZE = sizeof(short) * 8;
	const long C_INT_SIZE = sizeof(int) * 8;
	const long C_LONG_SIZE = sizeof(long) * 8;
	const long C_FLOAT_SIZE = sizeof(float) * 8;
	const long C_DOUBLE_SIZE = sizeof(double) * 8;
	int i;

	if (C_LONG_SIZE != 32 && C_LONG_SIZE != 64) {
		fprintf(stderr, "Minc only supports 32 or 64 bit systems, and this C compiler doesn't support either.\n");
		return 1;
	}

	sys_specs_h = fopen("sys_specs.h", "w");
	if (sys_specs_h == NULL) {
		fprintf(stderr, "Failed to create \"sys_specs.h.\"\n");
		return 1;
	}

	fputs(STATIC_TEXT, sys_specs_h);

	fprintf(sys_specs_h, "#define MINC_ARCH_%ld\n", C_LONG_SIZE);
	if (C_DOUBLE_SIZE == 64) {
		fputs("typedef double f64;\n", sys_specs_h);
	}

	for (i = 8; i <= C_LONG_SIZE; i *= 2) {
		const char* const TYPES[] = {
			"char",
			"short",
			"int",
			"long"
		};
		int type;
		if (C_CHAR_SIZE >= i) {
			type = 0;
		} else if (C_SHORT_SIZE >= i) {
			type = 1;
		} else if (C_INT_SIZE >= i) {
			type = 2;
		} else if (C_LONG_SIZE >= i) {
			type = 3;
		}
		fprintf(sys_specs_h, "typedef %-5s i%d;\ntypedef unsigned %-5s u%d;\n", TYPES[type], i, TYPES[type], i);
	}

	fputs("typedef u8 bool;\n#define true ((bool)1)\n#define false ((bool)0)\n", sys_specs_h);
	
	fprintf(sys_specs_h, "#endif\n\n");
	fclose(sys_specs_h);

	return 0;
}
