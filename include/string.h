#ifndef STRING_H
#define STRING_H

/* ============================================================================
 * INCLUDES
 * ========================================================================== */

#include <stdbool.h>
#include <stdlib.h>

/* ============================================================================
 * CONSTANTS
 * ========================================================================== */

#define DEFAULT_CAPACITY 16

/* ============================================================================
  VALID STRING INVARIANT:
  s != NULL
  ptr != NULL
  cap >= len + 1
  len >= 0
  ptr[len] == '\0'

  INVALID STATE:
  does not exist (freed memory is not a “state”)
 * ==========================================================================
 */

/* ============================================================================
 * ERROR CODES
 * ==========================================================================
 */

typedef enum {
  SUCCESS = 0,
  ERR_ALLOC_FAILED,
  ERR_NULL_ARGUMENT,
  ERR_STRING_INIT_FAILED,
  ERR_GENERAL,
} ErrorCode;

typedef struct {
  char *ptr;
  size_t len; // actual bytes
  size_t cap; // overall size of the memory-block
} String;

typedef struct {
  void *ptr;
  size_t len;
} Data;

/* ============================================================================
 * HELPER FUNCTIONS
 * ========================================================================== */

static inline size_t min_size(size_t a, size_t b) { return a < b ? a : b; }

size_t my_strlen(const char *string);

// return the amount of bytes copied
static size_t copy_bytes(char *restrict dest, const void *restrict src,
                         size_t count);

/* ============================================================================
 * STRING INITIALIZATION
 *
 * string[i] == *(str + 1)
 *
 * the lib:
 * 1. measures length
 * 2. allocates NEW memory
 * 3. copies bytes
 * 4. owns the new allocation
 *
 * this creating the deep-copy
 * the original string remains owned by caller
 * new string owns its own separate memory
 *
 * String owns ptr
 * caller must eventually destroy/free String
 *
 * TEST_CASES:
 * 1. "" as input
 * 2. a very big string
 * 3. NULL-input pointer
 * ========================================================================== */

ErrorCode string_init(String *s_out, const char *s_in);

/* ============================================================================
 * STRING DESTRUCTION
 *
 * destruction life-cycle:
 * create
 * use
 * destroy
 *
 * ownership termination + state invalidation
 * INVARIANTS SHOULD BE KEEPEN
 *
 * Before destroy:
 * valid String invariants hold.
 *
 * After destroy:
 * invalid/sentinel invariants should hold.
 *
 * ownership completion
 * freeing semantics
 * invalidation after destroy
 *
 * Suppose:
 *
 * String a = ...
 * String b = a;
 *
 * Now:
 * both structs contain SAME pointer.
 * fix???
 *
 * TEST_CASES:
 * 1. multiple destroys
 * ========================================================================== */

ErrorCode string_destroy(String *s);

/* ============================================================================
 * STRING MUTATION
 *
 * realloc
 * capacity growth strategies
 * preserving invariants during mutation
 * temporary invalid states
 * pointer relocation
 * amortized complexity
 *
 * STATE-MACHINE DESIGN
 *
 * Strict lifecycle
 * invalid -> init -> valid -> destroy -> invalid
 *
 * vs.
 *
 * Recoverable lifecycle
 * invalid -> append -> valid
 *
 * More flexible state machine
 * enables destroy + append reuse
 * ========================================================================== */

ErrorCode string_append(String *s, const char *slice);

/* ============================================================================
 * CAPACITY MANAGEMENT
 *
 * separation of len vs capacity
 * allocation policy
 * ========================================================================== */

ErrorCode string_reserve(String *s, size_t add_cap);

/* ============================================================================
 * COPYING
 *
 * deep copy vs shallow copy
 * need to create a deep copy of a string with a new
 * ownership and document the problem with char *b = a;
 * ========================================================================== */

ErrorCode string_clone(String *s_dest, String const *s_src);

#endif /* STRING_H */
