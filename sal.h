#ifndef SAL_SAL_H
#define SAL_SAL_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

    //todo pad structs before release
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////// Controls if you want to use the OpenAL wrapper. Comment it out if you don't want to use it. ///////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SL_OPENAL_WRAPPER

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////// Include Platform Specific Headers and define some platform specific things. ///////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
#define SLvoid void*

////////////////////////////////////////////////////////////////////////////////
///////////////// Tells the native endian-ness of the system ///////////////////
////////////////////////////////////////////////////////////////////////////////
static SLbool sysEndianness = 0;

/////////////////////////////////////////////////////////////////////////////
///////////////// Simple Audio Library Global Definitions ///////////////////
/////////////////////////////////////////////////////////////////////////////

#define BIG_ENDIAN 1
#define LITTLE_ENDIAN 0
#define SL_SUCCESS 69420
#define SL_FAIL 66666
#define SL_INVALID_VALUE 61616

/////////////////////////////////////////////////////////////////////////////////////
///////////////// Simple Audio Library Global Function Defintions ///////////////////
/////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Sound library init function. You MUST call this even when using the wave file parser.
 * @return SL_SUCCESS if succeeded. If anything else is returned, the function failed.
 */
static SLenum sl_init(void);

/**
 * @brief Sound library cleanup function. You should call this when your app execution ends.
 * @return SL_SUCCESS if succeeded. If anything else is returned, the function failed.
 */
static SLenum sl_cleanup(void);

//////////////////////////////////////////////////////////////////////////////////////////
///////////////// Simple Audio Library Global Function Implementations ///////////////////
//////////////////////////////////////////////////////////////////////////////////////////
SLenum sl_init(void) {
    //quickly check the endianness of the system
    int n = 1;
    sysEndianness = *(char*)&n == 1 ? LITTLE_ENDIAN : BIG_ENDIAN;

    return SL_SUCCESS;
}

SLenum sl_cleanup(void) {

    return SL_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
///////////////// Wave File Parser Return Code Definitions ///////////////////
//////////////////////////////////////////////////////////////////////////////

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
    SLbool usingRIFX;
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
    SLvoid  waveformData;
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
 * @brief Parses wave file at the path.
 * For more information on types of WAVE files supported, go to the Github.
 * @param path - Path of WAVE file to parse.
 * @param wavBufPtr - Buffer for the WAVE file.
 * @return SL_SUCCESS if succeeded. There are many useful return codes documented on the Github so you can debug anything else that happens.
 */
static SLenum sl_read_wave_file(SLstr path, SL_WAV_FILE** wavBufPtr);

/**
 * @brief Reads WAVE descriptor chunk. This is a helper function and should not be used except by SAL.
 * @param file - File ptr to WAVE file.
 * @param wavBuf - Buffer for the WAVE file.
 * @return SL_SUCCESS if it succeeded. Anything else means a failure.
 */
static SLenum sl_read_wave_descriptor(FILE* file, SL_WAV_FILE* wavBuf);

/**
 * @brief Parses WAVE chunks. This is a helper function and should not be used except by SAL.
 * @param file - File ptr to WAVE file.
 * @param wavBuf - Buffer for the WAVE file.
 * @return SL_SUCCESS if it succeeded. Anything else means a failure.
 */
static SLenum sl_parse_wave_chunks(FILE* file, SL_WAV_FILE* wavBuf);

/**
 * @brief Reads WAVE format chunk. This is a helper function and should not be used except by SAL.
 * @param file - File ptr to WAVE file.
 * @param wavBuf - Buffer for the WAVE file.
 * @return SL_SUCCESS if it succeeded. Anything else means a failure.
 */
static SLenum sl_read_wave_format_chunk(FILE* file, SL_WAV_FILE* wavBuf);

/**
 * @brief Reads WAVE data chunk. This is a helper function and should not be used except by SAL.
 * @param file - File ptr to WAVE file.
 * @param wavBuf - Buffer for the WAVE file.
 * @return SL_SUCCESS if it succeeded. Anything else means a failure.
 */
static SLenum sl_read_wave_data_chunk(FILE* file, SL_WAV_FILE* wavBuf);

/**
 * @brief Ensures WAVE data ends on a proper byte boundary. This is a helper function and should not be used except by SAL.
 * @param wavBuf - Buffer for the WAVE file.
 * @return SL_SUCCESS if it succeeded. Anything else means a failure.
 */
static SLenum sl_validate_wave_data(SL_WAV_FILE* wavBuf);

/**
 * @brief Ensures WAVE data endian-ness is correct. This is a helper function and should not be used except by SAL.
 * @param wavBuf - Buffer for the WAVE file.
 * @return SL_SUCCESS if it succeeded. Anything else means a failure.
 */
static SLenum sl_ensure_wave_endianness(SL_WAV_FILE* wavBuf);

/**
 * @brief Frees the memory associated with the WAVE file.
 * Also sets the buf to NULL.
 * @param buf - Buffer of WAVE file to free.
 */
static void sl_free_wave_file(SL_WAV_FILE** bufPtr);

/**
 * @brief This is just for checking if the path provided ends with ".wav" or ".wave".
 * @param str - Path to check.
 * @return SL_SUCCESS the file has the proper wav extension. SL_FAIL otherwise.
 */
static SLenum sl_is_wave_file(SLstr path);

/**
 * @brief Converts a SLuchar* buf to a SLushort, but takes into account the system's native endian-ness.
 * @param buf - Buffer to convert. This MUST be at least 2 bytes and valid.
 * @param bufLen - Length of SLuchar buffer to convert.
 * @return The value of the buffer represented as a SLushort taking into account the system's native endian-ness. Returns zero if it fails. I know it's not helpful, but you can debug!
 */
static SLushort sl_buf_to_native_ushort(const SLuchar* buf, SLullong bufLen);

/**
 * @brief Converts a SLuchar* buf to a SLuint, but takes into account the system's native endian-ness.
 * @param buf - Buffer to convert. This MUST be at least 4 bytes and valid.
 * @param bufLen - Length of SLuchar buffer to convert.
 * @return The value of the buffer represented as a SLuint taking into account the system's native endian-ness. Returns zero if it fails. I know it's not helpful, but you can debug!
 */
static SLuint sl_buf_to_native_uint(const SLuchar* buf, SLullong bufLen);

/**
 * @brief Flips the endian-ness of a SLshort.
 * @param s SLshort to flip.
 * @return SLshort with the endian-ness flipped.
 */
static SLshort sl_flip_endian_short(SLshort s);

/**
 * @brief Flips the endian-ness of a SLint.
 * @param i SLint to flip.
 * @return SLint with the endian-ness flipped.
 */
static SLint sl_flip_endian_int(SLint i);

/**
 * @brief Flips the endian-ness of a SLfloat.
 * @param f SLfloat to flip.
 * @return SLfloat with the endian-ness flipped.
 */
static SLfloat sl_flip_endian_float(SLfloat f);

/**
 * @brief Flips the endian-ness of a SLdouble.
 * @param d SLdouble to flip.
 * @return SLdouble with the endian-ness flipped.
 */
static SLdouble sl_flip_endian_double(SLdouble d);

///////////////////////////////////////////////////////////////////////////////
///////////////// Wave File Parser Function Implementations ///////////////////
///////////////////////////////////////////////////////////////////////////////

//TODO test rifx support.
SLenum sl_read_wave_file(SLstr path, SL_WAV_FILE** wavBufPtr) {
    SLenum ret = SL_SUCCESS;

    SL_WAV_FILE* wavBuf = NULL;
    FILE* file;

    //ensure pointers are good were just going to assume the user allocated stuff right
    if (!path) {
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

    // Allocate buf
    *wavBufPtr = (SL_WAV_FILE*)calloc(1, sizeof(SL_WAV_FILE));
    if (!(*wavBufPtr)) {
        ret = SL_FAIL;
        goto bufCleanup;
    }

    wavBuf = *wavBufPtr;

    //
    // Assign some stuff since we know buf has data now.
    //
    wavBuf->dataChunk.pcmType = 0;
    wavBuf->formatChunk.extensionSize = 0;
    wavBuf->dataChunk.waveformData = NULL;

    ret = sl_read_wave_descriptor(file, wavBuf);
    if(ret != SL_SUCCESS)
        goto bufCleanup;

    ret = sl_parse_wave_chunks(file, wavBuf);
    if(ret != SL_SUCCESS)
        goto bufCleanup;

    ret = sl_validate_wave_data(wavBuf);
    if(ret != SL_SUCCESS)
        goto bufCleanup;

    ret = sl_ensure_wave_endianness(wavBuf);
    if(ret != SL_SUCCESS)
        goto bufCleanup;

    if (wavBuf->dataChunk.waveformData) goto fileCleanup;
    else {
        ret = SL_FAIL;
        goto bufCleanup;
    }

    bufCleanup:
        if (wavBuf) {
            free(wavBuf->dataChunk.waveformData);
            wavBuf->dataChunk.waveformData = NULL;
        }
        free(wavBuf);
       *wavBufPtr = NULL;
    fileCleanup:
        fclose(file);
    exit:
        return ret;
}

SLenum sl_read_wave_descriptor(FILE* file, SL_WAV_FILE* wavBuf) {
    const SLuchar riffID_bytes[4] = {0x52, 0x49, 0x46, 0x46};
    const SLuchar rifxID_bytes[4] = {0x52, 0x49, 0x46, 0x58};
    const SLuchar waveID_bytes[4] = {0x57, 0x41, 0x56, 0x45};
    SLuchar buffer4[4] = {0x00, 0x00, 0x00, 0x00};
    SLullong blocksRead;

    //read and validate chunkID
    blocksRead = fread(wavBuf->descriptorChunk.descriptorId, 4, 1, file);
    if (!blocksRead)
        return SL_INVALID_CHUNK_DESCRIPTOR_ID;

    if (memcmp(wavBuf->descriptorChunk.descriptorId, riffID_bytes, 4) != 0)
        wavBuf->descriptorChunk.usingRIFX = 0;
    else if (memcmp(wavBuf->descriptorChunk.descriptorId, rifxID_bytes, 4) != 0)
        wavBuf->descriptorChunk.usingRIFX = 1;
    else
        return SL_INVALID_CHUNK_DESCRIPTOR_ID;


    //read and validate chunk size
    blocksRead = fread(buffer4, 4, 1, file);
    wavBuf->descriptorChunk.chunkSize = sl_buf_to_native_uint(buffer4, 4);
    if (!blocksRead ||wavBuf->descriptorChunk.chunkSize == 0)
        return SL_INVALID_CHUNK_DESCRIPTOR_SIZE;


    //read and validate wave format
    blocksRead = fread(wavBuf->descriptorChunk.chunkFormat, 4, 1, file);
    if (!blocksRead || memcmp(wavBuf->descriptorChunk.chunkFormat, waveID_bytes, 4) != 0)
        return SL_INVALID_CHUNK_DESCRIPTOR_FORMAT;

    return SL_SUCCESS;
}

SLenum sl_parse_wave_chunks(FILE* file, SL_WAV_FILE* wavBuf) {
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

            SLuint ret = sl_read_wave_format_chunk(file, wavBuf);
            if(ret != SL_SUCCESS)
                return ret;
        }

        // DATA CHUNK
        else if(!foundData && memcmp(buffer4, dataID_bytes, 4) == 0) {
            foundMatch = 1;
            foundData = 1;

            //store data id
            memcpy(wavBuf->dataChunk.dataId, buffer4, 4);
            SLuint ret = sl_read_wave_data_chunk(file, wavBuf);
            if(ret != SL_SUCCESS)
                return ret;
        }

        if(foundFmt && foundData) break;

        if(foundMatch) {
            blocksRead = fread(buffer4, 4, 1, file);
        } else {
            SLullong size;
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

SLenum sl_read_wave_format_chunk(FILE* file, SL_WAV_FILE* wavBuf) {
    SLuchar buffer4[4] = {0x00, 0x00, 0x00, 0x00};
    SLuchar buffer2[2] = {0x00, 0x00};
    SLullong blocksRead;

    //read and validate fmt chunk size
    blocksRead = fread(buffer4, 4, 1, file);
    wavBuf->formatChunk.subChunk1Size = sl_buf_to_native_uint(buffer4, 4);
    if (!blocksRead || wavBuf->formatChunk.subChunk1Size == 0)
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

SLenum sl_read_wave_data_chunk(FILE* file, SL_WAV_FILE* wavBuf) {
    SLuchar buffer4[4] = {0x00, 0x00, 0x00, 0x00};
    SLullong blocksRead;
    //read data chunk size
    blocksRead = fread(buffer4, 4, 1, file);
    wavBuf->dataChunk.subChunk2Size = sl_buf_to_native_uint(buffer4, 4);
    if (!blocksRead || wavBuf->dataChunk.subChunk2Size == 0)
        return SL_INVALID_CHUNK_DATA_SIZE;

    wavBuf->dataChunk.waveformData = malloc(wavBuf->dataChunk.subChunk2Size);
    if (!wavBuf->dataChunk.waveformData)
        return SL_FAIL;

    // Ensure the buffer size is even
    blocksRead = fread(wavBuf->dataChunk.waveformData, wavBuf->dataChunk.subChunk2Size, 1, file);
    if (!blocksRead)
        return SL_INVALID_CHUNK_DATA_DATA;

    return SL_SUCCESS;
}

SLenum sl_validate_wave_data(SL_WAV_FILE* wavBuf) {
    switch (wavBuf->dataChunk.pcmType) {
        case SL_UNSIGNED_8PCM:
        case SL_SIGNED_16PCM:
        case SL_SIGNED_32PCM:
        case SL_FLOAT_32PCM:
        case SL_FLOAT_64PCM:
            if (wavBuf->dataChunk.subChunk2Size % 2 != 0)
                return SL_INVALID_CHUNK_DATA_DATA;
            break;
        case SL_SIGNED_24PCM:
            if (wavBuf->dataChunk.subChunk2Size % 3 != 0)
                return SL_INVALID_CHUNK_DATA_DATA;
            break;
        default:
            return SL_INVALID_CHUNK_FMT_AUDIO_FORMAT;
    }
    return SL_SUCCESS;
}

SLenum sl_ensure_wave_endianness(SL_WAV_FILE* wavBuf) {
    if(wavBuf->descriptorChunk.usingRIFX) {
        switch (wavBuf->dataChunk.pcmType) {
            case SL_UNSIGNED_8PCM:
                break;
            case SL_SIGNED_16PCM: {
                SLshort* data = (SLshort*) wavBuf->dataChunk.waveformData;
                SLullong len = wavBuf->dataChunk.subChunk2Size / 2;
                for(SLullong i = 0; i < len; ++i) {
                    data[i] = sl_flip_endian_short(data[i]);
                }
                break;
            }
            case SL_SIGNED_24PCM: {
                SLchar* data = (SLchar*) wavBuf->dataChunk.waveformData;
                SLullong len = wavBuf->dataChunk.subChunk2Size / 3;
                for(SLullong i = 0; i < len; ++i) {
                    SLchar temp = data[i*3];
                    data[i*3] = data[i*3+2];
                    data[i*3+2] = temp;
                }
                break;
            }
            case SL_SIGNED_32PCM: {
                SLint* data = (SLint*) wavBuf->dataChunk.waveformData;
                SLullong len = wavBuf->dataChunk.subChunk2Size / 4;
                for(SLullong i = 0; i < len; ++i) {
                    data[i] = sl_flip_endian_int(data[i]);
                }
                break;
            }
            case SL_FLOAT_32PCM: {
                SLfloat* data = (SLfloat*) wavBuf->dataChunk.waveformData;
                SLullong len = wavBuf->dataChunk.subChunk2Size / 4;
                for(SLullong i = 0; i < len; ++i) {
                    data[i] = sl_flip_endian_float(data[i]);
                }
                break;
            }
            case SL_FLOAT_64PCM: {
                SLdouble* data = (SLdouble*) wavBuf->dataChunk.waveformData;
                SLullong len = wavBuf->dataChunk.subChunk2Size / 8;
                for(SLullong i = 0; i < len; ++i) {
                    data[i] = sl_flip_endian_double(data[i]);
                }
                break;
            }
            default:
                return SL_INVALID_CHUNK_FMT_AUDIO_FORMAT;
        }
    }
    return SL_SUCCESS;
}

void sl_free_wave_file(SL_WAV_FILE** bufPtr) {
    if (*bufPtr) {
        SL_WAV_FILE* buf = *bufPtr;
        free(buf->dataChunk.waveformData);
        buf->dataChunk.waveformData = NULL;
        free(buf);
        *bufPtr = NULL;
    }
}

SLenum sl_is_wave_file(SLstr path) {
    SLstr wavExtension = ".wav";
    SLstr waveExtension = ".wave";
    SLullong path_len = strlen(path);
    SLullong wavExtensionLen = strlen(wavExtension);
    SLullong waveExtensionLen = strlen(waveExtension);

    if (wavExtensionLen > path_len && waveExtensionLen > path_len) {
        return SL_FAIL;
    }

    SLenum res = (strcmp(wavExtension, path + path_len - wavExtensionLen) == 0 ||
                  strcmp(waveExtension, path + path_len - waveExtensionLen) == 0)
                 ? SL_SUCCESS
                 : SL_FAIL;

    return res;
}

SLushort sl_buf_to_native_ushort(const SLuchar* buf, SLullong bufLen) {
    //who needs comments, am i right?
    if(!buf || bufLen < 2) return 0;

    SLuint value;
    if(sysEndianness == LITTLE_ENDIAN) {
        value = buf[0] | (buf[1] << 8);
    } else {
        value = buf[1] | (buf[0] << 8);
    }
    return value;
}

SLuint sl_buf_to_native_uint(const SLuchar* buf, SLullong bufLen) {
    if(!buf || bufLen < 4) return 0;

    SLuint value;
    if(sysEndianness == LITTLE_ENDIAN) {
        value = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
    } else {
        value = buf[3] | (buf[2] << 8) | (buf[1] << 16) | (buf[0] << 24);
    }
    return value;
}

SLshort sl_flip_endian_short(SLshort s) {
    // simple bitwise stuff to flip the endianness
    return (s >> 8) | (s << 8);
}

SLint sl_flip_endian_int(SLint i) {
    // simple bitwise stuff to flip the endianness
    return ((i >> 24) & 0xff) |
           ((i << 8) & 0xff0000) |
           ((i >> 8) & 0xff00) |
           ((i << 24) & 0xff000000);
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

/////////////////////////////////////////////////////////
///////////////// OpenAL soft Wrapper ///////////////////
/////////////////////////////////////////////////////////

#ifdef SL_OPENAL_WRAPPER

//todo remove if any unused
#include <AL/al.h>
#include <AL/al.h>
#include <AL/alext.h>
#include <AL/alut.h>

////////////////////////////////////////////////////////////////
///////////////// Wrapper Struct Definitions ///////////////////
////////////////////////////////////////////////////////////////
typedef struct sl_sound {
    SL_WAV_FILE* waveBuf;
    SLfloat volume; // in % so 1.0 is 100%, 0.5 is 50% and so on.
    SLfloat speed; // in % so 1.0 is 100%, 0.5 is 50% and so on.
    SLbool mono;
} SL_SOUND;

//////////////////////////////////////////////////////////////////
///////////////// Wrapper Function Definitions ///////////////////
//////////////////////////////////////////////////////////////////

//todo add comments

static SL_SOUND* sl_gen_sound(SLstr path, SLfloat volume, SLfloat speed, SLbool mono);

static SL_SOUND* sl_gen_sound_a(SL_WAV_FILE* waveBuf, SLfloat volume, SLfloat speed, SLbool mono);

static SLenum sl_play_sound(SL_SOUND* sound);

static SLstr* sl_get_devices(void);

static void sl_destroy_sound();

static void sl_destroy_device_list(SLstr** devices);
//////////////////////////////////////////////////////////////////////
///////////////// Wrapper Function Implementations ///////////////////
//////////////////////////////////////////////////////////////////////

SLstr* sl_get_devices(void) {
    if (alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT") != AL_TRUE) return NULL;

    SLstr devices = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
    if (!devices) return NULL;

    SLstr device = devices;

    SLullong numDevices = 0;
    while (*device != '\0') {
        device += strlen(device) + 1;
        numDevices++;
    }

    // Allocate memory for the array of device names
    SLstr* arr = (SLstr*)malloc((numDevices + 1) * sizeof(SLstr));
    if (!arr) return NULL;

    // Copy the device names into the array
    device = devices;
    for (SLullong i = 0; i < numDevices; i++) {
        SLullong len = strlen(device);
        arr[i] = strdup(device);
        if (!arr[i]) {
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

void sl_destroy_device_list(SLstr** devices) {
    if (devices && *devices) {
        // free device names
        for (SLullong i = 0; (*devices)[i] != NULL; i++) {
            free((void*)((*devices)[i]));  // Cast to void* cause const
        }

        free(*devices);
        *devices = NULL;
    }
}
#endif

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //SAL_SAL_H
