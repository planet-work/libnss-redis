#!/bin/bash

set -e

REDIS='redis-cli -s /var/run/redis/redis-webconf.sock'

$REDIS SET USER/testuser "testuser:x:12345:54321:Web User:/home/testuser:/bin/false"
$REDIS SET USER/12345 "testuser:x:12345:54321:Web User:/home/testuser:/bin/false"
$REDIS SET GROUP/testgroup "testgroup:x:54321:www-data"
$REDIS SET GROUP/54321 "testgroup:x:54321:www-data"

exit 0
