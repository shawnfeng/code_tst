#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <netinet/in.h>

int check_cpu_endian()
{
	union {
		unsigned short int a;
		unsigned char b[2];
	} c;
	c.a = 1;
	//	printf("check=0x%x\n", c.a);
	return c.b[0] == 1;

}

int check_cpu_endian2()
{
	unsigned short int a = 1;

	return (a & 0x00ff) == 1;

}

int check_cpu_endian3()
{

	return ((unsigned short int)1 & 0x00ff) == 1;

}



#if BYTE_ORDER == BIG_ENDIAN
#define LITTLE_ENDIAN_CHANGE(N, SIZE) __bswap_##SIZE(N)
#elif BYTE_ORDER == LITTLE_ENDIAN
#define LITTLE_ENDIAN_CHANGE(N, SIZE) N
#else
# error "What kind of system is this?"
#endif

#if BYTE_ORDER == LITTLE_ENDIAN
#define BIG_ENDIAN_CHANGE(N, SIZE) __bswap_##SIZE(N)
#elif BYTE_ORDER == BIG_ENDIAN
#define BIG_ENDIAN_CHANGE(N, SIZE) N
#else
# error "What kind of system is this?"
#endif



uint16_t little_endian_chg16(uint16_t n)
{
#if BYTE_ORDER == BIG_ENDIAN
	return __bswap_16(n);
#elif BYTE_ORDER == LITTLE_ENDIAN
	return n;
#else
# error "What kind of system is this?"
#endif
}

uint32_t little_endian_chg32(uint32_t n)
{
#if BYTE_ORDER == BIG_ENDIAN
	return __bswap_32(n);
#elif BYTE_ORDER == LITTLE_ENDIAN
	return n;
#else
# error "What kind of system is this?"
#endif
}
	


uint64_t little_endian_chg64(uint64_t n)
{
#if BYTE_ORDER == BIG_ENDIAN
	return __bswap_64(n);
#elif BYTE_ORDER == LITTLE_ENDIAN
	return n;
#else
# error "What kind of system is this?"
#endif
}



int is_bigendian()
{

#if BYTE_ORDER == BIG_ENDIAN
	return 1;
#elif BYTE_ORDER == LITTLE_ENDIAN
	return 0;
#else
# error "What kind of system is this?"
#endif
}
//# error "fuck"
//# warning "fuck too"

#define BigLittleSwap16(A) (((uint16_t)(A) & ((uint16_t)((uint8_t)~0)<<8) ) >> 8) | \
	(((uint16_t)(A) & ((uint16_t)((uint8_t)~0)) ) << 8)


#define BigLittleSwap32(A) (((uint32_t)(A) & ((uint32_t)((uint8_t)~0)<<24) ) >> 24) | \
	(((uint32_t)(A) & ((uint32_t)((uint8_t)~0)<<16) ) >> 8) |	\
	(((uint32_t)(A) & ((uint32_t)((uint8_t)~0)<<8) ) << 8) |	\
	(((uint32_t)(A) & ((uint32_t)((uint8_t)~0)) ) << 24)

#define BigLittleSwap64(A) (((uint64_t)(A) & ((uint64_t)((uint8_t)~0)<<56) ) >> 56) | \
	(((uint64_t)(A) & ((uint64_t)((uint8_t)~0)<<48) ) >> 40) |	\
	(((uint64_t)(A) & ((uint64_t)((uint8_t)~0)<<40) ) >> 24) |	\
	(((uint64_t)(A) & ((uint64_t)((uint8_t)~0)<<32) ) >> 8)|	\
	(((uint64_t)(A) & ((uint64_t)((uint8_t)~0)<<24) ) << 8)|	\
	(((uint64_t)(A) & ((uint64_t)((uint8_t)~0)<<16) ) << 24)|	\
	(((uint64_t)(A) & ((uint64_t)((uint8_t)~0)<<8) ) << 40)|	\
	(((uint64_t)(A) & ((uint64_t)((uint8_t)~0)) ) << 56)


void print_endian(unsigned char* pc, int sz)
{
	int i = 0;
	for (i = 0; i < sz/8; ++i) {
		printf("[%d]: 0x%X addr:%p\n", i, pc[i], &pc[i]);
	}
}


const char *stream_anlys(const char *p, void *data, int len)
{
	memcpy(data, p, len);
	return p + len;
}


uint16_t stream_bit16(const char **p)
{
	uint16_t n = 0;
	*p = stream_anlys(*p, &n, sizeof(n));
	n = LITTLE_ENDIAN_CHANGE(n, 16);
	return n;
}


uint16_t stream_ltt_bit16(const uint8_t *p, int len)
{
	uint16_t n = 0;
	memcpy(&n, p, len/8);
	n = LITTLE_ENDIAN_CHANGE(n, 16);
	return n;

}

uint16_t stream_big_bit16(const uint8_t *p, int len)
{
	uint16_t n = 0;
	memcpy(&n, p, len/8);
	n = BIG_ENDIAN_CHANGE(n, 16);
	return n;

}

uint64_t stream_ltt_bit64(const uint8_t *p, int len)
{
	uint64_t n = 0;
	memcpy(&n, p, len/8);
	n = LITTLE_ENDIAN_CHANGE(n, 64);
	return n;

}


uint64_t stream_big_bit64(const uint8_t *p, int len)
{
	uint64_t n = 0;
	memcpy(&n, p, len/8);
	n = BIG_ENDIAN_CHANGE(n, 64);
	return n;

}

uint32_t stream_ltt_bit32(const char **p, int sz)
{
	uint32_t n = 0;
	memcpy(&n, *p, sz);
	n = LITTLE_ENDIAN_CHANGE(n, 32);
	*p += sz;
	return n;

}

char *bit32_ltt_stream(uint32_t n, char *p, int sz)
{
	n = LITTLE_ENDIAN_CHANGE(n, 32);
	memcpy(p, &n, sz);
	return p + sz;
}


int main()
{
	printf("%d\n", check_cpu_endian());
	printf("%d\n", check_cpu_endian2());
	printf("%d\n", check_cpu_endian3());
	printf("%d\n", is_bigendian());
	

	printf("sizeof(short int)=%lu\n", sizeof(short int));
	printf("sizeof(int)=%lu\n", sizeof(int));
	printf("sizeof(long int)=%lu\n", sizeof(long int));
	printf("sizeof(long long int)=%lu\n", sizeof(long long int));

	printf("~0=0x%X\n", (uint8_t)~0);
	printf("~0 << 8=0x%X\n", (uint8_t)~0 << 8);
	printf("~0 << 56=0x%lX\n", (uint64_t)((uint8_t)~0) << 56);

	int aa = -3;
	unsigned int bb = aa;
	int cc = bb;
	printf("int %d; uint %u int %d\n", aa, bb, cc);
	/*
	uint32_t dd = UINT32_MAX;
	int32_t ee = dd;
	printf("uintmax %u int %d\n", dd, ee);
	*/

	puts("32-------------------");
	int n1 = 0x12345678;
	unsigned char* pc = (unsigned char*)(&n1);
	printf("cpu order:\n");
	print_endian(pc, 32);

	n1 = htonl(n1);
	printf("htonl order:\n");
	print_endian(pc, 32);

	n1 = BigLittleSwap32(n1);
	printf("BigLittleSwap32 order:\n");
	print_endian(pc, 32);

	puts("64-------------------");

	uint64_t n2 = 0x1A2B3C4D12345678;
	pc = (unsigned char*)(&n2);
	printf("cpu order:\n");
	print_endian(pc, 64);

	n2 = BigLittleSwap64(n2);
	printf("BigLittleSwap64 order:\n");
	print_endian(pc, 64);

	n2 = __bswap_64(n2);
	printf("__bswap_64 order:\n");
	print_endian(pc, 64);

	n2 = LITTLE_ENDIAN_CHANGE(n2, 64);
	printf("LITTLE_ENDIAN_CHANGE64 order:\n");
	print_endian(pc, 64);

	n2 = BIG_ENDIAN_CHANGE(n2, 64);
	printf("BIG_ENDIAN_CHANGE64 order:\n");
	print_endian(pc, 64);


	puts("16-------------------");

	uint16_t n3 = 0x1234;
	pc = (unsigned char*)(&n3);
	printf("cpu order:\n");
	print_endian(pc, 16);

	n3 = BigLittleSwap16(n3);
	printf("BigLittleSwap16 order:\n");
	print_endian(pc, 16);


	n3 = little_endian_chg16(n3);
	printf("little_endian_chg16 order:\n");
	print_endian(pc, 16);

	n3 = LITTLE_ENDIAN_CHANGE(n3, 16);
	printf("LITTLE_ENDIAN_CHANGE16 order:\n");
	print_endian(pc, 16);

	n3 = BIG_ENDIAN_CHANGE(n3, 16);
	printf("BIG_ENDIAN_CHANGE16 order:\n");
	print_endian(pc, 16);


	n3 = stream_ltt_bit16(pc, 16);
	printf("stream_ltt_bit16 order:\n");
	print_endian(pc, 16);

	n3 = stream_big_bit16(pc, 16);
	printf("stream_big_bit16 order:\n");
	print_endian(pc, 16);

	puts("16-------------------");
	uint16_t tn = 0x1234;
	print_endian((unsigned char*)(&tn), 16);

	uint64_t n4 = stream_ltt_bit64((unsigned char*)(&tn), 16);
	pc = (unsigned char*)(&n4);
	printf("stream_ltt_bit64 order:\n");
	print_endian(pc, 64);


	n4 = stream_big_bit64((unsigned char*)(&tn), 16);
	pc = (unsigned char*)(&n4);
	printf("stream_big_bit64 order:\n");
	print_endian(pc, 64);


	puts("32-------------------");	
	char buff[4] = {'\x12', '\x34', '\x56', '\x78'};
	print_endian((unsigned char *)buff, 32);
	const char *p = buff;
	printf("p=%p\n", p);
	int m0 = stream_ltt_bit32(&p, 4);
	printf("p=%p m=0x%X\n", p, m0);

	char buff2[4];
	char *p2 = buff2;
	printf("p=%p\n", p2);
	p2 = bit32_ltt_stream(m0, p2, 4);
	print_endian((unsigned char *)buff2, 32);
	printf("p=%p\n", p2);
	


	return 0;

}
