#ifndef DECODE_H
#define DECODE_H

#include "encode.h"
#include "types.h"
#include "common.h"

typedef struct _DecodeInfo
{
    char* src_image_fname; // stores name of source file
    FILE* fptr_src_image; // file pointer of seource
    FILE* fptr_output; // file pointer of output
    char* output_file_name; // name of oputput file
    char* magic_string; // for storing magic string
    char* extension_name; // for storing name of extension
    int extentsion_size; // for storing size of extension
    int secret_data_size; // for storing secret data size
    char* users_extension; // for storing user input extension

}DecodeInfo;


Status read_and_validate_decode_args(char **argv,DecodeInfo *decInfo);

Status do_decoding(DecodeInfo* );

Status extension_checking(DecodeInfo* decInfo);

Status init_src_pointer(DecodeInfo* decInfo);

Status init_out_pointer(DecodeInfo* decInfo);

void decode_lsb_bits(char* ch,char* buffer );

Status decode_magic_string(DecodeInfo *decInfo);

void decode_int_bits(int* ch,char* buffer );

int decode_int_size(DecodeInfo *decInfo);

Status extract_extension_name(int size,DecodeInfo *decInfo);

Status extract_with_this_size(int size,DecodeInfo *decInfo);

Status default_file_creation(DecodeInfo *decInfo);

#endif

