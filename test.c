#include <stdio.h>
#include "redis_client.h"


int main() {
	char *data = malloc(1000);
	int res;

	memset(data, 0, 1000);
	res = redis_lookup("user","mutupw", "demo",data);
    printf("RES=%s\n",data);

	res = redis_lookup("user","mutupw", "homere",data);
    printf("RES=%s\n",data);

	res = redis_lookup("user","mutupw", "2000",data);
    printf("RES=%s\n",data);
	free(data);
    return 0;	
}
