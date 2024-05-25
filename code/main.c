#include <stdio.h>

#include <string.h>
#include "lib/parser.h"

/* ADD STRING LITERAL ADDITION */

void show_all_atoms(struct List* list) {
	int i = 0, j = 0;
	static int indentation = 0;

	for (i = 0; i < list->member_count; i++) {
		if (list->members[i].type == MEMBERTYPE_ATOM) {
			for (j = 0; j < indentation; j++) {
				printf("  ");
			}
			printf("%d: %s\n", i, list->members[i].generic.atom->name);
		} else {
			indentation++;
			show_all_atoms(list->members[i].generic.list);
			indentation--;
		}
	}
}
int main(int argc, char* argv[]) {
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
			show_all_atoms(jit_code);

			/* execute here */
		} else {
			running = False;
		}
	}

	return 0;
}
