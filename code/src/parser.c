#include "../lib/parser.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>

/* TODO */
/* assumes that the first char in "src" is a '(' */
static struct List* ParseInternal(char* src, size_t* const iter) {

	struct List* list_head = NULL;
	Bool reading = True;
	char atom_name_buffer[64] = { 0 };
	size_t atom_name_buffer_flag = 0;

	memset(atom_name_buffer, '\0', sizeof(char) * 64);

	list_head = malloc(sizeof(struct List));
	list_head->member_count = 0;
	list_head->member_capacity = 10;
	list_head->members = malloc(sizeof(struct MemberData) * list_head->member_capacity);

	*iter += 1;
	for (; src[*iter] != '\0'; (*iter)++) {
		if (src[*iter] == '#') {
			reading = !reading;
		} else if (src[*iter] == '\n') {
			reading = True;
		} else if (reading) {
			if (src[*iter] != '(' && src[*iter] != ')') {
				if (!isspace(src[*iter])) { /* you're reading an atom */
					atom_name_buffer[atom_name_buffer_flag++] = src[*iter];	
				} else {
					if (atom_name_buffer_flag > 0) {
						atom_name_buffer[atom_name_buffer_flag + 1] = '\0';
						AddAtom(list_head, atom_name_buffer);
						memset(atom_name_buffer, '\0', sizeof(char) * 64);
						atom_name_buffer_flag = 0;
					}
				}
			} else if (src[*iter] == '(') {
				struct List* child_list = NULL;

				if (atom_name_buffer_flag > 0) {
					atom_name_buffer[atom_name_buffer_flag + 1] = '\0';
					AddAtom(list_head, atom_name_buffer);
					memset(atom_name_buffer, '\0', sizeof(char) * 64);
					atom_name_buffer_flag = 0;
				}

				child_list = ParseInternal(src, iter);
				AddList(list_head, child_list);
			} else if (src[*iter] == ')') {
				if (atom_name_buffer_flag > 0) {
					atom_name_buffer[atom_name_buffer_flag + 1] = '\0';
					AddAtom(list_head, atom_name_buffer);
					memset(atom_name_buffer, '\0', sizeof(char) * 64);
					atom_name_buffer_flag = 0;
				}
				/* this is the end of the list */
				/* return what you've built so far */
				return list_head;
			}
		}
	}

	return list_head;
}

void AddAtom(struct List* list, char* name) {
	if (list != NULL) {
		if (list->member_count + 1 > list->member_capacity) {
			list->member_capacity *= 2;
			list->members = realloc(list->members, sizeof(struct MemberData) * list->member_capacity);
		}
		list->members[list->member_count].type = MEMBERTYPE_ATOM;
		list->members[list->member_count].generic.atom = malloc(sizeof(struct Atom));
		list->members[list->member_count].generic.atom->name = malloc(sizeof(char) * strlen(name));
		strcpy(list->members[list->member_count].generic.atom->name, name);
		list->member_count++;
	}
	/* please put error checking here later :) */
}

void AddList(struct List* parent_list, struct List* child_list) {
	if (parent_list != NULL && child_list != NULL) {
		if (parent_list->member_count + 1 > parent_list->member_capacity) {
			parent_list->member_capacity *= 2;
			parent_list->members = realloc(parent_list->members, sizeof(struct MemberData) * parent_list->member_capacity);
		}
		parent_list->members[parent_list->member_count].type = MEMBERTYPE_LIST;
		parent_list->members[parent_list->member_count].generic.list = child_list;
		parent_list->member_count++;
	}
}

Bool Validate(char* src) {
	int paren_count = 0;
	Bool reading = True;
	size_t i = 0;

	/* make sure first non-whitespace char is a list start */
	for (i = 0; src[i] != '\0'; i++) {
		if (src[i] == '#') {
			reading = !reading;
		} else if (src[i] == '\n') {
			reading = True;
		} else if (reading) {
			if (!isspace(src[i])) {
				if (src[i] != '(') {
					return False;
				} else {
					break;
				}
			}
		}
	}

	/* make sure there are an equal number of parentheses */
	reading = True;
	for (i = 0; src[i] != '\0'; i++) {
		if (src[i] == '#') {
			reading = !reading;
		} else if (src[i] == '\n') {
			reading = True;
		} else if (reading) {
			if (src[i] == '(') {
				paren_count++;
			} else if (src[i] == ')') {
				paren_count--;
			}
			if (paren_count < 0) {
				fprintf(stderr, "%s has too many close parentheses\n", src);
			}
		}
	}

	if (paren_count > 0) {
		fprintf(stderr, "%s has too many open parentheses\n", src);
	}

	return True;
}

/* TODO */
struct List* Parse(char* src) {
	size_t iter = 0;
	return ParseInternal(src, &iter);
}

