#ifndef ENCODE_H
#define ENCODE_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "types.h" // Contains user defined types


#define MAX_SECRET_BUF_SIZE 100 // size of the secret file
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8) // for 1 byte to store we need 8 byte right
#define MAX_FILE_SUFFIX 5


typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image; // file pointer of source image when fopening the beautiful.bmp
    uint image_capacity; // total size of the image // total size width * height
    uint bits_per_pixel; // how many bits is needed for an pixel this is that image size * 8 i think
    char image_data[MAX_IMAGE_BUF_SIZE]; // this will store the content of secret file 

    /* Secret File Info */
    char *secret_fname; // stores the name of the secret file
    FILE *fptr_secret; // pointer which points on secret file
    char extn_secret_file[MAX_FILE_SUFFIX]; // stores the secret file extension name
    char* secret_data; // i changed this for dynamic allocation of secret data
    long size_secret_file; // stores the size of secret file which will help oh decoding part
    uint space_needed; // this is to store how many space is needed to store some beginning nonchangable datas 

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} EncodeInfo;

Status read_and_validate_encode_args(char* arv[],EncodeInfo* encInfo);

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo);

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest,EncodeInfo* encInfo); // modfifed by adding encInfo

#endif
