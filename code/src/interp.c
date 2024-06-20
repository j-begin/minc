#include "../lib/interp.h"
#include <stdio.h>

enum MathOperator {
	MATH_OPERATOR_NIL = 0,
	MATH_OPERATOR_ADD,
	MATH_OPERATOR_SUB,
	MATH_OPERATOR_MUL,
	MATH_OPERATOR_DIV,
	MATH_OPERATOR_MOD
};

/* I wrote this on a 5am busride so I don't care if it is in LibC */
static long convertToIntegerLiteral(char* text) {
	long i = 0;
	long ret = 0;

	if (text == NULL) {
		return 0;
	}

	for (i = 0; text[i] != '\0'; i++) {
		ret = (ret * 10) + text[i] - '0';
	}

	return ret;
}

static enum MathOperator isMathOperator(char* text) {
	/* you can 100% make this more efficient */
	if (strcmp(text, "+") == 0) {
		return MATH_OPERATOR_ADD;
	} else if (strcmp(text, "-") == 0) {
		return MATH_OPERATOR_SUB;
	} else if (strcmp(text, "*") == 0) {
		return MATH_OPERATOR_MUL;
	} else if (strcmp(text, "/") == 0) {
		return MATH_OPERATOR_DIV;
	} else if (strcmp(text, "%") == 0) {
		return MATH_OPERATOR_MOD;
	} else {
		return MATH_OPERATOR_NIL;
	}
}

/* assume that member count is always above zero or very bad things will happen >:( */
struct MemberData InterpList(struct List* list) {
	struct MemberData member_data;
	struct List solved_form;
	long i = 0;

	puts("Interping");

	if (list == NULL) {
		fprintf(stderr, "A interp error has occured\n");
	} else {
		solved_form.member_count = 0;
		solved_form.member_capacity = list->member_capacity;
		solved_form.members = malloc(solved_form.member_capacity * sizeof(struct MemberData));	
		
		/* create solvable form */
		for (i = 0; i < list->member_count; i++) {

			/* THERE IS AN ISSUE HERE WITH THIS BLOCK OF CODE */
			if (list->members[i].type == MEMBERTYPE_LIST) {
				/* determine the member data of that list */
				member_data = InterpList(list->members[i].generic.list);
			} else if (list->members[i].type == MEMBERTYPE_ATOM) {
				member_data = list->members[i];
			}
			
			solved_form.members[solved_form.member_count].type = member_data.type;
			if (member_data.type == MEMBERTYPE_LIST) {
				solved_form.members[solved_form.member_count].generic.list = member_data.generic.list;
			} else if (member_data.type == MEMBERTYPE_ATOM) {
				solved_form.members[solved_form.member_count].generic.atom = member_data.generic.atom;
			}
			solved_form.member_count++;
		}

		if (solved_form.member_count == 0) {
			/* error stuff here */
		}

		/* solve that form */
		if (solved_form.member_count > 1) {
			{
				enum MathOperator op = MATH_OPERATOR_NIL;
				if ((op = isMathOperator(solved_form.members[0].generic.atom->identifier)) != MATH_OPERATOR_NIL) {
					long* arguments = NULL;
					long result = 0;
					size_t arg_flag = 0;

					if (op == MATH_OPERATOR_MUL) {
						result = 1;
					}

					arguments = malloc(sizeof(long) * (solved_form.member_count - 1));

					for (i = 1; i < solved_form.member_count; i++) {
						arguments[arg_flag++] = convertToIntegerLiteral(solved_form.members[i].generic.atom->identifier);
					}

					for (i = 0; i < solved_form.member_count - 1; i++) {
						switch (op) {
							case MATH_OPERATOR_ADD:
								result += arguments[i];
								break;
							case MATH_OPERATOR_SUB:
								if (i == 0) {
									result = arguments[i];
								} else {
									result -= arguments[i];
								}
								break;
							case MATH_OPERATOR_MUL:
								result *= arguments[i];
								break;
							case MATH_OPERATOR_DIV:
								if (i == 0) {
									result = arguments[i];
								} else {
									result /= arguments[i];
								}
								break;
						}
					}

					printf("=> %ld\n", result);

					free(arguments);
				}
			}
		}
	}

	return member_data;
}
