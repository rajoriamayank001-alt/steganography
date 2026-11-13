#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "common.h"
#include "types.h"

int main(int argc, char *argv[])
{
  if (argc < 2)
    {
      printf("Usage:\n");
      printf("Encoding: ./stego -e <source.bmp> <secret.txt> [output.bmp]\n");
      printf("Decoding: ./stego -d <stego.bmp> [output.txt]\n");
      return 1;
}
if (strcmp(argv[1], "-e") == 0)
{
    EncodeInfo encInfo;

    if (argc < 4 || argc > 5)
    {
        printf("Invalid encoding arguments\n");
        return 1;
    }

    if (read_and_validate_encode_args(argc, argv, &encInfo) == e_success)
    {
        if (open_files(&encInfo) == e_success)
        {
            if (do_encoding(&encInfo) == e_success)
            {
                printf("Encoding completed successfully\n");
            }
            else
            {
                printf("Encoding failed\n");
            }

            fclose(encInfo.fptr_src_image);
            fclose(encInfo.fptr_secret);
            fclose(encInfo.fptr_stego_image);
        }
        else
        {
            printf("Failed to open encoding files\n");
            return 1;
        }
    }
    else
    {
        printf("Invalid encoding arguments\n");
        return 1;
    }
}
else if (strcmp(argv[1], "-d") == 0)
{
    DecodeInfo decInfo = {0};

    if (argc < 3 || argc > 4)
    {
        printf("Invalid decoding arguments\n");
        return 1;
    }

    if (read_and_validate_decode_args(argc, argv, &decInfo) == e_success)
    {
        if (open_decode_files(&decInfo) == e_success)
        {
            if (do_decoding(&decInfo) != e_success)
            {
                printf("Decoding failed\n");
                fclose(decInfo.fptr_stego_image);
                return 1;
            }

            fclose(decInfo.fptr_stego_image);
        }
        else
        {
            printf("Failed to open encoded image\n");
            return 1;
        }
    }
    else
    {
        printf("Invalid decoding arguments\n");
        return 1;
    }
}
else
{
    printf("Unsupported operation. Use -e or -d\n");
    return 1;
}

return 0;

}
