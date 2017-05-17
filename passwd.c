#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <nss.h>
#include <pwd.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

#include "s_config.h"
#include "redis_client.h"

enum nss_status _nss_redis_getpwuid_r(uid_t, struct passwd *, char *, size_t, int *);
enum nss_status _nss_redis_setpwent(void);
enum nss_status _nss_redis_endpwent(void);
enum nss_status _nss_redis_getpwnam_r(const char *, struct passwd *, char *, size_t, int *);
enum nss_status _nss_redis_getpwent_r(struct passwd *, char *, size_t, int *);

static enum nss_status p_search(const char *name, const uid_t uid, struct passwd *pw, int *errnop, char *buffer, size_t buflen);

static inline enum nss_status p_search(const char *name, const uid_t uid, struct passwd *pw, int *errnop, char *buffer, size_t buflen) {
    char *pwd_data = buffer;
    char *token;
	const char *delim = ":";
    char *tenant = getenv("TENANT");

	if (tenant == NULL)
		return NSS_STATUS_UNAVAIL;


    if (uid == 0) {
        redis_lookup("USER",tenant, name,pwd_data);
    } else {
        char *_name = malloc(100);
        sprintf(_name, "%d", uid);
        redis_lookup("USER",tenant, _name,pwd_data); 
		free(_name);
    }

	*errnop = 0;
	// pw_name : string
    token = strtok(pwd_data, delim);
	pw->pw_name = (char*) malloc(strlen(token)+1);
    strncpy(pw->pw_name, token, strlen(token)+1);

	// pw_passwd : string
    token = strtok(NULL, delim);
	pw->pw_passwd = (char*) malloc(strlen(token)+1);
    strncpy(pw->pw_passwd, token, strlen(token)+1);

	//  pw_uid : int
    token = strtok(NULL, delim);
	pw->pw_uid = atoi(token);

	//  pw_gid : int
    token = strtok(NULL, delim);
	pw->pw_gid = atoi(token);

	//  pw_gecos : string
    token = strtok(NULL, delim);
	pw->pw_gecos = (char*) malloc(strlen(token)+1);
    strncpy(pw->pw_gecos, token, strlen(token)+1);

	//  pw_dir : string
    token = strtok(NULL, delim);
	pw->pw_dir = (char*) malloc(strlen(token)+1);
    strncpy(pw->pw_dir, token, strlen(token)+1);

	//  pw_shell : string
    token = strtok(NULL, delim);
	pw->pw_shell = (char*) malloc(strlen(token)+1);
    strncpy(pw->pw_shell, token, strlen(token)+1);

	return NSS_STATUS_SUCCESS;
}

enum nss_status _nss_redis_getpwuid_r(uid_t uid, struct passwd *result, char *buf, size_t buflen, int *errnop) {
	*errnop = 0;
	if (result)
		return p_search(NULL, uid, result, errnop, buf, buflen);
	else
		return NSS_STATUS_UNAVAIL;
}

enum nss_status _nss_redis_getpwnam_r(const char *name, struct passwd *result, char *buf, size_t buflen, int *errnop) {
	*errnop = 0;
	if (result)
		return p_search(name, 0, result, errnop, buf, buflen);
	else
		return NSS_STATUS_UNAVAIL;
}

enum nss_status _nss_redis_setpwent(void) {

	return NSS_STATUS_SUCCESS;
}

enum nss_status _nss_redis_endpwent(void) {
	return NSS_STATUS_SUCCESS;
}

enum nss_status _nss_redis_getpwent_r(struct passwd *pw, char *buffer, size_t buflen, int *errnop) {
	*errnop = -1;

	if (pw == NULL)
		return NSS_STATUS_UNAVAIL;
  	return p_search(NULL,0, pw, errnop, buffer, buflen);
}

