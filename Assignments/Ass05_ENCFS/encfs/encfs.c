/*
  ENCFS: Encrypted Filesystem
  Copyright (C) 2014 Daniel Morrissey <daniel.morrissey@colorado.edu>
  
  Adapted from FUSE: Filesystem in Userspace
  2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  Source: fuse-2.8.7.tar.gz examples directory
  http://sourceforge.net/projects/fuse/files/fuse-2.X/

  Minor modifications and note by Andy Sayler (2012) <www.andysayler.com>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

  gcc -Wall `pkg-config fuse --cflags` fusexmp.c -o fusexmp `pkg-config fuse --libs`

  Note: This implementation is largely stateless and does not maintain
        open file handels between open and release calls (fi->fh).
        Instead, files are opened and closed as necessary inside read(), write(),
        etc calls. As such, the functions that rely on maintaining file handles are
        not implmented (fgetattr(), etc). Those seeking a more efficient and
        more complete implementation may wish to add fi->fh support to minimize
        open() and close() calls and support fh dependent functions.

*/

#include "defines.h"
#include "djxattr.h"
#include "dj-crypt.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite() */
#define _XOPEN_SOURCE 500
#endif

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif


static void dj_fullpath(char fpath[PATH_MAX], const char* path)
{
		strcpy (fpath, DJ_DATA->encfs_root);
		strncat(fpath, path, PATH_MAX); // Breaks on long paths
}

static int encfs_getattr(const char *path, struct stat *stbuf)
{
	int res;
	
	char fpath[PATH_MAX]; // Full buffer to pass to static function
	dj_fullpath(fpath, path);

	res = lstat(fpath, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_access(const char *path, int mask)
{
	int res;
	
	char fpath[PATH_MAX]; // Full buffer to pass to static function
	dj_fullpath(fpath, path);

	res = access(fpath, mask);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_readlink(const char *path, char *buf, size_t size)
{
	int res;

	char fpath[PATH_MAX]; // Full buffer to pass to static function
	dj_fullpath(fpath, path);

	res = readlink(fpath, buf, size - 1);
	if (res == -1)
		return -errno;

	buf[res] = '\0';
	return 0;
}


static int encfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;
	
		char fpath[PATH_MAX]; // Full buffer to pass to static function
	dj_fullpath(fpath, path);

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if (filler(buf, de->d_name, &st, 0))
			break;
	}

	closedir(dp);
	return 0;
}

static int encfs_mknod(const char *path, mode_t mode, dev_t rdev)
{
	int res;
	
		char fpath[PATH_MAX]; // Full buffer to pass to static function
	dj_fullpath(fpath, path);

	/* On Linux this could just be 'mknod(path, mode, rdev)' but this
	   is more portable */
	if (S_ISREG(mode)) {
		res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(fpath, mode);
	else
		res = mknod(fpath, mode, rdev);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_mkdir(const char *path, mode_t mode)
{
	int res;
	
		char fpath[PATH_MAX]; // Full buffer to pass to static function
	dj_fullpath(fpath, path);

	res = mkdir(fpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_unlink(const char *path)
{
	int res;
	
		char fpath[PATH_MAX]; // Full buffer to pass to static function
	dj_fullpath(fpath, path);

	res = unlink(fpath);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_rmdir(const char *path)
{
	int res;
	
		char fpath[PATH_MAX]; // Full buffer to pass to static function
	dj_fullpath(fpath, path);

	res = rmdir(fpath);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_symlink(const char *from, const char *to)
{
	int res;
	

	res = symlink(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_rename(const char *from, const char *to)
{
	int res;
	
	res = rename(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_link(const char *from, const char *to)
{
	int res;
	
	res = link(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_chmod(const char *path, mode_t mode)
{
	int res;
	
		char fpath[PATH_MAX]; // Full buffer to pass to static function
	dj_fullpath(fpath, path);

	res = chmod(fpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_chown(const char *path, uid_t uid, gid_t gid)
{
	int res;
	
		char fpath[PATH_MAX]; // Full buffer to pass to static function
	dj_fullpath(fpath, path);

	res = lchown(fpath, uid, gid);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_truncate(const char *path, off_t size)
{
	int res;
	
		char fpath[PATH_MAX]; // Full buffer to pass to static function
	dj_fullpath(fpath, path);

	res = truncate(fpath, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_utimens(const char *path, const struct timespec ts[2])
{
	int res;
	struct timeval tv[2];
	
		char fpath[PATH_MAX]; // Full buffer to pass to static function
	dj_fullpath(fpath, path);

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(fpath, tv);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_open(const char *path, struct fuse_file_info *fi)
{
	int res;
	
	char fpath[PATH_MAX]; // Full buffer to pass to static function
	dj_fullpath(fpath, path);

	bool is_encrypted = IsEncrypted(fpath);
	
	if(is_encrypted) {
		res = open(fpath, fi->flags);
		if (res == -1)
			return -errno;

		close(res);
	} else {

		res = open(fpath, fi->flags);
		if (res == -1)
			return -errno;

		close(res);

	}
	return 0;
}

static int encfs_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	int res;

	(void) fi;

	char *key = DJ_DATA->encfs_keyphrase;
	
	char fpath[PATH_MAX]; // Full buffer to pass to static function
	dj_fullpath(fpath, path);
	
	bool is_encrypted = IsEncrypted(fpath);
	
	if (key == NULL)
		printf("\n\n **** KEY IS NULL **** \n\n");
	printf("\n\n***** FILE ENCRYPTED: %d *****\n\n", is_encrypted);
	if(is_encrypted) 
	{
		char tpath[PATH_MAX];
		tmpFileName(fpath, tpath);
		
		//int td = open(tpath, O_WRONLY);
		FILE* realFile = fopen(fpath, "rb");
		FILE* tmpFile = fopen(tpath, "wb");
		DecryptFile(realFile, tmpFile, key);
		//fflush(tmpFile);
		//actual_descriptor = dup(fileno(tmpFile));
		fclose(realFile);
		fclose(tmpFile);
		
		int fd = open(tpath, O_RDONLY);		
		res = pread(fd, buf, size, offset);
		close(fd);
		
	} else {
		int rd = open(fpath, O_RDONLY);
		if (rd == -1)
			return -errno;
			
		res = pread(rd, buf, size, offset);
		if (res == -1)
			res = -errno;

		close(rd);
	}

	return res;
}

static int encfs_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	int res;

	(void) fi;
	
	char fpath[PATH_MAX]; // Full buffer to pass to static function
	dj_fullpath(fpath, path);
	
	char *key = DJ_DATA->encfs_keyphrase;
	
	bool is_encrypted = IsEncrypted(fpath);
	if (is_encrypted) {
		
		// The solution i see for now involves creating a new buf and size
		// Write to the tmp file, Encrypt it, and then read the new file
		// Finally, pwrite to the original file using the new buffer and size
		char tpath[PATH_MAX];
		char tpath01[PATH_MAX];

		// Write unencrypted file
		tmpFileName(fpath, tpath);
		FILE* dec_file = fopen(tpath, "wb");
		fprintf(dec_file, "%s", buf);
		fflush(dec_file);

		// Write encrypted file
		tmpFileName(fpath, tpath01);
		FILE* enc_file = fopen(tpath01, "wb");
		EncryptFile(dec_file, enc_file, key);
		
		fclose(dec_file);
		fclose(enc_file);
		// Set new buf and 
		remove(tpath);
		
		int td = open(tpath01, O_RDONLY);
		
		off_t new_size;
		new_size = lseek(td, 0, SEEK_END);
		lseek(td, 0, SEEK_SET);	
			
		char* new_buf = malloc(new_size + 1);
		read(td, new_buf, new_size);
		
		close(td);
		remove(tpath01);
					
		int fd = open(fpath, O_WRONLY);
		if(fd == -1)
			return -errno;	
		// This is where its broken :'(
		// In order for unencryption to work, we need to maintain entire blocks
		// By encrypting the file, reading the encrypted file, the rewriting to the actual file, we lose some block data
		// and thus render decryption impossible.	
		res = pwrite(fd, new_buf, new_size, offset);
		if(res == -1)
			return -errno;
			
		close(fd);
		
		//~ char tpath[PATH_MAX];
		//~ tmpFileName(fpath, tpath);
		
		//~ int td = open(tpath, O_WRONLY);
		
		//~ res = pwrite(td, buf, size, offset);
		//~ close(td);
	
		//~ FILE* tmp = fopen(tpath, "w");
		//~ fprintf(tmp, "%s", buf);
		//~ fflush(tmp);
		//FILE* real = fopen(fpath, "w");
		
		//~ EncryptFile(tmp, real, key);
		//fclose(real);
		//fclose(tmp);
		
	} else {

		int fd = open(fpath, O_WRONLY);
	
		if (fd == -1)
			return -errno;

		res = pwrite(fd, buf, size, offset);
		if (res == -1)
			res = -errno;
	
		close(fd);
	}

	return res;
}

static int encfs_statfs(const char *path, struct statvfs *stbuf)
{
	int res;

	char fpath[PATH_MAX]; // Full buffer to pass to static function
	dj_fullpath(fpath, path);

	res = statvfs(fpath, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int encfs_create(const char* path, mode_t mode, struct fuse_file_info* fi) {

    (void) fi;
    
    char fpath[PATH_MAX]; // Full buffer to pass to static function
	char tpath[PATH_MAX];
	
	char *key = DJ_DATA->encfs_keyphrase;
	
	dj_fullpath(fpath, path);
	tmpFileName(fpath, tpath);
	

    int real, temp;
    temp = creat(tpath, mode);
    real = creat(fpath, mode);
    
    if(real == -1)
		return -errno;
	if(temp == -1)
		return -errno;
		
	FILE *realFile = fdopen(real, "wb");
	FILE *tmpFile = fdopen(temp, "wb");
	
	EncryptFile(tmpFile, realFile, key);

	fclose(realFile);
	fclose(tmpFile);
	
	remove(tpath);


	SetEncryptedFlag(fpath, true);

    return 0;
}


static int encfs_release(const char *path, struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	(void) path;
	(void) fi;
	return 0;
}

static int encfs_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	(void) path;
	(void) isdatasync;
	(void) fi;
	return 0;
}

#ifdef HAVE_SETXATTR
static int encfs_setxattr(const char *path, const char *name, const char *value,
			size_t size, int flags)
{
	
		char fpath[PATH_MAX]; // Full buffer to pass to static function
	dj_fullpath(fpath, path);
	
	int res = lsetxattr(fpath, name, value, size, flags);
	if (res == -1)
		return -errno;
	return 0;
}

static int encfs_getxattr(const char *path, const char *name, char *value,
			size_t size)
{
	
		char fpath[PATH_MAX]; // Full buffer to pass to static function
	dj_fullpath(fpath, path);
	int res = lgetxattr(fpath, name, value, size);
	if (res == -1)
		return -errno;
	return res;
}

static int encfs_listxattr(const char *path, char *list, size_t size)
{
		char fpath[PATH_MAX]; // Full buffer to pass to static function
	dj_fullpath(fpath, path);
	int res = llistxattr(fpath, list, size);
	if (res == -1)
		return -errno;
	return res;
}

static int encfs_removexattr(const char *path, const char *name)
{
	char fpath[PATH_MAX]; // Full buffer to pass to static function
	dj_fullpath(fpath, path);
	int res = lremovexattr(fpath, name);
	if (res == -1)
		return -errno;
	return 0;
}
#endif /* HAVE_SETXATTR */

static struct fuse_operations encfs_oper = {
	.getattr	= encfs_getattr,
	.access		= encfs_access,
	.readlink	= encfs_readlink,
	.readdir	= encfs_readdir,
	.mknod		= encfs_mknod,
	.mkdir		= encfs_mkdir,
	.symlink	= encfs_symlink,
	.unlink		= encfs_unlink,
	.rmdir		= encfs_rmdir,
	.rename		= encfs_rename,
	.link		= encfs_link,
	.chmod		= encfs_chmod,
	.chown		= encfs_chown,
	.truncate	= encfs_truncate,
	.utimens	= encfs_utimens,
	.open		= encfs_open,
	.read		= encfs_read,
	.write		= encfs_write,
	.statfs		= encfs_statfs,
	.create         = encfs_create,
	.release	= encfs_release,
	.fsync		= encfs_fsync,
#ifdef HAVE_SETXATTR
	.setxattr	= encfs_setxattr,
	.getxattr	= encfs_getxattr,
	.listxattr	= encfs_listxattr,
	.removexattr	= encfs_removexattr,
#endif
};

void print_usage()
{
	fprintf(stderr, "Encfs Usage: ./encfs <Encryption Key> <Mirror Dir>"
					" <Mount Point>\n");
	abort();
}

int main(int argc, char *argv[])
{
	struct dj_state *dj_data;
	
	umask(0);
	if(argc < 4)
		print_usage();
		
	// Create our private data struct
	dj_data = malloc(sizeof(struct dj_state));
	if(dj_data == NULL)
		abort();
		
	// Add the root directory to our data struct
	dj_data->encfs_root=realpath(argv[2], NULL);
	printf("Mirror Directory: %s\n", dj_data->encfs_root);

	// Add the keyphrase to our data struct
	strncpy(dj_data->encfs_keyphrase, argv[1], 256);
	printf("Keyphrase: %s\n", dj_data->encfs_keyphrase);
	
	// Passphrase and Mirror Dir info can be stripped from the 
	// fuse_main call
	argv[1] = argv[3];
	argv[2] = argv[4];
	argc -= 2;
	
	printf("Mount Point: %s\n", argv[1]);	
	char *test = "Makefile";
	char test01[PATH_MAX];
	tmpFileName(test, test01);
	

	return fuse_main(argc, argv, &encfs_oper, dj_data);
}
