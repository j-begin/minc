#include <stdio.h>
#include <string.h>
#include "lib/parser.h"

int main(int argc, char* argv[]) {

	if (argc <= 1) {
		Bool running = True;

		puts("REPL STARTED");

		while (running) {
			struct List* jit_code = NULL;
			char code[8192];
			size_t code_flag = 0;
			int i;

			memset(code, 0, sizeof(char) * 8192);
			printf("* ");
			scanf(" %8191[^\n]", code);
			if (code[0] != '\0') {
				if (Validate(code)) {
					jit_code = Parse(code);
				}
				#ifndef NDEBUG
				ShowAllAtoms(jit_code);
				#endif

				/* execute here */

				FreeList(jit_code);
			} else {
				running = False;
			}
		}
	} else {
		struct ListGroup* forms = NULL;

		{
			char* file_text = NULL;
			file_text = FileToCString(argv[1]);
			forms = CreateListGroup(file_text);
			free(file_text);
		}

	}

	return 0;
}
