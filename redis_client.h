

int redis_init(void);
int redis_close(void);
int redis_lookup(const char *service, const char *tenant, const char *name, char *data);
