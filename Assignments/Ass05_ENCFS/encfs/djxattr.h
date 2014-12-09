
#ifndef __DJ_XATTR
#define __DJ_XATTR

#include "defines.h"
#include <stdbool.h>

#ifndef XATTR_USER_PREFIX
#define XATTR_USER_PREFIX "user."
#define XATTR_USER_PREFIX_LEN (sizeof (XATTR_USER_PREFIX) - 1)
#endif

#define MAX_XATTR 16 // a maximum number of attributes

void ListXAttr(const char * path, char* buffer[MAX_XATTR]);
void GetXAttr(const char * path, char* key, char* buffer);
void SetXAttr(const char * path, char* key, char* val);
void RemoveXAttr(const char * path, char* key);

bool SetEncryptedFlag(const char * path, bool isSet);
bool IsEncrypted(const char * path);

#endif
