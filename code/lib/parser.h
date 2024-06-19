#ifndef MINC_PARSER
#define MINC_PARSER

#include <stdlib.h>
#include "util.h"

struct Atom {
	enum AtomType {
		ATOMTYPE_SYMBOL,
		ATOMTYPE_STRING_LITERAL,
		ATOMTYPE_INTEGER_LITERAL,
		ATOMTYPE_DECIMAL_LITERAL,
		ATOMTYPE_HEX_LITERAL,
		ATOMTYPE_CHARACTER_LITERAL,
		ATOMTYPE_BOOLEAN_LITERAL
	} type;
	char* identifier;
};

struct List {
	struct MemberData {
		enum MemberType {
			MEMBERTYPE_LIST,
			MEMBERTYPE_ATOM
		} type;
		union GenericMember {
			struct List* list;
			struct Atom* atom;
		} generic;
	}* members;
	size_t member_count;
	size_t member_capacity;
};

struct ListBuffer {
	struct List** lists;
	size_t list_count;
	size_t list_capacity;
};

#ifndef NDEBUG
extern void ShowAllAtoms(struct List* list);
#endif

extern char*              FileToCString    (char* filename);
extern void               AddAtom          (struct List* list, char* name);
extern void               AddList          (struct List* parent_list, struct List* child_list);
extern Bool               Validate         (char* src);
extern struct List*       Parse            (char* src);
extern void               FreeAtom         (struct Atom* atom);
extern void               FreeList         (struct List* list);
extern struct ListBuffer* CreateListBuffer (char* code);
extern void               FreeListBuffer   (struct ListBuffer* lg);

#endif

