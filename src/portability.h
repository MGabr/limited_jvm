#ifndef PORTABILITY_H

#ifndef LITTLE_ENDIAN
#ifndef BIG_ENDIAN

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define BIG_ENDIAN
#elif __BYTE_ORDER__ == __ORDER_LITLE_ENDIAN__
#define LITTLE_ENDIAN
#else
	#ifdef __BIG_ENDIAN__
	#define BIG_ENDIAN
	#else
		#ifdef __LITTLE_ENDIAN__
		#define LITTLE_ENDIAN
		#else
		#define ENDIAN_NOT_DETECTED
		#endif
	#endif
#endif

#endif
#endif

#endif
