/*
 * 
 * Aniruddha. A (aniruddha.a@gmail.com)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Read the whole file and return the length is successful
// 0 if not 
// If data is not a 16 byte multiple, pad it with NL
// This is the only part which changes from DES while
// reading file - i.e, in DES we use 8 bytes at a time
long flblkread (char *fname, char **data, bool pad)
{
    long  flen, p;
    FILE *fp;
    int i;
    int multiple = 16;

    fp = fopen (fname, "r+");
    if (!fp) {
        fprintf (stderr, "Could not open file %s\n", fname);
        return 0;
    }
    fseek(fp, 0L, SEEK_END);  /* Position to end of file */
    flen = ftell(fp);     /* Get file length */
    rewind(fp);               /* Back to start of file */
    *data = calloc(flen + 1, sizeof(char));

    if(!*data) {
        printf("\nInsufficient memory to read file.\n");
        return 0;
    }

    fread(*data, flen, 1, fp); /* Read the entire file into data */
    if (pad && (flen % multiple)) {
        p = multiple - (flen % multiple);
        *data = realloc(*data, flen + p + 1);
        if(!*data) {
            printf("\nInsufficient memory to pad file.\n");
            return 0;
        }
        for (i = 0; i < p; ++i)
            *(char*)(*data + (flen + i)) = 0xA;// Uhff! I finally figured it!
        *(char*)(*data + (flen + i)) = '\0';
        flen += p;
    }
    return flen;
} 
#if 0 
#include <stdio.h>

int main (int argc, char *argv[])
{
int i;
char *d;
 i = flblkread("adata.dat", &d);
 printf ("\n ANI: %d\n", strlen(d));
 printf ("\n ANI: len returned = %d\n", i);
return 0;
} 
#endif 
