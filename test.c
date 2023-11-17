#include "sal.h"

//#define PARSER_TEST
#define AL_TEST
#ifdef PARSER_TEST
int main(void) {
    sl_init();

    printf("Is this system LITTLE endian... %s\n", sysEndianness == LITTLE_ENDIAN ? "yes" : "no");

    printf("What WAVE file would you like to load?\n");

    char *input = malloc(256 * sizeof(char));
    if (input == NULL) {
        printf("Error allocating memory!\n");
        sl_cleanup();
        return -1;
    }

    fgets(input, 256, stdin);
    input[strcspn(input, "\n")] = 0;

    SL_WAV_FILE *buf = NULL;
    SLenum out = sl_read_wave_file(input, &buf);

    if(out == SL_SUCCESS || buf) {
        printf("Successfully parsed WAVE file at \"%s\".\n", input);
        sl_free_wave_file(&buf);
    } else {
        printf("Failed to parse WAVE file at \"%s\".\n", input);
    }


    sl_cleanup();
    return out;
}

#elif defined(AL_TEST)
int main(void) {
    sl_init();
    SLstr* devices = sl_get_devices();
    if (devices) {
        for (SLullong i = 0; devices[i] != NULL; i++) {
            printf("Device %llu: %s\n", i, devices[i]);
        }
        sl_destroy_device_list(&devices);
    }

    printf("What WAVE file would you like to load?\n");

    char *input = malloc(256 * sizeof(char));
    if (input == NULL) {
        printf("Error allocating memory!\n");
        sl_cleanup();
        return -1;
    }

    fgets(input, 256, stdin);
    input[strcspn(input, "\n")] = 0;

    SL_WAV_FILE *buf = NULL;
    SLenum out = sl_read_wave_file(input, &buf);

    if(out == SL_SUCCESS && buf) {
        printf("Successfully parsed WAVE file at \"%s\".\n", input);
        SL_SOUND* sound = NULL;
        out = sl_gen_sound_a(&sound, buf, 1.0f, 1.0f);
        if(out == SL_SUCCESS && sound) {
            sl_play_sound_a(sound, NULL);
        }

        sl_destroy_sound(&sound);
    } else {
        printf("Failed to parse WAVE file at \"%s\".\n", input);
    }

    sl_cleanup();
    return out;
}
#endif