/*
Based on parts of the GNU C Library:

   Common code for file-based database parsers in nss_files module.
   Copyright (C) 1996, 1997, 1998, 1999, 2000 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
*/
#define _GNU_SOURCE 1
#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <stdlib.h>
#include <nss.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <grp.h>
#include <json-c/json.h>

#include "s_config.h"

enum nss_status _nss_redis_setgrent(void);
enum nss_status _nss_redis_endgrent(void);
enum nss_status _nss_redis_getgrent_r(struct group *gr, char *buffer, size_t buflen, int *errnop);
enum nss_status _nss_redis_getgrnam_r(const char *name, struct group *gr, char *buffer, size_t buflen, int *errnop);
enum nss_status _nss_redis_getgrgid_r(const gid_t gid, struct group *gr, char *buffer, size_t buflen, int *errnop);


/* from clib/nss */
static inline char **parse_list(char *line, char *data, size_t datalen, int *errnop) {
	char *eol, **list, **p;

	if (line >= data && line < (char *) data + datalen)
		/* Find the end of the line buffer, we will use the space in DATA after
		 *        it for storing the vector of pointers.  */
		eol = strchr(line, '\0') + 1;
	else
		/* LINE does not point within DATA->linebuffer, so that space is
		 *        not being used for scratch space right now.  We can use all of
		 *               it for the pointer vector storage.  */
		eol = data;
	/* Adjust the pointer so it is aligned for storing pointers.  */
	eol += __alignof__(char *) - 1;
	eol -= (eol - (char *)0) % __alignof__(char *);
	/* We will start the storage here for the vector of pointers.  */
	list = (char **)eol;

	p = list;
	while (1)
	{
		char *elt;

		if ((size_t) ((char *)&p[1] - (char *)data) > datalen)
		{
			/* We cannot fit another pointer in the buffer.  */
			*errnop = ERANGE;
			return NULL;
		}
		if (*line == '\0')
			break;

		/* Skip leading white space.  This might not be portable but useful.  */
		while (isspace(*line))
			++line;

		elt = line;
		while (1) {
			if (*line == '\0' || *line == ',' ) {
				/* End of the next entry.  */
				if (line > elt)
					/* We really found some data.  */
					*p++ = elt;

				/* Terminate string if necessary.  */
				if (*line != '\0')
					*line++ = '\0';
				break;
			}
			++line;
		}
	}
	*p = NULL;

	return list;
}

int redis_lookup(const char *service,const char *name, char *addr);

static inline enum nss_status g_search(const char *name, const gid_t gid, struct group *gr, int *errnop, char *buffer, size_t buflen) {
    char *gr_data = malloc(1000); 

	if (gid != 0 && gid < MINGID) {
		*errnop = ENOENT;
		return NSS_STATUS_NOTFOUND;
	}
    if (gid == 0) {
        redis_lookup("group",name,gr_data);
    } else {
        char *_name = malloc(100);
        sprintf(_name, "%d", gid);
        redis_lookup("group",_name,gr_data); 
    }
    json_object *jpwd = json_tokener_parse(gr_data);
    if (jpwd == 0) {
        free(gr_data);
        return NSS_STATUS_UNAVAIL;
    }
    
	*errnop = 0;
    json_object *jobj = malloc(1000); 

    json_object_object_get_ex(jpwd, "name",&jobj);
	gr->gr_name = (char*) malloc(strlen(json_object_get_string(jobj))+1);
	strcpy(gr->gr_name,json_object_get_string(jobj));

    json_object_object_get_ex(jpwd, "passwd",&jobj);
	gr->gr_passwd = (char*) malloc(strlen(json_object_get_string(jobj))+1);
	strcpy(gr->gr_passwd,json_object_get_string(jobj));

    json_object_object_get_ex(jpwd, "gid",&jobj);
	gr->gr_gid = json_object_get_int(jobj);
    
    json_object_object_get_ex(jpwd, "members",&jobj);
    /*
	int i;
    json_object *jmem;
	char **t_mem;
    for (i=0; i < json_object_array_length(jobj) ; i++) {
        jmem = json_object_array_get_idx(jobj, i);     
    }
	gr->gr_mem = NULL;
	*/

	free(jobj);
	return NSS_STATUS_SUCCESS;
}


enum nss_status _nss_redis_setgrent(void) {
	return NSS_STATUS_SUCCESS;
}

enum nss_status _nss_redis_endgrent(void) {
	return NSS_STATUS_SUCCESS;
}

enum nss_status _nss_redis_getgrent_r(struct group *gr, char *buffer, size_t buflen, int *errnop) {
	*errnop = 0;
	return g_search(NULL, 0, gr, errnop, buffer, buflen);
}


enum nss_status _nss_redis_getgrnam_r(const char *name, struct group *gr, char *buffer, size_t buflen, int *errnop) {
	enum nss_status e;
	*errnop = 0;

	if (gr == NULL || name == NULL)
		return NSS_STATUS_UNAVAIL;

	e = g_search(name, 0, gr, errnop, buffer, buflen);
	return e;
}

enum nss_status _nss_redis_getgrgid_r(const gid_t gid, struct group *gr, char *buffer, size_t buflen, int *errnop) {
	enum nss_status e;
	*errnop = 0;
	if (gr == NULL)
		return NSS_STATUS_UNAVAIL;
	if (gid == 0 || gid < MINGID)
		return NSS_STATUS_NOTFOUND;

	e = g_search(NULL, gid, gr, errnop, buffer, buflen);
	return e;
}
