#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <grp.h>
#include <nss.h>
#include "redis_client.h"

#include "webhost.h"

enum nss_status _nss_redis_getpwuid_r(uid_t uid, struct passwd *result, char *buf, size_t buflen, int *errnop);
enum nss_status _nss_redis_getpwnam_r(const char *name, struct passwd *result, char *buf, size_t buflen, int *errnop);
enum nss_status _nss_redis_getgrnam_r(const char *name, struct group *gr, char *buffer, size_t buflen, int *errnop);
enum nss_status _nss_redis_getgrgid_r(const gid_t gid, struct group *gr, char *buffer, size_t buflen, int *errnop);
enum nss_status _nss_redis_getwebhostbyname_r(const char *name, struct webhost *result, char *buf, size_t buflen, int *errnop);

int main(void) {
	char *data = malloc(1000);
	int res;
	int errnop;
	struct passwd *pw = malloc(sizeof(struct passwd));
	struct group *gr = malloc(sizeof(struct group));
	struct webhost *wh = malloc(sizeof(struct webhost));
   
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

	wh->wh_name = NULL;
	wh->wh_vhost = NULL;
	wh->wh_user = NULL;
	wh->wh_dir = NULL;
	wh->wh_db_socket = NULL;
	wh->wh_php_mode = NULL;
	wh->wh_php_config = NULL;


    printf("===================================================\n");
	printf("             LOOKUP USER                           \n");
    printf("===================================================\n");

	res = redis_lookup("USER","tenant1", "nouser",data);
	assert(res == 1);

	//res = redis_lookup("USER",NULL, "testuser",data);
	//assert(res == 1);

	res = redis_lookup("USER","tenant1", "testuser",data);
	assert(res == 0);
	assert(strcmp(data,"testuser:x:12345:54321:Web User:/home/testuser:/bin/false") == 0);
    printf("RES=%s\n",data);

	res = redis_lookup("USER","tenant1", "12345",data);
	assert(res == 0);
	assert(strcmp(data,"testuser:x:12345:54321:Web User:/home/testuser:/bin/false") == 0);
    printf("RES=%s\n",data);


    printf("\n===================================================\n");

	unsetenv("TENANT");
	setenv("TENANT", "tenant1", 1);

    _nss_redis_getpwnam_r("testuser", pw, data, 1000, &errnop);
    assert(strcmp(pw->pw_name, "testuser") == 0);
	assert(strcmp(pw->pw_passwd, "x") == 0);
	assert(pw->pw_uid == 12345);
	assert(pw->pw_gid == 54321);
	assert(strcmp(pw->pw_gecos, "Web User") == 0);
	assert(strcmp(pw->pw_dir, "/home/testuser") == 0);
	assert(strcmp(pw->pw_shell, "/bin/false") == 0);


	_nss_redis_getpwuid_r(12345, pw, data, 1000, &errnop);
    assert(strcmp(pw->pw_name, "testuser") == 0);
	assert(strcmp(pw->pw_passwd, "x") == 0);
	assert(pw->pw_uid == 12345);
	assert(pw->pw_gid == 54321);
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

	free(pw->pw_name);
	free(pw->pw_passwd);
	free(pw->pw_gecos);
	free(pw->pw_dir);
	free(pw->pw_shell);
	free(pw);


    printf("===================================================\n");
	printf("             LOOKUP GROUP                          \n");
    printf("===================================================\n");


	res = redis_lookup("GROUP","tenant1", "nogroup",data);
	assert(res == 1);

	//res = redis_lookup("GROUP",NULL, "testgroup",data);
	//assert(res == 1);

	res = redis_lookup("GROUP","tenant1", "testgroup",data);
	assert(res == 0);
    printf("RES=%s\n",data);
	assert(strcmp(data,"testgroup:x:54321:www-data") == 0);

	res = redis_lookup("GROUP","tenant1", "54321",data);
	assert(res == 0);
	assert(strcmp(data,"testgroup:x:54321:www-data") == 0);
    printf("RES=%s\n",data);

    printf("===================================================\n");

	unsetenv("TENANT");
	setenv("TENANT", "tenant1", 1);

    _nss_redis_getgrnam_r("testgroup", gr, data, 1000, &errnop);
    assert(strcmp(gr->gr_name, "testgroup") == 0);
	assert(strcmp(gr->gr_passwd, "x") == 0);
	assert(gr->gr_gid == 54321);
	assert(strcmp(gr->gr_mem[0],"www-data") == 0);


	_nss_redis_getgrgid_r(54321, gr, data, 1000, &errnop);
    assert(strcmp(gr->gr_name, "testgroup") == 0);
	assert(strcmp(gr->gr_passwd, "x") == 0);
	assert(gr->gr_gid == 54321);

	free(gr->gr_name);
	free(gr->gr_passwd);
	free(gr);

    printf("===================================================\n");
	printf("             LOOKUP WEBHOST                        \n");
    printf("===================================================\n");


	res = redis_lookup("WEBHOST","tenant1", "nonexistantwebsite.com",data);
	assert(res == 1);

	res = redis_lookup("WEBHOST","tenant1", "website.com",data);
	assert(res == 0);
    printf("RES=%s\n",data);
	assert(strcmp(data,"website.com:www.website.com:testuser:/home/testuser/public_html/www/:/var/run/mysqld/mysql.sock:default:display_errors=1;magic_quotes_gpc=1;") == 0);

    printf("===================================================\n");

	unsetenv("TENANT");
	setenv("TENANT", "tenant1", 1);

    _nss_redis_getwebhostbyname_r("website.com", wh, data, 1000, &errnop);
	printf("RESULT={ wh_name=%s, wh_vhost=%s, wh_user=%s, wh_dir=%s, wh_db_socket=%s, wh_php_mode=%s, wh_php_config=%s }\n",
			wh->wh_name,
			wh->wh_vhost,
		    wh->wh_user,
		    wh->wh_dir,
		    wh->wh_db_socket,
		    wh->wh_php_mode,
		    wh->wh_php_config);

    assert(strcmp(wh->wh_name, "website.com") == 0);
    assert(strcmp(wh->wh_vhost, "www.website.com") == 0);
	assert(strcmp(wh->wh_user, "testuser") == 0);
	assert(strcmp(wh->wh_dir, "/home/testuser/public_html/www/") == 0);
	assert(strcmp(wh->wh_db_socket, "/var/run/mysqld/mysql.sock") == 0);
	assert(strcmp(wh->wh_php_mode, "default") == 0);
	assert(strcmp(wh->wh_php_config, "display_errors=1;magic_quotes_gpc=1;") == 0);

	free(wh->wh_name);
	free(wh->wh_vhost);
	free(wh->wh_user);
	free(wh->wh_dir);
	free(wh->wh_db_socket);
	free(wh->wh_php_mode);
	free(wh->wh_php_config);
	free(wh);


	// Cleanup
	free(data);


    return 0;	
}
