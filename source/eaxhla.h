#ifndef EAXHLA_H

#include <tommyds/tommyhashtbl.h>

#define WORD_SIZE_IN_BYTES (64/8)

typedef struct {
  union {
      long   value;
      void * array_value;
  };
  int        elements;
  char *     name;
  unsigned   _hash;
  tommy_node _node;
} variable_t;

#define REGISTER64_MASK 0x00;
#define REGISTER32_MASK 0x01;

typedef struct {
    int number : 6;
    int size   : 2;
} cpuregister_t;

extern int system_type;

extern int is_program_found;
extern int has_encountered_error;

extern char * scope;

extern int eaxhla_init(void);
extern int eaxhla_destroy(void);

extern char * make_scoped_name(const char * const scope, char * name);
extern int can_fit(int type, long long value);

extern void add_variable(variable_t variable);
extern variable_t * get_variable(const char * const name);

extern void append_instruction_t1 (int t1);
extern void append_instruction_t4 (int t4, int w, int d, int r);
extern void append_instruction_t6 (int t6, int w, int d, int r, int s, int i);

/*
-- THIS WILL BE REMOVED, HELPER COMMENT FOR ME...

--------------------------------------------------------------------------------

fast procedure XXX
	in u32 AAA
	in u32 BBB
begin
	{ IMPL }
end procedure

----------------------------------------

XXX:
	-- POSTPONE THOSE ARGUMENTS, IMPORTANT...
	-- BEGIN
	{ IMPL }
	-- END
	ret
...
XXX_AAA: dd 0 -- NOW WE DEFINE THOSE ARGUMENTS
XXX_BBB: dd 0 -- NOW WE DEFINE THOSE ARGUMENTS

----------------------------------------

ASMDIRREL [?] -- IDENTIFIER FOR XXX
{ IMPL }      -- USAGE OF OTHER 'append_<>' FUNCTIONS...
RETN
...
ASMDIRREL [?] ASMDIRIMM D32 0 -- IDENTIFIER FOR XXX_AAA
ASMDIRREL [?] ASMDIRIMM D32 0 -- IDENTIFIER FOR XXX_BBB

--------------------------------------------------------------------------------

fastcall XXX A B

----------------------------------------

	mov [XXX_AAA], A
	mov [XXX_BBB], B
	call XXX
	-- mov [XXX_AAA], rax      -- OKAY
	-- mov [XXX_AAA], 24       -- OKAY
	-- mov [XXX_AAA], MY_REL   -- OKAY
	-- mov [XXX_AAA], [MY_MEM] -- NOT OKAY
	-- FIX
	-- mov rax, [MY_MEM]
	-- mov [XXX_AAA], rax -- NOW OKAY
	-- NOW FAST PROCEDURE XXX DOES THE WORK...

----------------------------------------

	MOV D?? MEM [XXX_AAA] REG/MEM/IMM/REL [?]
	MOV D?? MEM [XXX_BBB] REG/MEM/IMM/REL [?]
	CALL D32 REL XXX

--------------------------------------------------------------------------------
*/

extern void append_fastcall           (void); // TODO
extern void append_fastcall_arguments (void); // TODO

extern void issue_warning(const char * format, ...);
extern void issue_error(const char * format, ...);

#define EAXHLA_H
#endif
