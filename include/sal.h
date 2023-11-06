#ifndef SAL_SAL_H
#define SAL_SAL_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdio.h>
#include <stdint.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>

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
#define SLstr const char*
/**
 * Very important. Tells us if the system is Little Endian or Big Endian.
 */
static SLbool leSys;

/**
 * ERROR/RETURN CODE DEFINITIONS
*/

#define SL_SUCCESS 69420
#define SL_PARSE_FAIL 34343
#define SL_INVALID_VALUE 57843
#define SL_FILE_ERROR 24354
#define SL_INVALID_WAVE_FORMAT 59023
#define SL_FAIL 66666
#define SL_INVALID_FILE 88920

#define BIG_ENDIAN 1
#define LITTLE_ENDIAN 0

typedef struct {
    SLuchar chunkID[4];
    SLuint chunkSize;
    SLuchar FORMAT[4];
} SL_WAV_DESCRIPTOR;

typedef struct {
    SLuchar subChunk1Id[4];
    SLuint subChunk1Size;
    SLushort audioFormat;
    SLushort numChannels;
    SLuint sampleRate;
    SLuint byteRate;
    SLushort blockAlign;
    SLushort bitsPerSample;
} SL_WAV_FMT;

typedef struct {
    SLuchar subChunk2Id[4];
    SLuint subChunk2Size;

    SLuchar*  waveformData;
} SL_WAV_DATA;

typedef struct WAV_FILE_DATA {
    //Chunk descriptor
    SL_WAV_DESCRIPTOR descriptorChunk;

    //FMT sub-chunk
    SL_WAV_FMT formatChunk;

    //data sub-chunk
    SL_WAV_DATA dataChunk;
} SL_WAV_FILE;


/**
 * Functions
 */
SLenum sl_init(void);

SLenum sl_parse_wave_file(SLstr path, SL_WAV_FILE* buf);

SLenum ends_with(SLstr str, SLstr suffix);
SLshort  sl_flip_endian_short(SLshort s);
SLushort sl_flip_endian_ushort(SLushort us);
SLint    sl_flip_endian_int(SLint i);
SLuint   sl_flip_endian_uint(SLuint ui);
SLenum   sl_flip_endian_enum(SLenum e);
SLfloat  sl_flip_endian_float(SLfloat f);
SLdouble sl_flip_endian_double(SLdouble d);
SLuint sl_uint_as_big_endian(SLuchar* buf, SLuint size);
SLuint sl_uint_as_little_endian(SLuchar* buf, SLuint size);



// user must manage the memory of path. note this in docs
SLenum sl_parse_wave_file(SLstr path, SL_WAV_FILE* buf) {
    SLenum ret = SL_SUCCESS;

    if (!path) {
        ret = SL_INVALID_VALUE;
        goto exit;
    }

    if (ends_with(path, ".wav") == SL_FAIL) {
        ret = SL_PARSE_FAIL;
        goto exit;
    }

    // try to open file
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        ret = SL_FILE_ERROR;
        goto exit;
    }

    // Allocate bufs
    SLuchar* buffer4 = (SLuchar*)calloc(4, sizeof(SLuchar));
    SLuchar* buffer2 = (SLuchar*)calloc(2, sizeof(SLuchar));

    buf = (SL_WAV_FILE*)malloc(sizeof(SL_WAV_FILE));

    //id put this in the lower one, but we have to verify because of the next call
    if(!buf) {
        ret = SL_FAIL;
        goto bufCleanup;
    }

    buf->dataChunk.waveformData = (SLuchar*)malloc(1);

    if (!buffer4 || !buffer2 || !buf->dataChunk.waveformData) {
       ret = SL_FAIL;
       goto bufCleanup;
    }

    //
    // Constants here for some byte names and buffers.
    // RIFF = 0x52494646 (big endian form)
    // RIFX = 0x52494658 (big endian form)
    //
    // const SLuchar rifxID_bytes[4] = {0x52, 0x49, 0x46, 0x58}; MIGHT add support for rifx later. depends on if i feel like it or not.

    const SLuchar riffID_bytes[4] = {0x52, 0x49, 0x46, 0x46};
    const SLuchar waveID_bytes[4] = {0x57, 0x41, 0x56, 0x45};
    const SLuchar fmtID_bytes [4] = {0x66, 0x6d, 0x74, 0x20};
    const SLuchar dataID_bytes[4] = {0x64, 0x61, 0x74, 0x61};

    fread(buf->descriptorChunk.chunkID, 1, 4, file);


    for (int i = 0; i < 4; ++i) {
        if(buf->descriptorChunk.chunkID[i] != riffID_bytes[i]) {
            ret = SL_INVALID_WAVE_FORMAT;
            goto bufCleanup;
        }
    }

    fread(buffer4, 1, 4, file);
    printf("As big: %d", sl_uint_as_big_endian(buffer4, 4));
    printf("As little: %d", sl_uint_as_big_endian(buffer4, 4));

    goto exit;

    bufCleanup:
        if(buf) free(buf->dataChunk.waveformData);
        free(buf);
        free(buffer4);
        free(buffer2);
    fileCleanup:
        fclose(file);
    exit:
        return ret;
}

SLenum sl_init(void) {
    // check endian status
    SLint n = 1;
    leSys = *(SLchar *)&n == 1;

    return SL_SUCCESS;
}

SLenum ends_with(SLstr str, SLstr suffix) {
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    if (suffix_len > str_len) {
        return SL_FAIL;
    }

    char sub[suffix_len];
    strncpy(sub, str + str_len - suffix_len - 1, suffix_len);
    sub[suffix_len] = '\0';

    return strcmp(suffix, sub) == 0 ? SL_SUCCESS : SL_FAIL;
}

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

SLuint sl_uint_as_big_endian(SLuchar* buf, SLuint size) {
    return (SLint)((buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3]);
}

SLuint sl_uint_as_little_endian(SLuchar* buf, SLuint size) {
    return (SLint)(buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24));
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //SAL_SAL_H
