/*
** beeversion - compare bee package versionnumbers
**
** Copyright (C) 2009-2012
**       Marius Tolzmann <tolzmann@molgen.mpg.de>
**       Tobias Dreyer <dreyer@molgen.mpg.de>
**       and other bee developers
**
** This file is part of bee.
**
** bee is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "bee_version.h"

#define EXTRA_UNKNOWN 200
#define EXTRA_ALPHA   1
#define EXTRA_BETA    2
#define EXTRA_RC      3
#define EXTRA_NONE    4
#define EXTRA_PATCH   5
#define EXTRA_ANY     6

struct extra_version {
    char         *string;
    unsigned int  priority;
    size_t        length;
};

static int _cmp_extraversion_string(char *v1, char *v2);

static int _cmp_version_string(char *v1, char *v2) {
    char *a, *b;
    long long i,j;

    a = v1;
    b = v2;

    while (*a && *b && *a == *b && *a != '_') {
        a++;
        b++;

        if (*a == *b)
            continue;

        /* skip leading zeros of numbers != 0 */
        if (isdigit(*a) && isdigit(*b)) {
            char *c;

            for (c=a; *c == '0'; c++)
               ;
            if (isdigit(*c))
               a = c;

            for (c=b; *c == '0'; c++)
               ;
            if (isdigit(*c))
               b = c;
        }
    }

    if (*a == *b && *a == '_')
        return _cmp_extraversion_string(a+1, b+1);

    /* strings are equal ; *a==*b==0*/
    if (*a == *b)
        return 0;

    if (isdigit(*a)) {
        if (isdigit(*b)) {
            /* rewind string to first digit */
            /* e.g. to compare 12 vs 100 and not 2 vs 00 */
            while (a > v1 && isdigit(*(a-1)) &&
                   b > v2 && isdigit(*(b-1))) {
                a--;
                b--;
            }
            i = atoll(a);
            j = atoll(b);

            if (i<j)
                return -1;
            if (i>j)
                return 1;

            /* numbers are equal but strings are not?           */
            /* yes ->  leading zeros: atoll("01") == atoll("1") */
            return 0;
        }
        /* a > ('.',alpha, 0) */
        return 1;
    }

    if (isalpha(*a)) {

        /*  alpha < digit */
        if (isdigit(*b))
            return -1;

        if (isalpha(*b)) {
            if (*a < *b)
                return -1;
            return 1;
        }
        return 1;
    }

    if (!*b)
        return 1;

    return -1;
}

static int _get_and_skip_extraversion(char **s)
{
    static struct extra_version extra[] = {
            { "alpha", EXTRA_ALPHA, 5 },
            { "a",     EXTRA_ALPHA, 1 },
            { "beta",  EXTRA_BETA,  4 },
            { "b",     EXTRA_BETA,  1 },
            { "rc",    EXTRA_RC,    2 },
            { "cr",    EXTRA_RC,    2 },
            { "patch", EXTRA_PATCH, 5 },
            { "p",     EXTRA_PATCH, 1 },
            { NULL,    EXTRA_ANY,   0 }
        };
    struct extra_version *ev;
    char *p;

    p = *s;

    ev = extra;

    while(ev->string && strncmp(ev->string, p, ev->length))
        ev++;

    *s = p+ev->length;

    return ev->priority;
}

static int _cmp_extraversion_string(char *v1, char *v2) {
    char *a, *b;
    int i, j;

    a = v1;
    b = v2;

    while(*a && *b && *a == *b) {
        a++;
        b++;
    }

    if (isalpha(*a) && isalpha(*b)) {
        while(a > v1 && isalpha(*(a-1))) {
            a--;
            b--;
        }

        /* fallback to version_string compare if */
        /* we do not hit ^<extra> or _<extra>    */
        if (a > v1 && *(a-1) != '_')
            return _cmp_version_string(a, b);

        i = _get_and_skip_extraversion(&a);
        j = _get_and_skip_extraversion(&b);

        if (i!=j)
            return i-j;
    }

    return _cmp_version_string(a, b);
}

static int _cmp_string(char *a, char *b) {
    int res;

    res = strcmp(a, b);
    return res;
}

static int _cmp_pkgname(struct bee_version *a, struct bee_version *b) {
    int res;
    char *sa, *sb;
    char *na, *nb;
    int la, lb, len;

    na = a->name;
    nb = b->name;

    sa = strchr(na, ':');
    sb = strchr(nb, ':');

    la = sa ? sa - na : strlen(na);
    lb = sb ? sb - nb : strlen(nb);

    len = MIN(la, lb);

    res = strncmp(na, nb, len);
    if (res)
        return res;

    if (na[len] == ':' && nb[len] && nb[len] != ':')
             return -1;

    if (nb[len] == ':' && na[len] && na[len] != ':')
             return 1;

    res = _cmp_string(na + len, nb + len);
    return res;
}

static int _cmp_pkgextraname(struct bee_version *a, struct bee_version *b) {
    int res;

    res = _cmp_string(a->extraname, b->extraname);
    return res;
}

static int _cmp_pkgversionepoch(struct bee_version *a, struct bee_version *b) {
    int res;

    res = _cmp_version_string(a->versionepoch, b->versionepoch);
    return res;
}

static int _cmp_pkgversion(struct bee_version *a, struct bee_version *b) {
    int res;

    res = _cmp_version_string(a->version, b->version);
    return res;
}

static int _cmp_pkgextraversion(struct bee_version *a, struct bee_version *b) {
    int res;

    res = _cmp_extraversion_string(a->extraversion, b->extraversion);
    return res;
}

static int _cmp_pkgrevision(struct bee_version *a, struct bee_version *b) {
    int res;

    res = _cmp_version_string(a->revision, b->revision);
    return res;
}

static int _cmp_pkgprefix(struct bee_version *a, struct bee_version *b) {
    int res;

    res = _cmp_string(a->prefix, b->prefix);
    return res;
}

static int _cmp_pkgarch(struct bee_version *a, struct bee_version *b) {
    int res;

    res = _cmp_string(a->arch, b->arch);
    return res;
}

static int _cmp_pkgsuffix(struct bee_version *a, struct bee_version *b) {
    int res;

    res = _cmp_string(a->suffix, b->suffix);
    return res;
}


static int _cmp_pkgfullname(struct bee_version *a, struct bee_version *b) {
    int res;

    res = _cmp_pkgname(a, b);
    if (res)
        return res;

    res = _cmp_pkgextraname(a, b);
    return res;
}

static int _cmp_pkgfullversion(struct bee_version *a, struct bee_version *b) {
    int res;

    res = _cmp_pkgversionepoch(a, b);
    if (res)
        return res;

    res = _cmp_pkgversion(a, b);
    if (res)
        return res;

    res = _cmp_pkgextraversion(a, b);
    return res;
}

static int _cmp_pkgfullpkg(struct bee_version *a, struct bee_version *b) {
    int res;

    res = _cmp_pkgfullname(a, b);
    if (res)
        return res;

    res = _cmp_pkgfullversion(a, b);
    if (res)
        return res;

    res = _cmp_pkgrevision(a, b);
    return res;
}

static int _cmp_pkgother(struct bee_version *a, struct bee_version *b) {
    int res;

    res = _cmp_pkgprefix(a, b);
    if (res)
        return res;

    res = _cmp_pkgarch(a, b);
    if (res)
        return res;

    res = _cmp_pkgsuffix(a, b);
    return res;
}

int bee_version_compare_pkgfullname(struct bee_version *a, struct bee_version *b) {
    int res;

    assert(a);
    assert(b);

    res = _cmp_pkgfullname(a, b);
    return res;
}

int bee_version_compare(struct bee_version *a, struct bee_version *b) {
    int res;

    assert(a);
    assert(b);
    
    res = _cmp_pkgfullpkg(a, b);
    if (res)
        return res;

    res = _cmp_pkgother(a, b);
    return res;
}
