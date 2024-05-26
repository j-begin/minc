#include "../lib/parser.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>

static Bool IsIntegerLiteral(char* text) {
	if (text == NULL) {
		return False;
	} else {
		int i;
		for (i = 0; text[i] != '\0'; i++) {
			if (!isdigit(text[i])) {
				return False;
			}
		}
		return True;
	}
}

static Bool IsDecimalLiteral(char* text) {
	if (text == NULL) {
		return False;
	} else {
		int i;
		Bool found_decimal = False;

		if (text[0] == '.') {
			return False;
		} else if (text[strlen(text)-1] == '.') {
			return False;
		}

		for (i = 0; text[i] != '\0'; i++) {
			if (!isdigit(text[i]) && text[i] != '.') {
				return False;
			} else {
				if (text[i] == '.') {
					if (found_decimal) {
						return False;
					}
					found_decimal = True;
				}
			}
		}
		return True;
	}
}

static Bool IsHexLiteral(char* text) {
	if (text == NULL) {
		return False;
	}

	if (strlen(text) > 10 || strlen(text) <= 2) {
		return False;
	}

	if (text[0] == '0' && text[1] == 'x') {
		int i;
		for (i = 2; text[i] != '\0'; i++) {
			if (!(isdigit(text[i]) || ('a' <= text[i] && text[i] <= 'f' || 'A' <= text[i] && text[i] <= 'F'))) {
				return False;
			}
		}
		return True;
	} else {
		return False;
	}
}

static Bool IsCharacterLiteral(char* text) {
	if (text == NULL) {
		return False;
	}

	if (strlen(text) == 2) {
		if (text[0] == '@' && !isspace(text[1])) {
			return True;
		} else {
			return False;
		}
	} else if (strlen(text) > 2) {
		char* escape_codes[] = {
			"@space",
			"@newline",
			"@tab",
			"@bell",
			"@backspace",
			"@newpage",
			"@return",
			"@vert"
		};
		int i;
		for (i = 0; i < 8; i++) {
			if (strcmp(text, escape_codes[i]) == 0) {
				return True;
			}
		}
		return False;
	} else {
		return False;
	}
}

static Bool IsBooleanLiteral(char* text) {
	if (text == NULL) {
		return False;
	}

	if (strcmp(text, "false") == 0 || strcmp(text, "true") == 0) {
		return True;
	} else {
		return False;
	}
}

static Bool IsSymbolLiteral(char* text) {
	int i;

	if (text == NULL) {
		return False;
	}

	for (i = 0; text[i] != '\0'; i++) {
		if (i > 0) {
			if (text[i] == '"') {
				return False;
			} else if (text[i] == '\'') {
				return False;
			} else if (text[i] == '`') {
				return False;
			}
		}
		if (text[i] == '.') {
			return False;
		}
	}

	return True;
}

/* assumes that the first char in "src" is a '(' */
static struct List* ParseInternal(char* src, size_t* const iter) {
	#define ATOM_NAME_BUFFER_SIZE 2048

	struct List* list_head = NULL;
	Bool reading = True;
	char atom_name_buffer[ATOM_NAME_BUFFER_SIZE] = { 0 };
	size_t atom_name_buffer_flag = 0;

	memset(atom_name_buffer, '\0', sizeof(char) * ATOM_NAME_BUFFER_SIZE);

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
					if (src[*iter] == '"') {
						(*iter)++;
						atom_name_buffer[atom_name_buffer_flag++] = '"';
						for (; src[*iter] != '"'; (*iter)++) {
							atom_name_buffer[atom_name_buffer_flag++] = src[*iter];
						}
						atom_name_buffer[atom_name_buffer_flag++] = '"';
					} else {
						atom_name_buffer[atom_name_buffer_flag++] = src[*iter];	
					}
				} else {
					if (atom_name_buffer_flag > 0) {
						atom_name_buffer[atom_name_buffer_flag + 1] = '\0';
						AddAtom(list_head, atom_name_buffer);
						memset(atom_name_buffer, '\0', sizeof(char) * ATOM_NAME_BUFFER_SIZE);
						atom_name_buffer_flag = 0;
					}
				}
			} else if (src[*iter] == '(') {
				struct List* child_list = NULL;

				if (atom_name_buffer_flag > 0) {
					atom_name_buffer[atom_name_buffer_flag + 1] = '\0';
					AddAtom(list_head, atom_name_buffer);
					memset(atom_name_buffer, '\0', sizeof(char) * ATOM_NAME_BUFFER_SIZE);
					atom_name_buffer_flag = 0;
				}

				child_list = ParseInternal(src, iter);
				AddList(list_head, child_list);
			} else if (src[*iter] == ')') {
				if (atom_name_buffer_flag > 0) {
					atom_name_buffer[atom_name_buffer_flag + 1] = '\0';
					AddAtom(list_head, atom_name_buffer);
					memset(atom_name_buffer, '\0', sizeof(char) * ATOM_NAME_BUFFER_SIZE);
					atom_name_buffer_flag = 0;
				}
				/* this is the end of the list */
				/* return what you've built so far */
				return list_head;
			}
		}
	}

	return list_head;
	#undef ATOM_NAME_BUFFER_SIZE
}

#ifndef NDEBUG
void ShowAllAtoms(struct List* list) {
	int i = 0, j = 0;
	static int indentation = 0;

	for (i = 0; i < list->member_count; i++) {
		if (list->members[i].type == MEMBERTYPE_ATOM) {
			for (j = 0; j < indentation; j++) {
				printf("  ");
			}
			printf("(%d) NAME: %s ", i, list->members[i].generic.atom->name);
			switch (list->members[i].generic.atom->type) {
				case ATOMTYPE_SYMBOL:
					printf("TYPE: SYMBOL");
					break;
				case ATOMTYPE_STRING_LITERAL:
					printf("TYPE: STRING");
					break;
				case ATOMTYPE_INTEGER_LITERAL:
					printf("TYPE: INTEGER");
					break;
				case ATOMTYPE_DECIMAL_LITERAL:
					printf("TYPE: DECIMAL");
					break;
				case ATOMTYPE_HEX_LITERAL:
					printf("TYPE: HEX");
					break;
				case ATOMTYPE_CHARACTER_LITERAL:
					printf("TYPE: CHARACTER");
					break;
				case ATOMTYPE_BOOLEAN_LITERAL:
					printf("TYPE: BOOLEAN");
					break;
				default:
					printf("TYPE: UNKNOWN");
					break;
			}
			puts("");
		} else {
			indentation++;
			ShowAllAtoms(list->members[i].generic.list);
			indentation--;
		}
	}
}
#endif

void AddAtom(struct List* list, char* name) {
	if (list != NULL) {
		/* allocate & add the name to the atom */
		if (list->member_count + 1 > list->member_capacity) {
			list->member_capacity *= 2;
			list->members = realloc(list->members, sizeof(struct MemberData) * list->member_capacity);
		}

		/* add data */
		list->members[list->member_count].type = MEMBERTYPE_ATOM;
		list->members[list->member_count].generic.atom = malloc(sizeof(struct Atom));
		list->members[list->member_count].generic.atom->name = malloc(sizeof(char) * strlen(name));
		strcpy(list->members[list->member_count].generic.atom->name, name);
		
		/* figure out the type of atom */
		if (IsIntegerLiteral(name)) {
			list->members[list->member_count].generic.atom->type = ATOMTYPE_INTEGER_LITERAL;
		} else if (IsDecimalLiteral(name)) {
			list->members[list->member_count].generic.atom->type = ATOMTYPE_DECIMAL_LITERAL;
		} else if (IsHexLiteral(name)) {
			list->members[list->member_count].generic.atom->type = ATOMTYPE_HEX_LITERAL;
		} else if (IsCharacterLiteral(name)) {
			list->members[list->member_count].generic.atom->type = ATOMTYPE_CHARACTER_LITERAL;
		} else if (IsBooleanLiteral(name)) {
			list->members[list->member_count].generic.atom->type = ATOMTYPE_BOOLEAN_LITERAL;
		} else if (IsSymbolLiteral(name)) {
			list->members[list->member_count].generic.atom->type = ATOMTYPE_SYMBOL;
		} else {
			printf("Could not determine type of: %s\n", name);
			exit(1);
		}
		
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

void FreeList(struct List* list) {
}

