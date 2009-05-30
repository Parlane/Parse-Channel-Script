#ifndef UTIL_H
#define UTIL_H

#include <features.h>
#include <stdint.h>


__BEGIN_DECLS

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef double f64;


u16 be16(u16 x);
u32 be32(u32 x);
u64 be64(u64 x);
f64 bef64(f64 x);

#define U8P(x) ((u8*)(x))
#define U16P(x) ((u16*)(x))
#define S16P(x) ((s16*)(x))
#define U32P(x) ((u32*)(x))
#define U64P(x) ((u64*)(x))
#define F64P(x) ((f64*)(x))




__END_DECLS

#endif
