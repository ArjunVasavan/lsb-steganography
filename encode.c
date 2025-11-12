#include "encode.h"
#include "types.h"
#include "common.h"


// function which will check the format is correct or not
Status read_and_validate_encode_args(char* argv[],EncodeInfo *encInfo) 
{

    if ( strstr(argv[2],".bmp")  == NULL || argv[2][0] == '.' )
    {
        printf("INFO : Error! source image format is not valid \n");
        return e_failure;
    }

    encInfo->src_image_fname = argv[2];

    if ( strstr(argv[3],".txt"))
    {
        encInfo->secret_fname = argv[3];
        strncpy(encInfo->extn_secret_file, strstr(argv[3],"."), MAX_FILE_SUFFIX - 1);
        encInfo->extn_secret_file[MAX_FILE_SUFFIX - 1] = '\0';

    }
    else if ( strstr(argv[3],".c"))
    {
        encInfo->secret_fname = argv[3];
        strncpy(encInfo->extn_secret_file, strstr(argv[3],"."), MAX_FILE_SUFFIX - 1);
        encInfo->extn_secret_file[MAX_FILE_SUFFIX - 1] = '\0';

    }
    else if ( strstr(argv[3],".sh"))
    {
        encInfo->secret_fname = argv[3];
        strncpy(encInfo->extn_secret_file, strstr(argv[3],"."), MAX_FILE_SUFFIX - 1);
        encInfo->extn_secret_file[MAX_FILE_SUFFIX - 1] = '\0';
    }
    else if ( strstr(argv[3],".h"))
    {
        encInfo->secret_fname = argv[3];
        strncpy(encInfo->extn_secret_file, strstr(argv[3],"."), MAX_FILE_SUFFIX - 1);
        encInfo->extn_secret_file[MAX_FILE_SUFFIX - 1] = '\0';
    }
    else
    {
        printf("INFO : Error! check the format of secret file \n");
        return e_failure;
    }


    if ( argv[4] != NULL )
    {
        if ( strstr(argv[4],".bmp") == NULL || argv[4][0] == '.' )
        {
            printf("INFO : Error! check the format of optional file \n");
            return e_failure;
        }

        encInfo->stego_image_fname = argv[4];

        print_sleep("INFO : %s file will be created  \n",encInfo->stego_image_fname);
    }
    else 
    {
        print_sleep("INFO : <optional.bmp> is not founded  \n");
        print_sleep("INFO : default file is created  !\n");
        encInfo->stego_image_fname = "stego.bmp";
    }

    return e_success;
}


uint get_image_size_for_bmp(FILE *fptr_image)
{

    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    print_sleep("INFO : width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    print_sleep("INFO : height = %u\n", height);

    // Return image capacity
    return width * height * 3; // one pixel contain 3 byte [R][G][B]
}

Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // opening on read binary mode

    print_sleep("INFO : opening %s \n",encInfo->src_image_fname);


    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "INFO : ERROR Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // as secret is onl test manipulation needed reading through read mode

    print_sleep("INFO : opening %s \n",encInfo->secret_fname);

    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "INFO : ERROR Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // as we are writing through bits by bits opening through write binary

    print_sleep("INFO : opening %s \n",encInfo->stego_image_fname);

    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "INFO : ERROR Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    print_sleep("INFO : all files are opened\n");
    return e_success;
}

// this function will gives the total size of secret file
uint get_size_for_secret(FILE* fptr_secret )
{
    fseek(fptr_secret,0,SEEK_END);
    uint size = ftell(fptr_secret); 
    fseek(fptr_secret,0,SEEK_SET);
    return size;
}

// for copying 54 bits which are mandatory for all bmp files
Status copy_bmp_header(FILE* source_image,FILE* destination_image )
{
    print_sleep("INFO : copying bmp header...\n");

    rewind(source_image);
    rewind(destination_image);

    char* data_buffer = (char*) malloc(60);
    fread(data_buffer,54,1,source_image);

    fwrite(data_buffer,54,1,destination_image);

    free(data_buffer);

    if(ftell(source_image) == ftell(destination_image))
    {
        print_sleep("INFO : successfully copied bmp header\n");
        return e_success;
    }

    return e_failure;
}

// to read the needed readings of size needed
Status check_capacity(EncodeInfo *encInfo)
{
    // storing size of source image
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    print_sleep("INFO : total image capacity => %u\n",encInfo->image_capacity);

    //storing size of secret file 
    encInfo->size_secret_file = get_size_for_secret(encInfo->fptr_secret);

    print_sleep("INFO : secret file size => %lu\n",encInfo->size_secret_file);

    if ( encInfo->size_secret_file == 0 )
    {
        printf("INFO : ERROR! There is nothing written on secret file\n");
        return e_failure;
    }

    //  space_needed will store the total number of byte to be stored inside the image
    encInfo->space_needed = (((strlen(MAGIC_STRING)+4+strlen(encInfo->extn_secret_file)+4+encInfo->size_secret_file)*8)+54); 
    // total is 366 during first time now it will vary


    // checking whether the size needed is less than required
    if( encInfo->image_capacity < encInfo->space_needed )
    {
        return e_failure;
    }

    encInfo->secret_data = malloc(encInfo->size_secret_file + 1);

    if (encInfo->secret_data == NULL) 
    {
        printf("INFO : ERROR: Memory allocation failed\n");
        return e_failure;
    }

    print_sleep("INFO : successfully read capacity of each files\n");
    return e_success;
}

// ecoding byte to lsb side
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for ( int i  = 7 ; i >= 0 ; i-- )
    {
        image_buffer[7-i] =((image_buffer[7-i]&(~1))|((data>>i)&1)); 
    }
    return e_success;
}

// encoding integer to lsb side
Status encode_int_to_lsb(int data,char* image_buffer)
{
    for ( int i = 31 ; i >= 0 ; i-- )
    {
        image_buffer[31-i] =((image_buffer[31-i]&(~1))|((data>>i)&1)); 
    }

    return e_success;
}

// here we will encode the secret file size
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{

    print_sleep("INFO : encoding secret file size...\n");

    char* databuffer = (char*)malloc(32);

    if(databuffer == NULL)
    {
        printf("INFO : failed at creating buffer at encoding file size\n");
        return e_failure;
    }

    fread(databuffer,32,1,encInfo->fptr_src_image);

    encode_int_to_lsb(file_size,databuffer);

    fwrite(databuffer,32,1,encInfo->fptr_stego_image);

    free(databuffer);

    if(ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_stego_image))
    {
        print_sleep("INFO : successfully encoded secret file size\n");
        return e_success;
    }

    return e_failure;

}


Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    print_sleep("INFO : encoding magic string...\n");
    char* databuffer = malloc(8);

    if( databuffer == NULL )
    {
        printf("INFO : failed at creating buffer at encode magic string\n");
        return e_failure;
    }

    for ( int i = 0 ; i < strlen(magic_string) ; i++ )
    {
        fread(databuffer,8,1,encInfo->fptr_src_image);

        if(encode_byte_to_lsb(magic_string[i],databuffer) == e_success)
        {
            fwrite(databuffer,8,1,encInfo->fptr_stego_image);
        }
        else 
    {
            free(databuffer);
            return e_failure;
        }
    }

    print_sleep("INFO : successfully encoded magic string\n");
    free(databuffer);
    return e_success;
}

// after encoding the size of extension now encoding the actual name of extension  
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    print_sleep("INFO : encoding secret extension...\n");
    char* databuffer = malloc(8);

    if( databuffer == NULL )
    {
        printf("INFO : Failed creating buffer at encode extension of file\n");
        return e_failure;
    }


    for ( int i = 0 ; i < strlen(file_extn) ; i++ )
    {
        fread(databuffer,8,1,encInfo->fptr_src_image);

        if(encode_byte_to_lsb(file_extn[i],databuffer) == e_success)
        {
            fwrite(databuffer,8,1,encInfo->fptr_stego_image);
        }
        else 
        {
            free(databuffer);
            return e_failure;
        }
    }

    free(databuffer);
    print_sleep("INFO : successfully encoded secret extension\n");
    return e_success;

}

// this will encode whatever data inside secret file data
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    print_sleep("INFO : encoding secret file data...\n");

    rewind(encInfo->fptr_secret);

    char* databuffer =(char*)malloc(8);

    if( databuffer == NULL )
    {
        printf("INFO : Failed at creating buffer at encode secret file data \n");
        return e_failure;
    }

    fread(encInfo->secret_data,encInfo->size_secret_file,1,encInfo->fptr_secret);
    for ( int i = 0 ; i < encInfo->size_secret_file ; i++ )
    {

        fread(databuffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(encInfo->secret_data[i],databuffer);
        fwrite(databuffer,8,1,encInfo->fptr_stego_image);

    }

    free(databuffer);

    print_sleep("INFO : successfully encoded secret file data \n");
    return e_success;
}

// this function copies the remaining data
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest,EncodeInfo* encInfo)
{

    print_sleep("INFO : copying remaining data...\n");

    char ch;
    while ( fread(&ch,1,1,fptr_src) > 0 )
    {
        fwrite(&ch,1,1,fptr_dest);
    }

    if(ftell(fptr_src) == ftell(fptr_dest))
    {
        print_sleep("INFO : successfully copied remaining data\n");
        return e_success;
    }
    else 
    {
        return e_failure;
    }
}


// all the encoding flow happens here
Status do_encoding(EncodeInfo *encInfo)
{

    if(open_files(encInfo) == e_failure) // done
    {
        printf("Error at open_files function\n");
        return e_failure;
    }

    if ( check_capacity(encInfo) == e_failure ) // done
    {
        return e_failure;
    }


    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_failure) // done
    {
        printf("Error at copy_bmp_header\n");
        return e_failure;
    }


    print_sleep("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ENCODING━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");

    if(encode_magic_string(MAGIC_STRING,encInfo) == e_failure) // done
    {
        printf("Error at encode_magic_string\n");
        return e_failure;
    }

    if(encode_secret_file_size(strlen(encInfo->extn_secret_file),encInfo) == e_failure )
    {
        printf("Error at encode_secret_file_size\n");
        return e_failure;
    }

    if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo) == e_failure )
    {
        printf("Error at encode_secret_file_extn\n");
        return e_failure;
    }

    if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_failure )
    {
        printf("Error at encode_secret_file_size\n");
        return e_failure;
    }

    if(encode_secret_file_data(encInfo) == e_failure )
    {
        printf("Error at encode_secret_file_data\n");
        return e_failure;
    }

    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image,encInfo) == e_failure )
    {
        printf("Error at copy_remaining_img_data\n");
        return e_failure;
    }

    printf("INFO : SUCCESSFULLY ENCODED! \n");
    printf("INFO : %s file is created with encoded data \n",encInfo->stego_image_fname);

    if (encInfo->secret_data) free(encInfo->secret_data);
    if (encInfo->fptr_src_image) fclose(encInfo->fptr_src_image);
    if (encInfo->fptr_secret) fclose(encInfo->fptr_secret);
    if (encInfo->fptr_stego_image) fclose(encInfo->fptr_stego_image);

    return e_success;
}
