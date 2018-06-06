#include <stdio.h>

void  __attribute__((cdecl)) print(const char *msg);
void  __attribute__((cdecl)) print_nl(void);
void  __attribute__((cdecl)) print_int(long x);
void  __attribute__((cdecl)) print_float(float x);
long  __attribute__((cdecl)) read_int(void);
float __attribute__((cdecl)) read_float(void);

void print(const char *msg)
{
	printf("%s", msg);
}

void print_nl(void)
{
	printf("\n");
}

void print_int(long x)
{
	printf("%ld", x);
}

void print_float(float x)
{
	printf("%f", x);
}

long read_int(void)
{
	long ret = 0;
	scanf("%ld", &ret);
	return ret;
}

float read_float(void)
{
	float ret = 0.0f;
	scanf("%f", &ret);
	return ret;
}
