#ifndef STRING_H
#define STRING_H

/*
  LIB INVARIANTES

  ptr != NULL
  len < cap
  ptr[len] == '\0'
  allocated_bytes >= cap
 */

#include <stdbool.h>
#include <stdlib.h>

#define DEFAULT_CAPACITY 16

typedef enum {
  SUCCESS = 0,
  ERR_NULL_PTR, /* maybe distinguish between null pointer, failed allocation et
                   al */
  ERR_EMPTY_STRING,
  ERR_EMPTY_SLICE,
  ERR_GENERAL,
} ErrorCode;

/*
 invalid String:
 - ptr == NULL
 - len == 0
 - cap == 0

  invalid/uninitialized
  ↓
  initialized/valid
  ↓
  destroyed/invalid

  Many systems abstractions are fundamentally: controlled state transitions.
*/

typedef struct {
  char *ptr;
  size_t len; // actual bytes
  size_t cap; // overall size of the memory-block
} String;

/*
  string[i] == *(str + 1)

  the lib:
  1. measures length
  2. allocates NEW memory
  3. copies bytes
  4. owns the new allocation

  this creating the deep-copy
  the original string remains owned by caller
  new string owns its own separate memory

  String owns ptr
  caller must eventually destroy/free String

  TEST_CASES:
  1. "" as input
  2. a very big string
  3. NULL-input pointer
*/
ErrorCode string_init(String *s_out, const char *s_in);

/*
  destruction life-cycle:
  create
  use
  destroy

  ownership termination + state invalidation
  INVARIANTS SHOULD BE KEEPEN

  Before destroy:
  valid String invariants hold.

  After destroy:
  invalid/sentinel invariants should hold.

  ownership completion
  freeing semantics
  invalidation after destroy

  Suppose:

  String a = ...
  String b = a;

  Now:
  both structs contain SAME pointer.
  fix???

  TEST_CASES:
  1. multiple destroys
*/
ErrorCode string_destroy(String *s);

/*
  realloc
  capacity growth strategies
  preserving invariants during mutation
  temporary invalid states
  pointer relocation
  amortized complexity

  STATE-MACHINE DESIGN

  Strict lifecycle
  invalid -> init -> valid -> destroy -> invalid

  vs.

  Recoverable lifecycle
  invalid -> append -> valid

  More flexible state machine
  enables destroy + append reuse

*/
ErrorCode string_append(String *s, char *slice);

// separation of len vs capacity
// allocation policy
ErrorCode string_reserve();

// deep copy vs shallow copy
ErrorCode string_clone();

#endif /* STRING_H */
