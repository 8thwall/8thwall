#!/bin/bash
pg_isready > /dev/null
dbReady=$?

set -e
cd "$(dirname "$0")"

if [ $dbReady -ne 0 ]
then
  pg_ctl -D /usr/local/var/postgres start
fi

printReport=false
while getopts :r flag
do
    case "${flag}" in
        r) printReport=true;;
        [?]) echo >&2 "Usage: $0 [-r]"
             exit 1;;
    esac
done

npm ci
if [ $printReport ]
then
  npm run test-build-report
else
  npm run test
fi
npm run clean

cd -
