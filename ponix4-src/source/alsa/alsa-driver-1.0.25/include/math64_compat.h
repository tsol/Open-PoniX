/*
 * div_u64() compat
 */

#ifndef MATH64_COMPAT_H
#define MATH64_COMPAT_H

#if BITS_PER_LONG >= 64

static inline u64 div_u64_rem(u64 n, u32 div, u32 *rem)
{
	*rem = n % div;
	return n / div;
}

static inline u64 div_u64(u64 n, u32 div)
{
	return n / div;
}

#elif defined(i386)

static inline u64 div_u64_rem(u64 n, u32 div, u32 *rem)
{
	u32 low, high;

	low = (u32)n;
	high = n >> 32;
	if (high) {
		u32 high1 = high % div;
		high /= div;
		asm("divl %2" : "=a" (low), "=d" (*rem) : \
		    "rm" (div), "a" (low), "d" (high1));
		return (u64)high << 32 | low;
	} else {
		*rem = low % div;
		return low / div;
	}
}

static inline u64 div_u64(u64 n, u32 div)
{
	u32 low, high;

	low = (u32)n;
	high = n >> 32;
	if (high) {
		u32 high1 = high % div;
		high /= div;
		asm("divl %2" : "=a" (low) : \
		    "rm" (div), "a" (low), "d" (high1));
		return (u64)high << 32 | low;
	} else
		return low / div;
}

#else

static inline void divl(u32 high, u32 low, u32 div, u32 *q, u32 *r)
{
	u64 n = (u64)high << 32 | low;
	u64 d = (u64)div << 31;
	u32 q1 = 0;
	int c = 32;
	while (n > 0xffffffffU) {
		q1 <<= 1;
		if (n >= d) {
			n -= d;
			q1 |= 1;
		}
		d >>= 1;
		c--;
	}
	q1 <<= c;
	if (n) {
		low = n;
		*q = q1 | (low / div);
		*r = low % div;
	} else {
		*r = 0;
		*q = q1;
	}
	return;
}

static inline u64 div_u64_rem(u64 n, u32 div, u32 *rem)
{
	u32 low, high;
	low = (u32)n;
	high = n >> 32;
	if (high) {
		u32 high1 = high % div;
		u32 low1 = low;
		high /= div;
		divl(high1, low1, div, &low, rem);
		return (u64)high << 32 | low;
	} else {
		*rem = low % div;
		return low / div;
	}
}

static inline u64 div_u64(u64 n, u32 div)
{
	u32 low, high, rem;
	low = (u32)n;
	high = n >> 32;
	if (high) {
		u32 high1 = high % div;
		u32 low1 = low;
		high /= div;
		divl(high1, low1, div, &low, rem);
		return (u64)high << 32 | low;
	} else {
		return low / div;
	}
}

#endif

#endif /* MATH64_COMPAT_H */
