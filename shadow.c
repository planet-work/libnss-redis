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

#include <stdlib.h>
#include <stdio.h>
#include <nss.h>
#include <string.h>
#include <shadow.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

#include "s_config.h"

enum nss_status _nss_redis_getspent_r(struct spwd *, char *, size_t, int *);
enum nss_status _nss_redis_getspnam_r(const char *, struct spwd *, char *, size_t, int *);
enum nss_status _nss_redis_setspent(void);
enum nss_status _nss_redis_endspent(void);

static enum nss_status shadow_search(const char *name, struct spwd *spw, char *buffer, size_t buflen, int *errnop);

enum nss_status _nss_redis_getspnam_r(const char *name, struct spwd *spw, char *buffer, size_t buflen, int *errnop) {
	enum nss_status s;

	if (spw == NULL || name == NULL)
	{
		*errnop = EPERM;
		return NSS_STATUS_UNAVAIL;
	}

	s = shadow_search(name, spw, buffer, buflen, errnop);
	return s;
}

enum nss_status _nss_redis_setspent(void) {
	return NSS_STATUS_SUCCESS;
}

enum nss_status _nss_redis_endspent(void) {
	return NSS_STATUS_SUCCESS;
}

enum nss_status _nss_redis_getspent_r(struct spwd *spw, char *buffer, size_t buflen, int *errnop) {
	enum nss_status s;

	if (spw == NULL) {
		*errnop = EPERM;
		return NSS_STATUS_UNAVAIL;
	}

	s = shadow_search(NULL, spw, buffer, buflen, errnop);
	return s;
}

static enum nss_status shadow_search(const char *name, struct spwd *spw, char *buffer, size_t buflen, int *errnop) {
	char *t_namp, *t_pwdp;
	long int t_lstchg, t_min, t_max, t_warn, t_inact, t_expire;
	unsigned long int t_flag;
	printf("=====> S_SEARCH %s",name); 
			return NSS_STATUS_UNAVAIL;
		*errnop = 0;
		spw->sp_namp = t_namp;
		spw->sp_pwdp = t_pwdp;
		spw->sp_lstchg = t_lstchg;
		spw->sp_min = t_min;
		spw->sp_max = t_max;
		spw->sp_warn = t_warn;
		spw->sp_inact = t_inact;
		spw->sp_expire = t_expire;
		spw->sp_flag = t_flag;
		return NSS_STATUS_SUCCESS;
}
