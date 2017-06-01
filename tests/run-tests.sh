#!/bin/bash

set -e

echo "======= Runing functionnal tests ======"

id testuser 
id 12345
id nonexistantuser || true
id 888888 || true

getent passwd root | grep bash
getent passwd testuser | grep home
getent passwd nonexistantuser || true
getent passwd 888888 || true

getent group testgroup | grep testgroup
getent group 54321 | grep 54321
getent group nonexistantgroup || true
getent group 888888 || true

echo "=== All tests passed ===="


exit 0
