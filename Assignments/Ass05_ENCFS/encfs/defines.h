/*
 * Copyright (C) 2014 Daniel Morrissey <daniel.morrissey@colorado.edu>
 * 
 * This file can be distributed under the terms of the GNU GPLv3
 * 
 * See COPYING
 */
 
 /*
  This file contains global defines for use in the encfs
  */
  
#ifndef __DJ_DEFINES
#define __DJ_DEFINES
  
#define FUSE_USE_VERSION 28
#define HAVE_SETXATTR

#define ENC_ATTR_NAME "pa5-encfs.encrypted"

#ifdef linux
/* Linux is missing ENOATTR error, using ENODATA instead */
#define ENOATTR ENODATA
#endif

#include <limits.h>
#include <stdio.h>
struct dj_state {
	FILE* logfile;
	char* encfs_root;
	char encfs_keyphrase[256];
};

#define DJ_DATA ((struct dj_state *) fuse_get_context()->private_data)

#endif
