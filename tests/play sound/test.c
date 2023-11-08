#include "../../include/sal.h"

#include <windows.h>
#include <audioclient.h>

int main(void) {
    sl_init();

    printf("Is this system LITTLE endian... %s\n", leSys == 1 ? "yes" : "no");

    printf("What WAVE file would you like to load?\n");

    char *input = malloc(256 * sizeof(char));
    if (input == NULL) {
        printf("Error allocating memory!\n");
        sl_cleanup();
        return -1;
    }

    fgets(input, 256, stdin);
    input[strcspn(input, "\n")] = 0;

    SL_WAV_FILE* buf = NULL;
    SLenum out = sl_parse_wave_file(input, &buf);

    if(out == SL_SUCCESS && buf) {
        printf("Successfully parsed WAVE file at \"%s\".\n", input);
        sl_free_wave_file(&buf);
    } else {
        printf("Failed to parse WAVE file at \"%s\".\n", input);
    }

    sl_cleanup();
    return out;
}