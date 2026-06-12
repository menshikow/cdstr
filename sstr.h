// Copyright 2026 Andrey Menshikov <andreydmenshikov@gmail.com@>

// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:

// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef SSTR_H
#define SSTR_H

#define DEFAULT_CAPACITY 16
#define SSTR_NPOS ((size_t)-1)

/*
 * VALID STRING:
 *
 *   s != NULL
 *
 *   Either:
 *     ptr == NULL
 *     len == 0
 *     cap == 0
 *
 *   Or:
 *     ptr != NULL
 *     cap >= len + 1
 *     ptr[len] == '\0'
 *
 * Functions that modify the buffer (clear, insert, trim, etc.)
 * return ERR_INVALID_STATE if the string is in the zero/unallocated state.
 */

#include <stdbool.h>
#include <stdlib.h>

typedef enum {
  SUCCESS = 0,
  ERR_INVALID_STATE,
  ERR_NULL_ARGUMENT,
  ERR_OUT_OF_MEMORY,
  ERR_OUT_OF_BOUNDS,
  ERR_STRING_INIT_FAILED,
} Err;

typedef struct {

  char *ptr;
  size_t len;
  size_t cap;
} String;

// Helper functions — static inline (always visible)
static inline size_t min_size(size_t a, size_t b) { return a < b ? a : b; }

static inline size_t sstr_strlen(const char *string_start) {
  if (string_start == NULL) {
    return 0;
  }

  const char *string_end = string_start;
  while (*string_end != '\0') {
    ++string_end;
  }

  return string_end - string_start;
}

static inline size_t sstr_memcpy(char *restrict to, const void *restrict from,
                                 size_t bytes_num) {
  const unsigned char *s = from;
  size_t copied = bytes_num;

  while (bytes_num--) {
    *to++ = *s++;
  }

  return copied;
}

// Public API declarations
Err sstr_init(String *s, const char *cstr);
Err sstr_destroy(String *s);
Err sstr_clear(String *s);

Err sstr_append(String *s, const char *slice);
Err sstr_append_n(String *s, const char *slice, size_t len);

Err sstr_insert(String *s, size_t index, const char *slice);

static inline size_t sstr_len(const String *s) {
  if (s == NULL) {
    return 0;
  }
  return s->len;
}

static inline size_t sstr_cap(const String *s) {
  if (s == NULL) {
    return 0;
  }
  return s->cap;
}

Err sstr_copy(String *s_dest, const String *s_src);
Err sstr_reserve(String *s, size_t count);

static inline bool sstr_empty(const String *s) {
  if (s == NULL) {
    return true;
  }
  return (s->len == 0);
}

Err sstr_shrink(String *s);

Err sstr_trim(String *s);
Err sstr_ltrim(String *s);
Err sstr_rtrim(String *s);

char *sstr_cstr(const String *s, char *buf, size_t buf_size);

Err sstr_tolower(String *s);
Err sstr_toupper(String *s);

// < 0 if a < b
// 0 if a == b
// > 0 if a > b
int sstr_cmp(const String *s1, const String *s2);

Err sstr_at(const String *s, size_t index,
            char *out); // bounds-checked index access

size_t sstr_find(const String *str, const char *substr);

// ── Implementation ────────────────────────────────────────────────────
#if defined(SSTR_IMPLEMENTATION) && !defined(SSTR_IMPLEMENTED)
#define SSTR_IMPLEMENTED

#include <ctype.h>
#include <string.h>

Err sstr_init(String *s_out, const char *s_in) {
  if (s_out == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  if (s_in == NULL) {
    s_in = "";
  }

  size_t len = sstr_strlen(s_in);
  size_t cap = len + 1 > DEFAULT_CAPACITY ? len + 1 : DEFAULT_CAPACITY;

  char *buf = (char *)malloc(cap);

  if (buf == NULL) {
    return ERR_OUT_OF_MEMORY;
  }

  sstr_memcpy(buf, s_in, len + 1);

  s_out->ptr = buf;
  s_out->len = len;
  s_out->cap = cap;

  return SUCCESS;
}

Err sstr_destroy(String *s) {
  if (s == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  free(s->ptr);

  *s = (String){0};

  return SUCCESS;
}

Err sstr_clear(String *s) {
  if (s == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  if (s->ptr == NULL) {
    return ERR_INVALID_STATE;
  }

  s->ptr[0] = '\0';
  s->len = 0;

  return SUCCESS;
}

Err sstr_append(String *s, const char *slice) {
  if (s == NULL || slice == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  if (s->ptr == NULL) {
    return ERR_INVALID_STATE;
  }

  size_t slice_len = sstr_strlen(slice);
  size_t new_len = s->len + slice_len;

  if (s->cap < new_len + 1) {
    size_t tmp_cap = new_len * 2;
    if (tmp_cap < new_len + 1) {
      tmp_cap = new_len + 1;
    }

    char *tmp_ptr = realloc(s->ptr, tmp_cap);
    if (tmp_ptr == NULL) {
      return ERR_OUT_OF_MEMORY;
    }

    s->ptr = tmp_ptr;
    s->cap = tmp_cap;
  }

  sstr_memcpy(s->ptr + s->len, slice, slice_len + 1);

  s->len = new_len;

  return SUCCESS;
}

Err sstr_append_n(String *s, const char *slice, size_t len) {
  if (s == NULL || slice == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  if (len == 0) {
    return SUCCESS;
  }

  if (s->ptr == NULL) {
    return ERR_INVALID_STATE;
  }

  size_t new_len = s->len + len;

  if (s->cap < new_len + 1) {
    size_t tmp_cap = s->cap * 2;

    if (tmp_cap < new_len + 1) {
      tmp_cap = new_len + 1;
    }

    char *tmp_ptr = realloc(s->ptr, tmp_cap);
    if (tmp_ptr == NULL) {
      return ERR_OUT_OF_MEMORY;
    }

    s->ptr = tmp_ptr;
    s->cap = tmp_cap;
  }

  sstr_memcpy(s->ptr + s->len, slice, len);

  s->len = new_len;
  s->ptr[s->len] = '\0';

  return SUCCESS;
}

Err sstr_insert(String *s, size_t index, const char *slice) {
  if (s == NULL || slice == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  if (s->ptr == NULL) {
    return ERR_INVALID_STATE;
  }

  if (index > s->len) {
    return ERR_OUT_OF_BOUNDS;
  }

  size_t slice_len = sstr_strlen(slice);
  if (slice_len == 0) {
    return SUCCESS;
  }

  size_t new_len = s->len + slice_len;

  if (new_len + 1 > s->cap) {
    size_t tmp_cap = (new_len + 1) * 2;

    char *tmp_ptr = realloc(s->ptr, tmp_cap);
    if (tmp_ptr == NULL) {
      return ERR_OUT_OF_MEMORY;
    }

    s->ptr = tmp_ptr;
    s->cap = tmp_cap;
  }

  if (index < s->len) {
    memmove(s->ptr + index + slice_len, s->ptr + index, s->len - index);
  }

  sstr_memcpy(s->ptr + index, slice, slice_len);

  s->len = new_len;
  s->ptr[s->len] = '\0';

  return SUCCESS;
}

Err sstr_copy(String *dest, String const *src) {
  if (src == NULL || dest == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  if (dest->ptr == NULL || src->ptr == NULL) {
    return ERR_INVALID_STATE;
  }

  if (dest->cap < src->len + 1) {
    size_t tmp_cap = src->len * 2;
    char *tmp_ptr = realloc(dest->ptr, tmp_cap);

    if (tmp_ptr == NULL) {
      return ERR_OUT_OF_MEMORY;
    }

    dest->ptr = tmp_ptr;
    dest->cap = tmp_cap;
    sstr_memcpy(dest->ptr, src->ptr, src->len + 1);
    dest->len = src->len;

    return SUCCESS;
  }

  sstr_memcpy(dest->ptr, src->ptr, src->len + 1);
  dest->len = src->len;

  return SUCCESS;
}

Err sstr_reserve(String *s, size_t count) {
  if (s == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  if (s->ptr == NULL) {
    return ERR_INVALID_STATE;
  }

  if (count == 0) {
    return SUCCESS;
  }

  size_t new_cap = s->cap + count;

  char *tmp_ptr = realloc(s->ptr, new_cap);
  if (tmp_ptr == NULL) {
    return ERR_OUT_OF_MEMORY;
  }

  s->ptr = tmp_ptr;
  s->cap = new_cap;

  return SUCCESS;
}

Err sstr_shrink(String *s) {
  if (s == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  if (s->ptr == NULL) {
    return ERR_INVALID_STATE;
  }

  if (s->cap == s->len + 1) {
    return SUCCESS;
  }

  size_t new_cap = s->len + 1;

  char *tmp_ptr = realloc(s->ptr, new_cap);
  if (tmp_ptr == NULL) {
    return ERR_OUT_OF_MEMORY;
  }

  s->ptr = tmp_ptr;
  s->cap = new_cap;

  return SUCCESS;
}

Err sstr_ltrim(String *s) {
  if (s == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  if (s->ptr == NULL) {
    return ERR_INVALID_STATE;
  }

  if (s->len == 0) {
    return SUCCESS;
  }

  size_t start = 0;
  for (; start < s->len; start++) {
    if (!isspace((unsigned char)s->ptr[start])) {
      break;
    }
  }

  memmove(s->ptr, s->ptr + start, s->len - start + 1);
  s->len -= start;

  return SUCCESS;
}

Err sstr_rtrim(String *s) {
  if (s == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  if (s->ptr == NULL) {
    return ERR_INVALID_STATE;
  }

  if (s->len == 0) {
    return SUCCESS;
  }

  // end isn't s->len, if s->len = 0, len - 1 would overflow size_t
  size_t end = s->len;
  while (end > 0 && isspace((unsigned char)s->ptr[end - 1])) {
    end--;
  }

  s->len = end;
  s->ptr[end] = '\0';

  return SUCCESS;
}

Err sstr_trim(String *s) {
  if (s == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  if (s->ptr == NULL) {
    return ERR_INVALID_STATE;
  }

  if (s->len == 0) {
    return SUCCESS;
  }

  size_t start = 0;
  for (; start < s->len; start++) {
    if (!isspace((unsigned char)s->ptr[start])) {
      break;
    }
  }

  size_t end = s->len;
  while (end > 0 && isspace((unsigned char)s->ptr[end - 1])) {
    end--;
  }

  size_t new_len = end - start;
  memmove(s->ptr, s->ptr + start, new_len);

  s->len = new_len;
  s->ptr[new_len] = '\0';

  return SUCCESS;
}

Err sstr_tolower(String *s) {
  if (s == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  if (s->ptr == NULL) {
    return ERR_INVALID_STATE;
  }

  if (s->len == 0) {
    return SUCCESS;
  }

  for (size_t i = 0; i < s->len; i++) {
    s->ptr[i] = (char)tolower((unsigned char)s->ptr[i]);
  }

  return SUCCESS;
}

Err sstr_toupper(String *s) {
  if (s == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  if (s->ptr == NULL) {
    return ERR_INVALID_STATE;
  }

  if (s->len == 0) {
    return SUCCESS;
  }

  for (size_t i = 0; i < s->len; i++) {
    s->ptr[i] = (char)toupper((unsigned char)s->ptr[i]);
  }

  return SUCCESS;
}

// caller provided buffer
char *sstr_cstr(const String *s, char *buf, size_t buf_size) {
  if (buf == NULL || buf_size == 0) {
    return NULL;
  }

  if (s == NULL || s->ptr == NULL) {
    buf[0] = '\0';
    return buf;
  }

  if (buf_size < s->len + 1) {
    return NULL; // insufficient space
  }

  memcpy(buf, s->ptr, s->len);
  buf[s->len] = '\0';

  return buf;
}

int sstr_cmp(const String *s1, const String *s2) {
  if (s1 == NULL || s2 == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  const unsigned char *p1 = (const unsigned char *)s1->ptr;
  const unsigned char *p2 = (const unsigned char *)s2->ptr;

  // '\0' is considered false
  while (*p1 && *p1 == *p2) {
    ++p1;
    ++p2;
  }

  return *p1 - *p2;
}

Err sstr_at(const String *s, size_t index, char *out) {
  if (s == NULL || out == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  if (index >= s->len) {
    return ERR_OUT_OF_BOUNDS;
  }

  *out = s->ptr[index];
  return SUCCESS;
}

size_t sstr_find(const String *str, const char *substr) {
  if (str == NULL || substr == NULL) {
    return 0;
  }

  if (*substr == '\0') {
    return 0;
  }

  for (size_t i = 0; i < str->len; ++i) {
    size_t j = 0;

    while (substr[j] != '\0' && i + j < str->len &&
           str->ptr[i + j] == substr[j]) {
      ++j;
    }

    if (substr[j] == '\0') {
      return i;
    }
  }

  return SSTR_NPOS;
}

#endif /* SSTR_IMPLEMENTATION */

#endif /* SSTR_H */
