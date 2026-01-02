#include <stdio.h>
#include <fcntl.h>
#include <io.h>  // Required for _setmode on Windows
#include "randombytes.h"

int main(void) {
    // CRITICAL FIX: Prevent Windows from corrupting data (newline translation)
    if (_setmode(_fileno(stdout), _O_BINARY) == -1) {
        fprintf(stderr, "Error setting binary mode\n");
        return 1;
    }

    uint8_t buffer[4096];

    while (1) {
        randombytes(buffer, sizeof(buffer));
        // Write raw bytes to stdout
        if (fwrite(buffer, 1, sizeof(buffer), stdout) != sizeof(buffer)) {
            break; // Stop if the pipe closes
        }
    }
    return 0;
}