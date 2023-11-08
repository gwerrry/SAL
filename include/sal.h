#ifndef SAL_SAL_H
#define SAL_SAL_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/**
 * Include platform specific headers
 */
#ifdef __WIN32
    #include <combaseapi.h>
    #include <mmdeviceapi.h>
    #include <audioclient.h>
    #include <audiopolicy.h>
#elif defined(__linux__)

#elif defined(__APPLE__) || defined(__MACH__)

#endif

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
static SLbool sysEndianness;

/**
 * ERROR/RETURN CODE DEFINITIONS
*/
#define SL_SUCCESS 69420
#define SL_PARSE_FAIL 34343
#define SL_INVALID_VALUE 57843
#define SL_FILE_ERROR 24354

#define SL_INVALID_WAVE_FORMAT 59023

//NEW ERROR CODES
#define SL_INVALID_CHUNK_DESCRIPTOR_ID 40000
#define SL_INVALID_CHUNK_DESCRIPTOR_SIZE 41111
#define SL_INVALID_CHUNK_DESCRIPTOR_FORMAT 42222

#define SL_INVALID_CHUNK_FMT_ID 50000
#define SL_INVALID_CHUNK_FMT_SIZE 53010
#define SL_INVALID_CHUNK_FMT_AUDIO_FORMAT 50190
#define SL_INVALID_CHUNK_FMT_CHANNELS 51600
#define SL_INVALID_CHUNK_FMT_SAMPLE_RATE 55021
#define SL_INVALID_CHUNK_FMT_BYTE_RATE 50205
#define SL_INVALID_CHUNK_FMT_BLOCK_ALIGN 52008
#define SL_INVALID_CHUNK_FMT_BITS_PER_SAMPLE 50321

#define SL_INVALID_CHUNK_DATA_ID 60000
#define SL_INVALID_CHUNK_DATA_SIZE 60070
#define SL_INVALID_CHUNK_DATA_DATA 60600




#define SL_FAIL 66666

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
    SLushort*  waveformData;
} SL_WAV_DATA;

typedef struct WAV_FILE_DATA {
    SL_WAV_DESCRIPTOR descriptorChunk;
    SL_WAV_FMT formatChunk;
    SL_WAV_DATA dataChunk;
} SL_WAV_FILE;

/**
 * Functions
 */
SLenum sl_init(void);
SLenum sl_cleanup(void);

SLenum sl_parse_wave_file(SLstr path, SL_WAV_FILE** wavBuf);
void sl_free_wave_file(SL_WAV_FILE** buf);
SLenum ends_with(SLstr str, SLstr suffix);

SLshort  sl_flip_endian_short(SLshort s);
SLushort sl_flip_endian_ushort(SLushort us);
SLint    sl_flip_endian_int(SLint i);
SLuint   sl_flip_endian_uint(SLuint ui);
SLenum   sl_flip_endian_enum(SLenum e);
SLfloat  sl_flip_endian_float(SLfloat f);
SLdouble sl_flip_endian_double(SLdouble d);
Slullong sl_flip_endian_ullong(Slullong ull);
SLllong  sl_flip_endian_llong(SLllong ll);
SLushort sl_buf_to_native_ushort(SLuchar* buf, SLuint bufLen);
SLuint sl_buf_to_native_uint(SLuchar* buf, SLuint bufLen);

SLuint   sl_uint_as_big_endian(SLuchar* buf);
SLuint   sl_uint_as_little_endian(SLuchar* buf);
SLushort sl_ushort_as_big_endian(SLuchar* buf);
SLushort sl_ushort_as_little_endian(SLuchar* buf);

SLbool   sl_uint_endianness(SLuint ui);
SLbool   sl_ushort_endianness(SLushort us);
SLuint   sl_uint_as_native_endianness(SLuint ui);
SLushort sl_ushort_as_native_endianness(SLushort us);

// user must manage the memory of path. note this in docs
SLenum sl_parse_wave_file(SLstr path, SL_WAV_FILE** wavBuf) {
    SLenum ret = SL_SUCCESS;

    // this is so everything looks nice i dont feel like doing (*wavBuf) everytime i want to do anything
    SL_WAV_FILE* buf = NULL;
    *wavBuf = NULL;

    //ensure pointers are good were just going to assume the user allocated stuff right
    if (!path || !wavBuf) {
        ret = SL_INVALID_VALUE;
        goto exit;
    }

    //ensures that this file we are reading is a wave file-or at least ends with it
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

    // Allocate buf
    buf = (SL_WAV_FILE*)malloc(sizeof(SL_WAV_FILE));
    if(!buf) {
        ret = SL_FAIL;
        goto bufCleanup;
    }

    buf->dataChunk.waveformData = NULL;

    //
    // Constants here for some byte names and buffers.
    //
    SLuchar buffer4[4];
    SLuchar buffer2[2];
    const SLuchar riffID_bytes[4] = {0x52, 0x49, 0x46, 0x46};
    const SLuchar waveID_bytes[4] = {0x57, 0x41, 0x56, 0x45};
    const SLuchar fmtID_bytes [4] = {0x66, 0x6d, 0x74, 0x20};
    const SLuchar dataID_bytes[4] = {0x64, 0x61, 0x74, 0x61};

    Slullong bytesRead = 0;

    //read and validate chunkID
    bytesRead = fread(buf->descriptorChunk.chunkID, 1, 4, file);
    if (bytesRead != 4) goto bufCleanupIWF;

    for (int i = 0; i < 4; ++i) {
        if(buf->descriptorChunk.chunkID[i] != riffID_bytes[i]) goto bufCleanupIWF;
    }

    //read and validate chunk size
    bytesRead = fread(buffer4, 1, 4, file);
    if (bytesRead != 4) goto bufCleanupIWF;
    buf->descriptorChunk.chunkSize = sl_buf_to_native_uint(buffer4, 4);
    if (buf->descriptorChunk.chunkSize == 0) goto bufCleanupIWF;


    //read and validate wave format
    bytesRead = fread(buf->descriptorChunk.chunkFormat, 1, 4, file);
    if (bytesRead != 4) goto bufCleanupIWF;

    for (int i = 0; i < 4; ++i) {
        if(buf->descriptorChunk.chunkFormat[i] != waveID_bytes[i]) goto bufCleanupIWF;
    }

    //read and validate fmt chunk id
    fread(buf->formatChunk.subChunk1Id, 1, 4, file);
    if (bytesRead != 4) goto bufCleanupIWF;

    for (int i = 0; i < 4; ++i) {
        if(buf->formatChunk.subChunk1Id[i] != fmtID_bytes[i]) {
            goto bufCleanupIWF;
        }
    }

    //read and validate fmt chunk size
    fread(buffer4, 1, 4, file);
    if (bytesRead != 4) goto bufCleanupIWF;
    buf->formatChunk.subChunk1Size = sl_buf_to_native_uint(buffer4, 4);
    if (buf->formatChunk.subChunk1Size == 0) goto bufCleanupIWF;

    //read and validate audio format. ensure PCM
    fread(buffer2, 1, 2, file);
    if (bytesRead != 2) goto bufCleanupIWF;
    buf->formatChunk.audioFormat = sl_buf_to_native_ushort(buffer2, 2);
    if (buf->formatChunk.audioFormat != 1) goto bufCleanupIWF;


    //read and validate num channels.
    fread(buffer2, 1, 2, file);
    if (bytesRead != 2) goto bufCleanupIWF;
    buf->formatChunk.numChannels = sl_buf_to_native_ushort(buffer2, 2);
    if (buf->formatChunk.numChannels == 0) goto bufCleanupIWF;

    //read and validate sample rate
    fread(buffer4, 1, 4, file);
    if (bytesRead != 4) goto bufCleanupIWF;
    buf->formatChunk.sampleRate = sl_buf_to_native_uint(buffer4, 4);
    if (buf->formatChunk.sampleRate == 0) goto bufCleanupIWF;


    //read and validate byte rate
    fread(buffer4, 1, 4, file);
    if (bytesRead != 4) goto bufCleanupIWF;
    buf->formatChunk.byteRate = sl_buf_to_native_uint(buffer4, 4);
    if (buf->formatChunk.byteRate == 0) goto bufCleanupIWF;

    //read and validate block align
    fread(buffer2, 1, 2, file);
    if (bytesRead != 2) goto bufCleanupIWF;
    buf->formatChunk.blockAlign = sl_buf_to_native_ushort(buffer2, 2);
    if (buf->formatChunk.blockAlign == 0) goto bufCleanupIWF;

    //read and validate bits per sample
    fread(buffer2, 1, 2, file);
    if (bytesRead != 2) goto bufCleanupIWF;
    buf->formatChunk.bitsPerSample = sl_buf_to_native_ushort(buffer2, 2);
    if (buf->formatChunk.bitsPerSample == 0) goto bufCleanupIWF;


    //keep reading until find data
    bytesRead = fread(buf->dataChunk.subChunk2Id, 1, 4, file);
    int found = 0;

    while(bytesRead == 4) {
        // check if everything is valid
        if (feof(file) || ferror(file)) goto bufCleanupIWF;

        // check if bytes match omg
        SLuint matchingBytes = 0;
        for (int i = 0; i < 4; ++i) {
            if(buf->dataChunk.subChunk2Id[i] == dataID_bytes[i]) {
                matchingBytes++;
            }
        }

        // get size of that chunk :eyes:
        fread(buffer4, 1, 4, file);
        if (bytesRead != 4) goto bufCleanupIWF;
        buf->dataChunk.subChunk2Size = sl_buf_to_native_uint(buffer4, 4);
        if (buf->dataChunk.subChunk2Size == 0) goto bufCleanupIWF;

        // check if found that data :smirk:
        if(matchingBytes == 4) {
            found = 1;
            break;
        }

        // skip chunk and read next ID
        fseek(file, buf->dataChunk.subChunk2Size, SEEK_CUR);
        fread(buf->dataChunk.subChunk2Id, 1, 4, file);
    }


    if(!found) goto bufCleanupIWF;


    //validate samples
    Slullong num_samples = (8 * buf->dataChunk.subChunk2Size) / (buf->formatChunk.numChannels * buf->formatChunk.bitsPerSample);
    Slullong size_of_each_sample = (buf->formatChunk.numChannels * buf->formatChunk.bitsPerSample) / 8;
    if (num_samples == 0 || size_of_each_sample == 0) goto bufCleanupIWF;


    //read data
    free(buf->dataChunk.waveformData);
    buf->dataChunk.waveformData = malloc(num_samples * sizeof(SLshort));
    if (!buf->dataChunk.waveformData) {
        ret = SL_FAIL;
        goto bufCleanup;
    }

    fread(buf->dataChunk.waveformData, size_of_each_sample, num_samples, file);
    if (bytesRead != num_samples) goto bufCleanupIWF;
    //ensure all the data is something the system recognizes
    if(sl_ushort_endianness(buf->dataChunk.waveformData[0]) != sysEndianness) {
        for (int i = 0; i < num_samples; i++) {
            buf->dataChunk.waveformData[i] = sl_flip_endian_ushort(buf->dataChunk.waveformData[i]);
        }
    }

    *wavBuf = buf;

    goto fileCleanup;

    bufCleanupIWF:
        ret = SL_INVALID_WAVE_FORMAT;
    bufCleanup:
        if(buf) free(buf->dataChunk.waveformData);
        buf->dataChunk.waveformData = NULL;
        free(buf);
        buf = NULL;
        *wavBuf = NULL;
    fileCleanup:
        fclose(file);
    exit:
        return ret;
}

void sl_free_wave_file(SL_WAV_FILE** buf) {
    if(buf && *buf) {
        free((*buf)->dataChunk.waveformData);
        (*buf)->dataChunk.waveformData = NULL;
        free(*buf);
        *buf = NULL;
    }
}

SLenum sl_init(void) {
    //quickly check the endianness of the system
    int n = 1;
    sysEndianness = *(char*)&n == 1 ? LITTLE_ENDIAN : BIG_ENDIAN;

    return SL_SUCCESS;
}

SLenum sl_cleanup(void) {

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

SLushort sl_buf_to_native_ushort(SLuchar* buf, SLuint bufLen) {
    //who needs comments, am i right?
    if(!buf || bufLen < 2) return 0;

    SLuint value = 0;
    if(sysEndianness == LITTLE_ENDIAN) {
        value = buf[0] | (buf[1] << 8);
    } else {
        value = buf[1] | (buf[0] << 8);
    }
    return value;
}

SLuint sl_buf_to_native_uint(SLuchar* buf, SLuint bufLen) {
    //who needs comments, am i right?
    if(!buf || bufLen < 4) return 0;

    SLuint value = 0;
    if(sysEndianness == LITTLE_ENDIAN) {
        if((unsigned char)buf[0] < (unsigned char)buf[3]) value = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
        else value = buf[3] | (buf[2] << 8) | (buf[1] << 16) | (buf[0] << 24);
    } else {
        if((unsigned char)buf[0] < (unsigned char)buf[3]) value = buf[3] | (buf[2] << 8) | (buf[1] << 16) | (buf[0] << 24);
        else value = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
    }
    return value;
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

SLbool sl_uint_endianness(SLuint ui) {
    char* c = (char*)&ui;
    return *c > *(c + sizeof(SLuint) - 1) ? BIG_ENDIAN : LITTLE_ENDIAN;
}

SLbool sl_ushort_endianness(SLushort us) {
    char* c = (char*)&us;
    return *c > *(c + sizeof(SLushort) - 1) ? BIG_ENDIAN : LITTLE_ENDIAN;
}

SLuint sl_uint_as_native_endianness(SLuint ui) {
    return sl_uint_endianness(ui) == sysEndianness ? ui : sl_flip_endian_uint(ui);
}

SLushort sl_ushort_as_native_endianness(SLushort us) {
    return sl_ushort_endianness(us) == sysEndianness ? us : sl_flip_endian_ushort(us);
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //SAL_SAL_H
