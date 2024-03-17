/**
 * @file sal.h
 * @author gwerry
 * @brief Simple Audio Library for parsing WAVE files and playing WAVE sounds.
 * @version 2.0
 * @date 2024/03/17
 *
 * Copyright 2024 gwerry
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef SAL_SAL_H
#define SAL_SAL_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/////////////////////////////////////////////////////////////
///////////////// Option for DLL linking. ///////////////////
/////////////////////////////////////////////////////////////
#define USE_DLL_LINKING // un-comment this if you want to use DLL linking

#define DLL_EXPORT

#ifdef USE_DLL_LINKING

#ifdef _MSC_VER
#undef DLL_EXPORT
#define DLL_EXPORT __declspec(dllexport)
#endif //_MSC_VER

#endif // USE_DLL_LINKING

////////////////////////////////////////////////////////////
///////////////// Version of SAL header. ///////////////////
////////////////////////////////////////////////////////////
#define SL_VERSION "v2.0.0"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////// Controls if you want to use the OpenAL wrapper. Comment it out if you don't want to use it. ///////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SL_OPENAL_WRAPPER

///////////////////////////////////////////////////////////////////////////
///////////////// Define some compiler specific things. ///////////////////
///////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
//i know what im doing microsoft pls leave me alone
#pragma warning(disable : 4996)
#endif

//////////////////////////////////////////////////////
///////////////// Type definitions ///////////////////
//////////////////////////////////////////////////////

DLL_EXPORT typedef int8_t      SLbool;
DLL_EXPORT typedef int8_t      SLchar;
DLL_EXPORT typedef uint8_t     SLuchar;
DLL_EXPORT typedef int16_t     SLshort;
DLL_EXPORT typedef uint16_t    SLushort;
DLL_EXPORT typedef int32_t     SLint;
DLL_EXPORT typedef uint32_t    SLuint;
DLL_EXPORT typedef uint64_t    SLullong;
DLL_EXPORT typedef float       SLfloat;
DLL_EXPORT typedef double      SLdouble;
DLL_EXPORT typedef const char* SLstr;
DLL_EXPORT typedef void*       SLvoid;

///////////////////////////////////////////////////
/////////////////  Return Codes ///////////////////
///////////////////////////////////////////////////

DLL_EXPORT typedef enum {
    SL_SUCCESS = 69420,
    SL_FAIL = 66666,
    SL_INVALID_VALUE = 61616,

    SL_FILE_ERROR = 62636,
    SL_INVALID_WAVE_FORMAT = 63293,

    SL_INVALID_CHUNK_DESCRIPTOR_ID = 10000,
    SL_INVALID_CHUNK_DESCRIPTOR_SIZE = 11111,
    SL_INVALID_CHUNK_DESCRIPTOR_FORMAT = 12222,

    SL_CHUNK_FORMAT_NOT_FOUND = 20000,
    SL_INVALID_CHUNK_FMT_SIZE = 23010,
    SL_INVALID_CHUNK_FMT_AUDIO_FORMAT = 20190,
    SL_INVALID_CHUNK_FMT_CHANNELS = 21600,
    SL_INVALID_CHUNK_FMT_SAMPLE_RATE = 25021,
    SL_INVALID_CHUNK_FMT_BYTE_RATE = 20205,
    SL_INVALID_CHUNK_FMT_BLOCK_ALIGN = 22008,
    SL_INVALID_CHUNK_FMT_BITS_PER_SAMPLE = 20321,

    SL_CHUNK_DATA_NOT_FOUND = 30000,
    SL_INVALID_CHUNK_DATA_SIZE = 30777,
    SL_INVALID_CHUNK_DATA_DATA = 30666
} SL_RETURN_CODE;

//TODO add version getting and make sys endianness macro into function taht cna be used
/////////////////////////////////////////////////////////////////////
///////////////// Simple Audio Library Global stuff /////////////////
/////////////////////////////////////////////////////////////////////

DLL_EXPORT typedef enum {
    SL_BIG_ENDIAN = 1,
    SL_LITTLE_ENDIAN = 0
} SL_ENDIANNESS;

// get native endian-ness of the system
DLL_EXPORT SL_ENDIANNESS get_native_endianness() {
    int n = 1;
    SL_ENDIANNESS ret = *(char*)&n == 1 ? SL_LITTLE_ENDIAN : SL_BIG_ENDIAN;

    return ret;
}

////////////////////////////////////////////////////////////////
///////////////// Wave File Parser Pcm types ///////////////////
////////////////////////////////////////////////////////////////

DLL_EXPORT typedef enum {
    SL_UNSIGNED_8PCM = 1,
    SL_SIGNED_16PCM = 2,
    SL_SIGNED_24PCM = 3,
    SL_SIGNED_32PCM = 4,
    SL_FLOAT_32PCM = 5,
    SL_FLOAT_64PCM = 6
} SL_WAVE_PCM_TYPE;

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

/////////////////////////////////////////////////////////////////////////
///////////////// Wave File Parser Struct Definitions ///////////////////
/////////////////////////////////////////////////////////////////////////

DLL_EXPORT typedef struct sl_wav_descriptor {
    SLuint descriptorChunkSize;
    SLuchar descriptorId[4];
    SLuchar chunkFormat[4];
} SL_WAV_DESCRIPTOR;

DLL_EXPORT typedef struct sl_wav_fmt {
    SLuint fmtChunkSize;
    SLuint sampleRate;
    SLuint byteRate;
    SLushort audioFormat;
    SLushort numChannels;
    SLushort blockAlign;
    SLushort bitsPerSample;
    SLushort extensionSize;
    SLuchar fmtId[4];
} SL_WAV_FMT;

DLL_EXPORT typedef struct sl_wav_data {
    SLuint dataChunkSize;
    SLuint pcmType;
    SLuchar dataId[4];
    SLvoid  waveformData;
} SL_WAV_DATA;

DLL_EXPORT typedef struct sl_wav_file {
    SL_WAV_DESCRIPTOR descriptorChunk;
    SL_WAV_FMT formatChunk;
    SL_WAV_DATA dataChunk;
} SL_WAV_FILE;

///////////////////////////////////////////////////////////////////////////
///////////////// Wave File Parser Function Definitions ///////////////////
///////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses wave file at the path.
 * For more information on types of WAVE files supported, go to the Github.
 * @param path - Path of WAVE file to parse.
 * @param wavBufPtr - Buffer for the WAVE file.
 * @return SL_SUCCESS if succeeded. There are many useful return codes documented on the Github so you can debug anything else that happens.
 */
DLL_EXPORT SL_RETURN_CODE sl_read_wave_file(SLstr path, SL_WAV_FILE* wavBuf);

/**
 * @brief Frees the memory associated with the WAVE file.
 * @param buf - Buffer of WAVE file to free.
 */
DLL_EXPORT void sl_cleanup_wave_file(SL_WAV_FILE* wavBuf);

/**
 * @brief Reads WAVE descriptor chunk. This is a helper function and should not be used except by SAL.
 * @param file - File ptr to WAVE file.
 * @param wavBuf - Buffer for the WAVE file.
 * @return SL_SUCCESS if it succeeded. Anything else means a failure.
 */
DLL_EXPORT SL_RETURN_CODE sl_read_wave_descriptor(FILE* file, SL_WAV_FILE* wavBuf);

/**
 * @brief Parses WAVE chunks. This is a helper function and should not be used except by SAL.
 * @param file - File ptr to WAVE file.
 * @param wavBuf - Buffer for the WAVE file.
 * @return SL_SUCCESS if it succeeded. Anything else means a failure.
 */
DLL_EXPORT SL_RETURN_CODE sl_parse_wave_chunks(FILE* file, SL_WAV_FILE* wavBuf);

/**
 * @brief Reads WAVE format chunk. This is a helper function and should not be used except by SAL.
 * @param file - File ptr to WAVE file.
 * @param wavBuf - Buffer for the WAVE file.
 * @return SL_SUCCESS if it succeeded. Anything else means a failure.
 */
DLL_EXPORT SL_RETURN_CODE sl_read_wave_format_chunk(FILE* file, SL_WAV_FILE* wavBuf);

/**
 * @brief Reads WAVE data chunk. This is a helper function and should not be used except by SAL.
 * @param file - File ptr to WAVE file.
 * @param wavBuf - Buffer for the WAVE file.
 * @return SL_SUCCESS if it succeeded. Anything else means a failure.
 */
DLL_EXPORT SL_RETURN_CODE sl_read_wave_data_chunk(FILE* file, SL_WAV_FILE* wavBuf);

/**
 * @brief Ensures WAVE data ends on a proper byte boundary. This is a helper function and should not be used except by SAL.
 * @param wavBuf - Buffer for the WAVE file.
 * @return SL_SUCCESS if it succeeded. Anything else means a failure.
 */
DLL_EXPORT SL_RETURN_CODE sl_validate_wave_data(SL_WAV_FILE* wavBuf);

/**
 * @brief Ensures WAVE data endian-ness is correct. This is a helper function and should not be used except by SAL.
 * @param wavBuf - Buffer for the WAVE file.
 * @return SL_SUCCESS if it succeeded. Anything else means a failure.
 */
DLL_EXPORT SL_RETURN_CODE sl_ensure_wave_endianness(SL_WAV_FILE* wavBuf);

/**
 * @brief This is just for checking if the path provided ends with ".wav" or ".wave".
 * @param str - Path to check.
 * @return SL_SUCCESS the file has the proper wav extension. SL_FAIL otherwise.
 */
DLL_EXPORT SL_RETURN_CODE sl_is_wave_file(SLstr path);

/**
 * @brief Converts a SLuchar* buf to a SLushort, but takes into account the system's native endian-ness.
 * @param buf - Buffer to convert. This MUST be at least 2 bytes and valid.
 * @param bufLen - Length of SLuchar buffer to convert.
 * @return The value of the buffer represented as a SLushort taking into account the system's native endian-ness. Returns zero if it fails. I know it's not helpful, but you can debug!
 */
DLL_EXPORT SLushort sl_buf_to_native_ushort(const SLuchar* buf, SLullong bufLen);

/**
 * @brief Converts a SLuchar* buf to a SLuint, but takes into account the system's native endian-ness.
 * @param buf - Buffer to convert. This MUST be at least 4 bytes and valid.
 * @param bufLen - Length of SLuchar buffer to convert.
 * @return The value of the buffer represented as a SLuint taking into account the system's native endian-ness. Returns zero if it fails. I know it's not helpful, but you can debug!
 */
DLL_EXPORT SLuint sl_buf_to_native_uint(const SLuchar* buf, SLullong bufLen);

/**
 * @brief Flips the endian-ness of a SLshort.
 * @param s SLshort to flip.
 * @return SLshort with the endian-ness flipped.
 */
DLL_EXPORT SLshort sl_flip_endian_short(SLshort s);

/**
 * @brief Flips the endian-ness of a SLint.
 * @param i SLint to flip.
 * @return SLint with the endian-ness flipped.
 */
DLL_EXPORT SLint sl_flip_endian_int(SLint i);

/**
 * @brief Flips the endian-ness of a SLfloat.
 * @param f SLfloat to flip.
 * @return SLfloat with the endian-ness flipped.
 */
DLL_EXPORT SLfloat sl_flip_endian_float(SLfloat f);

/**
 * @brief Flips the endian-ness of a SLdouble.
 * @param d SLdouble to flip.
 * @return SLdouble with the endian-ness flipped.
 */
DLL_EXPORT SLdouble sl_flip_endian_double(SLdouble d);

///////////////////////////////////////////////////////////////////////////////
///////////////// Wave File Parser Function Implementations ///////////////////
///////////////////////////////////////////////////////////////////////////////

DLL_EXPORT SL_RETURN_CODE sl_read_wave_file(SLstr path, SL_WAV_FILE* wavBuf) {
    SL_RETURN_CODE ret = SL_SUCCESS;
    memset(wavBuf, 0, sizeof(SL_WAV_FILE));

    FILE* file;

    //ensure pointers are good were just going to assume the user allocated stuff right
    if (path == NULL) {
        ret = SL_INVALID_VALUE;
        goto exit;
    }

    //ensures that this file we are reading is a wave file-or at least ends with it
    if(sl_is_wave_file(path) == SL_FAIL) {
        ret = SL_FILE_ERROR;
        goto exit;
    }

    // try to open file
    file = fopen(path, "rb");
    if (file == NULL) {
        ret = SL_FILE_ERROR;
        goto exit;
    }

    ret = sl_read_wave_descriptor(file, wavBuf);
    if(ret != SL_SUCCESS) goto bufCleanup;

    ret = sl_parse_wave_chunks(file, wavBuf);
    if(ret != SL_SUCCESS) goto bufCleanup;

    ret = sl_validate_wave_data(wavBuf);
    if(ret != SL_SUCCESS) goto bufCleanup;

    ret = sl_ensure_wave_endianness(wavBuf);
    if(ret != SL_SUCCESS) goto bufCleanup;

    if (wavBuf->dataChunk.waveformData != NULL) goto fileCleanup;
    else {
        ret = SL_FAIL;
        goto bufCleanup;
    }

    bufCleanup:
        if(wavBuf->dataChunk.waveformData != NULL) free(wavBuf->dataChunk.waveformData);
            wavBuf->dataChunk.waveformData = NULL;
    fileCleanup:
        fclose(file);
    exit:
        return ret;
}

DLL_EXPORT void sl_cleanup_wave_file(SL_WAV_FILE* wavBuf) {
    if(wavBuf != NULL) {
        if(wavBuf->dataChunk.waveformData != NULL) free(wavBuf->dataChunk.waveformData);
        wavBuf->dataChunk.waveformData = NULL;
    }
}

DLL_EXPORT SL_RETURN_CODE sl_read_wave_descriptor(FILE* file, SL_WAV_FILE* wavBuf) {
    const SLuchar riffID_bytes[4] = {0x52, 0x49, 0x46, 0x46};
    const SLuchar waveID_bytes[4] = {0x57, 0x41, 0x56, 0x45};
    SLuchar buffer4[4] = {0x00, 0x00, 0x00, 0x00};
    SLullong blocksRead;

    //read and validate chunkID
    blocksRead = fread(wavBuf->descriptorChunk.descriptorId, 4, 1, file);
    if (!blocksRead)
        return SL_INVALID_CHUNK_DESCRIPTOR_ID;

    if (memcmp(wavBuf->descriptorChunk.descriptorId, riffID_bytes, 4) != 0)
        return SL_INVALID_CHUNK_DESCRIPTOR_ID;

    //read and validate chunk size
    blocksRead = fread(buffer4, 4, 1, file);
    wavBuf->descriptorChunk.descriptorChunkSize = sl_buf_to_native_uint(buffer4, 4);
    if (!blocksRead || wavBuf->descriptorChunk.descriptorChunkSize == 0)
        return SL_INVALID_CHUNK_DESCRIPTOR_SIZE;

    //read and validate wave format
    blocksRead = fread(wavBuf->descriptorChunk.chunkFormat, 4, 1, file);
    if (!blocksRead || memcmp(wavBuf->descriptorChunk.chunkFormat, waveID_bytes, 4) != 0)
        return SL_INVALID_CHUNK_DESCRIPTOR_FORMAT;

    return SL_SUCCESS;
}

DLL_EXPORT SL_RETURN_CODE sl_parse_wave_chunks(FILE* file, SL_WAV_FILE* wavBuf) {
    SLuchar buffer4[4] = {0x00, 0x00, 0x00, 0x00};
    const SLuchar fmtID_bytes [4] = {0x66, 0x6d, 0x74, 0x20};
    const SLuchar dataID_bytes[4] = {0x64, 0x61, 0x74, 0x61};
    SLullong blocksRead;
    SLbool foundFmt = 0;
    SLbool foundData = 0;

    // Implement the logic for reading the wave descriptor here
    blocksRead = fread(buffer4, 4, 1, file);

    while(blocksRead) {
        SLuint foundMatch = 0;

        // check if everything is valid
        if (feof(file) || ferror(file))
            return SL_INVALID_WAVE_FORMAT;

        //FORMAT CHUNK
        if(!foundFmt && memcmp(buffer4, fmtID_bytes, 4) == 0) {
            foundMatch = 1;
            foundFmt = 1;
            //store format id
            memcpy(wavBuf->formatChunk.fmtId, buffer4, 4);

            SL_RETURN_CODE ret = sl_read_wave_format_chunk(file, wavBuf);
            if(ret != SL_SUCCESS)
                return ret;
        }

        // DATA CHUNK
        else if(!foundData && memcmp(buffer4, dataID_bytes, 4) == 0) {
            foundMatch = 1;
            foundData = 1;

            //store data id
            memcpy(wavBuf->dataChunk.dataId, buffer4, 4);
            SL_RETURN_CODE ret = sl_read_wave_data_chunk(file, wavBuf);
            if(ret != SL_SUCCESS) return ret;
        }

        if(foundFmt && foundData) break;

        if(foundMatch) {
            blocksRead = fread(buffer4, 4, 1, file);
        } else {
            SLuint size;
            blocksRead = fread(buffer4, 4, 1, file);
            if (!blocksRead)
                return SL_INVALID_WAVE_FORMAT;

            size = sl_buf_to_native_uint(buffer4, 4);
            if (size == 0)
                return SL_INVALID_WAVE_FORMAT;

            // skip chunk and read next ID
            fseek(file, size, SEEK_CUR);
            blocksRead = fread(buffer4, 4, 1, file);
        }
    }

    if(!foundFmt)
        return SL_CHUNK_FORMAT_NOT_FOUND;
    else if(!foundData)
        return SL_CHUNK_DATA_NOT_FOUND;

    return SL_SUCCESS;
}

DLL_EXPORT SL_RETURN_CODE sl_read_wave_format_chunk(FILE* file, SL_WAV_FILE* wavBuf) {
    SLuchar buffer4[4] = {0x00, 0x00, 0x00, 0x00};
    SLuchar buffer2[2] = {0x00, 0x00};
    SLullong blocksRead;

    //read and validate fmt chunk size
    blocksRead = fread(buffer4, 4, 1, file);
    wavBuf->formatChunk.fmtChunkSize = sl_buf_to_native_uint(buffer4, 4);
    if (!blocksRead || wavBuf->formatChunk.fmtChunkSize == 0)
        return SL_INVALID_CHUNK_FMT_SIZE;

    //read only. audio format is verified later when parsing bits per sample
    blocksRead = fread(buffer2, 2, 1, file);
    if (!blocksRead)
        return SL_INVALID_CHUNK_FMT_AUDIO_FORMAT;
    wavBuf->formatChunk.audioFormat = sl_buf_to_native_ushort(buffer2, 2);

    //read and validate num channels.
    blocksRead = fread(buffer2, 2, 1, file);
    wavBuf->formatChunk.numChannels = sl_buf_to_native_ushort(buffer2, 2);
    if (!blocksRead || wavBuf->formatChunk.numChannels == 0)
        return SL_INVALID_CHUNK_FMT_CHANNELS;


    //read and validate sample rate
    blocksRead = fread(buffer4, 4, 1, file);
    wavBuf->formatChunk.sampleRate = sl_buf_to_native_uint(buffer4, 4);
    if (!blocksRead || wavBuf->formatChunk.sampleRate == 0)
        return SL_INVALID_CHUNK_FMT_SAMPLE_RATE;


    //read and validate byte rate
    blocksRead = fread(buffer4, 4, 1, file);
    wavBuf->formatChunk.byteRate = sl_buf_to_native_uint(buffer4, 4);
    if (!blocksRead || wavBuf->formatChunk.byteRate == 0)
        return SL_INVALID_CHUNK_FMT_BYTE_RATE;

    //read and validate block align
    blocksRead = fread(buffer2, 2, 1, file);
    wavBuf->formatChunk.blockAlign = sl_buf_to_native_ushort(buffer2, 2);
    if (!blocksRead || wavBuf->formatChunk.blockAlign == 0)
        return SL_INVALID_CHUNK_FMT_BLOCK_ALIGN;

    //read and validate bits per sample
    blocksRead = fread(buffer2, 2, 1, file);
    if (!blocksRead)
        return SL_INVALID_CHUNK_FMT_BITS_PER_SAMPLE;

    wavBuf->formatChunk.bitsPerSample = sl_buf_to_native_ushort(buffer2, 2);

    if(wavBuf->formatChunk.audioFormat == 1) {
        switch (wavBuf->formatChunk.bitsPerSample) {
            case 8: {
                wavBuf->dataChunk.pcmType = SL_UNSIGNED_8PCM;
                break;
            }
            case 16: {
                wavBuf->dataChunk.pcmType = SL_SIGNED_16PCM;
                break;
            }
            case 24: {
                wavBuf->dataChunk.pcmType = SL_SIGNED_24PCM;
                break;
            }
            case 32: {
                wavBuf->dataChunk.pcmType = SL_SIGNED_32PCM;
                break;
            }
            default: break;
        }
    } else if (wavBuf->formatChunk.audioFormat == 3) {
        switch (wavBuf->formatChunk.bitsPerSample) {
            case 32: {
                wavBuf->dataChunk.pcmType = SL_FLOAT_32PCM;
                break;
            }
            case 64: {
                wavBuf->dataChunk.pcmType = SL_FLOAT_64PCM;
                break;
            }
            default: break;
        }
    } else {
        return SL_INVALID_CHUNK_FMT_AUDIO_FORMAT;
    }

    if (wavBuf->dataChunk.pcmType == 0)
        return SL_INVALID_CHUNK_FMT_BITS_PER_SAMPLE;

    return SL_SUCCESS;
}

DLL_EXPORT SL_RETURN_CODE sl_read_wave_data_chunk(FILE* file, SL_WAV_FILE* wavBuf) {
    SLuchar buffer4[4] = {0x00, 0x00, 0x00, 0x00};
    SLullong blocksRead;
    //read data chunk size
    blocksRead = fread(buffer4, 4, 1, file);
    wavBuf->dataChunk.dataChunkSize = sl_buf_to_native_uint(buffer4, 4);
    if (!blocksRead || wavBuf->dataChunk.dataChunkSize == 0)
        return SL_INVALID_CHUNK_DATA_SIZE;

    wavBuf->dataChunk.waveformData = malloc(wavBuf->dataChunk.dataChunkSize);
    if (wavBuf->dataChunk.waveformData == NULL)
        return SL_FAIL;

    // Ensure the buffer size is even
    blocksRead = fread(wavBuf->dataChunk.waveformData, wavBuf->dataChunk.dataChunkSize, 1, file);
    if (!blocksRead)
        return SL_INVALID_CHUNK_DATA_DATA;

    return SL_SUCCESS;
}

DLL_EXPORT SL_RETURN_CODE sl_validate_wave_data(SL_WAV_FILE* wavBuf) {
    switch (wavBuf->dataChunk.pcmType) {
        case SL_UNSIGNED_8PCM:
        case SL_SIGNED_16PCM:
        case SL_SIGNED_32PCM:
        case SL_FLOAT_32PCM:
        case SL_FLOAT_64PCM:
            if (wavBuf->dataChunk.dataChunkSize % 2 != 0) return SL_INVALID_CHUNK_DATA_DATA;
            break;
        case SL_SIGNED_24PCM:
            if (wavBuf->dataChunk.dataChunkSize % 3 != 0) return SL_INVALID_CHUNK_DATA_DATA;
            break;
        default:
            return SL_INVALID_CHUNK_FMT_AUDIO_FORMAT;
    }
    return SL_SUCCESS;
}

DLL_EXPORT SL_RETURN_CODE sl_ensure_wave_endianness(SL_WAV_FILE* wavBuf) {
    volatile SLbool endianness = get_native_endianness();
    if(endianness != SL_LITTLE_ENDIAN) {
        switch (wavBuf->dataChunk.pcmType) {
            case SL_UNSIGNED_8PCM:
                break;
            case SL_SIGNED_16PCM: {
                SLshort* data = (SLshort*) wavBuf->dataChunk.waveformData;
                SLullong len = wavBuf->dataChunk.dataChunkSize / 2;
                for(SLullong i = 0; i < len; ++i) data[i] = sl_flip_endian_short(data[i]);

                break;
            }
            case SL_SIGNED_24PCM: {
                SLchar* data = (SLchar*) wavBuf->dataChunk.waveformData;
                SLullong len = wavBuf->dataChunk.dataChunkSize / 3;
                for(SLullong i = 0; i < len; ++i) {
                    SLchar temp = data[i*3];
                    data[i*3] = data[i*3+2];
                    data[i*3+2] = temp;
                }
                break;
            }
            case SL_SIGNED_32PCM: {
                SLint* data = (SLint*) wavBuf->dataChunk.waveformData;
                SLullong len = wavBuf->dataChunk.dataChunkSize / 4;
                for(SLullong i = 0; i < len; ++i) data[i] = sl_flip_endian_int(data[i]);

                break;
            }
            case SL_FLOAT_32PCM: {
                SLfloat* data = (SLfloat*) wavBuf->dataChunk.waveformData;
                SLullong len = wavBuf->dataChunk.dataChunkSize / 4;
                for(SLullong i = 0; i < len; ++i) data[i] = sl_flip_endian_float(data[i]);

                break;
            }
            case SL_FLOAT_64PCM: {
                SLdouble* data = (SLdouble*) wavBuf->dataChunk.waveformData;
                SLullong len = wavBuf->dataChunk.dataChunkSize / 8;
                for(SLullong i = 0; i < len; ++i)     data[i] = sl_flip_endian_double(data[i]);

                break;
            }
            default:
                return SL_INVALID_CHUNK_FMT_AUDIO_FORMAT;
        }
    }
    return SL_SUCCESS;
}

DLL_EXPORT SL_RETURN_CODE sl_is_wave_file(SLstr path) {
    SLstr wavExtension = ".wav";
    SLstr waveExtension = ".wave";
    SLullong path_len = strlen(path);
    SLullong wavExtensionLen = strlen(wavExtension);
    SLullong waveExtensionLen = strlen(waveExtension);

    if (wavExtensionLen > path_len && waveExtensionLen > path_len) return SL_FAIL;

    SL_RETURN_CODE res = (strcmp(wavExtension, path + path_len - wavExtensionLen) == 0 ||
                  strcmp(waveExtension, path + path_len - waveExtensionLen) == 0)
                 ? SL_SUCCESS
                 : SL_FAIL;

    return res;
}

DLL_EXPORT SLushort sl_buf_to_native_ushort(const SLuchar* buf, SLullong bufLen) {
    //who needs comments, am i right?
    if(buf == NULL || bufLen < 2) return 0;

    SLuint value;
    volatile SLbool endianness = get_native_endianness();
    if(endianness == SL_LITTLE_ENDIAN) value = buf[0] | (buf[1] << 8);
    else value = buf[1] | (buf[0] << 8);

    return value;
}

DLL_EXPORT SLuint sl_buf_to_native_uint(const SLuchar* buf, SLullong bufLen) {
    if(buf == NULL || bufLen < 4) return 0;

    SLuint value;
    volatile SLbool endianness = get_native_endianness();
    if(endianness == SL_LITTLE_ENDIAN) value = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
    else value = buf[3] | (buf[2] << 8) | (buf[1] << 16) | (buf[0] << 24);

    return value;
}

DLL_EXPORT SLshort sl_flip_endian_short(SLshort s) {
    return (s >> 8) | (s << 8);
}

DLL_EXPORT SLint sl_flip_endian_int(SLint i) {
    return ((i >> 24) & 0xff) |
           ((i << 8) & 0xff0000) |
           ((i >> 8) & 0xff00) |
           ((i << 24) & 0xff000000);
}

DLL_EXPORT SLfloat sl_flip_endian_float(SLfloat f) {
    // i love c because of this right here
    unsigned long num_int;
    memcpy(&num_int, &f, sizeof(float));

    unsigned long swapped_int = ((num_int >> 24) & 0xff) |
                                ((num_int << 8) & 0xff0000) |
                                ((num_int >> 8) & 0xff00) |
                                ((num_int << 24) & 0xff000000);
    float swapped;
    memcpy(&swapped, &swapped_int, sizeof(float));
    return swapped;
}

DLL_EXPORT SLdouble sl_flip_endian_double(SLdouble d) {
    // i love c because of this right here
    uint64_t num_int;
    memcpy(&num_int, &d, sizeof(double));

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

////////////////////////////////////////////////////
///////////////// OpenAL Wrapper ///////////////////
////////////////////////////////////////////////////

#ifdef SL_OPENAL_WRAPPER

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/alut.h>

////////////////////////////////////////////////////////////////
///////////////// Wrapper Struct Definitions ///////////////////
////////////////////////////////////////////////////////////////

DLL_EXPORT typedef struct sl_sound {
    SL_WAV_FILE* waveBuf;

    ALCdevice* device;
    ALCcontext* context;
    ALuint source;
    ALuint buffer;
    ALsizei size;
    ALsizei freq;
    ALenum format;
    ALfloat duration;
    ALfloat pitch; // in % so 1.0 is 100%, 0.5 is 50% and so on.
    ALfloat gain; // in % so 1.0 is 100%, 0.5 is 50% and so on.
} SL_SOUND;

//////////////////////////////////////////////////////////////////
///////////////// Wrapper Function Definitions ///////////////////
//////////////////////////////////////////////////////////////////

/**
 * @brief Lowest level of playing a sound that this wrapper supports.
 * @param sound - Sound to play.
 * @param device - Device to play sound to.
 * @return SL_SUCCESS if succeeded. SL_FAIL otherwise.
 */
DLL_EXPORT SL_RETURN_CODE sl_play_sound(SL_SOUND* sound, SLstr device);

/**
 * @brief Lower level way of playing a sound.
 * This function requires you to manually generate the sound you want to play.
 * Does NOT clean up after itself. Call sl_cleanup_sound() after using this.
 * @param sound - Sound to play.
 * @return SL_SUCCESS if succeeded. SL_FAIL otherwise.
 */
DLL_EXPORT SL_RETURN_CODE sl_play_sound_b(SL_SOUND* sound);

/**
 * @brief Plays sound in highest level way.
 * This function parses the WAVE file at the specified path, generates a SL_SOUND, plays the sound, and frees the memory for you.
 * @param path - Path of WAVE file to parse and play.
 * @param device - Device to play the audio to.
 * @param gain - Control the volume of the sound. (1.f is 100%, 0.5 is 50% and so on)
 * @param pitch - Control the speed/pitch of the sound. (1.f is 100%, 0.5 is 50% and so on)
 * @return SL_SUCCESS if succeeded. SL_FAIL otherwise.
 */
DLL_EXPORT SL_RETURN_CODE sl_play_sound_c(SLstr path, SLstr device, float gain, float pitch);

/**
 * @brief Parses sound format for specified sound.
 * @param sound - Sound to parse sound format for.
 * @return SL_SUCCESS if proeprly parsed. SL_FAIL otherwise.
 */
DLL_EXPORT SL_RETURN_CODE sl_parse_sound_format(SL_SOUND* sound);

/**
 * @brief Parses PCM type of sound to mono (1 channel) for specified sound.
 * This is a helper function for generating sounds.
 * @param sound - Sound to parse PCM type for.
 * @return SL_SUCCESS if parsed correctly. SL_FAIL otherwise.
 */
DLL_EXPORT SL_RETURN_CODE sl_parse_mono(SL_SOUND* sound);

/**
 * @brief Parses PCM type of sound to stereo (2 channels) for specified sound.
 * This is a helper function for generating sounds.
 * @param sound - Sound to parse PCM type for.
 * @return SL_SUCCESS if parsed correctly. SL_FAIL otherwise.
 */
DLL_EXPORT SL_RETURN_CODE sl_parse_stereo(SL_SOUND* sound);

/**
 * @brief Parses PCM type of sound to QUAD (4 channels) for specified sound.
 * This is a helper function for generating sounds.
 * @param sound - Sound to parse PCM type for.
 * @return SL_SUCCESS if parsed correctly. SL_FAIL otherwise.
 */
DLL_EXPORT SL_RETURN_CODE sl_parse_quad(SL_SOUND* sound);

/**
 * @brief Parses PCM type of sound to 5.1 (6 channels) for specified sound.
 * This is a helper function for generating sounds.
 * @param sound - Sound to parse PCM type for.
 * @return SL_SUCCESS if parsed correctly. SL_FAIL otherwise.
 */
DLL_EXPORT SL_RETURN_CODE sl_parse_51(SL_SOUND* sound);

/**
 * @brief Parses PCM type of sound to 6.1 (7 channels) for specified sound.
 * This is a helper function for generating sounds.
 * @param sound - Sound to parse PCM type for.
 * @return SL_SUCCESS if parsed correctly. SL_FAIL otherwise.
 */
DLL_EXPORT SL_RETURN_CODE sl_parse_61(SL_SOUND* sound);

/**
 * @brief Parses PCM type of sound to 7.1 (8 channels) for specified sound.
 * This is a helper function for generating sounds.
 * @param sound - Sound to parse PCM type for.
 * @return SL_SUCCESS if parsed correctly. SL_FAIL otherwise.
 */
DLL_EXPORT SL_RETURN_CODE sl_parse_71(SL_SOUND* sound);

/**
 * @brief Stops the sound if it is currently playing and cleans up OpenAL related things.
 * This does not free any other sound things. It simply stops the sound and cleans up OpenAL stuff.
 * @param sound - Sound to stop.
 */
DLL_EXPORT void sl_stop_sound(SL_SOUND* sound);

/**
 * @brief Cleans up the sound.
 * Make sure to call this if you are doing things manually.
 * @param sound - Sound to clean up.
 */
DLL_EXPORT void sl_cleanup_sound(SL_SOUND* sound);

/**
 * @brief Generates a SL_SOUND from the provided parameters and stores it in the provided buffer.
 * @param sound - Buffer for the sound.
 * @param waveBuf - WAVE buffer for the sound.
 * @param gain - Control the volume of the sound. (1.f is 100%, 0.5 is 50% and so on)
 * @param pitch - Control the speed/pitch of the sound. (1.f is 100%, 0.5 is 50% and so on)
 * @return SL_SUCCESS if everything went right. Anything else means something happened.
 */
DLL_EXPORT SL_RETURN_CODE sl_gen_sound_a(SL_SOUND* sound, SL_WAV_FILE* waveBuf, SLfloat gain, SLfloat pitch);

/**
 * @brief Generates a SL_SOUND from the provided parameters and stores it in the provided buffer.
 * @param sound - Buffer for the sound.
 * @param path - Path to the sound. Sound MUST be a WAVE file.
 * @param gain - Control the volume of the sound. (1.f is 100%, 0.5 is 50% and so on)
 * @param pitch - Control the speed/pitch of the sound. (1.f is 100%, 0.5 is 50% and so on)
 * @return SL_SUCCESS if everything went right. Anything else means something happened.
 */
DLL_EXPORT SL_RETURN_CODE sl_gen_sound(SL_SOUND* sound, SLstr path, SLfloat gain, SLfloat pitch);

/**
 * @brief Returns an array of audio devices.
 * @return SLstr* array of audio devices.
 */
DLL_EXPORT SLstr* sl_get_devices(void);

/**
 * @brief Frees the device list that is returned by sl_get_devices(void).
 * Must be called on the device list that was returned to free it.
 * @param devices - Device list to free.
 */
DLL_EXPORT void sl_destroy_device_list(SLstr** devices);

//////////////////////////////////////////////////////////////////////
///////////////// Wrapper Function Implementations ///////////////////
//////////////////////////////////////////////////////////////////////

DLL_EXPORT SL_RETURN_CODE sl_play_sound(SL_SOUND* sound, SLstr device) {

    if(sound == NULL) return SL_FAIL;
    // Initialize OpenAL
    sound->device = alcOpenDevice(device);
    sound->context = alcCreateContext(sound->device, NULL);
    alcMakeContextCurrent(sound->context);

    // Generate a buffer
    alGenBuffers(1, &sound->buffer);

    //Buffer stuff to data
    alBufferData(sound->buffer, sound->format, sound->waveBuf->dataChunk.waveformData, sound->size, sound->freq);

    // Generate a source
    if (sound->source) alDeleteSources(1, &sound->source);
    alGenSources(1, &sound->source);

    // Set the pitch
    alSourcef(sound->source, AL_PITCH, sound->pitch);

    // Set the gain
    alSourcef(sound->source, AL_GAIN, sound->gain);


    // Queue the buffer for playback
    alSourceQueueBuffers(sound->source, 1, &sound->buffer);

    // Start playback
    alSourcePlay(sound->source);

    // Wait for playback to finish
    alutSleep(sound->duration);

    //ensure it's done playing
    ALint state;
    do {
        alGetSourcei(sound->source, AL_SOURCE_STATE, &state);
    } while (state == AL_PLAYING);

    // cleanup
    sl_stop_sound(sound);

    return SL_SUCCESS;
}

DLL_EXPORT SL_RETURN_CODE sl_play_sound_b(SL_SOUND* sound) {
    return sl_play_sound(sound, NULL);
}

DLL_EXPORT SL_RETURN_CODE sl_play_sound_c(SLstr path, SLstr device, float gain, float pitch) {
    SL_SOUND sound;
    SL_WAV_FILE buf;
    SL_RETURN_CODE out = sl_read_wave_file(path, &buf);

    if (out != SL_SUCCESS) return out;

    out = sl_gen_sound_a(&sound, &buf, gain, pitch);

    if (out != SL_SUCCESS) {
        sl_cleanup_wave_file(&buf);
        return out;
    }

    out = sl_play_sound(&sound, device);

    sl_cleanup_sound(&sound);

    return out;
}

DLL_EXPORT SL_RETURN_CODE sl_parse_sound_format(SL_SOUND* sound) {
    SL_RETURN_CODE ret;

    switch(sound->waveBuf->formatChunk.numChannels) {
        case 1: {
            ret = sl_parse_mono(sound);
            break;
        }
        case 2: {
            ret = sl_parse_stereo(sound);
            break;
        }
        case 4: {
            ret = sl_parse_quad(sound);
            break;
        }
        case 6: {
            ret = sl_parse_51(sound);
            break;
        }
        case 7: {
            ret = sl_parse_61(sound);
            break;
        }
        case 8: {
            ret = sl_parse_71(sound);
            break;
        }
        default: {
            ret = SL_FAIL;
            break;
        }
    }

    return ret;
}

DLL_EXPORT SL_RETURN_CODE sl_parse_mono(SL_SOUND* sound) {
    switch(sound->waveBuf->dataChunk.pcmType) {
        case SL_UNSIGNED_8PCM: {
            sound->format = AL_FORMAT_MONO8;
            break;
        }
        case SL_SIGNED_16PCM: {
            sound->format = AL_FORMAT_MONO16;
            break;
        }
        case SL_FLOAT_32PCM: {
            sound->format = AL_FORMAT_MONO_FLOAT32;
            break;
        }
        case SL_FLOAT_64PCM: {
            sound->format = AL_FORMAT_MONO_DOUBLE_EXT;
            break;
        }
        default:
            return SL_FAIL;
    }
    return SL_SUCCESS;
}

DLL_EXPORT SL_RETURN_CODE sl_parse_stereo(SL_SOUND* sound) {
    switch(sound->waveBuf->dataChunk.pcmType) {
        case SL_UNSIGNED_8PCM: {
            sound->format = AL_FORMAT_STEREO8;
            break;
        }
        case SL_SIGNED_16PCM: {
            sound->format = AL_FORMAT_STEREO16;
            break;
        }
        case SL_FLOAT_32PCM: {
            sound->format = AL_FORMAT_STEREO_FLOAT32;
            break;
        }
        case SL_FLOAT_64PCM: {
            sound->format = AL_FORMAT_STEREO_DOUBLE_EXT;
            break;
        }
        default:
            return SL_FAIL;
    }
    return SL_SUCCESS;
}

DLL_EXPORT SL_RETURN_CODE sl_parse_quad(SL_SOUND* sound) {
    switch(sound->waveBuf->dataChunk.pcmType) {
        case SL_UNSIGNED_8PCM: {
            sound->format = AL_FORMAT_QUAD8;
            break;
        }
        case SL_SIGNED_16PCM: {
            sound->format = AL_FORMAT_QUAD16;
            break;
        }
        case SL_SIGNED_32PCM: {
            sound->format = AL_FORMAT_QUAD32;
            break;
        }
        case SL_FLOAT_32PCM: {
            sound->format = AL_FORMAT_UHJ4CHN_FLOAT32_SOFT;
            break;
        }
        default:
            return SL_FAIL;
    }
    return SL_SUCCESS;
}

DLL_EXPORT SL_RETURN_CODE sl_parse_51(SL_SOUND* sound) {
    switch(sound->waveBuf->dataChunk.pcmType) {
        case SL_UNSIGNED_8PCM: {
            sound->format = AL_FORMAT_51CHN8;
            break;
        }
        case SL_SIGNED_16PCM: {
            sound->format = AL_FORMAT_51CHN16;
            break;
        }
        case SL_SIGNED_32PCM: {
            sound->format = AL_FORMAT_51CHN32;
            break;
        }
        default:
            return SL_FAIL;
    }
    return SL_SUCCESS;
}

DLL_EXPORT SL_RETURN_CODE sl_parse_61(SL_SOUND* sound) {
    switch(sound->waveBuf->dataChunk.pcmType) {
        case SL_UNSIGNED_8PCM: {
            sound->format = AL_FORMAT_61CHN8;
            break;
        }
        case SL_SIGNED_16PCM: {
            sound->format = AL_FORMAT_61CHN16;
            break;
        }
        case SL_SIGNED_32PCM: {
            sound->format = AL_FORMAT_61CHN32;
            break;
        }
        default:
            return SL_FAIL;
    }
    return SL_SUCCESS;
}

DLL_EXPORT SL_RETURN_CODE sl_parse_71(SL_SOUND* sound) {
    switch(sound->waveBuf->dataChunk.pcmType) {
        case SL_UNSIGNED_8PCM: {
            sound->format = AL_FORMAT_71CHN8;
            break;
        }
        case SL_SIGNED_16PCM: {
            sound->format = AL_FORMAT_71CHN16;
            break;
        }
        case SL_SIGNED_32PCM: {
            sound->format = AL_FORMAT_71CHN32;
            break;
        }
        default:
            return SL_FAIL;
    }
    return SL_SUCCESS;
}

DLL_EXPORT void sl_stop_sound(SL_SOUND* sound) {
    if (sound->source) {
        // Stop the source and delete the source
        alSourceStop(sound->source);
        alDeleteSources(1, &sound->source);
        sound->source = 0;
    }

    if (sound->buffer) {
        // Delete the buffer
        alDeleteBuffers(1, &sound->buffer);
        sound->buffer = 0;
    }

    if (sound->context) {
        // Destroy the context
        alcMakeContextCurrent(NULL);
        alcDestroyContext(sound->context);
        sound->context = NULL;
    }

    if (sound->device) {
        // Close the device
        alcCloseDevice(sound->device);
        sound->device = NULL;
    }
}

DLL_EXPORT void sl_cleanup_sound(SL_SOUND* sound) {
    if(sound != NULL) {
        //stop sound
        sl_stop_sound(sound);

        //free wav file
        sl_cleanup_wave_file(sound->waveBuf);
        sound->waveBuf = NULL;
    }
}

DLL_EXPORT SL_RETURN_CODE sl_gen_sound_a(SL_SOUND* sound, SL_WAV_FILE* waveBuf, SLfloat gain, SLfloat pitch) {

    if(waveBuf == NULL) return SL_FAIL;

    memset(sound, 0, sizeof(SL_SOUND));

    sound->waveBuf = waveBuf;
    sound->gain    = gain;
    sound->pitch   = pitch;
    sound->freq    = waveBuf->formatChunk.sampleRate;
    sound->size    = waveBuf->dataChunk.dataChunkSize;

    SLint denom = sound->freq * waveBuf->formatChunk.numChannels * (waveBuf->formatChunk.bitsPerSample / 8);
    sound->duration = ((sound->size / denom) / pitch) + 0.5;

    return sl_parse_sound_format(sound);
}

DLL_EXPORT SL_RETURN_CODE sl_gen_sound(SL_SOUND* sound, SLstr path, SLfloat gain, SLfloat pitch) {
    SL_WAV_FILE buf;
    SL_RETURN_CODE out = sl_read_wave_file(path, &buf);

    if (out == SL_SUCCESS) out = sl_gen_sound_a(sound, &buf, gain, pitch);

    return out;
}

DLL_EXPORT SLstr* sl_get_devices(void) {
    if (alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT") != AL_TRUE) return NULL;

    SLstr devices = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
    if (devices == NULL) return NULL;

    SLstr device = devices;

    SLullong numDevices = 0;
    while (*device != '\0') {
        device += strlen(device) + 1;
        numDevices++;
    }

    // Allocate memory for the array of device names
    SLstr* arr = (SLstr*)malloc((numDevices + 1) * sizeof(SLstr));
    if (arr == NULL) return NULL;

    // Copy the device names into the array
    device = devices;
    for (SLullong i = 0; i < numDevices; i++) {
        SLullong len = strlen(device);
        arr[i] = strdup(device);
        if (arr[i] == NULL) {
            // clean up and return NULL
            for (SLullong j = 0; j < i; j++) {
                free((void*)arr[j]);
            }
            free(arr);
            return NULL;
        }
        device += len + 1;
    }

    // Null-terminate the array
    arr[numDevices] = NULL;

    return arr;
}

DLL_EXPORT void sl_destroy_device_list(SLstr** devices) {
    if (devices != NULL && *devices != NULL) {
        // free device names
        for (SLullong i = 0; (*devices)[i] != NULL; i++) free((void*)((*devices)[i]));

        free(*devices);
        *devices = NULL;
    }
}

#endif // SL_OPENAL_WRAPPER

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //SAL_SAL_H