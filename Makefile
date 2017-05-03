# Makefile for nss-redis

CC = gcc  -fdiagnostics-color=auto
prefix = /usr
exec_prefix = ${prefix}
BITSOFS=
libprefix = ${exec_prefix}/lib$(BITSOFS)
DESTDIR=
OBJSUFFIX=$(BITSOFS).o
OBJECTS=shadow$(OBJSUFFIX) passwd$(OBJSUFFIX) group$(OBJSUFFIX)  redis_client$(OBJSUFFIX)
SHARED_OBJECT = libnss_redis$(BITSOFS).so.2
INSTALL_NAME = libnss_redis.so.2
# This only works sometimes, give manually when needed:
BIT_CFLAGS = $(if $(BITSOFS),-m$(BITSOFS))
CFLAGS = $(BIT_CFLAGS) -g -O2 -Wall -Wstrict-prototypes -Wpointer-arith -Wmissing-prototypes
CPPFLAGS =
LIBS = -lc -ljson-c
LDLIBFLAGS = -shared -Wl,-soname,$(INSTALL_NAME)
LDFLAGS = -Wl,-z,defs
VERSION = unreleased

all: $(SHARED_OBJECT)

$(SHARED_OBJECT): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDLIBFLAGS) $(LDFLAGS) -o $(SHARED_OBJECT) $(OBJECTS) $(LIBS)

%$(OBJSUFFIX): %.c s_config.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -fPIC -c -o $@ $<

install:
	install -m755 -d $(DESTDIR)$(libprefix)/
	install -m644 $(SHARED_OBJECT) $(DESTDIR)$(libprefix)/$(INSTALL_NAME)

clean:
	rm -f $(OBJECTS)
	rm -f $(SHARED_OBJECT)

distclean: clean

dist: Makefile README s_config.h $(patsubst %$(OBJSUFFIX),%.c,$(OBJECTS))
	mkdir libnss-redis-$(VERSION)
	install -m 644 $^ libnss-redis-$(VERSION)
	tar -cvvzf libnss-redis_$(VERSION).orig.tar.gz libnss-redis-$(VERSION)
	rm -r libnss-redis-$(VERSION)

.PHONY: all
