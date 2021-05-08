#ifndef BASE_H
#define BASE_H

#define MUST_USE_RESULT  __attribute__((warn_unused_result))
#define DONT_OPTIMIZE    __attribute__((optimize("-O0")))

#define ALWAYS_INLINE    __attribute__((always_inline))
#define ALWAYS_NO_INLINE __attribute__((noinline))

#define MAYBE_UNUSED     __attribute__((unused))

#define BEGIN_NO_OPTIMIZATION           _Pragma("GCC push_options") \
                                        _Pragma("GCC optimize(\"O0\")")
#define END_NO_OPTIMIZATION             _Pragma("GCC pop_options")

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif  // UNUSED

#ifndef COMPILER_BARRIER
#define COMPILER_BARRIER()              asm volatile ("":::"memory")
#endif  // COMPILER_BARRIER

#endif