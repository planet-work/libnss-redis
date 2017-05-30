# Makefile for nss-redis

CC = gcc 
prefix = /usr
exec_prefix = ${prefix}
libprefix = ${exec_prefix}/lib
DESTDIR=
OBJECTS=shadow.o passwd.o group.o webhost.o redis_client.o  
SHARED_OBJECT = libnss_redis$(BITSOFS).so.2
INSTALL_NAME = libnss_redis.so.2
# This only works sometimes, give manually when needed:
CFLAGS = -g -Wall -Wstrict-prototypes -Wpointer-arith -Wmissing-prototypes  -D_FORTIFY_SOURCE=2\
		 -Wformat -Werror=format-security -Wall \
		 -Wextra -Wmissing-prototypes -Wstrict-prototypes -Wpointer-arith -fPIC
CPPFLAGS =
LIBS = -lc -lhiredis
LDLIBFLAGS = -shared -Wl,-soname,$(INSTALL_NAME)
LDFLAGS = -Wl,-z,defs
VERSION = unreleased

all: $(SHARED_OBJECT)

$(SHARED_OBJECT): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDLIBFLAGS) $(LDFLAGS) -o $(SHARED_OBJECT) $(OBJECTS) $(LIBS)

%.o: %.c config.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -fPIC -c -o $@ $<

install:
	install -m755 -d $(DESTDIR)$(libprefix)/
	install -m644 $(SHARED_OBJECT) $(DESTDIR)$(libprefix)/$(INSTALL_NAME)

clean:
	rm -f $(OBJECTS) test.o
	rm -f $(SHARED_OBJECT)
	rm -f a.out
	rm -f test

distclean: clean

dist: Makefile README config.h $(patsubst %.o,%.c,$(OBJECTS))
	mkdir libnss-redis-$(VERSION)
	install -m 644 $^ libnss-redis-$(VERSION)
	tar -cvvzf libnss-redis_$(VERSION).orig.tar.gz libnss-redis-$(VERSION)
	rm -r libnss-redis-$(VERSION) 

test: test.o redis_client.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o test redis_client.o test.o passwd.o group.o webhost.o $(LIBS)


.PHONY: all
