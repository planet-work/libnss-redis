# libnss_redis

`libnss-redis` is a [Redis](https://redis.io/) dabatase backend for[GNU Libc Name Service Switch.](https://www.gnu.org/software/libc/manual/html_node/Name-Service-Switch.html)  It can get users and groups from a remote database.

This backend has been created for mass web hosting with dynamic configuration.

## Usage

### Redis database

The connection configuration is done at compile time with values in `config.h`. The socket will be use if `REDIS_SOCKET` is defined.

The database format is :

* keys: USER/_username_, USER/_uid_, GROUP/_groupname_, GROUP/_gid_
* Values: same format as in `/etc/passwd` and `/etc/group`

There are examples in `.drone.yml` configuration file.

### Local configuration

With the following lines in `/etc/nsswitch.conf`

```
passwd:         compat redis
group:          compat redis
shadow:         compat
```

##Missing features

 * Listing users : `getent passwd|group`
 * shadow support
 * write support 

##Security considerations

Always use after compat in nsswitch.conf,
otherwise it could overwrite the shadow-password for root.
(shadow has no uids, so this cannot be ruled out)

	If someone is able to place terminals instead of the
	files, that could cause all programs to get a new
	controling terminal, making DoS attacks possible.


## Author

Frédéric VANNIÈRE <f.vanniere@planet-work.com>