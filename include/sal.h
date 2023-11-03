#ifndef SAL_SAL_H
#define SAL_SAL_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdio.h>
#include <stdint.h>
#include <memory.h>

/**
 * Type definitions
 */
#define SLbool   int8_t
#define SLchar   int8_t
#define SLuchar  uint8_t
#define SLshort  int16_t
#define SLushort uint16_t
#define SLint    int32_t
#define SLuint   uint32_t
#define SLenum   uint32_t
#define SLfloat float
#define SLdouble double

/**
 * Very important. Tells us if the system is Little Endian or Big Endian.
 */
static SLbool leSys;

/**
 * ERROR/RETURN CODE DEFINITIONS
*/

#define SL_SUCCESS 69420
#define BIG_ENDIAN 1
#define LITTLE_ENDIAN 0

/**
 * Functions
 */
SLenum sl_init(void);

SLshort  sl_flip_endian_short(SLshort s);
SLushort sl_flip_endian_ushort(SLushort us);
SLint    sl_flip_endian_int(SLint i);
SLuint   sl_flip_endian_uint(SLuint ui);
SLenum   sl_flip_endian_enum(SLenum e);
SLfloat  sl_flip_endian_float(SLfloat f);
SLdouble sl_flip_endian_double(SLdouble d);


SLshort sl_flip_endian_short(SLshort s) {
    // simple bitwise stuff to flip the endianness
    return (s >> 8) | (s << 8);
}

SLushort sl_flip_endian_ushort(SLushort us) {
    // simple bitwise stuff to flip the endianness
    return (us >> 8) | (us << 8);
}

SLint sl_flip_endian_int(SLint i) {
    // simple bitwise stuff to flip the endianness
        return ((i >> 24) & 0xff) |
               ((i << 8) & 0xff0000) |
               ((i >> 8) & 0xff00) |
               ((i << 24) & 0xff000000);
}

SLuint sl_flip_endian_uint(SLuint ui) {
    // simple bitwise stuff to flip the endianness
    return ((ui >> 24) & 0xff) |
           ((ui << 8) & 0xff0000) |
           ((ui >> 8) & 0xff00) |
           ((ui << 24) & 0xff000000);
}

SLenum sl_flip_endian_enum(SLenum e) {
    return sl_flip_endian_uint(e);
}

SLfloat sl_flip_endian_float(SLfloat f) {
    // i love c because of this right here
    unsigned long num_int;
    memcpy(&num_int, &f, sizeof(float));

    // simple bitwise stuff to flip the endianness
    unsigned long swapped_int = ((num_int >> 24) & 0xff) |
                                ((num_int << 8) & 0xff0000) |
                                ((num_int >> 8) & 0xff00) |
                                ((num_int << 24) & 0xff000000);
    float swapped;
    memcpy(&swapped, &swapped_int, sizeof(float));
    return swapped;
}

SLdouble sl_flip_endian_double(SLdouble d) {
    // i love c because of this right here
    uint64_t num_int;
    memcpy(&num_int, &d, sizeof(double));

    // simple bitwise stuff to flip the endianness
    uint64_t swapped_int = ((num_int >> 56) & 0x00000000000000ff) |
                       ((num_int >> 40) & 0x000000000000ff00) |
                       ((num_int >> 24) & 0x0000000000ff0000) |
                       ((num_int >> 8)  & 0x00000000ff000000) |
                       ((num_int << 8)  & 0x000000ff00000000) |
                       ((num_int << 24) & 0x0000ff0000000000) |
                       ((num_int << 40) & 0x00ff000000000000) |
                       ((num_int << 56) & 0xff00000000000000);

    double swapped;
    memcpy(&swapped, &swapped_int, sizeof(double));
    return swapped;
}

SLenum sl_init(void) {
        // check endian status
        SLint n = 1;
        leSys = *(SLchar *)&n == 1;

    return SL_SUCCESS;
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //SAL_SAL_H
