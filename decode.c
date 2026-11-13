#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "common.h"

Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    *data = 0;
    for(int i=0;i<8;i++)
    {
        int bit = image_buffer[i] & 1;
        *data = *data |(bit << (7-i));
    }
    return e_success;
}

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
    char data;
    char image_buffer[8];

    for (int i = 0; i < 2; i++)
    {
        fread(image_buffer, 1, 8, decInfo->fptr_stego_image);

        decode_byte_from_lsb(&data, image_buffer);

        if (data != magic_string[i])
        {
            return e_failure;
        }
    }

    return e_success;
}

Status decode_size_from_lsb(uint *size, char *image_buffer)
{
    *size = 0;

    for (int i = 0; i < 32; i++)
    {
        int bit = image_buffer[i] & 1;

        *size = *size | (bit << (31 - i));
    }

    return e_success;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char image_buffer[32];
    uint size = 0;

    if (fread(image_buffer, 1, 32, decInfo->fptr_stego_image) != 32)
    {
        return e_failure;
    }

    if (decode_size_from_lsb(&size, image_buffer) == e_failure)
    {
        return e_failure;
    }

    printf("Decoded extension size = %u\n", size);

    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char image_buffer[8];
    char data;

    for (int i = 0; i < 4; i++)
    {
        if (fread(image_buffer, 1, 8, decInfo->fptr_stego_image) != 8)
        {
            return e_failure;
        }

        decode_byte_from_lsb(&data, image_buffer);

        decInfo->extn_secret_file[i] = data;
    }

    decInfo->extn_secret_file[4] = '\0';

    printf("Decoded extension = %s\n", decInfo->extn_secret_file);

    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char image_buffer[32];
    uint size = 0;

    if (fread(image_buffer, 1, 32, decInfo->fptr_stego_image) != 32)
    {
        return e_failure;
    }

    if (decode_size_from_lsb(&size, image_buffer) == e_failure)
    {
        return e_failure;
    }

    decInfo->size_secret_file = size;

    printf("Decoded secret file size = %u bytes\n", size);

    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char image_buffer[8];
    char data;

    for (long i = 0; i < decInfo->size_secret_file; i++)
    {
        if (fread(image_buffer, 1, 8, decInfo->fptr_stego_image) != 8)
        {
            return e_failure;
        }

        decode_byte_from_lsb(&data, image_buffer);

        fwrite(&data, 1, 1, decInfo->fptr_output);
    }

    return e_success;
}


OperationType check_decode_operation_type(char opt) 
{
    if (opt == 'e')
    {
        return e_encode;
    }
    else if (opt == 'd')
    {
        return e_decode;
    }

    return e_unsupported;
}

Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo)
{
    if (argc < 3 || argc > 4)
    {
        printf("Invalid number of arguments\n");
        return e_failure;
    }

    if (strlen(argv[2]) < 4 ||
        strcmp(argv[2] + strlen(argv[2]) - 4, ".bmp") != 0)
    {
        printf("Invalid BMP image file\n");
        return e_failure;
    }

    decInfo->stego_image_fname = argv[2];

    if (argc == 4)
    {
        decInfo->output_fname = argv[3];
    }
    else
    {
        decInfo->output_fname = "output.txt";
    }

    return e_success;
}

Status open_output_file(DecodeInfo *decInfo)
{
    decInfo->fptr_output = fopen(decInfo->output_fname, "w");

    if (decInfo->fptr_output == NULL)
    {
        return e_failure;
    }

    return e_success;
}


Status open_decode_files(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");

    if (decInfo->fptr_stego_image == NULL)
    {
        printf("ERROR: Unable to open image file\n");
        return e_failure;
    }

    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    if (decode_magic_string(MAGIC_STRING, decInfo) == e_failure)
    {
        printf("ERROR: Magic string does not match\n");
        return e_failure;
    }

    if (decode_secret_file_extn_size(decInfo) == e_failure)
    {
        return e_failure;
    }

    if (decode_secret_file_extn(decInfo) == e_failure)
    {
        return e_failure;
    }

    if (decode_secret_file_size(decInfo) == e_failure)
    {
        return e_failure;
    }

    if (open_output_file(decInfo) == e_failure)
    {
        printf("ERROR: Unable to open output file\n");
        return e_failure;
    }

    if (decode_secret_file_data(decInfo) == e_failure)
    {
        printf("ERROR: Unable to decode secret data\n");
        fclose(decInfo->fptr_output);
        return e_failure;
    }

    fclose(decInfo->fptr_output);

    printf("Decoding completed successfully\n");

    return e_success;
}