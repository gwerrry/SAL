#ifndef SAL_SAL_H
#define SAL_SAL_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////// Include Platform Specific Headers and define some platform specific things. ///////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
//i know what im doing microsoft pls leave me alone
#pragma warning(disable : 4996)
#elif defined(__linux__)

#elif defined(__APPLE__) || defined(__MACH__)

#endif

//////////////////////////////////////////////////////
///////////////// Type definitions ///////////////////
//////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////// Tells us the native endian-ness of the system so we don't have to convert every time. ///////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static SLbool sysEndianness;

//////////////////////////////////////////////////////////////////////////////
///////////////// Wave File Parser Return Code Definitions ///////////////////
//////////////////////////////////////////////////////////////////////////////

#define SL_SUCCESS 69420

#define SL_FAIL 66666
#define SL_INVALID_VALUE 61616
#define SL_FILE_ERROR 62636
#define SL_INVALID_WAVE_FORMAT 63293

#define SL_INVALID_CHUNK_DESCRIPTOR_ID 10000
#define SL_INVALID_CHUNK_DESCRIPTOR_SIZE 11111
#define SL_INVALID_CHUNK_DESCRIPTOR_FORMAT 12222

#define SL_CHUNK_FORMAT_NOT_FOUND 20000
#define SL_INVALID_CHUNK_FMT_SIZE 23010
#define SL_INVALID_CHUNK_FMT_AUDIO_FORMAT 20190
#define SL_INVALID_CHUNK_FMT_CHANNELS 21600
#define SL_INVALID_CHUNK_FMT_SAMPLE_RATE 25021
#define SL_INVALID_CHUNK_FMT_BYTE_RATE 20205
#define SL_INVALID_CHUNK_FMT_BLOCK_ALIGN 22008
#define SL_INVALID_CHUNK_FMT_BITS_PER_SAMPLE 20321

#define SL_CHUNK_DATA_NOT_FOUND 30000
#define SL_INVALID_CHUNK_DATA_SIZE 30777
#define SL_INVALID_CHUNK_DATA_DATA 30666


///////////////////////////////////////////////////////////////////
///////////////// Wave File Parser Useful types ///////////////////
///////////////////////////////////////////////////////////////////

#define BIG_ENDIAN 1
#define LITTLE_ENDIAN 0

#define SL_UNSIGNED_8PCM 1
#define SL_SIGNED_16PCM 2
#define SL_SIGNED_24PCM 3
#define SL_SIGNED_32PCM 4
#define SL_FLOAT_32PCM 5
#define SL_FLOAT_64PCM 6

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////                                                                          The Wave file format                                                                        //////////////
//////////////                          Thanks to http://soundfile.sapp.org/doc/WaveFormat/ and https://wavefilegem.com/how_wave_files_work.html for this.                          //////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////                                                                          WAVE Descriptor Chunk                                                                       //////////////
//////////////                 Big Endian Chunk ID contains the letters "RIFF" in ASCII form. It can be something else like RIFX, but this parser only support RIFF.                //////////////
//////////////                                                 Little Endian Chunk Size contains the size of the rest of the chunk.                                                 //////////////
//////////////                          Big Endian Format contains WAVE. I don't know if this can be anything else, but this parser will only support WAVE.                         //////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////                                                                            WAVE Format Chunk                                                                         //////////////
//////////////                                                   Big Endian Chunk ID contains the letters "fmt " (with the space).                                                  //////////////
//////////////                                                  Little Endian Chunk Size contains the size of the rest of the chunk.                                                //////////////
//////////////                           Little Endian Audio Format contains the format of the audio. 1 for pcm, 3 for float, and there's probably a lot more.                      //////////////
//////////////                                                   Little Endian Number of Channels contains the number of channels.                                                  //////////////
//////////////                                                   Little Endian Sample rate contains the sample rate of the audio.                                                   //////////////
//////////////                                                     Little Endian Byte rate contains the byte rate of the audio.                                                     //////////////
//////////////                                       Little Endian Block Align contains number of bytes for one sample including all samples.                                       //////////////
//////////////                                                   Little Endian Bits Per Sample contains how many bits per sample.                                                   //////////////
//////////////             Little Endian ExtraParamSize contains size of extra params. doesn't exist for PCM (i think. I haven't seen it in what this parser supports).             //////////////
//////////////                                                                              Extra params                                                                            //////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////                                                                             WAVE Data Chunk                                                                          //////////////
//////////////                                                               Big Endian Chunk ID contains letters "data".                                                           //////////////
//////////////                                                   Little Endian Chunk Suze contains the size of the rest of the chunk.                                               //////////////
//////////////                                                                          Little Endian Data                                                                          //////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////                                                                                 Notes                                                                                //////////////
//////////////                                           I'm pretty sure RIFX makes it so all the little endian numbers are in big endian form.                                     //////////////
//////////////    I might add support for more audio formats, but it shouldn't affect performance heavily, as it would just be if statements and potentially a few more parameters. //////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
///////////////// Wave File Parser Struct Definitions ///////////////////
/////////////////////////////////////////////////////////////////////////

typedef struct sl_wav_descriptor {
    SLuchar descriptorId[4];
    SLuint chunkSize;
    SLuchar chunkFormat[4];
} SL_WAV_DESCRIPTOR;

typedef struct sl_wav_fmt {
    SLuchar fmtId[4];
    SLuint subChunk1Size;
    SLushort audioFormat;
    SLushort numChannels;
    SLuint sampleRate;
    SLuint byteRate;
    SLushort blockAlign;
    SLushort bitsPerSample;
    SLushort extensionSize;
} SL_WAV_FMT;

typedef struct sl_wav_data {
    SLuchar dataId[4];
    SLuint subChunk2Size;
    SLenum pcmType;
    SLuchar*  waveformData;
    SLchar*  waveformData_signed;
} SL_WAV_DATA;

typedef struct sl_wav_file {
    SL_WAV_DESCRIPTOR descriptorChunk;
    SL_WAV_FMT formatChunk;
    SL_WAV_DATA dataChunk;
} SL_WAV_FILE;

///////////////////////////////////////////////////////////////////////////
///////////////// Wave File Parser Function Definitions ///////////////////
///////////////////////////////////////////////////////////////////////////

/**
 * @brief Sound library init function. You MUST call this even when using the wave file parser.
 * @return SL_SUCCESS if succeeded. If anything else is returned, the function failed.
 */
SLenum sl_init(void);

/**
 * @brief Sound library cleanup function. You should call this when your app execution ends.
 * @return SL_SUCCESS if succeeded. If anything else is returned, the function failed.
 */
SLenum sl_cleanup(void);

/**
 * @brief Parses wave file at the path.
 * For more information on types of WAVE files supported, go to the Github.
 * @param path - Path of WAVE file to parse.
 * @param wavBuf - Buffer for the WAVE file.
 * @return SL_SUCCESS if succeeded. There are many useful return codes documented on the Github so you can debug anything else that happens.
 */
SLenum sl_parse_wave_file(SLstr path, SL_WAV_FILE** wavBuf);

/**
 * @brief Frees the memory associated with the WAVE file.
 * Also sets the buf to NULL.
 * @param buf - Buffer of WAVE file to free.
 */
void sl_free_wave_file(SL_WAV_FILE** buf);

/**
 * @brief This is just for checking if the file provided in the parser ends with ".wav".
 * That's it.
 * You could use it for anything else, but up that's to you.
 * @param str - String to check.
 * @param suffix - Suffix to check for.
 * @return SL_SUCCESS if it worked and SL_FAIL if it failed.
 */
SLenum ends_with(SLstr str, SLstr suffix);

/**
 * @brief Converts a SLuchar* buf to a SLushort, but takes into account the system's native endian-ness.
 * @param buf - Buffer to convert. This MUST be at least 2 bytes and valid.
 * @param bufLen - Length of SLuchar buffer to convert.
 * @return The value of the buffer represented as a SLushort taking into account the system's native endian-ness. Returns zero if it fails. I know it's not helpful, but you can debug!
 */
SLushort sl_buf_to_native_ushort(SLuchar* buf, SLuint bufLen);

/**
 * @brief Converts a SLuchar* buf to a SLuint, but takes into account the system's native endian-ness.
 * @param buf - Buffer to convert. This MUST be at least 4 bytes and valid.
 * @param bufLen - Length of SLuchar buffer to convert.
 * @return The value of the buffer represented as a SLuint taking into account the system's native endian-ness. Returns zero if it fails. I know it's not helpful, but you can debug!
 */
SLuint sl_buf_to_native_uint(SLuchar* buf, SLuint bufLen);

///////////////////////////////////////////////////////////////////////////////
///////////////// Wave File Parser Function Implementations ///////////////////
///////////////////////////////////////////////////////////////////////////////

SLenum sl_parse_wave_file(SLstr path, SL_WAV_FILE** wavBuf) {
    SLenum ret = SL_SUCCESS;

    SLuchar buffer4[4];
    SLuchar buffer2[2];
    const SLuchar riffID_bytes[4] = {0x52, 0x49, 0x46, 0x46};
    const SLuchar rifxID_bytes[4] = {0x52, 0x49, 0x46, 0x58};
    const SLuchar waveID_bytes[4] = {0x57, 0x41, 0x56, 0x45};
    const SLuchar fmtID_bytes [4] = {0x66, 0x6d, 0x74, 0x20};
    const SLuchar dataID_bytes[4] = {0x64, 0x61, 0x74, 0x61};

    SLullong blocksRead = 0;
    SLbool found = 0;
    SLbool foundFmt = 0;
    SLbool foundData = 0;
    SLbool usingRIFX = 0;
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
        ret = SL_FILE_ERROR;
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
    void* temp_data = NULL;
    if(!buf) {
        ret = SL_FAIL;
        goto bufCleanup;
    }

    //
    // Define some stuff since we know buf has data now.
    //
    buf->dataChunk.pcmType = 0;
    buf->formatChunk.extensionSize = 0;
    buf->dataChunk.waveformData = NULL;
    buf->dataChunk.waveformData_signed = NULL;

    //read and validate chunkID
    blocksRead = fread(buf->descriptorChunk.descriptorId, 4, 1, file);
    if (!blocksRead) {
        ret = SL_INVALID_CHUNK_DESCRIPTOR_ID;
        goto bufCleanup;
    }
    else if (memcmp(buf->descriptorChunk.descriptorId, riffID_bytes, 4) != 0) {}
    else if (memcmp(buf->descriptorChunk.descriptorId, rifxID_bytes, 4) != 0) usingRIFX = 1;
    else {
        ret = SL_INVALID_CHUNK_DESCRIPTOR_ID;
        goto bufCleanup;
    }

    //read and validate chunk size
    blocksRead = fread(buffer4, 4, 1, file);
    if (!blocksRead) {
        ret = SL_INVALID_CHUNK_DESCRIPTOR_SIZE;
        goto bufCleanup;
    }
    buf->descriptorChunk.chunkSize = sl_buf_to_native_uint(buffer4, 4);
    if (buf->descriptorChunk.chunkSize == 0) {
        ret = SL_INVALID_CHUNK_DESCRIPTOR_SIZE;
        goto bufCleanup;
    }

    //read and validate wave format
    blocksRead = fread(buf->descriptorChunk.chunkFormat, 4, 1, file);
    if (!blocksRead || memcmp(buf->descriptorChunk.chunkFormat, waveID_bytes, 4) != 0) {
        ret = SL_INVALID_CHUNK_DESCRIPTOR_FORMAT;
        goto bufCleanup;
    }

    //search and parse other sections
    blocksRead = fread(buffer4, 4, 1, file);

    while(blocksRead) {
        SLuint foundMatch = 0;

        // check if everything is valid
        if (feof(file) || ferror(file)) {
            ret = SL_INVALID_WAVE_FORMAT;
            goto bufCleanup;
        }

        //FORMAT CHUNK
        if(!foundFmt && memcmp(buffer4, fmtID_bytes, 4) == 0) {
            foundMatch = 1;
            foundFmt = 1;
            //store format id
            memcpy(buf->formatChunk.fmtId, buffer4, 4);

            //read and validate fmt chunk size
            blocksRead = fread(buffer4, 4, 1, file);
            if (!blocksRead) {
                ret = SL_INVALID_CHUNK_FMT_SIZE;
                goto bufCleanup;
            }
            buf->formatChunk.subChunk1Size = sl_buf_to_native_uint(buffer4, 4);
            if (buf->formatChunk.subChunk1Size == 0) {
                ret = SL_INVALID_CHUNK_FMT_SIZE;
                goto bufCleanup;
            }

            //read only. audio format is verified later when parsing bits per sample
            blocksRead = fread(buffer2, 2, 1, file);
            if (!blocksRead) {
                ret = SL_INVALID_CHUNK_FMT_AUDIO_FORMAT;
                goto bufCleanup;
            }
            buf->formatChunk.audioFormat = sl_buf_to_native_ushort(buffer2, 2);

            //read and validate num channels.
            blocksRead = fread(buffer2, 2, 1, file);
            if (!blocksRead) {
                ret = SL_INVALID_CHUNK_FMT_CHANNELS;
                goto bufCleanup;
            }
            buf->formatChunk.numChannels = sl_buf_to_native_ushort(buffer2, 2);
            if (buf->formatChunk.numChannels == 0) {
                ret = SL_INVALID_CHUNK_FMT_CHANNELS;
                goto bufCleanup;
            }

            //read and validate sample rate
            blocksRead = fread(buffer4, 4, 1, file);
            if (!blocksRead) {
                ret = SL_INVALID_CHUNK_FMT_SAMPLE_RATE;
                goto bufCleanup;
            }
            buf->formatChunk.sampleRate = sl_buf_to_native_uint(buffer4, 4);
            if (buf->formatChunk.sampleRate == 0) {
                ret = SL_INVALID_CHUNK_FMT_SAMPLE_RATE;
                goto bufCleanup;
            }

            //read and validate byte rate
            blocksRead = fread(buffer4, 4, 1, file);
            if (!blocksRead) {
                ret = SL_INVALID_CHUNK_FMT_BYTE_RATE;
                goto bufCleanup;
            }
            buf->formatChunk.byteRate = sl_buf_to_native_uint(buffer4, 4);
            if (buf->formatChunk.byteRate == 0) {
                ret = SL_INVALID_CHUNK_FMT_BYTE_RATE;
                goto bufCleanup;
            }

            //read and validate block align
            blocksRead = fread(buffer2, 2, 1, file);
            if (!blocksRead) {
                ret = SL_INVALID_CHUNK_FMT_BLOCK_ALIGN;
                goto bufCleanup;
            }
            buf->formatChunk.blockAlign = sl_buf_to_native_ushort(buffer2, 2);
            if (buf->formatChunk.blockAlign == 0) {
                ret = SL_INVALID_CHUNK_FMT_BLOCK_ALIGN;
                goto bufCleanup;
            }

            //read and validate bits per sample
            blocksRead = fread(buffer2, 2, 1, file);
            if (!blocksRead) {
                ret = SL_INVALID_CHUNK_FMT_BITS_PER_SAMPLE;
                goto bufCleanup;
            }
            buf->formatChunk.bitsPerSample = sl_buf_to_native_ushort(buffer2, 2);
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
        }

        // DATA CHUNK
        else if(!foundData && memcmp(buffer4, dataID_bytes, 4) == 0) {
            foundMatch = 1;
            foundData = 1;

            //store data id
            memcpy(buf->dataChunk.dataId, buffer4, 4);

            //data chunk size
            blocksRead = fread(buffer4, 4, 1, file);
            if (!blocksRead) {
                ret = SL_INVALID_CHUNK_DATA_SIZE;
                goto bufCleanup;
            }

            buf->dataChunk.subChunk2Size = sl_buf_to_native_uint(buffer4, 4);
            if (buf->dataChunk.subChunk2Size == 0) {
                ret = SL_INVALID_CHUNK_DATA_SIZE;
                goto bufCleanup;
            }

            temp_data = malloc(buf->dataChunk.subChunk2Size);
            if (!temp_data) {
                ret = SL_FAIL;
                goto bufCleanup;
            }

            blocksRead = fread(temp_data, buf->dataChunk.subChunk2Size, 1, file);
            if (!blocksRead) {
                ret = SL_INVALID_CHUNK_DATA_DATA;
                goto bufCleanup;
            }
        }

        if(foundFmt && foundData) break;

        //we are using this instead of an else for expandability
        if(foundMatch != 1) {
            SLullong size = 0;
            blocksRead = fread(buffer4, 4, 1, file);
            if (!blocksRead) {
                ret = SL_INVALID_WAVE_FORMAT;
                goto bufCleanup;
            }

            size = sl_buf_to_native_uint(buffer4, 4);
            if (size == 0) {
                ret = SL_INVALID_WAVE_FORMAT;
                goto bufCleanup;
            }

            // skip chunk and read next ID
            fseek(file, size, SEEK_CUR);
            blocksRead = fread(buffer4, 4, 1, file);
        } else {
            blocksRead = fread(buffer4, 4, 1, file);
        }

    }

    if(!foundFmt) {
        ret = SL_CHUNK_FORMAT_NOT_FOUND;
        goto bufCleanup;
    } else if(!foundData) {
        ret = SL_CHUNK_DATA_NOT_FOUND;
        goto bufCleanup;
    }

    //TODO test this signed and unsigned when we play sounds because in theory we shouldnt have to wonder about the sign bits. if we dont then thats a REALLY NICE optimization
    //copy data into buf
    if(buf->dataChunk.pcmType == SL_UNSIGNED_8PCM) {
        buf->dataChunk.waveformData = malloc(buf->dataChunk.subChunk2Size);
        if(!usingRIFX) memcpy(buf->dataChunk.waveformData, temp_data, buf->dataChunk.subChunk2Size);
        else {
            //memcpy... BUT IN REVERSE?!?!
            SLullong len = buf->dataChunk.subChunk2Size;
            SLuchar *d = buf->dataChunk.waveformData + len - 1;
            const SLuchar *s = (SLuchar*)temp_data;
            while (len--) *d-- = *s++;
        }
    } else {
        buf->dataChunk.waveformData_signed = malloc(buf->dataChunk.subChunk2Size);
        if(!usingRIFX) memcpy(buf->dataChunk.waveformData_signed, temp_data, buf->dataChunk.subChunk2Size);
        else {
            //memcpy... BUT IN REVERSE?!?!
            SLullong len = buf->dataChunk.subChunk2Size;
            SLchar *d = buf->dataChunk.waveformData_signed + len - 1;
            const SLchar *s = (SLchar*)temp_data;
            while (len--) *d-- = *s++;
        }
    }


    free(temp_data);

    *wavBuf = buf;

    if (buf->dataChunk.waveformData_signed || buf->dataChunk.waveformData) goto fileCleanup;
    else {
        ret = SL_FAIL;
        goto bufCleanup;
    }

    bufCleanup:
        if (buf) {
            free(buf->dataChunk.waveformData);
            free(buf->dataChunk.waveformData_signed);
            buf->dataChunk.waveformData = NULL;
            buf->dataChunk.waveformData_signed = NULL;
        }
        free(buf);
        free(temp_data);
        temp_data = NULL;
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
        (*buf)->dataChunk.waveformData_signed = NULL;
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
    char* sub = (char*)malloc((suffix_len + 1) * sizeof(char));
    if (!sub) return SL_FAIL;
    memcpy(sub, str + str_len - suffix_len, suffix_len);
    sub[suffix_len] = '\0';

    SLenum res = strcmp(suffix, sub) == 0 ? SL_SUCCESS : SL_FAIL;

    free(sub);

    return res;
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
