libnss_redis:

With the following lines in /etc/nsswitch.conf

passwd:         compat redis
group:          compat redis
shadow:         compat redis

and /lib/libnss_redis.so.2 from this package, glibc will
not only look in /etc/{passwd,shadow,group} but also in
/var/lib/redis/{passwd,shadow,group}. It will limit itself
to uids and gids of at least 500 - where detectable - to avoid
root or system account access. (Except users with gid 100
are also allowed, see below).

Since version 0.2, lines starting with # are ignored.

Since version 0.3 gid 100 is also allowed, because that is
the "users" group on Debian systems. (So people can put users
in that group instead of their own (though I strongly
discourage not every user having their own group by default)
without needing to have a special group with gid >= 500 for that).

Since version 0.5 behaviour if a group with too low gid is the
same as it has with users before: They are ignored but no longer
considered an error causing the file no longer being processed.

Since version 0.6 empty lines are ignored (before they were
considered broken lines and thus ended the processing of the file).

Security considerations:
	Always use after compat in nsswitch.conf,
	otherwise it could overwrite the shadow-password for root.
	(shadow has no uids, so this cannot be ruled out)

	If someone is able to place terminals instead of the
	files, that could cause all programs to get a new
	controling terminal, making DoS attacks possible.

