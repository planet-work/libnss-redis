#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <hiredis/hiredis.h>
#include "config.h"
#include "redis_client.h"



#define BUFFER_SIZE (256*1024) // 256kB


int redis_init(void) {
	return 0;
}

int redis_close(void) {
	return 0;
}

/*
 * Given a service name, queries Consul API and returns numeric address
 */

int redis_lookup(const char *service, const char *tenant, const char *name, char *data) {

    redisContext *c;
    redisReply *reply;

    struct timeval timeout = { 1, 500000 }; // 1.5 seconds

           
#if DEBUG
    printf( "@ %s::(in) name=%s\n", __FUNCTION__, name ) ;
#endif

#if defined(REDIS_SOCKET)
    c = redisConnectUnixWithTimeout(REDIS_SOCKET, timeout);
#else
	c = redisConnectWithTimeout(REDIS_HOST, REDIS_PORT, timeout);
#endif
    if (c == NULL || c->err) {
		redisFree(c);
        goto fail;
    }


#if DEBUG
#if USE_TENANT
    printf( "@ %s::(in) cmd=GET %s/%s/%s\n", __FUNCTION__, service, tenant, name );
	if (tenant == NULL) {
		redisFree(c);
        goto fail;
	}
#else
    printf( "@ %s::(in) cmd=GET %s/%s\n", __FUNCTION__, service, name );
#endif
#endif 

#if USE_TENANT
	if (tenant == NULL) {
		redisFree(c);
        goto fail;
	}
    reply = redisCommand(c,"GET %s/%s/%s", service, tenant, name);
#else
    reply = redisCommand(c,"GET %s/%s", service, name);
#endif


#if DEBUG
    printf("@ %s::(out) value=%s\n", __FUNCTION__, reply->str);
#endif
	if (reply->type == REDIS_REPLY_STRING) {
    	strncpy(data,reply->str,reply->len+1);  
 	} else {
        freeReplyObject(reply);
		redisFree(c);
		goto fail;
	}



    freeReplyObject(reply);
    redisFree(c);
    return 0;


fail:
#if DEBUG
     printf( "@ %s::(out) Error\n", __FUNCTION__ ); 
#endif
    return 1;
}




