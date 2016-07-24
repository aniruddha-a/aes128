/*
 * 
 * Aniruddha. A (aniruddha.a@gmail.com)
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#define PRINT_USAGE \
        printf ("Usage: %s [-h] <-e|-d> -k <key> -f <file>\n", argv[0])

void handle_args (int argc, char **argv, int *encode, int *decode,
                  char **file, char **key) 
{
    int c;
    bool help = false;
    short margs = 0;

    if (argc < 2) {
        PRINT_USAGE;
        exit(1);
    }

    *key = *file  = NULL;
    *encode = *decode = 0;
    opterr = 0;
    while ((c = getopt (argc, argv, "hedk:f:")) != -1)
        switch (c)
        {
            case 'e':
                *encode = 1;
                margs++;
                break;
            case 'd':
                *decode = 1;
                margs++;
                break;
            case 'k':
                *key = optarg;
                margs++;
                break;
            case 'f':
                *file = optarg;
                margs++;
                break;
            case 'h':
                help = true;
                break;
            case '?':
                if  ( (optopt == 'k') || (optopt == 'f') )
                    fprintf (stderr, "Option -%c requires an argument (%s).\n", 
                            optopt,
                            (optopt =='k') ? "key" : "filename");
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,
                            "Unknown option character `\\x%x'.\n",
                            optopt);
                exit(1);
            default:
                printf("Unknown : abort\n");
                abort ();
        }

    if (help) {
        PRINT_USAGE;
        printf ("       -h : This help text\n"
                "       -k : Specify a 16 byte key\n"
                "       -e : Encode the file contents with key\n"
                "       -d : Decode the file contents with key\n"
                "       -f : File to encode/decode\n"
                " Note: The file is read as ASCII character stream\n"
                "       while encode and as hex stream while decode\n");
        exit(1);
    }
    
    if (*encode && *decode) {
        fprintf (stderr,"Use either encode(-e) or decode(-d) \n");
        exit(1);
    }
    if (margs < 3) {
        PRINT_USAGE;
        fprintf(stderr,"       A key(-k), file(-f), and either Encode(-e)\n"
                       "       or decode (-d) are mandatory arguments\n"); 
        exit(1);
    }
    if (strlen(*key) != 16) {
        fprintf(stderr, "Key length has to be 16\n");
        exit(1);
    }
}

#if 0 
int main (int argc, char **argv)
{
    int encode = 0;
    int decode = 0;
    char *key = NULL;
    char *file = NULL;

    handle_args (argc, argv, &encode, &decode, &key, &file);

    printf ("encode = %d, decode = %d, key = %s file = %s\n",
            encode, decode, key, file );

    return 0;
}

    //for (index = optind; index < argc; index++)
      //  printf ("Non-option argument %s\n", argv[index]);
#endif 
