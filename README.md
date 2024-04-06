# Simple Audio Library (SAL)
This library is designed to be a simple way to parse and play WAVE files in C. It includes a basic WAVE file parser and an optional OpenAL wrapper.

[![windows](https://github.com/gwerrry/SAL/actions/workflows/windows.yml/badge.svg)](https://github.com/gwerrry/SAL/actions/workflows/windows.yml)
[![linux](https://github.com/gwerrry/SAL/actions/workflows/linux.yml/badge.svg)](https://github.com/gwerrry/SAL/actions/workflows/linux.yml)

## Including
SAL is a single include header. 
Simply copy `sal.h` in your project and you are good to go.   

- If you want to use the OpenAL wrapper and just want the WAVE file parser, you must use `OpenAL Soft` or another variation that includes the multi-channel `OpenAL` extension.
This library was tested using `OpenAL Soft`.

- If you do not want to use the `OpenAL` wrapper, comment out the line found near the top of the header file:
```c
#define SL_OPENAL_WRAPPER
```  

- If you want to use sal as a DLL, you need to uncomment the line foundnear the top of the header file:
```c
// #define USE_DLL_LINKING // un-comment this if you want to use DLL linking
```

> Note that the filename `sal.h` can cause issues with linking on Windows in certain cases. To fix this, rename `sal.h` to something else.

## Supported Sound formats
SAL only supports standard WAVE files in little-endian.

### PCM types supported by the parser:

- ```8 bit unsigned int```
- ```16 bit signed int```
- ```24 bit signed int```
- ```32 bit signed int```
- ```32 bit float```
- ```64 bit float```

### PCM types and channel setups supported by the ```OpenAL``` wrapper:

#### 8 bit unsigned int
- ```Mono```
- ```Stereo```
- ```Quad (4 channels)```
- ```5.1 (6 channels)```
- ```6.1 (7 channels)```
- ```7.1 (8 channels)```

#### 16 bit signed int
- ```Mono```
- ```Stereo```
- ```Quad (4 channels)```
- ```5.1 (6 channels)```
- ```6.1 (7 channels)```
- ```7.1 (8 channels)```

#### 32 bit signed int
- ```Quad (4 channels)```
- ```5.1 (6 channels)```
- ```6.1 (7 channels)```
- ```7.1 (8 channels)```

#### 32 bit float
- ```Mono```
- ```Stereo```
- ```Quad (4 channels)```

#### 64 bit float
- ```Mono```
- ```Stereo```

## Usage

### Types
SAL uses some custom typenames.
You can ignore them in most use cases and use the STL counterpart.

```
SLbool   🠲 int8_t
SLchar   🠲 int8_t
SLuchar  🠲 uint8_t
SLshort  🠲 int16_t
SLushort 🠲 uint16_t
SLint    🠲 int32_t
SLuint   🠲 uint32_t
SLenum   🠲 uint32_t
SLullong 🠲 uint64_t
SLfloat  🠲 float
SLdouble 🠲 double
SLstr    🠲 const char*
SLvoid   🠲 void*
```

### SAL Functions to care about
If you want to know ALL of the functions, there are clearly commented sections in the header file.   
This section will provide all the functions the user (YOU) should use.

```c
/////////////////////////////////////////////////////////////////////
///////////////// Simple Audio Library Global stuff /////////////////
/////////////////////////////////////////////////////////////////////

// get sal version
DLL_EXPORT SLstr sl_get_version() {
    return SAL_VERSION;
}

//////////////////////////////////////////////////////
///////////////// Parser Functions ///////////////////
//////////////////////////////////////////////////////

// Used to parse the WAVE file at the specified path.
// Takes a path and a buffer.
// Returns SL_SUCCESS if succeeded.
static SLenum sl_read_wave_file(SLstr path, SL_WAV_FILE* wavBuf);

// Used to free the memory allocated for the WAVE file.
static void sl_cleanup_wave_file(SL_WAV_FILE* wavBuf);

// Returns SL_SUCCESS if the file has a proper wav extension. SL_FAIL otherwise.
static SLenum sl_is_wave_file(SLstr path); 

///////////////////////////////////////////////////////
///////////////// Wrapper Functions ///////////////////
///////////////////////////////////////////////////////

// Plays the specified sound at the specified device. Use NULL for default device.
// Returns SL_SUCCESS if everything went right. Anything else means something happened.
static SLenum sl_play_sound(SL_SOUND* sound, SLstr device);

// Does the same thing as the above function except it chooses the default device.
// Returns SL_SUCCESS if everything went right. Anything else means something happened.
static SLenum sl_play_sound_b(SL_SOUND* sound);

// High level function that parses the WAVE file at the specified path, generates a SL_SOUND, plays the sound, and frees the memory for you.
// Use this if you want the easy way out.
// Returns SL_SUCCESS if everything went right. Anything else means something happened.
static SLenum sl_play_sound_c(SLstr path, SLstr device, float gain, float pitch);

// Stops the sound if it is currently playing and cleans up OpenAL related things. This does not free any other sound things. It simply stops the sound and cleans up OpenAL stuff.
static void sl_stop_sound(SL_SOUND* sound);

// Cleans up the sound. Make sure you call this to clean it up. It will free everything that needs to be freed for you.
static void sl_cleanup_sound(SL_SOUND* sound);

// Generates a SL_SOUND from the provided parameters and stores it in the provided SL_SOUND buffer.
// This function takes the wavBuf instead of path. It is more work for you :)
// Gain in pitch are in percent so 1.0 is 100%, 0.5 is 50% and so on.
// Returns SL_SUCCESS if everything went right. Anything else means something happened.
static SLenum sl_gen_sound_a(SL_SOUND* sound, SL_WAV_FILE* waveBuf, SLfloat gain, SLfloat pitch);

// Generates a SL_SOUND from the provided parameters and stores it in the provided buffer.
// Gain in pitch are in percent so 1.0 is 100%, 0.5 is 50% and so on.
// Returns SL_SUCCESS if everything went right. Anything else means something happened.
static SLenum sl_gen_sound(SL_SOUND* sound, SLstr path, SLfloat gain, SLfloat pitch);

// Returns an array of SLstr audio devices.
// The array returned is NULL at the end, so just loop until null when using this. This can return just NULL if something goes wrong.
static SLstr* sl_get_devices(void);

// Frees the device list that is returned by sl_get_devices(void).
// Must be called on the device list that was returned to free it. You could do it yourself, but this makes it easy.
static void sl_destroy_device_list(SLstr** devices);
```
## Examples
You can find usage examples in [test.c](test.c).

## License

This uses the Apache License, Version 2.0. Information can be found in the [license file](LICENSE).
