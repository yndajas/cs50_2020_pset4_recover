#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc > 2)
    {
        fprintf(stderr, "Usage: ./recover <filename>\n");
        return 1;
    }

    // open input file
    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
        fprintf(stderr, "Could not open %s\n", argv[1]);
        return 1;
    }

    // create variable for referring to the file to write to
    FILE *out; // work out how to iterate the file number

    // initialise block array to store up to 512 bytes
    BYTE block[512];

    // initialise file number tracking and set first file name
    int file_number = 0;
    char filename[sizeof "000.jpg"];
    sprintf(filename, "%03d.jpg", file_number);

    // find start of first image
    int next = 0;

    while (next == 0)
    {
        // read up to 512 bytes from file and store in block array
        fread(block, sizeof(BYTE), 512, file);

        // check for signature
        if (block[0] == 0xff && block[1] == 0xd8 && block[2] == 0xff && (block[3] == 0xe0 || block[3] == 0xe1 || block[3] == 0xe2
                || block[3] == 0xe3 || block[3] == 0xe4 || block[3] == 0xe5 || block[3] == 0xe6 || block[3] == 0xe7 || block[3] == 0xe8
                || block[3] == 0xe9 || block[3] == 0xea || block[3] == 0xeb || block[3] == 0xec || block[3] == 0xed || block[3] == 0xee
                || block[3] == 0xef))
        {
            // open file to write to and update out variable to refer to this
            out = fopen(filename, "w");

            // add block to file
            fwrite(block, sizeof(BYTE), 512, out);

            // move on to next loop
            next = 1;
        }
    }

    // look through rest of file for the rest of the current image and remaining images
    while (!feof(file))
    {
        // read up to 512 bytes from file and store in block array, and set read_size to size of read block (this accounts for the final block in the file being less than 512 bytes)
        int read_size = 0;
        read_size = fread(block, sizeof(BYTE), 512, file);

        // check for signature
        if (block[0] == 0xff && block[1] == 0xd8 && block[2] == 0xff && (block[3] == 0xe0 || block[3] == 0xe1 || block[3] == 0xe2
                || block[3] == 0xe3 || block[3] == 0xe4 || block[3] == 0xe5 || block[3] == 0xe6 || block[3] == 0xe7 || block[3] == 0xe8
                || block[3] == 0xe9 || block[3] == 0xea || block[3] == 0xeb || block[3] == 0xec || block[3] == 0xed || block[3] == 0xee
                || block[3] == 0xef))
        {
            // close previous file
            fclose(out);

            // iterate file number and open new file for writing
            file_number++;
            sprintf(filename, "%03d.jpg", file_number);
            out = fopen(filename, "w");

            // add the amount of read bytes to the file
            fwrite(block, sizeof(BYTE), read_size, out);
        }

        else
        {
            // add the amount of read bytes to the file
            fwrite(block, sizeof(BYTE), read_size, out);
        }
    }

    // close last file
    fclose(out);
}
