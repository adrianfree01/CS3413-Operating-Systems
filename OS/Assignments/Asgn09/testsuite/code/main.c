#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>

// CRC code adapted from W3C PNG Specification:
// https://www.w3.org/TR/png/#D-CRCAppendix

uint32_t crcTable[256];

void makeCrcTable(void)
{
    uint32_t c;
    int n, k;

    for (n = 0; n < 256; n++)
    {
        c = (uint32_t)n;
        for (k = 0; k < 8; k++)
        {
            if (c & 1)
            {
                c = 0xedb88320UL ^ (c >> 1); // use UL because it is unsigned long int.. using 0xedb88320L treats it as a signed long int
            }
            else
            {
                c = c >> 1;
            }
        }
        crcTable[n] = c;
    }
}

uint32_t updateCrc(uint32_t crc, unsigned char *buf, int len)
{
    uint32_t c = crc;
    int n;

    for (n = 0; n < len; n++)
    {
        c = crcTable[(c ^ buf[n]) & 0xff] ^ (c >> 8);
    }
    return c;
}

uint32_t calculateCrc32(unsigned char *buf, int len)
{
    return updateCrc(0xffffffffUL, buf, len) ^ 0xffffffffUL;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <filename.png>\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];

    FILE *fp = fopen(filename, "rb+");
    if (fp == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    unsigned char signature[8];
    if (fread(signature, 1, 8, fp) != 8)
    {
        printf("Failed to read the file signature.\n");
        fclose(fp);
        return 1;
    }

    unsigned char pngSignature[8] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
    if (memcmp(signature, pngSignature, 8) != 0)
    {
        printf("Not a PNG file.\n");
        fclose(fp);
        return 1;
    }
    else
    {
        printf("This is a PNG file.\n");
    }

    makeCrcTable();

    while (1)
    {
        unsigned char lengthBytes[4];
        unsigned char chunkType[5];
        uint32_t length;

        size_t readLen = fread(lengthBytes, 1, 4, fp);
        if (readLen != 4)
        {
            break; // Reached EOF or error
        }
        length = ntohl(*(uint32_t *)lengthBytes);

        if (fread(chunkType, 1, 4, fp) != 4)
        {
            printf("Failed to read chunk type.\n");
            break;
        }
        chunkType[4] = '\0';

        printf("Chunk Type: %s, Length: %u\n", chunkType, length);

        long dataPosition = ftell(fp);

        unsigned char *chunkData = (unsigned char *)malloc(length);
        if (chunkData == NULL)
        {
            printf("Memory allocation failed.\n");
            break;
        }
        if (fread(chunkData, 1, length, fp) != length)
        {
            printf("Failed to read chunk data.\n");
            free(chunkData);
            break;
        }

        unsigned char crcBytes[4];
        if (fread(crcBytes, 1, 4, fp) != 4)
        {
            printf("Failed to read CRC.\n");
            free(chunkData);
            break;
        }
        uint32_t fileCrc = ntohl(*(uint32_t *)crcBytes);

        unsigned char *crcBuffer = (unsigned char *)malloc(4 + length);
        if (crcBuffer == NULL)
        {
            printf("Memory allocation failed.\n");
            free(chunkData);
            break;
        }
        memcpy(crcBuffer, chunkType, 4);
        memcpy(crcBuffer + 4, chunkData, length);
        uint32_t computedCrc = calculateCrc32(crcBuffer, 4 + length);
        free(crcBuffer);

        if (computedCrc != fileCrc)
        {
            printf("CRC mismatch for chunk %s.\n", chunkType);

            if (strcmp((char *)chunkType, "IDAT") == 0)
            {
                for (uint32_t i = 0; i < length; i++)
                {
                    chunkData[i] ^= 42;
                }

                crcBuffer = (unsigned char *)malloc(4 + length);
                if (crcBuffer == NULL)
                {
                    printf("Memory allocation failed.\n");
                    free(chunkData);
                    break;
                }
                memcpy(crcBuffer, chunkType, 4);
                memcpy(crcBuffer + 4, chunkData, length);
                computedCrc = calculateCrc32(crcBuffer, 4 + length);
                free(crcBuffer);

                if (computedCrc == fileCrc)
                {
                    printf("Successfully corrected IDAT chunk.\n");
                    fseek(fp, dataPosition, SEEK_SET);

                    size_t writeLen = fwrite(chunkData, 1, length, fp);
                    if (writeLen != length)
                    {
                        printf("Failed to write corrected data.\n");
                        free(chunkData);
                        break;
                    }

                    fseek(fp, 4, SEEK_CUR);
                }
                else
                {
                    printf("Failed to correct IDAT chunk.\n");
                }
            }
        }
        else
        {
            printf("CRC verified for chunk %s.\n", chunkType);
        }

        free(chunkData);

        if (strcmp((char *)chunkType, "IEND") == 0)
        {
            printf("End of PNG file reached.\n");
            break;
        }
    }

    fclose(fp);
    return 0;
}
