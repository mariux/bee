#!/bin/bash

PROG=beesep
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

echo -n "${PROG}: Testing separation: "
OUTPUT=$(./beesep foo=bar:bar=foo 2>/dev/null)
COMPARE=$(echo -e "foo='bar'\nbar='foo'")

if [ "${OUTPUT[@]}" != "${COMPARE[@]}" ]; then
    failed
else
    success
fi

echo -n "${PROG}: Testing escape characters: "
OUTPUT=$(./beesep "foo=bar':bar=f'oo" 2>/dev/null)
COMPARE=$(echo -e "foo='bar'\'''\nbar='f'\''oo'")

if [ "${OUTPUT[@]}" != "${COMPARE[@]}" ]; then
    failed
else
    success
fi

echo -n "${PROG}: Testing empty values: "
OUTPUT=$(./beesep "foo=:bar=" 2>/dev/null)
COMPARE=$(echo -e "foo=''\nbar=''")

if [ "${OUTPUT[@]}" != "${COMPARE[@]}" ]; then
    failed
else
    success
fi

echo -n "${PROG}: Testing empty key in value: "
OUTPUT=$(./beesep "ffff=foo:=foo:foo=bar" 2>/dev/null)
COMPARE=$(echo -e "ffff='foo:=foo'\nfoo='bar'")

if [ "${OUTPUT[@]}" != "${COMPARE[@]}" ]; then
    failed
else
    success
fi

echo -n "${PROG}: Testing ':' character in value: "
OUTPUT=$(./beesep "file=aaa=bbbb:ccc:ffff=xxxx" 2>/dev/null)
COMPARE=$(echo -e "file='aaa=bbbb:ccc'\nffff='xxxx'")

if [ "${OUTPUT[@]}" != "${COMPARE[@]}" ]; then
    failed
else
    success
fi

echo -n "${PROG}: Testing '=' character in value: "
OUTPUT=$(./beesep "file=foo=bar:foo=bar" 2>/dev/null)
COMPARE=$(echo -e "file='foo=bar'\nfoo='bar'")

if [ "${OUTPUT[@]}" != "${COMPARE[@]}" ]; then
    failed
else
    success
fi

echo -n "${PROG}: Testing values with many ' characters: "
OUTPUT=$(./beesep "file=foo'bar:foo=bar:baz=b'o'b:twak='b'q'':foo=ba123'" 2>/dev/null)
COMPARE=$(echo -e "file='foo'\''bar'\nfoo='bar'\nbaz='b'\''o'\''b'\ntwak=''\''b'\''q'\'''\'''\nfoo='ba123'\'''")

if [ "${OUTPUT[@]}" != "${COMPARE[@]}" ]; then
    failed
else
    success
fi

echo -n "${PROG}: Testing ':' character at the beginning: "
./beesep ":ffff=xxxx" 1>/dev/null 2>&1

if [ "$?" == "0" ]; then
    failed
else
    success
fi

echo -n "${PROG}: Testing '=' character at the beginning: "
./beesep "=ffff=xxxx" 1>/dev/null 2>&1

if [ "$?" == "0" ]; then
    failed
else
    success
fi

echo -n "${PROG}: Testing missing argument: "
./beesep 1>/dev/null 2>&1

if [ "$?" == "0" ]; then
    failed
else
    success
fi

echo -n "${PROG}: Testing key-like value: "
./beesep "foo=bar:fo,o=bar" 1>/dev/null 2>&1

if [ "$?" != "0" ]; then
    failed
else
    success
fi

echo -n "${PROG}: Testing non-alnum characters in key: "
./beesep "foo,2=bar:foo=bar" 1>/dev/null 2>&1

if [ "$?" == "0" ]; then
    failed
else
    success
fi

echo -n "${PROG}: Testing missing equal sign: "
./beesep "foo=bar:foo:bar=r" 1>/dev/null 2>&1

if [ "$?" != "0" ]; then
    failed
else
    success
fi

echo -n "${PROG}: Testing missing equal sign at the beginning: "
./beesep "foo:bar=r" 1>/dev/null 2>&1

if [ "$?" == "0" ]; then
    failed
else
    success
fi

echo -n "${PROG}: Testing return value on success: "
./beesep "foo=bar:bar=foo" 1>/dev/null 2>&1

if [ "$?" != "0" ]; then
    failed
else
    success
fi

exit 0
