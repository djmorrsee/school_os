
#include "djxattr.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/xattr.h>
#include <linux/xattr.h>
#include <sys/types.h>

void ListXAttr(const char * path, char* buffer[MAX_XATTR])
{
	char* lst = NULL;
	char* chr = NULL;
	char* start = NULL;
	
	ssize_t cnt = 0;
	ssize_t lstsize = 0;
	
	lstsize = listxattr(path, NULL, 0);
	if(lstsize < 0){
	    perror("listxattr error");
	    fprintf(stderr, "path  = %s\n", path);
	    exit(EXIT_FAILURE);
	}
	/* Malloc space for list items */
	lst = malloc(sizeof(*lst)*lstsize);
	if(!lst){
	    perror("malloc of 'lst' error");
	    exit(EXIT_FAILURE);
	}
	/* Call xattr list to get data */
	lstsize = listxattr(path, lst, lstsize);
	if(lstsize < 0 || !lst){
	    perror("listxattr error");
	    fprintf(stderr, "path  = %s\n", path);
	    exit(EXIT_FAILURE);
	}
	
	
	
	///* Tokenize null seperated array on \0 */
	int at = 0;
	chr = start = lst;
	cnt = 0;
	while(cnt < lstsize){
	    /* Print attribute names, one per line */
	    buffer[at++] = start;
	    while(*chr != '\0'){
		chr++;
		cnt++;
	    }
	    start = ++chr;
	    cnt++;
	}
	buffer[at]='\0';
	return;

}
void GetXAttr(const char * path, char* key, char* buffer)
{
	ssize_t valsize;
	
	char* tmpstr = malloc(strlen(key) + XATTR_USER_PREFIX_LEN + 1);
	strcpy (tmpstr, XATTR_USER_PREFIX);
	strcat(tmpstr, key);
	
	valsize = getxattr(path, tmpstr, NULL, 0);
	if(valsize < 0)
	{
		if(errno == ENOATTR) 
		{
			buffer[0] = '\0';
			free(tmpstr);
			return;
		}
		else {
			fprintf(stderr, "GetXAttr Error");
			abort();
		}
	}
	
	if(!buffer) {
		fprintf(stderr, "GetXAttr Malloc Error");
		abort();
	}
	valsize = (getxattr(path, tmpstr, buffer, valsize));
	if( valsize < 0) {
		if(errno == ENOATTR) 
		{
			buffer[0] = '\0';

			free(tmpstr);
			return;
		}
		else {
			fprintf(stderr, "GetXAttr Error");
			abort();
		}
	}
	
	buffer[valsize] = '\0';

	free(tmpstr);
	return;
}
void SetXAttr(const char * path, char* key, char* val)
{
	char* tmpstr = malloc(strlen(key) + XATTR_USER_PREFIX_LEN + 1);
	strcpy(tmpstr, XATTR_USER_PREFIX);
	strcat(tmpstr, key);
	
	if(setxattr(path, tmpstr, val, strlen(val), 0)) {
		fprintf(stderr, "Error setting attributes");
		abort();
	}
	free(tmpstr);
}
void RemoveXAttr(const char * path, char* key)
{
	char* tmpstr = malloc(strlen(path) + XATTR_USER_PREFIX_LEN + 1);
	if(!tmpstr) {
		fprintf(stderr, "Error malloc");
		abort();
	}
	strcpy (tmpstr, XATTR_USER_PREFIX);
	strcat (tmpstr, path);
	
	if(removexattr(key, tmpstr))
	{
		if(errno == ENOATTR) {
			return;
		} else {
			fprintf(stderr, "Error removing attr");
			abort();
		}
	}
	free(tmpstr);
}

bool SetEncryptedFlag(const char * path, bool isSet)
{
	// This function sets the xattr flag for our encryption scheme //
	// Has nothing to do with actual ecryption //
	
	SetXAttr(path, ENC_ATTR_NAME, isSet ? "true" : "false");
	return IsEncrypted(path);
}

bool IsEncrypted(const char * path)
{
	char* list[MAX_XATTR];
	ListXAttr(path, list);
	
	char enc_val[256];
	char* appended_key = "user."ENC_ATTR_NAME;
	
	int i = 0;
	while (list[i] != '\0')
	{
		if(strcmp(appended_key, list[i++]) == 0)
		{
			GetXAttr(path, ENC_ATTR_NAME, enc_val);
			if (strcmp(enc_val, "true") == 0)
			{
				return 1;
			}
		}
	}
	return 0;
}
