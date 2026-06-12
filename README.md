# sstr — simple C string library

Single-header library providing a dynamic string type with bounds-checked operations.

## Usage

Copy `sstr.h` into your project. In exactly **one** source file:

```c
#define SSTR_IMPLEMENTATION
#include "sstr.h"
```

Everywhere else, just `#include "sstr.h"`.

## API

```c
// Lifecycle
Err sstr_init(String *s, const char *cstr);
Err sstr_destroy(String *s);

// Inspection
size_t sstr_len(const String *s);
size_t sstr_cap(const String *s);
bool   sstr_empty(const String *s);
int    sstr_cmp(const String *s1, const String *s2);
size_t sstr_find(const String *str, const char *substr);
char  *sstr_cstr(const String *s, char *buf, size_t buf_size);

// Mutation
Err sstr_clear(String *s);
Err sstr_append(String *s, const char *slice);
Err sstr_append_n(String *s, const char *slice, size_t len);
Err sstr_insert(String *s, size_t index, const char *slice);
Err sstr_copy(String *dest, const String *src);
Err sstr_reserve(String *s, size_t count);
Err sstr_shrink(String *s);
Err sstr_trim(String *s);
Err sstr_ltrim(String *s);
Err sstr_rtrim(String *s);
Err sstr_tolower(String *s);
Err sstr_toupper(String *s);
Err sstr_at(const String *s, size_t index, char *out);
```

All mutating functions return `ERR_INVALID_STATE` if the string is in the zero/uninitialized state (`ptr == NULL`).

## Build

```sh
make        # build
make clean  # remove build artifacts
make asan   # build with Address Sanitizer
```

## License

MIT
