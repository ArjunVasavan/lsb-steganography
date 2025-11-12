#include "decode.h"
#include "common.h"
#include "types.h"

 // checks the format of argument
Status read_and_validate_decode_args(char **argv, DecodeInfo *decInfo)
{
    if( strstr( argv[2] , ".bmp" ) == NULL || argv[2][0] == '.'  || (strcmp(".bmp",strstr( argv[2] ,".bmp" ) ) != 0 ) ) 
    {
        printf("Error! source image format is not valid \n");
        return e_failure;
    }

    decInfo->src_image_fname = argv[2]; // storing input bmp name


    if(argv[3] != NULL )
    {
        if ( strstr(argv[3],".txt") && strcmp(".txt", strstr(argv[3],".txt")) == 0 ) // checks if its .txt
        {
            decInfo->output_file_name = argv[3]; // storing name of argument
            decInfo->users_extension = strstr(argv[3],".txt"); // storing ext name
        }
        else if ( strstr(argv[3],".c") && strcmp(".c", strstr(argv[3],".c")) == 0 )
        {
            decInfo->output_file_name = argv[3];
            decInfo->users_extension = strstr(argv[3],".c");
        }
        else if ( strstr(argv[3],".sh") && strcmp(".sh", strstr(argv[3],".sh")) == 0 )
        {
            decInfo->output_file_name = argv[3];
            decInfo->users_extension = strstr(argv[3],".sh");
        }
        else if ( strstr(argv[3],".h") && strcmp(".h", strstr(argv[3],".h")) == 0 )
        {
            decInfo->output_file_name = argv[3];
            decInfo->users_extension = strstr(argv[3],".h");
        }
        else 
        {
            printf("Error! check the format of secret file  \n");
            return e_failure;
        }
    }

    else 
    {
 // if user didnt add optional file then name is stego
        decInfo->output_file_name = "stego";
        decInfo->users_extension = NULL;
        print_sleep("INFO : <optional file> is not founded  \n");
        print_sleep("INFO : default file is created  !\n");

    }
    return e_success;
}

 // this functions checks the extension is matching or not e.t.c
Status extension_checking(DecodeInfo* decInfo)
{

    if(strcmp(decInfo->extension_name,decInfo->users_extension) != 0 )
    {
        print_sleep("WARNING : The following file extension is not matching with decoded extension!\n");

        print_sleep("INFO : Converting to decoded extension...\n");

        decInfo->output_file_name = strtok(decInfo->output_file_name,".");

        char* temporary = malloc(strlen(decInfo->output_file_name) + strlen(decInfo->extension_name) + 1 );

        if (temporary == NULL )
        {
            printf("INFO: Error Unable to create temporary\n");
            return e_failure;
        }

        strcpy(temporary,decInfo->output_file_name);

        decInfo->output_file_name = strcat(temporary,decInfo->extension_name);

        print_sleep("INFO : created an converted file named %s\n",decInfo->output_file_name);

    }

        return e_success;
}

 // this will initialise source pointer
Status init_src_pointer(DecodeInfo* decInfo)
{

    decInfo->fptr_src_image = fopen(decInfo->src_image_fname,"rb");
    print_sleep("INFO : opening %s \n",decInfo->src_image_fname);

    if ( decInfo->fptr_src_image == NULL )
    {
        return e_failure;
    }

    print_sleep("INFO : source file is opened\n");
    return e_success;
}

//this will initialize output pointer
Status init_out_pointer(DecodeInfo* decInfo)
{
    decInfo->fptr_output = fopen(decInfo->output_file_name,"w");
    print_sleep("INFO : opening %s \n",decInfo->output_file_name);

    if ( decInfo->fptr_output == NULL )
    {
        return e_failure;
    }

    print_sleep("INFO : output file is opened\n");
    return e_success;
}

 // this decodes the character individual lsb bits
void decode_lsb_bits(char* ch,char* buffer )
{


    for ( int i = 0 ; i < 8 ; i++ )
    {
        buffer[i] = buffer[i] & 1;
        buffer[i] = buffer[i] << ( 7 - i );
        (*ch) = (*ch) | buffer[i];

    }
}


 // this decodes the magic string
Status decode_magic_string(DecodeInfo *decInfo)
{

    print_sleep("INFO : decoding magic string...\n");
    char* buffer = malloc(8);

    if( buffer == NULL )
    {
        printf("INFO : failed creating buffer at decoding magic string\n");
        return e_failure;
    }

    char* ch = calloc(1,strlen(decInfo->magic_string) + 1);

    for (int i = 0 ; i < strlen(decInfo->magic_string) ; i++ )
    {
        fread(buffer,8,1,decInfo->fptr_src_image);
        decode_lsb_bits(ch+i,buffer);
    }

    if(strcmp(ch,decInfo->magic_string) == 0 )
    {
        print_sleep("INFO : successfully decoded magic string\n");
        print_sleep("INFO : magic string matched \n");


        free(buffer);
        free(ch);
        return e_success;
    }

    free(buffer);
    free(ch);
    return e_failure;

}

 // this will decode the integers hidden inside the bmp
void decode_int_bits(int* ch,char* buffer )
{

    for ( int i = 0 ; i < 32 ; i++ )
    {
        buffer[i] = buffer[i] & 1;
        buffer[i] = buffer[i] << ( 31 - i );
        (*ch) = (*ch) | buffer[i];

    }
}

 // this is to decode by fetching each ones and passing to decode_int_bits
int decode_int_size(DecodeInfo *decInfo)
{
    char* buffer = malloc(32);

    int* int_container = calloc(1,sizeof(int));

    fread(buffer,32,1,decInfo->fptr_src_image);

    decode_int_bits(int_container,buffer);


    int size = *int_container;

    free(buffer);
    free(int_container);
    return size;
}

  // this will decode the extension name
Status extract_extension_name(int size,DecodeInfo *decInfo)
{
    print_sleep("INFO : decoding extension name...\n");
    char* buffer = malloc(8);

    if( buffer == NULL )
    {
        printf("INFO : failed buffer at extract_extension_name\n");
        return e_failure;
    }
    
    char* data = calloc(1,size);

    if( data == NULL )
    {
        printf("INFO : failed data buffer at extract_with_this_size\n");
        return e_failure;
    }

    for (int i = 0 ; i < size ; i++ )
    {
        fread(buffer,8,1,decInfo->fptr_src_image);
        decode_lsb_bits(data+i,buffer);
    }

    decInfo->extension_name = data;

    print_sleep("INFO : decoded extension name %s\n",decInfo->extension_name);
    free(buffer);
    return e_success;

}


 // this will decode from the readed integer value
Status extract_with_this_size(int size,DecodeInfo *decInfo)
{
    print_sleep("INFO : decoding data...\n");
    char* buffer = malloc(8);

    if( buffer == NULL )
    {
        printf("INFO : Failed buffer at extract_with_this_size\n");
        return e_failure;
    }

    char* data = calloc(1,size+1);
    
    if( data == NULL )
    {
        printf("INFO : failed data buffer at extract_with_this_size\n");
        return e_failure;
    }

    for (int i = 0 ; i < size ; i++ )
    {
        fread(buffer,8,1,decInfo->fptr_src_image);
        decode_lsb_bits(data+i,buffer);

    }

    if ( decInfo->fptr_output == NULL )
    {
        free(data);
        free(buffer);
        return e_failure;
    }

    fwrite(data,size,1,decInfo->fptr_output);

    free(data);
    free(buffer);
    return e_success;

}

 // this is for default file creation
Status default_file_creation(DecodeInfo *decInfo)
{
    print_sleep("INFO : creating output file...\n");

    char* temporary = malloc(strlen(decInfo->output_file_name) + strlen(decInfo->extension_name) + 1 );

    if (temporary == NULL) return e_failure;

    strcpy(temporary,decInfo->output_file_name);

    decInfo->output_file_name = strcat(temporary,decInfo->extension_name);

    return e_success;
}


 // this is where the all flow of decoding is happened
Status do_decoding(DecodeInfo *decInfo)
{
    if(init_src_pointer(decInfo) == e_failure) // for initializing source pointers
    {
        return e_failure;
    }

    fseek(decInfo->fptr_src_image,54,SEEK_SET); // skipping unnecessery parts

    decInfo->magic_string = MAGIC_STRING; // assigning magic string

    if(decode_magic_string(decInfo) == e_failure )
    {
        printf("INFO: Magic string hasnt been founded\n");
        return e_failure;
    }

    decInfo->extentsion_size = decode_int_size(decInfo);

    // extracts the name of the extension
    
    if(extract_extension_name(decInfo->extentsion_size,decInfo) == e_failure )
    {
        printf("INFO: Error at extracting data\n");
        return e_failure;
    }

    decInfo->secret_data_size = decode_int_size(decInfo);

    if(strcmp(decInfo->output_file_name,"stego") == 0 )
    {
        if(default_file_creation(decInfo) == e_failure)
        {
            printf("INFO: Error unable to create from the extracted file\n");
            return e_failure;
        }
    }
    else {

        if(extension_checking(decInfo) == e_failure )
        {
            printf("INFO: Error at extension checking\n");
            return e_failure;
        }

    }


    if(init_out_pointer(decInfo) == e_failure )
    {
        printf("INFO: Error at init output pointer\n");
        return e_failure;
    }



    if(extract_with_this_size(decInfo->secret_data_size,decInfo) == e_failure )
    {
        printf("INFO: Error at extracting data\n");
        return e_failure;
    }


    if (decInfo->fptr_src_image) fclose(decInfo->fptr_src_image);
    if (decInfo->fptr_output) fclose(decInfo->fptr_output);
    if (decInfo->extension_name) free(decInfo->extension_name);

    printf("INFO : SUCCESSFULLY DECODED! \n");
    printf("INFO : %s file is created with encoded data \n",decInfo->output_file_name);

    return e_success;
}

