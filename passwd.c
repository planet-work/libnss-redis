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
#include <json-c/json.h>

#include "s_config.h"

enum nss_status _nss_redis_getpwuid_r(uid_t, struct passwd *, char *, size_t, int *);
enum nss_status _nss_redis_setpwent(void);
enum nss_status _nss_redis_endpwent(void);
enum nss_status _nss_redis_getpwnam_r(const char *, struct passwd *, char *, size_t, int *);
enum nss_status _nss_redis_getpwent_r(struct passwd *, char *, size_t, int *);

int prout = 0;

struct passwd *_pw;

static enum nss_status p_search(const char *name, const uid_t uid, struct passwd *pw, int *errnop, char *buffer, size_t buflen);

int redis_lookup(const char *service,const char *name, char *addr);


static inline enum nss_status p_search(const char *name, const uid_t uid, struct passwd *pw, int *errnop, char *buffer, size_t buflen) {
    char *pwd_data = malloc(1000);
    if (uid == 0) {
        redis_lookup("user",name,pwd_data);
    } else {
        char *_name = malloc(100);
        sprintf(_name, "%d", uid);
        redis_lookup("user",_name,pwd_data); 
    }
	_pw = pw;
    // {"shell": "/bin/bash", "workphone": "", "uid": 1234500002, "passwd": "x", "roomnumber": "", "gid": 1234500002, "groups": ["truc"], "home": "/home/truc", "fullname": "Truc", "homephone": "", "name": "truc"}
	json_object *jpwd = json_tokener_parse(pwd_data);
    if (jpwd == 0) {
         free(pwd_data);
         return NSS_STATUS_UNAVAIL;
   
    }
	*errnop = 0;
	json_object *jobj = malloc(1000);

	json_object_object_get_ex(jpwd, "name",&jobj);
	pw->pw_name = (char*) malloc(strlen(json_object_get_string(jobj))+1);
	strcpy(pw->pw_name,json_object_get_string(jobj));

	json_object_object_get_ex(jpwd, "uid",&jobj);
	pw->pw_uid = json_object_get_int(jobj);

	json_object_object_get_ex(jpwd, "passwd",&jobj);
	pw->pw_passwd = (char*) malloc(strlen(json_object_get_string(jobj))+1);
	strcpy(pw->pw_passwd,json_object_get_string(jobj));

	json_object_object_get_ex(jpwd, "gid",&jobj);
	pw->pw_gid = json_object_get_int(jobj);

	json_object_object_get_ex(jpwd, "fullname",&jobj);
	pw->pw_gecos = (char*) malloc(strlen(json_object_get_string(jobj))+1);
	strcpy(pw->pw_gecos,json_object_get_string(jobj));

	json_object_object_get_ex(jpwd, "home",&jobj);
	pw->pw_dir = (char*) malloc(strlen(json_object_get_string(jobj))+1);
	strcpy(pw->pw_dir,json_object_get_string(jobj));

	json_object_object_get_ex(jpwd, "shell",&jobj);
	pw->pw_shell = (char*) malloc(strlen(json_object_get_string(jobj))+1);
	strcpy(pw->pw_shell,json_object_get_string(jobj));

	free(jobj);
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
	/*
    free(_pw->pw_name);
	free(_pw->pw_passwd);
	free(_pw->pw_gecos);
	free(_pw->pw_dir);
	free(_pw->pw_shell);*/
	return NSS_STATUS_SUCCESS;
}

enum nss_status _nss_redis_getpwent_r(struct passwd *pw, char *buffer, size_t buflen, int *errnop) {
	*errnop = -1;

	if (pw == NULL)
		return NSS_STATUS_UNAVAIL;
  	return p_search(NULL,0, pw, errnop, buffer, buflen);
}

