#include <stdio.h>
#include <string.h>
#include "lib/parser.h"

int main(int argc, char* argv[]) {

	if (argc <= 1) {
		Bool running = True;

		puts("REPL STARTED");

		while (running) {
			struct ListBuffer* forms;
			char code[8192];
			size_t code_flag = 0;
			int i;

			memset(code, 0, sizeof(char) * 8192);
			printf("* ");
			scanf(" %8191[^\n]", code);
			if (code[0] != '\0') {
				int j = 0;

				forms = CreateListBuffer(code);

				#ifndef NDEBUG
				for (j = 0; j < forms->list_count; j++) {
					ShowAllAtoms(forms->lists[j]);
					puts("");
				}
				#endif

				FreeListBuffer(forms);
			} else {
				running = False;
			}
		}
	} else {
		struct ListBuffer* forms = NULL;

		{
			char* file_text = NULL;
			file_text = FileToCString(argv[1]);
			forms = CreateListBuffer(file_text);
			free(file_text);
		}

	}

	return 0;
}
