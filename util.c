#include "util.h"

u16 be16(u16 x)
{
	return (x>>8) | 
        (x<<8);
}

u32 be32(u32 x)
{
	return (x>>24) | 
        ((x<<8) & 0x00FF0000) |
        ((x>>8) & 0x0000FF00) |
        (x<<24);
}

u64 be64(u64 x)
{
    return (x>>56) |
        ((x<<40) & (u64)0x00FF000000000000LL) |
        ((x<<24) & (u64)0x0000FF0000000000LL) |
        ((x<<8) & (u64)0x000000FF00000000LL) |
        ((x>>8) & (u64)0x00000000FF000000LL) |
        ((x>>24) & (u64)0x0000000000FF0000LL) |
        ((x>>40) & (u64)0x000000000000FF00LL) |
        (x<<56);
}


f64 bef64(f64 x){
	union  {u64 a; f64 b;} hax;
	hax.b = x;
	hax.a = be64(hax.a);
	return hax.b;
}
