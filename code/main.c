#include <stdio.h>
#include <string.h>
#include "lib/parser.h"
#include "lib/interp.h"

int main(int argc, char* argv[]) {

	Bool running = True;

	puts("REPL STARTED");

	while (running) {
		struct ListBuffer* list_buffer;
		char code[8192];
		size_t code_flag = 0;
		int i;

		memset(code, 0, sizeof(char) * 8192);
		printf("* ");
		scanf(" %8191[^\n]", code);
		if (code[0] != '\0') {
			int j = 0;

			list_buffer = CreateListBuffer(code);

			#ifndef NDEBUG
			for (j = 0; j < list_buffer->list_count; j++) {
				ShowAllAtoms(list_buffer->lists[j]);
			}
			#endif

			for (j = 0; j < list_buffer->list_count; j++) {
				InterpList(list_buffer->lists[j]);
			}

			FreeListBuffer(list_buffer);
		} else {
			running = False;
		}
	}

	return 0;
}
