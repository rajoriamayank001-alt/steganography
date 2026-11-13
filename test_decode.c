#include <stdio.h>
#include "decode.h"
#include "common.h"

int main(void)
{
    DecodeInfo decInfo;

    decInfo.fptr_stego_image = fopen("stego.bmp", "rb");

    if (decInfo.fptr_stego_image == NULL)
    {
        printf("Unable to open image\n");
        return 1;
    }

    if (decode_magic_string(MAGIC_STRING, &decInfo) == e_success)
    {
        printf("Magic string matched!\n");
    }
    else
    {
        printf("Magic string did not match!\n");
    }

    if (decode_secret_file_extn_size(&decInfo) == e_success)
    {
       printf("Extension size decoded successfully!\n");
    }

    if (decode_secret_file_extn(&decInfo) == e_success)
    {
        printf("Extension decoded successfully!\n");
    }

    if (decode_secret_file_size(&decInfo) == e_success)
    {
         printf("Secret file size decoded successfully!\n");
    }

    if (open_output_file(&decInfo) == e_success)
    {
      printf("Output file opened successfully!\n");
    }
    else
    {
      printf("Failed to open output file!\n");
    }

    if (decode_secret_file_data(&decInfo) == e_success)
    {
      printf("Secret file data decoded successfully!\n");
    }
    else
    {
    printf("Failed to decode secret file data!\n");
    }


    fclose(decInfo.fptr_output);
    fclose(decInfo.fptr_stego_image);

    return 0;
}