# Blog: Building a C String Library — What I Learned

> Draft / outline. Use this to write a blog post after finishing the project.

---

## 1. Why Build a String Library in C?

C strings are notoriously primitive: null-terminated `char*` with no length, no bounds, no safety. Every serious C project ends up wrapping them. I built `ctringslib` from scratch to understand **what actually goes into a string abstraction**.

Key realizations:
- Higher-level languages (Python, Rust, C++) all have string types — but they don't teach you *why* they're designed that way.
- A string library touches almost every major systems concept: memory allocation, ownership, capacity growth, API design, testing, build systems.
- Redis's SDS, C++ `std::string`, Rust's `String` — they all solve the same core problem: giving C strings a proper lifetime.

---

## 2. What's Inside a String? (The Struct)

```c
typedef struct {
  char *ptr;    // pointer to heap memory
  size_t len;   // actual bytes (not including null terminator)
  size_t cap;   // allocated capacity
} String;
```

Key insight: **separating len and cap** is the heart of a dynamic string. Capacity allows appending without reallocating every time. This is the foundation of amortized O(1) append.

Invariants I learned to enforce:
- `ptr != NULL` — always point to valid memory (or a sentinel state)
- `len < cap` — room for null terminator
- `ptr[len] == '\0'` — C compatibility
- `allocated_bytes >= cap` — we own what we think we own

---

## 3. Ownership: The Hardest Lesson

> "Almost every difficult C bug is really an ownership bug."
> — from my notes

The core question: **who is responsible for freeing this memory?**

In my string library:
- `String` owns its `ptr` — it allocates and frees
- Functions create *deep copies* — the caller keeps ownership of their input
- Destroy sets `ptr=NULL, len=0, cap=0` — explicit invalidation

**The shallow copy problem:**
```c
String a = string_init("hello");
String b = a; // both structs hold SAME ptr
string_destroy(&a); // frees the memory
// b.ptr is now a dangling pointer
```

This is why Rust's ownership model exists. In C, you either:
1. Ban copying (like `FILE*`)
2. Use reference counting
3. Document: "you must not do this"

**Ownership lifecycle as a state machine:**
```
invalid → init → valid → destroy → invalid
```

This pattern came up repeatedly. Many systems abstractions are fundamentally controlled state transitions.

---

## 4. Memory Management

Three regions:
- **Stack**: fast, automatic, limited lifetime
- **Heap**: manual, flexible, needs explicit free
- **Static**: lives for program duration

For dynamic strings, you *must* use the heap. The stack doesn't work for runtime-sized data.

`malloc` gives you raw bytes. `free` returns them. The operating system tracks allocated pages, but **the OS doesn't know about individual malloc calls** — that's the libc allocator's job (sbrk, mmap, free lists).

Key learnings:
- `realloc` may change the pointer. Any cached pointers into the buffer become invalid.
- Amortized growth (doubling capacity) means O(1) average append, O(n) worst case.
- SSO (Small String Optimization): store small strings directly in the struct, no heap allocation. C++ `std::string` typically stores 15-22 bytes inline.

---

## 5. API Design Philosophy

Things I learned about designing interfaces:

- **Return error codes, not NULL**: gives callers information about *what* went wrong
- **Parameters: output first, inputs after** (`string_init(String *out, const char *in)`)
- **Invalid states are a feature**: `destroy` + `append` reuse creates a more flexible lifecycle than strict init→use→destroy
- **Test cases in comments**: writing test cases in the header comment forces you to think about edge cases before implementing

Error code design taught me: distinguish between null pointer, failed allocation, empty input — each needs different handling.

---

## 6. The Build Pipeline

> "Build systems are one of the most important things to learn early in C because C exposes how software actually becomes a program."

The full pipeline:
```
.c → preprocessor → compiler → assembler → .o → linker → executable
```

I learned:
- Compilation units: each `.c` is compiled independently
- Object files contain symbols (functions, variables) that the linker resolves
- Headers are not compiled — they're textually included by the preprocessor
- Static libraries are archives of `.o` files; dynamic libraries are linked at runtime
- `#include` is literally text replacement — this is why header guards exist

---

## 7. Testing Mindset

> "Testing is systematically attacking your own assumptions."

Not "run program, see if works" — but **deliberately trying to break your code**.

For a string library, edge cases include:
- Empty strings
- NULL pointers
- Very large strings
- Appending to self (overlapping memory)
- Multiple destroys
- Reallocation boundaries

Testing in C is especially critical because the language gives you almost no safety. A memory bug can look correct, corrupt data silently, and crash only on a different machine in release mode.

Tools I want to use:
- AddressSanitizer (ASan): detects use-after-free, buffer overflows
- Unit tests for every public function
- Property-based testing for invariants (e.g., `split + join == original`)

---

## 8. Learning Process (Meta)

How I approached this project:

1. **Learn a tiny concept** (e.g., pointer + length)
2. **Implement the tiny thing** (e.g., string slice)
3. **Break it** — find edge cases, invalid states
4. **Debug it** — use sanitizers, GDB
5. **Read how experts solved it** — Redis SDS, Lua strings, C++ std::string
6. **Redesign** — this is where deep understanding forms

**Using AI effectively:**
- Treat it as a thinking partner, not a solution generator
- Ask *why* something works, not just *what* to write
- The confusion + debugging + design struggle is where expertise forms — don't skip it by asking for ready solutions

---

## 9. What's Left to Build

- [ ] Complete `string_append` and all remaining operations
- [ ] `string_view` (borrowed, non-owning slice)
- [ ] Test suite and CI
- [ ] Build system
- [ ] SSO implementation
- [ ] Reference counting / COW for safe sharing
- [ ] README and documentation

---

## 10. Biggest Takeaways

1. **Ownership is the fundamental concept in systems programming.** Everything else follows from it.
2. **State machines** (invalid → valid → invalid) are everywhere in low-level design.
3. **Capacity ≠ length.** The gap between them is where performance lives.
4. **Write test cases before implementing.** It clarifies the contract.
5. **Read real code.** Redis SDS, musl, glibc — these are the textbooks.
6. **"C is simple, not easy."** — The language is small, but building correct software in it requires understanding everything underneath.

---

*Started: May 2026*
*Status: In progress — core types and lifecycle implemented, functions and tests underway*
