#include <stdio.h>
#include "encode.h"
#include "types.h"

int main(int argc , char *argv[])
{
    EncodeInfo encInfo;
    if(argc < 4  || argc > 5)
    {
        printf("Insufficent argument\n");
        return 1;
    }

if (read_and_validate_encode_args(argc, argv, &encInfo) == e_success)
{
    if (open_files(&encInfo) == e_success)
    {
        printf("Files opened successfully\n");

        if (do_encoding(&encInfo) == e_success)
        {
            printf("Encoding completed successfully\n");
        }
        else
        {
            printf("Encoding failed\n");
        }
    }
    else
    {
        printf("Failed to open files\n");
    }
}
else
{
    printf("Invalid arguments\n");
}

    return 0;
}

OperationType check_operation_type(char opt)
{
    if(opt == 'e')
    {
        return e_encode;
    }
    else if(opt == 'd')
    {
        return e_decode;
    }
    return e_unsupported;
}
