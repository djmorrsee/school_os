#include "defines.h"
#include "dj-crypt.h"
#include <unistd.h>


void EncryptFile(FILE* in, FILE* out, char* key)
{
	if(!do_crypt(in, out, 1, key))
	{
		fprintf(stderr, "Error encrypting**\n\n");
	}

	return;
}
void DecryptFile(FILE* in, FILE* out, char* key)
{
	if(!do_crypt(in, out, -1, key))
	{
		fprintf(stderr, "Error decrypting**\n\n");
	}
	
	return;
}


void tmpFileName(char* rpath, char buffer[PATH_MAX])
{
	strcpy(buffer, rpath);
	strcat(buffer, ".tmp");
	
	char tmp[8];
	int i = 0;
	while(access(buffer, F_OK) != -1)
	{
		// Tmp file exists, try another

		strcpy(buffer, rpath);
		strcat(buffer, ".tmp");
		sprintf(tmp, "%d", i++);
		strcat(buffer, tmp);
	}

}
