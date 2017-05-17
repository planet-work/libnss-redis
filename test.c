#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <grp.h>
#include <nss.h>
#include "redis_client.h"

enum nss_status _nss_redis_getpwuid_r(uid_t uid, struct passwd *result, char *buf, size_t buflen, int *errnop);
enum nss_status _nss_redis_getpwnam_r(const char *name, struct passwd *result, char *buf, size_t buflen, int *errnop);
enum nss_status _nss_redis_getgrnam_r(const char *name, struct group *gr, char *buffer, size_t buflen, int *errnop);
enum nss_status _nss_redis_getgrgid_r(const gid_t gid, struct group *gr, char *buffer, size_t buflen, int *errnop);

int main(void) {
	char *data = malloc(1000);
	int res;
	int errnop;
	struct passwd *pw = malloc(sizeof(struct passwd));
	struct group *gr = malloc(sizeof(struct group));
   
	pw->pw_name = NULL; 
	pw->pw_passwd = NULL;
	pw->pw_uid = 0;
	pw->pw_gid = 0;
	pw->pw_gecos = NULL;
	pw->pw_dir = NULL;
	pw->pw_shell = NULL;

	gr->gr_name = NULL;
	gr->gr_passwd = NULL;
	gr->gr_gid = 0;
	gr->gr_mem = NULL;

	memset(data, 0, 1000);


    printf("===================================================\n");
	printf("             LOOKUP USER                           \n");
    printf("===================================================\n");

	res = redis_lookup("USER","tenant1", "nouser",data);
	assert(res == 1);

	res = redis_lookup("USER",NULL, "testuser",data);
	assert(res == 1);

	res = redis_lookup("USER","tenant1", "testuser",data);
	assert(res == 0);
	assert(strcmp(data,"testuser:x:2000:1002:Web User:/home/testuser:/bin/false") == 0);
    printf("RES=%s\n",data);

	res = redis_lookup("USER","tenant1", "2000",data);
	assert(res == 0);
	assert(strcmp(data,"testuser:x:2000:1002:Web User:/home/testuser:/bin/false") == 0);
    printf("RES=%s\n",data);

	memset(data, 0, 1000);

    printf("\n===================================================\n");

	unsetenv("TENANT");
	setenv("TENANT", "tenant1", 1);

	memset(data, 0, 1000);
    _nss_redis_getpwnam_r("testuser", pw, data, 1000, &errnop);
    assert(strcmp(pw->pw_name, "testuser") == 0);
	assert(strcmp(pw->pw_passwd, "x") == 0);
	assert(pw->pw_uid == 2000);
	assert(pw->pw_gid == 1002);
	assert(strcmp(pw->pw_gecos, "Web User") == 0);
	assert(strcmp(pw->pw_dir, "/home/testuser") == 0);
	assert(strcmp(pw->pw_shell, "/bin/false") == 0);


	memset(data, 0, 1000);
	_nss_redis_getpwuid_r(2000, pw, data, 1000, &errnop);
    assert(strcmp(pw->pw_name, "testuser") == 0);
	assert(strcmp(pw->pw_passwd, "x") == 0);
	assert(pw->pw_uid == 2000);
	assert(pw->pw_gid == 1002);
	assert(strcmp(pw->pw_gecos, "Web User") == 0);
	assert(strcmp(pw->pw_dir, "/home/testuser") == 0);
	assert(strcmp(pw->pw_shell, "/bin/false") == 0);

	printf("RESULT={ pw_name=%s, pw_passwd=%s, pw_uid=%i, pw_gid=%i, pw_gecos=%s, pw_dir=%s, pw_shell=%s }\n",
			pw->pw_name,
			pw->pw_passwd,
		    pw->pw_uid,
		    pw->pw_gid,
		    pw->pw_gecos,
		    pw->pw_dir,
		    pw->pw_shell );


    printf("===================================================\n");
	printf("             LOOKUP GROUP                          \n");
    printf("===================================================\n");


	memset(data, 0, 1000);
	res = redis_lookup("GROUP","tenant1", "nogroup",data);
	assert(res == 1);

	memset(data, 0, 1000);
	res = redis_lookup("GROUP",NULL, "webusers",data);
	assert(res == 1);

	memset(data, 0, 1000);
	res = redis_lookup("GROUP","tenant1", "webusers",data);
	assert(res == 0);
    printf("RES=%s\n",data);
	assert(strcmp(data,"webusers:x:1002:www-data") == 0);

	res = redis_lookup("GROUP","tenant1", "1002",data);
	assert(res == 0);
	assert(strcmp(data,"webusers:x:1002:www-data") == 0);
    printf("RES=%s\n",data);

    printf("===================================================\n");

	unsetenv("TENANT");
	setenv("TENANT", "tenant1", 1);

	memset(data, 0, 1000);
    _nss_redis_getgrnam_r("webusers", gr, data, 1000, &errnop);
    assert(strcmp(gr->gr_name, "webusers") == 0);
	assert(strcmp(gr->gr_passwd, "x") == 0);
	assert(gr->gr_gid == 1002);


	memset(data, 0, 1000);
	_nss_redis_getgrgid_r(1002, gr, data, 1000, &errnop);
    assert(strcmp(gr->gr_name, "webusers") == 0);
	assert(strcmp(gr->gr_passwd, "x") == 0);
	assert(gr->gr_gid == 1002);


	// Cleanup
	free(data);

	free(pw->pw_name);
	free(pw->pw_passwd);
	free(pw->pw_gecos);
	free(pw->pw_dir);
	free(pw->pw_shell);
	free(pw);

	free(gr->gr_name);
	free(gr->gr_passwd);
	free(gr);

    return 0;	
}
