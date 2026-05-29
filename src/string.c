#include "../include/string.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

ErrorCode string_init(String *s_out, const char *s_in) {
  if (s_in == NULL) {
    s_out->ptr = NULL;
    s_out->len = 0;
    s_out->cap = 0;

    return ERR_NULL_PTR;
  }

  if (s_out == NULL) {
    return ERR_NULL_PTR;
  }

  s_out->ptr = NULL;
  s_out->len = 0;
  s_out->cap = DEFAULT_CAPACITY;

  for (size_t i = 0; s_in[i] != '\0'; i++) {
    s_out->len += 1;
  }

  if (s_out->len >= s_out->cap) {
    s_out->cap = s_out->len * 2;
  }

  s_out->ptr = (char *)malloc(s_out->cap);
  if (s_out->ptr == NULL) {

    return ERR_NULL_PTR;
  }

  for (size_t i = 0; s_in[i] != '\0'; i++) {
    s_out->ptr[i] = s_in[i];
  }
  s_out->ptr[s_out->len] = '\0';

  return SUCCESS;
}

ErrorCode string_destroy(String *s) {
  if (s == NULL) {
    return ERR_NULL_PTR;
  }

  free(s->ptr);
  s->ptr = NULL;
  s->len = 0;
  s->cap = 0;

  return SUCCESS;
}

ErrorCode string_append(String *s, char *slice) {
  if (slice == NULL) {
    return ERR_EMPTY_SLICE;
  }

  if (s == NULL) {
    return ERR_EMPTY_STRING;
  }

  if (s->ptr == NULL) {
    string_init(s, slice);
  }

  // TODO: compelete the function

  return SUCCESS;
}
