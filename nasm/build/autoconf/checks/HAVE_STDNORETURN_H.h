// HAVE_STDNORETURN_H

#undef HAVE_STDNORETURN_H

/* Check for the <stdnoreturn.h> header.
 *
 * A C11 freestanding header, provided by the compiler rather than the C
 * library. Available since GCC 4.7 and Clang 3.1, covering Linux/glibc,
 * the BSDs, Mac OS, and Windows via MinGW. Not available on Windows
 * except MinGW.
 *
 * Real MSVC does not ship the header. Clang targeting the MSVC ABI
 * (_MSC_VER defined) does ship its own copy, but it must not be used
 * there either: the header's noreturn macro has no #ifndef guard, and
 * once defined it breaks any MSVC system header parsed later in the
 * same translation unit that spells the attribute literally, such as
 * <setjmp.h>'s declaration of longjmp() via __declspec(noreturn). This
 * produces a __declspec attributes must be an identifier or string
 * literal error, so _MSC_VER is excluded regardless of compiler family.
 */
#if (defined(__GNUC__) && !defined(__clang__) &&                               \
      (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7))) ||             \
    (defined(__clang__) &&                                                     \
      (__clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 1)) && \
        !defined(_MSC_VER))
#define HAVE_STDNORETURN_H 1
#endif
