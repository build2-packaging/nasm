// HAVE_TYPEOF

#undef HAVE_TYPEOF

/* Checks for support of the __typeof__ keyword which is a commonly-supported
 * alternative for the typeof keyword which allows type inference in C.
 *
 * - Supported by GCC ≥ 2.5.
 * - Supported by Clang ≥ 3.0 (including AppleClang).
 * - Not supported by MSVC.
 * - Supported on most platforms where GCC/Clang is used, except limited environments.
 *
 * The HAVE_TYPEOF name is probaly not appropriate but the custom upstream
 * check at autoconf/m4/pa_c_typeof.m4 does something similar.
 */

#if (defined(__GNUC__) && (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 5))) || \
   (defined(__clang__) && (__clang_major__ >= 3))
#  define HAVE_TYPEOF 1
#  define typeof __typeof__
#endif
