#include "sal.h"

//CHANGE THESE FOR WHAT TEST YOU WANT

#define PARSER_TEST
//#define AL_TEST
//#define SIMPLE_SOUND_TEST

#ifdef PARSER_TEST
int main(void) {
    sl_init();
    // get WAVE file path
    printf("What WAVE file would you like to load?\n");
    char path[256];
    fgets(path, 256, stdin);
    path[strcspn(path, "\n")] = 0;

    // load WAVE file
    SL_WAV_FILE buf;
    SLenum out = sl_read_wave_file(path, &buf);
    if(out == SL_SUCCESS) {
        printf("Successfully parsed WAVE file at \"%s\".\n", path);
        sl_cleanup_wave_file(&buf);
    } else {
        printf("Failed to parse WAVE file at \"%s\".\n", path);
    }
    
    sl_cleanup();
    return out;
}

#elif defined(AL_TEST)
int main(void) {
    sl_init();
    // get devices
    SLstr* devices = sl_get_devices();
    SLstr chosen_device = NULL;

    if (devices) {
        // Print the list of devices
        printf("List of sound devices: \n");
        for (SLullong i = 0; devices[i] != NULL; i++)
            printf("Device %llu: %s\n", i, devices[i]);

        // Ask the user to choose a device
        printf("\nWhat device would you like to play? (-1 for default): ");
        char device_num_str[20];
        fgets(device_num_str, 20, stdin);
        SLullong device_num = strtoull(device_num_str, NULL, 10);

        // Make a copy of the chosen device
        if (device_num != -1) chosen_device = strdup(devices[device_num]);

        // Clean up the device list
        sl_destroy_device_list(&devices);
    } else {
        printf("Failed to get devices.\n");
        return SL_FAIL;
    }

    // get WAVE file path
    printf("\nWhat WAVE file would you like to load?\n");
    char path[256];
    fgets(path, 256, stdin);
    path[strcspn(path, "\n")] = 0;

    // read WAVE file
    SL_WAV_FILE buf;
    SLenum out = sl_read_wave_file(path, &buf);
    if(out == SL_SUCCESS) {
        printf("Successfully parsed WAVE file at \"%s\".\n", path);

        //Generate and play sound
        SL_SOUND sound;
        out = sl_gen_sound_a(&sound, &buf, 1.0f, 1.0f);
        if(out == SL_SUCCESS) {
            printf("Successfully generated the sound.\n");
            out = sl_play_sound_a(&sound, chosen_device);
            printf(out == SL_SUCCESS ? "Successfully played the sound.\n" : "Failed to play the sound.\n");
        } else {
            printf("Failed to generate the sound.\n");
        }
    } else {
        printf("Failed to parse WAVE file at \"%s\".\n", path);
    }

    free((void*)chosen_device);
    sl_cleanup();
    return out;
}
#elif defined(SIMPLE_SOUND_TEST)
int main(void) {
    sl_init();

    // get devices
    SLstr* devices = sl_get_devices();
    SLstr chosen_device = NULL;

    if (devices) {
        // Print the list of devices
        printf("List of sound devices: \n");
        for (SLullong i = 0; devices[i] != NULL; i++)
            printf("Device %llu: %s\n", i, devices[i]);

        // Ask the user to choose a device
        printf("\nWhat device would you like to play? (-1 for default): ");
        char device_num_str[20];
        fgets(device_num_str, 20, stdin);
        SLullong device_num = strtoull(device_num_str, NULL, 10);

        // Make a copy of the chosen device
        if (device_num != -1) chosen_device = strdup(devices[device_num]);

        // Clean up the device list
        sl_destroy_device_list(&devices);
    } else {
        printf("Failed to get devices.\n");
        return SL_FAIL;
    }

    // get WAVE file path
    printf("\nWhat WAVE file would you like to load?\n");
    char path[256];
    fgets(path, 256, stdin);
    path[strcspn(path, "\n")] = 0;

    SLenum out = sl_play_sound_c(path, chosen_device, 1.0, 1.0);
    if(out == SL_SUCCESS) printf("Successfully played the sound.\n");
    else printf("Failed to play the sound.\n");

    free((void*)chosen_device);
    sl_cleanup();
    return out;
}
#endif
