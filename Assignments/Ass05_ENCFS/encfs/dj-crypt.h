#ifndef __DJ_CRYPT
#define __DJ_CRYPT
#include "aes-crypt.h"


void EncryptFile(FILE* in, FILE* out, char* key);
void DecryptFile(FILE* in, FILE* out, char* key);
void tmpFileName(char* rpath, char buffer[PATH_MAX]);

#endif
