#include <stdio.h>
#include <string.h>
#include "common.h"
#include "encode.h"
#include "types.h"


uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    
    fseek(fptr_image, 18, SEEK_SET);

    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    return width * height * 3;
}

Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");

    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    return e_success;
}

Status read_and_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo)
{

    if (strlen(argv[2]) < 4)
    {
        return e_failure;
    }

    if (strcmp(argv[2] + strlen(argv[2]) - 4, ".bmp") != 0)
    {
        return e_failure;
    }

    encInfo->src_image_fname = argv[2];

    encInfo->secret_fname = argv[3];
    FILE *fptr = fopen(encInfo->secret_fname,"r");

    if(fptr == NULL)
    {
        return e_failure;
    }
    fclose(fptr);

    if(argc == 5)
    {
        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        encInfo->stego_image_fname = "default.bmp";
    }

    if(strlen(encInfo->stego_image_fname) < 4)
    {
        return e_failure;
    }
    if(strcmp(encInfo->stego_image_fname + strlen(encInfo->stego_image_fname
                         ) -4,".bmp") != 0)
                         {
                            return e_failure;
                        }
            return e_success;            
}

long get_file_size(FILE *fptr)
{
    fseek(fptr,0, SEEK_END);
    return ftell(fptr);
}

Status do_encoding(EncodeInfo *encInfo)
{
    if(check_capacity(encInfo) == e_failure)
    {
        printf("ERROR: Insufficient image capacity\n");
        return e_failure;
    }

    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_failure)
    {
        printf("ERROR: Unable to copy BMP header\n");
        return e_failure;
    }

    if(encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        printf("ERROR: Unable to encode magic string\n");
        return e_failure;
    }

    char *dot = strrchr(encInfo->secret_fname, '.');
     if (dot == NULL)
    {
       printf("ERROR: Extension not found\n");
       return e_failure;
    }
    strcpy(encInfo->extn_secret_file, dot);
     
    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo)==e_failure)
    {
        printf("ERROR: unable to encode extension size\n");
        return e_failure;
    }

    if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)== e_failure)
    {
        printf("ERROR: Unable to encode secret file extension\n");
        return e_failure;
    }

    if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_failure)
    {
        printf("ERROR: Unable to encode secret file size\n");
        return e_failure;
    }

    if(encode_secret_file_data(encInfo)==e_failure)
    {
        printf("ERROR: Unable to encode secret file data\n");
        return e_failure;
    }

    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
    {
        printf("ERROR: Unable to copy remaining image data\n");
        return e_failure;
    }

   return e_success; 
   
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    if(get_image_size_for_bmp(encInfo->fptr_src_image) > ((encInfo->size_secret_file + 14)*8))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}   

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buff[54];
    rewind(fptr_src_image);
    if(fread(buff,1,54,fptr_src_image) != 54)
    {
        return e_failure;
    }
    if(fwrite(buff,1,54,fptr_dest_image) != 54)
    {
        return e_failure;
    }
    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char buff[8];
     size_t i;

    for(i=0;i<strlen(magic_string); i++)
    {
         if (fread(buff, 1, 8, encInfo->fptr_src_image) != 8)
        {
            return e_failure;
        }

        if (encode_byte_to_lsb(magic_string[i], buff) == e_failure)
        {
            return e_failure;
        }

        if (fwrite(buff, 1, 8, encInfo->fptr_stego_image) != 8)
        {
            return e_failure;
        }
    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
   for(int i=7;i>=0;i--)
   {
    int bit = (data >> i) & 1;
    int index = 7 - i;

    if(bit == 1)
    {
        image_buffer[index] = image_buffer[index] | 1;
    }
    else
    {
       image_buffer[index] = image_buffer[index] & 0xFE ;
    }
   }
   return e_success;
}

Status encode_secret_file_extn_size(uint size, EncodeInfo *encInfo)
{
    char buff[32];

    if (fread(buff, 1, 32, encInfo->fptr_src_image) != 32)
    {
        return e_failure;
    }

    if (encode_size_to_lsb(size, buff) == e_failure)
    {
        return e_failure;
    }

    if (fwrite(buff, 1, 32, encInfo->fptr_stego_image) != 32)
    {
        return e_failure;
    }

    return e_success;
}  

Status encode_size_to_lsb(uint size, char *image_buffer)
{
   for(int i=31;i>=0;i--)
   {
    int bit = (size >> i) & 1;
    int index = 31-i;
    if(bit == 1)
    {
        image_buffer[index] = image_buffer[index] | 1;
    }
    else
    {
        image_buffer[index] = image_buffer[index] & 254;
    }
   }
   return e_success;
} 

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
   char buff[8];
   size_t i;
   
   for(i=0;i<strlen(file_extn);i++)
   {
    if(fread(buff,1,8,encInfo->fptr_src_image) != 8)
    {
        return e_failure;
    }
    if(encode_byte_to_lsb(file_extn[i], buff)==e_failure)
    {
        return e_failure;
    }
    if(fwrite(buff,1,8,encInfo->fptr_stego_image) != 8)
    {
        return e_failure;
    }
   }
   return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buff[32];

    if (fread(buff, 1, 32, encInfo->fptr_src_image) != 32)
    {
        return e_failure;
    }

    if (encode_size_to_lsb((uint)file_size, buff) == e_failure)
    {
        return e_failure;
    }

    if (fwrite(buff, 1, 32, encInfo->fptr_stego_image) != 32)
    {
        return e_failure;
    }

    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char buff[8];
    char data;
    long i;

    rewind(encInfo->fptr_secret);

    for(i=0;i<encInfo->size_secret_file;i++)
    {
        if(fread(&data,1,1,encInfo->fptr_secret) != 1)
        {
            return e_failure;
        }
        if(fread(buff,1,8,encInfo->fptr_src_image) != 8)
        {
            return e_failure;
        }
        if(encode_byte_to_lsb(data,buff) == e_failure)
        {
            return e_failure;
        }
        if(fwrite(buff,1,8,encInfo->fptr_stego_image) != 8)
        {
            return e_failure;
        }
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;

    while (fread(&ch, 1, 1, fptr_src) == 1)
    {
        if (fwrite(&ch, 1, 1, fptr_dest) != 1)
        {
            return e_failure;
        }
    }

    return e_success;
}
