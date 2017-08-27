#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <byteswap.h>
#include <getopt.h>
#include "zergmap.h"
#include "math_functions.h"

double convert32(uint32_t num);
double convert64(uint64_t num);
uint32_t swap24(uint8_t *val);
uint32_t swap32(uint32_t val);
uint64_t swap64(uint64_t val);
double binary32(uint32_t i);
double binary64(uint64_t i);

double convert32(uint32_t num)
{
    uint8_t sign, exponent;
    uint32_t mantisa;
    double result = 0;

    sign = num >> 31;
    exponent = (num >> 23 & 0xFF) - 127;
    mantisa = num & 0x7FFFFF;

    result = (mantisa *pow(2, -23)) + 1;
    result *= pow(1, sign) * pow(2, exponent);

    return result;
}	

double convert64(uint64_t num)
{
    uint8_t sign;
    uint16_t exponent;
    uint64_t mantisa;
    double result = 0;

    sign = num >> 63;
    exponent = (num >> 52 & 0x7FF) - 1023;
    mantisa = num & 0xFFFFFFFFFFFFF;
    result = (mantisa *pow(2, -52)) + 1;
    result *= pow(1, sign) * pow(2, exponent);
    return result;
}

uint32_t swap24(uint8_t *val)
{
	return (uint32_t) val[2] << 0 | (uint32_t) val[1] <<  8 | 
		   (uint32_t) val[0] << 16;
}

uint32_t swap32(uint32_t val)
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | (val >> 16);
}

uint64_t swap64(uint64_t val)
{
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL ) | ((val >> 8) & 
		0x00FF00FF00FF00FFULL );
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL ) | ((val >> 16) & 
		0x0000FFFF0000FFFFULL );
    return (val << 32) | (val >> 32);
}

double binary32(uint32_t i)
{
	union
	{
		uint32_t decimal_value;
		float float_value;
	}
	union_f;
	union_f.decimal_value = bswap_32(i);
	return union_f.float_value;
}

double binary64(uint64_t i)
{
	union
	{
		uint64_t decimal_value;
		float float_value;
	}
	union_f;
	union_f.decimal_value = bswap_64(i);
	return union_f.float_value;
}

