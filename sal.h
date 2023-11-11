#ifndef SAL_SAL_H
#define SAL_SAL_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
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
#define SLullong uint64_t
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

#define SL_UNSIGNED_8PCM 1
#define SL_SIGNED_16PCM 2
#define SL_SIGNED_24PCM 3
#define SL_SIGNED_32PCM 4
#define SL_FLOAT_32PCM 5
#define SL_FLOAT_64PCM 6

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
    SLushort extensionSize;
} SL_WAV_FMT;

typedef struct {
    SLuchar subChunk2Id[4];
    SLuint subChunk2Size;
    SLenum pcmType;
    SLuchar*  waveformData;
    SLchar*  waveformData_signed;
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

SLuint   sl_flip_endian_uint(SLuint ui);
SLushort sl_buf_to_native_ushort(SLuchar* buf, SLuint bufLen);
SLuint sl_buf_to_native_uint(SLuchar* buf, SLuint bufLen);

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
    FILE* file = fopen(path, "rb");
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
    buf->dataChunk.waveformData_signed = NULL;
    //
    // Constants here for some byte names and buffers.
    //
    SLuchar buffer4[4];
    SLuchar buffer2[2];
    const SLuchar riffID_bytes[4] = {0x52, 0x49, 0x46, 0x46};
    const SLuchar waveID_bytes[4] = {0x57, 0x41, 0x56, 0x45};
    const SLuchar fmtID_bytes [4] = {0x66, 0x6d, 0x74, 0x20};
    const SLuchar dataID_bytes[4] = {0x64, 0x61, 0x74, 0x61};

    SLullong bytesRead = 0;

    //read and validate chunkID
    bytesRead = fread(buf->descriptorChunk.chunkID, 1, 4, file);
    if (bytesRead != 4) {
        ret = SL_INVALID_CHUNK_DESCRIPTOR_ID;
        goto bufCleanup;
    }

    for (int i = 0; i < 4; ++i) {
        if(buf->descriptorChunk.chunkID[i] != riffID_bytes[i]) {
            ret = SL_INVALID_CHUNK_DESCRIPTOR_ID;
            goto bufCleanup;
        }
    }

    //read and validate chunk size
    bytesRead = fread(buffer4, 1, 4, file);
    if (bytesRead != 4) {
        ret = SL_INVALID_CHUNK_DESCRIPTOR_SIZE;
        goto bufCleanup;
    }
    buf->descriptorChunk.chunkSize = sl_buf_to_native_uint(buffer4, 4);
    if (buf->descriptorChunk.chunkSize == 0) {
        ret = SL_INVALID_CHUNK_DESCRIPTOR_SIZE;
        goto bufCleanup;
    }

    //read and validate wave format
    bytesRead = fread(buf->descriptorChunk.chunkFormat, 1, 4, file);
    if (bytesRead != 4) {
        ret = SL_INVALID_CHUNK_DESCRIPTOR_FORMAT;
        goto bufCleanup;
    }

    for (int i = 0; i < 4; ++i) {
        if(buf->descriptorChunk.chunkFormat[i] != waveID_bytes[i]) {
            ret = SL_INVALID_CHUNK_DESCRIPTOR_FORMAT;
            goto bufCleanup;
        }
    }

    //read and validate fmt chunk id
    bytesRead = fread(buf->formatChunk.subChunk1Id, 1, 4, file);
    if (bytesRead != 4) {
        ret = SL_INVALID_CHUNK_FMT_ID;
        goto bufCleanup;
    }

    for (int i = 0; i < 4; ++i) {
        if(buf->formatChunk.subChunk1Id[i] != fmtID_bytes[i]) {
            ret = SL_INVALID_CHUNK_FMT_ID;
            goto bufCleanup;
        }
    }

    //read and validate fmt chunk size
    bytesRead = fread(buffer4, 1, 4, file);
    if (bytesRead != 4) {
        ret = SL_INVALID_CHUNK_FMT_SIZE;
        goto bufCleanup;
    }
    buf->formatChunk.subChunk1Size = sl_buf_to_native_uint(buffer4, 4);
    if (buf->formatChunk.subChunk1Size == 0) {
        ret = SL_INVALID_CHUNK_FMT_SIZE;
        goto bufCleanup;
    }

    //read only. audio format is verified later when parsing bits per sample
    bytesRead = fread(buffer2, 1, 2, file);
    if (bytesRead != 2) {
        ret = SL_INVALID_CHUNK_FMT_AUDIO_FORMAT;
        goto bufCleanup;
    }
    buf->formatChunk.audioFormat = sl_buf_to_native_ushort(buffer2, 2);

    //read and validate num channels.
    bytesRead = fread(buffer2, 1, 2, file);
    if (bytesRead != 2) {
        ret = SL_INVALID_CHUNK_FMT_CHANNELS;
        goto bufCleanup;
    }
    buf->formatChunk.numChannels = sl_buf_to_native_ushort(buffer2, 2);
    if (buf->formatChunk.numChannels == 0) {
        ret = SL_INVALID_CHUNK_FMT_CHANNELS;
        goto bufCleanup;
    }

    //read and validate sample rate
    bytesRead = fread(buffer4, 1, 4, file);
    if (bytesRead != 4) {
        ret = SL_INVALID_CHUNK_FMT_SAMPLE_RATE;
        goto bufCleanup;
    }
    buf->formatChunk.sampleRate = sl_buf_to_native_uint(buffer4, 4);
    if (buf->formatChunk.sampleRate == 0) {
        ret = SL_INVALID_CHUNK_FMT_SAMPLE_RATE;
        goto bufCleanup;
    }

    //read and validate byte rate
    bytesRead = fread(buffer4, 1, 4, file);
    if (bytesRead != 4) {
        ret = SL_INVALID_CHUNK_FMT_BYTE_RATE;
        goto bufCleanup;
    }
    buf->formatChunk.byteRate = sl_buf_to_native_uint(buffer4, 4);
    if (buf->formatChunk.byteRate == 0) {
        ret = SL_INVALID_CHUNK_FMT_BYTE_RATE;
        goto bufCleanup;
    }

    //read and validate block align
    bytesRead = fread(buffer2, 1, 2, file);
    if (bytesRead != 2) {
        ret = SL_INVALID_CHUNK_FMT_BLOCK_ALIGN;
        goto bufCleanup;
    }
    buf->formatChunk.blockAlign = sl_buf_to_native_ushort(buffer2, 2);
    if (buf->formatChunk.blockAlign == 0) {
        ret = SL_INVALID_CHUNK_FMT_BLOCK_ALIGN;
        goto bufCleanup;
    }

    //read and validate bits per sample
    bytesRead = fread(buffer2, 1, 2, file);
    if (bytesRead != 2) {
        ret = SL_INVALID_CHUNK_FMT_BITS_PER_SAMPLE;
        goto bufCleanup;
    }
    buf->formatChunk.bitsPerSample = sl_buf_to_native_ushort(buffer2, 2);
    buf->dataChunk.pcmType = 0;
    buf->formatChunk.extensionSize = 0;
    if(buf->formatChunk.audioFormat == 1) {
        switch (buf->formatChunk.bitsPerSample) {
            case 8:
                buf->dataChunk.pcmType = SL_UNSIGNED_8PCM;
                break;
            case 16:
                buf->dataChunk.pcmType = SL_SIGNED_16PCM;
                break;
            case 24:
                buf->dataChunk.pcmType = SL_SIGNED_24PCM;
                break;
            case 32:
                buf->dataChunk.pcmType = SL_SIGNED_32PCM;
                break;
            default: break;
        }
    } else if (buf->formatChunk.audioFormat == 3) {
        switch (buf->formatChunk.bitsPerSample) {
            case 32:
                buf->dataChunk.pcmType = SL_FLOAT_32PCM;
                break;
            case 64:
                buf->dataChunk.pcmType = SL_FLOAT_64PCM;
                break;
            default: break;
        }
        //its pcm so we dont have to do anything with extension size
    } else {
        ret = SL_INVALID_CHUNK_FMT_AUDIO_FORMAT;
        goto bufCleanup;
    }

    if (buf->dataChunk.pcmType == 0) {
        ret = SL_INVALID_CHUNK_FMT_BITS_PER_SAMPLE;
        goto bufCleanup;
    }

    //keep reading until find data
    bytesRead = fread(buf->dataChunk.subChunk2Id, 1, 4, file);
    int found = 0;

    while(bytesRead == 4) {
        // check if everything is valid
        if (feof(file) || ferror(file)) {
            ret = SL_INVALID_CHUNK_DATA_ID;
            goto bufCleanup;
        }

        // check if bytes match omg
        SLuint matchingBytes = 0;
        for (int i = 0; i < 4; ++i) {
            if(buf->dataChunk.subChunk2Id[i] == dataID_bytes[i]) {
                matchingBytes++;
            }
        }

        // get size of that chunk :eyes:
        bytesRead = fread(buffer4, 1, 4, file);
        if (bytesRead != 4) {
            ret = SL_INVALID_CHUNK_DATA_SIZE;
            goto bufCleanup;
        }
        buf->dataChunk.subChunk2Size = sl_buf_to_native_uint(buffer4, 4);
        if (buf->dataChunk.subChunk2Size == 0) {
            ret = SL_INVALID_CHUNK_DATA_SIZE;
            goto bufCleanup;
        }

        // check if found that data :smirk:
        if(matchingBytes == 4) {
            found = 1;
            break;
        }

        // skip chunk and read next ID
        fseek(file, buf->dataChunk.subChunk2Size, SEEK_CUR);
        bytesRead = fread(buf->dataChunk.subChunk2Id, 1, 4, file);
    }

    if(!found) {
        ret = SL_INVALID_CHUNK_DATA_ID;
        goto bufCleanup;
    }

    //alloc data
    if(buf->dataChunk.pcmType == SL_UNSIGNED_8PCM) {
        buf->dataChunk.waveformData = malloc(buf->dataChunk.subChunk2Size);
        if (!buf->dataChunk.waveformData) {
            ret = SL_FAIL;
            goto bufCleanup;
        }
        bytesRead = fread(buf->dataChunk.waveformData, 1, buf->dataChunk.subChunk2Size, file);
    } else {
        buf->dataChunk.waveformData_signed = malloc(buf->dataChunk.subChunk2Size);
        if (!buf->dataChunk.waveformData_signed) {
            ret = SL_FAIL;
            goto bufCleanup;
        }
        bytesRead = fread(buf->dataChunk.waveformData_signed, 1, buf->dataChunk.subChunk2Size, file);
    }

    if (bytesRead != buf->dataChunk.subChunk2Size) {
        ret = SL_INVALID_CHUNK_DATA_DATA;
        goto bufCleanup;
    }

    *wavBuf = buf;

    goto fileCleanup;

    bufCleanup:
        if(buf) {
            free(buf->dataChunk.waveformData);
            free(buf->dataChunk.waveformData_signed);
        }
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
        free((*buf)->dataChunk.waveformData_signed);
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

SLuint sl_flip_endian_uint(SLuint ui) {
    // simple bitwise stuff to flip the endianness
    return ((ui >> 24) & 0xff) |
           ((ui << 8) & 0xff0000) |
           ((ui >> 8) & 0xff00) |
           ((ui << 24) & 0xff000000);
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
    if(!buf || bufLen < 4) return 0;

    SLuint value = 0;
    if(sysEndianness == LITTLE_ENDIAN) {
        value = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
    } else {
        value = buf[3] | (buf[2] << 8) | (buf[1] << 16) | (buf[0] << 24);
    }
    return value;
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //SAL_SAL_H
