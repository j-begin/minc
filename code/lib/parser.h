#ifndef MINC_PARSER
#define MINC_PARSER

#include <stdlib.h>
#include "util.h"

struct Atom {
	char* name;
};

enum MemberType {
	MEMBERTYPE_LIST,
	MEMBERTYPE_ATOM
};

struct List {
	struct MemberData {
		enum MemberType type;
		union GenericMember {
			struct List* list;
			struct Atom* atom;
		} generic;
	}* members;
	size_t member_count;
	size_t member_capacity;
};

extern void         AddAtom  (struct List* list, char* name);
extern void         AddList  (struct List* parent_list, struct List* child_list);
extern Bool         Validate (char* src);
extern struct List* Parse    (char* src);

#endif

