#include "encode.h"
#include "types.h"
#include "decode.h"


 // checks wheather to decode or encode
OperationType check_operation_type(char* argv) 
{
    if ( strcmp(argv,"-e") == 0 )
    {
        return e_encode;
    }
    else if ( strcmp(argv,"-d") == 0 )
    {
        return e_decode;
    }
    else {
        return e_unsupported;
    }
}

 // for printing encode error parts
void encode_error()
{
    printf("./a.out : Encoding: ./a.out  -e <.bmp file> <.txt file> [output file]\n");
}

// for printing decode errors
void decode_error()
{
    printf("./a.out : Decoding: ./a.out  -d <.bmp file> [output file]\n");
}


//for printing an error where the user will know the format
void error()
{
    printf("./a.out : Encoding: ./a.out  -e <.bmp file> <.txt file> [output file]\n");
    printf("./a.out : Decoding: ./a.out  -d <.bmp file> [output file]\n");
}

int main(int argc,char** argv)
{

    if (argc < 2)
    {
        error();
        return 0;
    }

    EncodeInfo encInfo;
    DecodeInfo decInfo;

    OperationType ret = check_operation_type(argv[1]);



    if ( ret == e_encode )
    {
        if ( argc >= 4 )
        {

            if(read_and_validate_encode_args(argv,&encInfo) == e_success)
            {
                print_sleep("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━START━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
                if(do_encoding(&encInfo) == e_failure)
                {
                    printf("INFO : Error Doing the encode \n");
                    encode_error();
                    return 0;
                }

            }
            else {

                encode_error();
                return 0;
            }

        }
        else
    {

            encode_error();
            return 0;
        }
    }
    else if ( ret == e_decode )
    {
        if( argc >= 3 )
        {
            if(read_and_validate_decode_args(argv,&decInfo) == e_success )
            {
                print_sleep("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━START━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
                if(do_decoding(&decInfo) == e_failure )
                {
                    printf("INFO : Error Doing the decode \n");
                    decode_error();
                    return 0;
                }


            }
            else {

                decode_error();
                return 0;
            }


        }
        else {

            decode_error();
            return 0;
        }
    }
    else{
        printf("INFO : ERROR FORMATTING \n");
        error();
        return 0;
    }


    print_sleep("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━END━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    return 0;
}
