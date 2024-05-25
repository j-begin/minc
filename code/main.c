#include <stdio.h>

#include "lib/parser.h"

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

	return 0;
}
