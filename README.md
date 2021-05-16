# portable-endianness

This repo is a formal collection of code to make copy-pasting Rob Pike's [The byte order fallacy](https://commandcenter.blogspot.com/2012/04/byte-order-fallacy.html) simpler. Historically, many codebases use complicated endian detection patterns in their build systems to detect whether the host CPU is big or little endian and whether they need to do a byteswap when loading data from a bytestream. As Rob Pike has shown, knowing the endianness of the host CPU is a fallacy, and by avoiding detection and using simple bitshift patterns, we can arrive at assembly that is as efficient using modern compilers as using byteswap macros.

The code in `include/` and `src/` is vanilla C99 (the tests are written in C++, because C++ has better random sampling routines), has been tested on various little- and big-endian platforms (PPC64-BE and SPARC) and is free of any clever tricks with undefined behavior.

# License

The code here is placed under the Unlicense, i.e., it is placed in the public domain to encourage copying to other repos.

# FAQ

### I've sprinkled `#ifdef BIG_ENDIAN` macros all over my code to make it more efficient, why should I bother with these routines?

Practice has shown that `#ifdef BIG_ENDIAN`-style conditionals are brittle, especially in "double big-endian" scenarios, that is, loading big-endian bytestreams on big-endian CPUs, as these don't require a byteswap, yet code in practice often isn't tested on these platforms and the "double negation" logic breaks.

Rob Pike's approach eschews such macros, and relies on first principles to avoid byteswaps. Modern compilers recognize common patterns and can optimize them for the target platform. Here are a few examples, focusing on the 64-bit routines:

##### [GCC 10 `-O3 -march=x86-64`](https://godbolt.org/z/xa49vcGP7)

```assembly_x86
load64_from_LE(unsigned char const*):
        mov     rax, QWORD PTR [rdi]
        ret
load64_from_BE(unsigned char const*):
        mov     rax, QWORD PTR [rdi]
        bswap   rax
        ret
store64_to_LE(unsigned long, unsigned char*):
        mov     QWORD PTR [rsi], rdi
        ret
store64_to_BE(unsigned long, unsigned char*):
        bswap   rdi
        mov     QWORD PTR [rsi], rdi
        ret
```

Notice that the produced assembly is optimal. The compiler recognizes the pattern and adds the necessary byteswap instructions for the big-endian routines.

##### [GCC 10 `-O3 -march=haswell`](https://godbolt.org/z/17G3jKToo)

```assembly_x86
load64_from_LE(unsigned char const*):
        mov     rax, QWORD PTR [rdi]
        ret
load64_from_BE(unsigned char const*):
        movbe   rax, QWORD PTR [rdi]
        ret
store64_to_LE(unsigned long, unsigned char*):
        mov     QWORD PTR [rsi], rdi
        ret
store64_to_BE(unsigned long, unsigned char*):
        movbe   QWORD PTR [rsi], rdi
        ret
```

When targeting Intel's Haswell, GCC makes use of Haswell's new `MOVBE` instruction to reduce the number of instructions down to two.

##### [Clang 11 `-O3 -march=haswell`](https://godbolt.org/z/Goa9rv1jc)

```assembly_x86
load64_from_LE(unsigned char const*):  # @load64_from_LE(unsigned char const*)
        mov     rax, qword ptr [rdi]
        ret
load64_from_BE(unsigned char const*):  # @load64_from_BE(unsigned char const*)
        movbe   rax, qword ptr [rdi]
        ret
store64_to_LE(unsigned long, unsigned char*):  # @store64_to_LE(unsigned long, unsigned char*)
        mov     qword ptr [rsi], rdi
        ret
store64_to_BE(unsigned long, unsigned char*):  # @store64_to_BE(unsigned long, unsigned char*)
        movbe   qword ptr [rsi], rdi
        ret
```

Like GCC, Clang also recognizes this common pattern and boils it down to a `MOVBE` instruction when targeting Haswell.

##### [Clang 11 `-O3 -march=armv8.6-a`](https://godbolt.org/z/bxzMbcfbz)

```assembly_x86
load64_from_LE(unsigned char const*):  // @load64_from_LE(unsigned char const*)
        ldr     x0, [x0]
        ret
load64_from_BE(unsigned char const*):  // @load64_from_BE(unsigned char const*)
        ldr     x8, [x0]
        rev     x0, x8
        ret
store64_to_LE(unsigned long, unsigned char*):  // @store64_to_LE(unsigned long, unsigned char*)
        str     x0, [x1]
        ret
store64_to_BE(unsigned long, unsigned char*):  // @store64_to_BE(unsigned long, unsigned char*)
        rev     x8, x0
        str     x8, [x1]
        ret
```

On ARM64, Clang 11 produces optimal code too, similar to the vanilla `-march=x86-64` example by inserting necessary byteswap instructions.

##### [Clang trunk `-O3 -mcpu=power9`](https://godbolt.org/z/MqGYadY4T)

```assembly_x86
load64_from_LE(unsigned char const*):  # @load64_from_LE(unsigned char const*)
[...]
        ldbrx 3, 0, 3
        blr
load64_from_BE(unsigned char const*):  # @load64_from_BE(unsigned char const*)
[...]
        ld 3, 0(3)
        blr
store64_to_LE(unsigned long long, unsigned char*):  # @store64_to_LE(unsigned long long, unsigned char*)
[...]
        stdbrx 3, 0, 4
        blr
store64_to_BE(unsigned long long, unsigned char*):  # @store64_to_BE(unsigned long long, unsigned char*)
[...]
        std 3, 0(4)
        blr
```

On a big-endian POWER9, Clang injects the byteswap instructions at the right place too, using the optimal "Load Doubleword Byte-Reverse Indexed" (`ldbrx`) and "Store Doubleword Byte-Reverse Indexed" (`stdbrx`) instructions.

### How are these routines different from the `<endian.h>` ones?

In a meta way, they aren't. There are a few gotchas with `<endian.h>`:

1. `<endian.h>` isn't portable. On BSDs, you have to use `<sys/endian.h>` instead of `<endian.h>`. OpenBSD uses `betoh32`, whereas NetBSD, FreeBSD, and glibc use `be32toh`, which makes things worse. Finally, `<endian.h>` doesn't work on Windows, requiring more masking of `#include`s on Windows.
2. Ultimately, the only endianness that matters is that of serialized bytestreams, **not** that of actual integer types loaded into memory. `<endian.h>` focuses on the wrong abstraction: By working on (unsigned) integers alone, the programmer is forced to load these integers from the bytestream and then only convert them. Unfortunately, code around these places often also invokes undefined behavior in the form of type-punning:

```c
#include <endian.h>

unsigned foo(const unsigned char* in) {
    const unsigned* ptr = (const unsigned*)in;
    return be32toh(*ptr);
}
```

The `in` pointer is not guaranteed to be aligned to a 4-byte boundary and such code is often broken on platforms that do not allow unaligned access (SPARC in particular). Furthermore, even on x86 (which does allow unaligned access), GCC and Clang are known to break such code with their aggressive optimizers. *portable-endianness* only operates on bytes, and only casts loaded values (**not** pointers), avoiding any undefined behavior pitfalls.
3. Even in cases where an `<endian.h>`-style cast is desired, *portable-endianness* can still be used:

```c
// definition of store64_to_BE visible to the compiler

uint64_t foo(const uint64_t val) {
    uint64_t result;
    store64_to_BE(val, (unsigned char*)&result);
    return result;
}
```

which yields optimal

```assembly_x86
foo(unsigned long):  # @foo(unsigned long)
        mov     rax, rdi
        bswap   rax
        ret
```
