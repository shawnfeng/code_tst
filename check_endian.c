#include <stdio.h>
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



int main()
{
	printf("%d\n", check_cpu_endian());
	printf("%d\n", check_cpu_endian2());
	printf("%d\n", check_cpu_endian3());

	printf("sizeof(short int)=%lu\n", sizeof(short int));
	printf("sizeof(int)=%lu\n", sizeof(int));
	printf("sizeof(long int)=%lu\n", sizeof(long int));
	printf("sizeof(long long int)=%lu\n", sizeof(long long int));

	printf("~0=0x%X\n", (uint8_t)~0);
	printf("~0 << 8=0x%X\n", (uint8_t)~0 << 8);
	printf("~0 << 56=0x%lX\n", (uint64_t)((uint8_t)~0) << 56);

	puts("32-------------------");
	int n1 = 0x12345678;
	unsigned char* pc = (unsigned char*)(&n1);
	printf("cpu order:\n");
	printf("[0]: 0x%X addr:%p\n", pc[0], &pc[0]);
	printf("[1]: 0x%X addr:%p\n", pc[1], &pc[1]);
	printf("[2]: 0x%X addr:%p\n", pc[2], &pc[2]);
	printf("[3]: 0x%X addr:%p\n", pc[3], &pc[3]);

	n1 = htonl(n1);
	printf("htonl order:\n");
	printf("[0]: 0x%X addr:%p\n", pc[0], &pc[0]);
	printf("[1]: 0x%X addr:%p\n", pc[1], &pc[1]);
	printf("[2]: 0x%X addr:%p\n", pc[2], &pc[2]);
	printf("[3]: 0x%X addr:%p\n", pc[3], &pc[3]);

	n1 = BigLittleSwap32(n1);
	printf("BigLittleSwap32 order:\n");
	printf("[0]: 0x%X addr:%p\n", pc[0], &pc[0]);
	printf("[1]: 0x%X addr:%p\n", pc[1], &pc[1]);
	printf("[2]: 0x%X addr:%p\n", pc[2], &pc[2]);
	printf("[3]: 0x%X addr:%p\n", pc[3], &pc[3]);

	puts("64-------------------");

	uint64_t n2 = 0x1A2B3C4D12345678;
	pc = (unsigned char*)(&n2);
	printf("cpu order:\n");
	printf("[0]: 0x%X addr:%p\n", pc[0], &pc[0]);
	printf("[1]: 0x%X addr:%p\n", pc[1], &pc[1]);
	printf("[2]: 0x%X addr:%p\n", pc[2], &pc[2]);
	printf("[3]: 0x%X addr:%p\n", pc[3], &pc[3]);
	printf("[4]: 0x%X addr:%p\n", pc[4], &pc[4]);
	printf("[5]: 0x%X addr:%p\n", pc[5], &pc[5]);
	printf("[6]: 0x%X addr:%p\n", pc[6], &pc[6]);
	printf("[7]: 0x%X addr:%p\n", pc[7], &pc[7]);

	n2 = BigLittleSwap64(n2);
	printf("BigLittleSwap64 order:\n");
	printf("[0]: 0x%X addr:%p\n", pc[0], &pc[0]);
	printf("[1]: 0x%X addr:%p\n", pc[1], &pc[1]);
	printf("[2]: 0x%X addr:%p\n", pc[2], &pc[2]);
	printf("[3]: 0x%X addr:%p\n", pc[3], &pc[3]);
	printf("[4]: 0x%X addr:%p\n", pc[4], &pc[4]);
	printf("[5]: 0x%X addr:%p\n", pc[5], &pc[5]);
	printf("[6]: 0x%X addr:%p\n", pc[6], &pc[6]);
	printf("[7]: 0x%X addr:%p\n", pc[7], &pc[7]);

	puts("16-------------------");

	uint16_t n3 = 0x1234;
	pc = (unsigned char*)(&n3);
	printf("cpu order:\n");
	printf("[0]: 0x%X addr:%p\n", pc[0], &pc[0]);
	printf("[1]: 0x%X addr:%p\n", pc[1], &pc[1]);

	n3 = BigLittleSwap16(n3);
	printf("BigLittleSwap16 order:\n");
	printf("[0]: 0x%X addr:%p\n", pc[0], &pc[0]);
	printf("[1]: 0x%X addr:%p\n", pc[1], &pc[1]);

	


	return 0;

}
