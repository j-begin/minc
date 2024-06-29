#include <stdio.h>
#include <string.h>

#include "sys_specs.h"

#define SSTR(X) #X
#define STR(X) SSTR(X)
#define REPL_INPUT_LENGTH 5000

i32 main(i32 argc, char* argv[]) {
	char input[REPL_INPUT_LENGTH];

	puts("Starting Minc REPL");

	while (1) {
		memset(input, 0, sizeof(char) * REPL_INPUT_LENGTH);
		printf("* ");
		scanf(" %"STR(REPL_INPUT_LENGTH)"[^\n]", input);
		if (input[0] == '\0') {
			break;
		}
		printf("=> %s\n", input);
	}

	puts("Shutting Down Minc REPL");

	return 0;
}
