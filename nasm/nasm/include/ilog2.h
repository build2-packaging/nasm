/* ----------------------------------------------------------------------- *
 *
 *   Copyright 1996-2017 The NASM Authors - All Rights Reserved
 *   See the file AUTHORS included with the NASM distribution for
 *   the specific copyright holders.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following
 *   conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *
 *     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *     CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *     INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *     MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *     CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *     SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *     NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *     HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *     CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *     OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *     EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ----------------------------------------------------------------------- */

#ifndef ILOG2_H
#define ILOG2_H

#include "compiler.h"

/* Support either C99 inline semantics or no inlining.
 *
 * The reason we had to patch this code is because MSVC issues "duplicate
 * symbol" linker errors when including this header from ilog2.c which results
 * in the following code:
 *
 *   inline void foo() {} // From the header
 *   extern void foo();   // Generate definition with external linkage
 *
 * Changing the `extern` to `extern inline` removes the warnings. Most online
 * references mention only `extern line` but the C standard seems to say that
 * both are correct, and GCC does accept both. So assume the problem is MSVC's
 * C support which has been called less than solid in the past.
 *
 * The Chromium issue at https://issues.chromium.org/issues/41473981 claims
 * that MSVC's `inline` has C++ semantics. The MSVC docs don't cover linkage (
 * https://learn.microsoft.com/en-us/cpp/c-language/inline-functions). And as
 * far as I can tell NASM's custom autoconf test will enable C99 inline on
 * MSVC. So it's hard to tell what is supposed to be correct. @@ TODO So let's
 * enable C99 inlining for now and see what happens during CI under other
 * versions of MSVC.
 *
 * C99 inline semantics (in the NASM context) are as follows:
 *
 * All TUs but ilog2.c contain only an inline-qualified definition of each
 * function. These definitions all have internal linkage.
 *
 * The ilog2.c TU contains the same inline-qualified definitions but also, for
 * each function, an extern inline-qualified declaration. These generate the
 * sole external-linkage definition of each function.
 *
 * This is what we need to end up with:
 *
 * Inlining enabled (C99 semantics):
 *
 *   All TUs but ilog2.c:
 *
 *     inline void foo() {}
 *
 *   ilog2.c:
 *
 *     extern inline void foo();
 *     inline void foo() {}
 *
 * Inlining disabled:
 *
 *   All TUs but ilog2.c:
 *
 *     void foo();
 *
 *   ilog2.c:
 *
 *     void foo();
 *     void foo() {}
 */

/* Inline qualifier for function declarations. If undefined don't emit
 * function declarations.
 */
#undef decl_inline

/* Inline qualifier for function definitions. If undefined don't emit function
 * definitions.
 */
#undef defn_inline

/* Note: make only a basic attempt at supporting GNU inline semantics; this
 * won't be tested. Upstream only supports GNU inline semantics in order to
 * work around a GCC bug fixed 11 years ago (GCC 4.9.0) in which the "missing
 * prototypes" or "no previous declaration" warnings are emitted for C99-style
 * inline functions. Presumably not many users are still on gcc < 4.9.0. (See
 * the upstream autoconf/m4/pa_check_bad_stdc_inline.m4 for more details.)
 */
#ifdef HAVE_STDC_INLINE
#  ifdef ILOG2_C
#    define decl_inline extern inline
#  endif
#  define defn_inline inline
#elif defined(HAVE_GNU_INLINE)
#  ifdef ILOG2_C
#    define decl_inline extern
#  endif
#  define defn_inline extern inline
#else
#  define decl_inline
#  ifdef ILOG2_C
#    define defn_inline
#  endif
#endif

#ifdef decl_inline
decl_inline unsigned int const_func ilog2_32(uint32_t v);
decl_inline unsigned int const_func ilog2_64(uint64_t v);
decl_inline unsigned int const_func ilog2_64(uint64_t vv);
decl_inline int const_func alignlog2_32(uint32_t v);
decl_inline int const_func alignlog2_64(uint64_t v);
#endif

#ifdef defn_inline

#define ROUND(v, a, w)                                  \
    do {                                                \
        if (v & (((UINT32_C(1) << w) - 1) << w)) {      \
            a  += w;                                    \
            v >>= w;                                    \
        }                                               \
    } while (0)


#if defined(HAVE___BUILTIN_CLZ) && INT_MAX == 2147483647

defn_inline unsigned int const_func ilog2_32(uint32_t v)
{
    if (!v)
        return 0;

    return __builtin_clz(v) ^ 31;
}

#elif defined(__GNUC__) && defined(__x86_64__)

defn_inline unsigned int const_func ilog2_32(uint32_t v)
{
    unsigned int n;

    __asm__("bsrl %1,%0"
            : "=r" (n)
            : "rm" (v), "0" (0));
    return n;
}

#elif defined(__GNUC__) && defined(__i386__)

defn_inline unsigned int const_func ilog2_32(uint32_t v)
{
    unsigned int n;

#ifdef __i686__
    __asm__("bsrl %1,%0 ; cmovz %2,%0\n"
            : "=&r" (n)
            : "rm" (v), "r" (0));
#else
    __asm__("bsrl %1,%0 ; jnz 1f ; xorl %0,%0\n"
            "1:"
            : "=&r" (n)
            : "rm" (v));
#endif
     return n;
}

#elif defined(HAVE__BITSCANREVERSE)

defn_inline unsigned int const_func ilog2_32(uint32_t v)
{
    unsigned long ix;
    return _BitScanReverse(&ix, v) ? v : 0;
}

#else

defn_inline unsigned int const_func ilog2_32(uint32_t v)
{
    unsigned int p = 0;

    ROUND(v, p, 16);
    ROUND(v, p,  8);
    ROUND(v, p,  4);
    ROUND(v, p,  2);
    ROUND(v, p,  1);

    return p;
}

#endif

#if defined(HAVE__BUILTIN_CLZLL) && LLONG_MAX == 9223372036854775807LL

defn_inline unsigned int const_func ilog2_64(uint64_t v)
{
    if (!v)
        return 0;

    return __builtin_clzll(v) ^ 63;
}

#elif defined(__GNUC__) && defined(__x86_64__)

defn_inline unsigned int const_func ilog2_64(uint64_t v)
{
    uint64_t n;

    __asm__("bsrq %1,%0"
            : "=r" (n)
            : "rm" (v), "0" (UINT64_C(0)));
    return n;
}

#elif defined(HAVE__BITSCANREVERSE64)

defn_inline unsigned int const_func ilog2_64(uint64_t v)
{
    unsigned long ix;
    return _BitScanReverse64(&ix, v) ? ix : 0;
}

#else

defn_inline unsigned int const_func ilog2_64(uint64_t vv)
{
    unsigned int p = 0;
    uint32_t v;

    v = vv >> 32;
    if (v)
        p += 32;
    else
        v = vv;

    return p + ilog2_32(v);
}

#endif

/*
 * v == 0 ? 0 : is_power2(x) ? ilog2_X(v) : -1
 */
defn_inline int const_func alignlog2_32(uint32_t v)
{
    if (unlikely(v & (v-1)))
        return -1;              /* invalid alignment */

    return ilog2_32(v);
}

defn_inline int const_func alignlog2_64(uint64_t v)
{
    if (unlikely(v & (v-1)))
        return -1;              /* invalid alignment */

    return ilog2_64(v);
}

#undef ROUND

#endif /* defn_inline */

#endif /* ILOG2_H */
