#include "../../include/sal.h"

int main(void) {
    sl_init();

    printf("Is this system LITTLE endian... %s\n", leSys == 1 ? "yes" : "no");

    printf("What WAVE file would you like to load?\n");

    char *input = malloc(256 * sizeof(char));
    if (input == NULL) {
        printf("Error allocating memory!\n");
        return -1;
    }

    fgets(input, 256, stdin);
    input[strcspn(input, "\n")] = 0;

    SL_WAV_FILE buf;
    SLenum out = sl_parse_wave_file(input, &buf);



    return out;
}