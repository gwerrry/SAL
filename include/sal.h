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
#define SLllong  int64_t
#define Slullong uint64_t
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
    SLuchar chunkFormat[4];
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

    SLshort*  waveformData;
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

Slullong sl_flip_endian_ullong(Slullong ull);
SLllong sl_flip_endian_llong(SLllong ll);

SLuint sl_uint_as_big_endian(SLuchar* buf);
SLuint sl_uint_as_little_endian(SLuchar* buf);
SLushort sl_ushort_as_big_endian(SLuchar* buf);
SLushort sl_ushort_as_little_endian(SLuchar* buf);


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

    buf->dataChunk.waveformData = (SLshort*)malloc(1);

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

    //read and validate chunkID
    fread(buf->descriptorChunk.chunkID, 1, 4, file);

    for (int i = 0; i < 4; ++i) {
        if(buf->descriptorChunk.chunkID[i] != riffID_bytes[i]) {
            ret = SL_INVALID_WAVE_FORMAT;
            goto bufCleanup;
        }
    }
    printf("DEBUG: Chunk ID: %c %c %c %c\n", buf->descriptorChunk.chunkID[0], buf->descriptorChunk.chunkID[1], buf->descriptorChunk.chunkID[2], buf->descriptorChunk.chunkID[3]);

    //read and validate chunkSize
    fread(buffer4, 1, 4, file);
    buf->descriptorChunk.chunkSize = sl_uint_as_little_endian(buffer4);
    if (buf->descriptorChunk.chunkSize == 0) {
        ret = SL_INVALID_WAVE_FORMAT;
        goto bufCleanup;
    }
    printf("DEBUG: Chunk Size: %d", buf->descriptorChunk.chunkSize);

    fread(buf->descriptorChunk.chunkFormat, 1, 4, file);
    for (int i = 0; i < 4; ++i) {
        if(buf->descriptorChunk.chunkFormat[i] != waveID_bytes[i]) {
            ret = SL_INVALID_WAVE_FORMAT;
            goto bufCleanup;
        }
    }
    printf("DEBUG: Chunk Format: %c %c %c %c\n", buf->descriptorChunk.chunkFormat[0], buf->descriptorChunk.chunkFormat[1], buf->descriptorChunk.chunkFormat[2], buf->descriptorChunk.chunkFormat[3]);

    fread(buf->formatChunk.subChunk1Id, 1, 4, file);
    for (int i = 0; i < 4; ++i) {
        if(buf->formatChunk.subChunk1Id[i] != fmtID_bytes[i]) {
            ret = SL_INVALID_WAVE_FORMAT;
            goto bufCleanup;
        }
    }
    printf("DEBUG: Format Chunk ID: %c %c %c %c\n", buf->formatChunk.subChunk1Id[0], buf->formatChunk.subChunk1Id[1], buf->formatChunk.subChunk1Id[2], buf->formatChunk.subChunk1Id[3]);

    fread(buffer4, 1, 4, file);
    buf->formatChunk.subChunk1Size = sl_uint_as_little_endian(buffer4);
    if (buf->formatChunk.subChunk1Size == 0) {
        ret = SL_INVALID_WAVE_FORMAT;
        goto bufCleanup;
    }
    printf("DEBUG: Format Chunk Size: %d\n", buf->formatChunk.subChunk1Size);

    fread(buffer2, 1, 2, file);
    buf->formatChunk.audioFormat = sl_ushort_as_little_endian(buffer2);
    printf("DEBUG: Format Chunk Audio Format: %d\n", buf->formatChunk.audioFormat);
    if (buf->formatChunk.audioFormat != 1) {
        ret = SL_INVALID_WAVE_FORMAT;
        goto bufCleanup;
    }

    fread(buffer2, 1, 2, file);
    buf->formatChunk.numChannels = sl_ushort_as_little_endian(buffer2);
    if (buf->formatChunk.numChannels == 0) {
        ret = SL_INVALID_WAVE_FORMAT;
        goto bufCleanup;
    }
    printf("DEBUG: Format Chunk Num Channels: %d\n", buf->formatChunk.numChannels);

    fread(buffer4, 1, 4, file);
    buf->formatChunk.sampleRate = sl_uint_as_little_endian(buffer4);
    if (buf->formatChunk.sampleRate == 0) {
        ret = SL_INVALID_WAVE_FORMAT;
        goto bufCleanup;
    }
    printf("DEBUG: Format Chunk Sample Rate: %d\n", buf->formatChunk.sampleRate);

    fread(buffer4, 1, 4, file);
    buf->formatChunk.byteRate = sl_uint_as_little_endian(buffer4);
    if (buf->formatChunk.byteRate == 0) {
        ret = SL_INVALID_WAVE_FORMAT;
        goto bufCleanup;
    }
    printf("DEBUG: Format Chunk Byte Rate: %d\n", buf->formatChunk.byteRate);


    fread(buffer2, 1, 2, file);
    buf->formatChunk.blockAlign = sl_ushort_as_little_endian(buffer2);
    if (buf->formatChunk.blockAlign == 0) {
        ret = SL_INVALID_WAVE_FORMAT;
        goto bufCleanup;
    }
    printf("DEBUG: Format Chunk Block Align: %d\n", buf->formatChunk.blockAlign);

    fread(buffer2, 1, 2, file);
    buf->formatChunk.bitsPerSample = sl_ushort_as_little_endian(buffer2);
    if (buf->formatChunk.bitsPerSample == 0) {
        ret = SL_INVALID_WAVE_FORMAT;
        goto bufCleanup;
    }
    printf("DEBUG: Format Chunk Bits Per Sample: %d\n", buf->formatChunk.bitsPerSample);
    //printf("Current Bytes: %x %x %x %x\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);


    //keep reading until find data
    size_t bytesRead = fread(buf->dataChunk.subChunk2Id, 1, 4, file);
    int found = 0;

    while(bytesRead == 4) {
        if (feof(file)) {
            ret = SL_INVALID_WAVE_FORMAT;
            goto bufCleanup;
        } else if (ferror(file)) {
            ret = SL_INVALID_WAVE_FORMAT;
            goto bufCleanup;
        }

        SLuint amt = 0;
        for (int i = 0; i < 4; ++i) {
            if(buf->dataChunk.subChunk2Id[i] == dataID_bytes[i]) {
                amt++;
            }
        }

        if(amt == 4) {
            found = 1;
            break;
        }

        fread(buf->dataChunk.subChunk2Id, 1, 4, file);
        amt = sl_uint_as_little_endian(buf->dataChunk.subChunk2Id);

        fseek(file, amt, SEEK_CUR);
        fread(buf->dataChunk.subChunk2Id, 1, 4, file);
    }

    if(!found) {
        ret = SL_INVALID_WAVE_FORMAT;
        goto bufCleanup;
    }

    printf("DEBUG: Format Chunk ID: %c %c %c %c\n", buf->dataChunk.subChunk2Id[0], buf->dataChunk.subChunk2Id[1], buf->dataChunk.subChunk2Id[2], buf->dataChunk.subChunk2Id[3]);

    fread(buffer4, 1, 4, file);
    buf->dataChunk.subChunk2Size = sl_uint_as_little_endian(buffer4);
    if (buf->dataChunk.subChunk2Size == 0) {
        ret = SL_INVALID_WAVE_FORMAT;
        goto bufCleanup;
    }
    printf("DEBUG: Data Chunk Size: %d\n", buf->dataChunk.subChunk2Size);


    Slullong num_samples = (8 * buf->dataChunk.subChunk2Size) / (buf->formatChunk.numChannels * buf->formatChunk.bitsPerSample);
    Slullong size_of_each_sample = (buf->formatChunk.numChannels * buf->formatChunk.bitsPerSample) / 8;

    free(buf->dataChunk.waveformData);
    buf->dataChunk.waveformData = malloc(num_samples * sizeof(SLshort));
    fread(buf->dataChunk.waveformData, size_of_each_sample, num_samples, file);

    for (int i = 0; i < num_samples; i++) {
        buf->dataChunk.waveformData[i] = sl_flip_endian_short(buf->dataChunk.waveformData[i]);
    }

    goto fileCleanup;

    bufCleanup:
        if(buf) free(buf->dataChunk.waveformData);
        free(buf);
    fileCleanup:
        fclose(file);
        free(buffer4);
        free(buffer2);
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
    strncpy(sub, str + str_len - suffix_len, suffix_len);
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

Slullong sl_flip_endian_ullong(Slullong ull) {
    return (Slullong) (((ull >> 56) & 0x00000000000000ff) |
                      ((ull >> 40) & 0x000000000000ff00) |
                      ((ull >> 24) & 0x0000000000ff0000) |
                      ((ull >> 8)  & 0x00000000ff000000) |
                      ((ull << 8)  & 0x000000ff00000000) |
                      ((ull << 24) & 0x0000ff0000000000) |
                      ((ull << 40) & 0x00ff000000000000) |
                      ((ull << 56) & 0xff00000000000000));
}

SLllong sl_flip_endian_llong(SLllong ll) {
    return (SLllong) (((ll >> 56) & 0x00000000000000ff) |
            ((ll >> 40) & 0x000000000000ff00) |
            ((ll >> 24) & 0x0000000000ff0000) |
            ((ll >> 8)  & 0x00000000ff000000) |
            ((ll << 8)  & 0x000000ff00000000) |
            ((ll << 24) & 0x0000ff0000000000) |
            ((ll << 40) & 0x00ff000000000000) |
            ((ll << 56) & 0xff00000000000000));
}

SLuint sl_uint_as_big_endian(SLuchar* buf) {
    return (SLint)((buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3]);
}

SLuint sl_uint_as_little_endian(SLuchar* buf) {
    return (SLint)(buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24));
}

SLushort sl_ushort_as_big_endian(SLuchar* buf) {
    return (SLushort)((buf[0] << 8) | buf[1]);
}

SLushort sl_ushort_as_little_endian(SLuchar* buf) {
    return (SLushort)(buf[0] | (buf[1] << 8));
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //SAL_SAL_H
