#!/bin/bash

#set -x 

: ${BEEVERSION=${0%/*}/../beeversion}

function beeversion() {
    echo "beeversion ${@}"
    ${BEEVERSION} "${@}"
}

function test_parse_beeversion() {
    pkg=${1}
    cmp=${2}
    parsed=$( ${BEEVERSION} --format='/%p/%x/%v/%e/%r/%a/%s/%P/%V/%F/%A/%-x/%-e/%-i/%i/@e/@v/@V/@E/\n' "${pkg}" )
    
    echo -n "parse beeversion ${pkg} .. "

    if [ $? != 0 ] ; then
        echo "FAILED (beeversion failed with exit status $?)"
        return 1
    fi
    
    if [ "${parsed}" != "${cmp}" ] ; then
        echo "FAILED (strings do not match)"
        return 2
    fi
    
    echo "PASSED"
}

test_parse_beeversion \
    'pkg-1.2-3' \
    '/pkg//1.2//3///pkg/1.2/pkg-1.2-3/pkg-1.2-3////0//1.2 1 1.2/1.2 1 2//'

test_parse_beeversion \
    'pkg' \
    '/pkg///////pkg//pkg/pkg////0/////'

test_parse_beeversion \
    'pkg-abc' \
    '/pkg-abc///////pkg-abc//pkg-abc/pkg-abc////0/////'

test_parse_beeversion \
    'pkg-1.2' \
    '/pkg//1.2/////pkg/1.2/pkg-1.2/pkg-1.2////0//1.2 1 1.2/1.2 1 2//'

test_parse_beeversion \
    'pkg-1.2-abc' \
    '/pkg-1.2-abc///////pkg-1.2-abc//pkg-1.2-abc/pkg-1.2-abc////0/////'

test_parse_beeversion \
    'pkg-1.2-abc-0' \
    '/pkg-1.2-abc//0/////pkg-1.2-abc/0/pkg-1.2-abc-0/pkg-1.2-abc-0////0//0 0/0 0//'

test_parse_beeversion \
    'pkg_1.2-7' \
    '/pkg/1.2/7/////pkg_1.2/7/pkg_1.2-7/pkg_1.2-7/-1.2///0//7 7/7 7//'

test_parse_beeversion \
    'pn_pe-1_pe2-3.4_e5_e6-7' \
    '/pn/pe-1_pe2/3.4/e5_e6/7///pn_pe-1_pe2/3.4_e5_e6/pn_pe-1_pe2-3.4_e5_e6-7/pn_pe-1_pe2-3.4_e5_e6-7/-pe-1_pe2/-e5_e6//0/e5_e6 e5 e5_e6/3.4 3 3.4/3.4 3 4/e5_e6 e5 e6/'

test_parse_beeversion \
    'package_pkg-1.2-3-4.5-6' \
    '/package/pkg-1.2-3/4.5//6///package_pkg-1.2-3/4.5/package_pkg-1.2-3-4.5-6/package_pkg-1.2-3-4.5-6/-pkg-1.2-3///0//4.5 4 4.5/4.5 4 5//'

