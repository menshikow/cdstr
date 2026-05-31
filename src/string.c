#include "../include/string.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// later impelemnt a strlen in a slice

size_t my_strlen(const char *string_start) {
  if (string_start == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  const char *string_end = string_start;
  while (*string_end != '\0') {
    ++string_end;
  }

  return string_end - string_start;
}

static size_t copy_bytes(char *restrict dest, const void *restrict src,
                         size_t count) {

  size_t bytes_to_copy = count;
  size_t copied = bytes_to_copy;

  const unsigned char *s = src;

  while (bytes_to_copy--) {
    *dest++ = *s++;
  }

  return copied;
}

ErrorCode string_init(String *s_out, const char *s_in) {
  if (s_out == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  if (s_in == NULL) {
    s_out->ptr = NULL;
    s_out->len = 0;
    s_out->cap = 0;

    return ERR_NULL_ARGUMENT;
  }

  s_out->ptr = NULL;
  s_out->len = 0;
  s_out->cap = DEFAULT_CAPACITY;

  s_out->len = my_strlen(s_in);

  if (s_out->len >= s_out->cap) {
    s_out->cap = s_out->len * 2;
  }

  s_out->ptr = (char *)malloc(s_out->cap);
  if (s_out->ptr == NULL) {
    return ERR_ALLOC_FAILED;
  }

  copy_bytes(s_out->ptr, s_in, s_out->len + 1);

  return SUCCESS;
}

ErrorCode string_destroy(String *s) {
  if (s == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  free(s->ptr);
  s->ptr = NULL;
  s->len = 0;
  s->cap = 0;

  return SUCCESS;
}

ErrorCode string_append(String *s, const char *slice) {
  if (s == NULL || slice == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  if (s->ptr == NULL) {
    return (string_init(s, slice) == SUCCESS) ? SUCCESS
                                              : ERR_STRING_INIT_FAILED;
  }

  size_t slice_len = my_strlen(slice);

  size_t new_len = s->len + slice_len;

  if (s->cap < new_len + 1) {
    size_t tmp_cap = new_len * 2;
    char *tmp_ptr = realloc(s->ptr, tmp_cap);

    if (tmp_ptr == NULL) {
      return ERR_ALLOC_FAILED;
    }

    s->ptr = tmp_ptr;
    s->cap = tmp_cap;
  }

  copy_bytes(s->ptr + s->len, slice, slice_len + 1);

  s->len = new_len;

  return SUCCESS;
}

// think about 1. Self-clone, 2. What if src is destroyed?, 3. What if dest is
// in the destroyed/empty state?

ErrorCode string_clone(String *dest, String const *src) {
  if (src == NULL || dest == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  if (dest->cap < src->len + 1) {
    size_t tmp_cap = src->len * 2;
    char *tmp_ptr = realloc(dest->ptr, tmp_cap);

    if (tmp_ptr == NULL) {
      return ERR_ALLOC_FAILED;
    }

    dest->ptr = tmp_ptr;
    dest->cap = tmp_cap;
    copy_bytes(dest->ptr, src->ptr, src->len + 1);
    dest->len = src->len;

    return SUCCESS;
  }

  copy_bytes(dest->ptr, src->ptr, src->len + 1);
  dest->len = src->len;

  return SUCCESS;
}

int main() { return 0; }
