#!/bin/bash

PROG=beeversion
COLOR_RED="\\033[0;31m"
COLOR_GREEN="\\033[0;32m"
COLOR_NORMAL="\\033[0;39m\\033[0;22m"

function failed() {
    if [ -t 1 ]; then
        echo -e ${COLOR_RED}failed${COLOR_NORMAL}
    else
        echo failed
    fi
}

function success() {
    if [ -t 1 ]; then
        echo -e ${COLOR_GREEN}success${COLOR_NORMAL}
    else
        echo success
    fi
}

echo -n "${PROG}: Testing simple version: "
OUTPUT=$(./beeversion foo-1 2>/dev/null)
COMPARE=$(echo "PKGNAME=foo
PKGEXTRANAME=
PKGEXTRANAME_UNDERSCORE=
PKGEXTRANAME_DASH=
PKGVERSION=( 1 1 )
PKGEXTRAVERSION=
PKGEXTRAVERSION_UNDERSCORE=
PKGEXTRAVERSION_DASH=
PKGREVISION=
PKGARCH=
PKGFULLNAME=foo
PKGFULLVERSION=1
PKGFULLPKG=foo-1
PKGALLPKG=foo-1
PKGSUFFIX=")

if [ "${OUTPUT[@]}" != "${COMPARE[@]}" ]; then
    failed
else
    success
fi

echo -n "${PROG}: Testing invalid pkgname: "
./beesep foo 1>/dev/null 2>&1

if [ "$?" == "0" ]; then
    failed
else
    success
fi

exit 0
