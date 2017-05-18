#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <nss.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

#include "config.h"
#include "redis_client.h"

#include "webhost.h"

enum nss_status _nss_redis_setwebhostent(void);
enum nss_status _nss_redis_endwebhostent(void);
enum nss_status _nss_redis_getwebhostbyname_r(const char *, struct webhost *, char *, size_t, int *);
enum nss_status _nss_redis_getwebhostent_r(struct webhost *wh, char *buffer, size_t buflen, int *errnop);

static enum nss_status w_search(const char *host, struct webhost *wh, int *errnop, char *buffer, size_t buflen);

static inline enum nss_status w_search(const char *host, struct webhost *wh, int *errnop, char *buffer, size_t buflen) {
    char *webhost_data = buffer;
    char *token;
	const char *delim = ":";
	int res = 0;
	char *tenant = NULL;

#if USE_TENANT
    tenant = getenv("TENANT");
	if (tenant == NULL)
		return NSS_STATUS_UNAVAIL;
#endif

	memset(webhost_data, 0, buflen);

    res = redis_lookup("WEBHOST",tenant, host, webhost_data);

	if (res > 0)
		return NSS_STATUS_NOTFOUND;

	*errnop = 0;
	// wh_name : string
    token = strtok(webhost_data, delim);
	wh->wh_name = (char*) malloc(strlen(token)+1);
    strncpy(wh->wh_name, token, strlen(token)+1);

	// wh_vhost : string
    token = strtok(NULL, delim);
	wh->wh_vhost = (char*) malloc(strlen(token)+1);
    strncpy(wh->wh_vhost, token, strlen(token)+1);

	//  wh_user : string
    token = strtok(NULL, delim);
	wh->wh_user = (char*) malloc(strlen(token)+1);
    strncpy(wh->wh_user, token, strlen(token)+1);

	//  wh_dir : string
    token = strtok(NULL, delim);
	wh->wh_dir = (char*) malloc(strlen(token)+1);
    strncpy(wh->wh_dir, token, strlen(token)+1);

	//  wh_db_socket : string
    token = strtok(NULL, delim);
	wh->wh_db_socket = (char*) malloc(strlen(token)+1);
    strncpy(wh->wh_db_socket, token, strlen(token)+1);

	//  wh_php_mode : string
    token = strtok(NULL, delim);
	wh->wh_php_mode = (char*) malloc(strlen(token)+1);
    strncpy(wh->wh_php_mode, token, strlen(token)+1);

	//  wh_php_config : string
    token = strtok(NULL, delim);
	wh->wh_php_config = (char*) malloc(strlen(token)+1);
    strncpy(wh->wh_php_config, token, strlen(token)+1);
	return NSS_STATUS_SUCCESS;
}

enum nss_status _nss_redis_getwebhostbyname_r(const char *name, struct webhost *result, char *buf, size_t buflen, int *errnop) {
	*errnop = 0;
	if (result)
		return w_search(name, result, errnop, buf, buflen);
	else
		return NSS_STATUS_UNAVAIL;
}

enum nss_status _nss_redis_setwebhostent(void) {
	printf("_nss_redis_setpwent\n");
	return NSS_STATUS_SUCCESS;
}

enum nss_status _nss_redis_endwebhostent(void) {
	printf("_nss_redis_endpwent\n");
	return NSS_STATUS_SUCCESS;
}

enum nss_status _nss_redis_getwebhostent_r(struct webhost *wh, char *buffer, size_t buflen, int *errnop) {
	*errnop = -1;

	// Not yet implemented
	return NSS_STATUS_UNAVAIL;

	if (wh == NULL)
		return NSS_STATUS_UNAVAIL;
  	return w_search(NULL, wh, errnop, buffer, buflen);
}

