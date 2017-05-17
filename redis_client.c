#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <hiredis/hiredis.h>
#include "s_config.h"
#include "redis_client.h"



#define BUFFER_SIZE (256*1024) // 256kB

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

	c = redisConnectWithTimeout(REDIS_HOST, REDIS_PORT, timeout);
    if (tenant == NULL || c == NULL || c->err) {
		redisFree(c);
        goto fail;
    }


#if DEBUG
    printf( "@ %s::(in) cmd=GET %s/%s/%s\n", __FUNCTION__, "USER",tenant,name ) ;
#endif 

    reply = redisCommand(c,"GET %s/%s/%s","USER",tenant,name);
#if DEBUG
    printf("@ %s::(out) value=%s\n", __FUNCTION__, reply->str);
#endif
	if (reply->type == REDIS_REPLY_STRING) {
    	strncpy(data,reply->str,reply->len);  
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




