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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>

#include <sys/utsname.h>

#include "bee_version.h"

#define EXTRA_UNKNOWN 200
#define EXTRA_ALPHA   1
#define EXTRA_BETA    2
#define EXTRA_RC      3
#define EXTRA_NONE    4
#define EXTRA_PATCH   5
#define EXTRA_ANY     6

#define _BEE_ACCEPT_DIGITS    "0123456789"
#define _BEE_ACCEPT_LOWERCASE "abcdefghijklmnopqrstuvwxyz"
#define _BEE_ACCEPT_UPPERCASE "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define _BEE_ACCEPT_ALPHA     _BEE_ACCEPT_LOWERCASE _BEE_ACCEPT_UPPERCASE

#define _BEE_ACCEPT_REVISION     _BEE_ACCEPT_DIGITS _BEE_ACCEPT_LOWERCASE "."
#define _BEE_ACCEPT_FULLVERSION  _BEE_ACCEPT_DIGITS _BEE_ACCEPT_LOWERCASE "._+"
#define _BEE_ACCEPT_FULLNAME     _BEE_ACCEPT_DIGITS _BEE_ACCEPT_ALPHA     "._+-"

void bee_version_parse_start(struct bee_version *v)
{
    assert(v);
    memset(v, 0, sizeof(*v));
}

void bee_version_parse_finish(struct bee_version *v)
{
    assert(v);
    free(v->_input);
    v->_input = NULL;
}

void bee_version_parse_reset(struct bee_version *v)
{
    bee_version_parse_finish(v);
    bee_version_parse_start(v);
}

struct bee_version *bee_version_alloc(void)
{
    struct bee_version *v;

    v = malloc(sizeof(*v));

    if(v)
        bee_version_parse_start(v);

    return v;
}

void bee_version_free(struct bee_version *v)
{
    bee_version_parse_finish(v);
    free(v);
}

BEE_STATIC_INLINE int _parse_setup(struct bee_version *v, char *input)
{
    char   *s;
    size_t  len;

    assert(input);
    assert(v);

    bee_version_parse_reset(v);

    s = strdup(input);

    if (!s) {
        perror("strdup");
        return 0;
    }

    v->_input = s;

    len  = strlen(s);
    s   += len;

    v->prefix       = s;
    v->name         = s;
    v->extraname    = s;
    v->version      = s;
    v->extraversion = s;
    v->revision     = s;
    v->arch         = s;
    v->suffix       = s;

    return 1;
}

BEE_STATIC_INLINE short _is_local_arch(char *s) {
    static struct utsname unm;
    static char *machine = NULL;

    if (!machine) {
        if(uname(&unm)) {
            perror("uname");
            return 0;
        }
        machine = unm.machine;
    }

    return !strcmp(s, machine);
}

BEE_STATIC_INLINE short _is_supported_arch(char *s)
{
    char *supported[] = { SUPPORTED_ARCHITECTURES, NULL };
    char **a;

    for(a = supported; *a; a++) {
        if(strcmp(s, *a))
            continue;
        return 1;
    }
    return 0;
}

/* set v->prefix and v->name */
BEE_STATIC_INLINE short _parse_prefix(struct bee_version *v)
{
    char *p;

    assert(v);
    assert(v->_input);

    p = strrchr(v->_input, '/');

    if (!p) {
        v->name = v->_input;
        return 0;
    }

    *p = 0;

    v->prefix = v->_input;;
    v->name   = p+1;

    return 1;
}

BEE_STATIC_INLINE short _parse_suffix(struct bee_version *v) 
{
    char *p;

    assert(v);
    assert(v->name);

    p = strstr(v->name, ".bee");

    if (!p)
        p = strstr(v->name, ".iee");

    if (!p)
        return 0;

    *p = 0;

    v->suffix = p+1;

    return 1;
}

BEE_STATIC_INLINE short _parse_arch(struct bee_version *v)
{
    char *p;

    assert(v);
    assert(v->name);

    p = strrchr(v->name, '.');

    if (!p)
        return 0;

    /* fast path: arch may not contain '-' */
    if (strchr(p, '-'))
        return 0;

    if (!_is_local_arch(p+1))
        if (!_is_supported_arch(p+1))
            return 0;

    v->arch = p+1;

    *p = 0;

    return 1;
}

BEE_STATIC_INLINE short _contains_invalid_chars(char *s, char *ok)
{
    size_t accepted;
    size_t len;

    assert(s);
    assert(ok);

    len      = strlen(s);
    accepted = strspn(s, ok);

    if (accepted != len) {
        return *(s+accepted);
    }

    return 0;
}

static int _verify_revision_string(char *s)
{
    assert(s);

    if(!*s) {
        fprintf(stderr, "beeversion: ERROR revision is empty.\n");
        return 0;
    }

    if (!isdigit(*s)) {
        fprintf(stderr, "beeversion: ERROR revision does not start with a digit (%s).\n", s);
        return 0;
    }

    if (_contains_invalid_chars(s, _BEE_ACCEPT_REVISION)) {
        fprintf(stderr, "beeversion: ERROR revision contains invalid chars (%s).\n", s);
        return 0;
    }

    return 1;
}

static int _verify_fullversion_string(char *s)
{
    size_t accepted;
    size_t len;

    assert(s);

    if (!isdigit(*s)) {
        fprintf(stderr, "beeversion: ERROR version does not start with a digit (%s).\n", s);
        return 0;
    }

    len      = strlen(s);
    accepted = strspn(s, _BEE_ACCEPT_FULLVERSION);

    if (accepted != len) {
        fprintf(stderr, "beeversion: ERROR version contains invalid chars (%s).\n", s);
        return 0;
    }

    return 1;
}

static int _verify_fullname_string(char *s)
{
    size_t accepted;
    size_t len;

    assert(s);

    if (!isalpha(*s)) {
        fprintf(stderr, "beeversion: ERROR name does not start with an alphabetic character (%s).\n", s);
        return 0;
    }

    len      = strlen(s);
    accepted = strspn(s, _BEE_ACCEPT_FULLNAME);

    if (accepted != len) {
        fprintf(stderr, "beeversion: ERROR name contains invalid chars (%s).\n", s);
        return 0;
    }

    return 1;
}

BEE_STATIC_INLINE int _parse_pkgfullversion(struct bee_version *v)
{
    char *p;

    assert(v);
    assert(v->version);

    p = strchr(v->version, '_');
    if (p) {
        *p++ = 0;
        v->extraversion = p;
    }

    return 1;
}

BEE_STATIC_INLINE int _parse_pkgfullname(struct bee_version *v)
{
    char *p;

    assert(v);
    assert(v->name);

    p = strchr(v->name, '_');
    if (p) {
        *p++ = 0;
        v->extraname = p;
    }

    return 1;
}

static int _parse_pkgfullpkg(struct bee_version *v)
{
    char *revision;
    char *version;

    assert(v);
    assert(v->name);

    revision = strrchr(v->name, '-');
    if (!revision) {
        fprintf(stderr, "beeversion: ERROR can't find revision when parsing pkgfullpkg (%s).\n", v->name);
        return 0;
    }

    if(!_verify_revision_string(revision+1))
        return 0;

    *revision++ = 0;

    v->revision = revision;

    version = strrchr(v->name, '-');
    if (!version) {
        fprintf(stderr, "beeversion: ERROR can't find version when parsing pkgfullpkg (%s).\n", v->name);
        goto restore_revision;
    }

    if(!_verify_fullversion_string(version+1))
        goto restore_revision;

    *version++ = 0;

    v->version  = version;

    if(!_verify_fullname_string(v->name))
        goto restore_version;

    _parse_pkgfullversion(v);
    _parse_pkgfullname(v);

    return 1;

restore_version:
    *--version = '-';

restore_revision:
    *--revision = '-';
    return 0;

}

BEE_STATIC_INLINE short _is_valid_revision_string(char *s)
{

    assert(s);

    if (!*s)
       return 0;

    if (!isdigit(*s))
       return 0;

    if (_contains_invalid_chars(s, _BEE_ACCEPT_REVISION))
       return 0;

    return 1;
}

BEE_STATIC_INLINE short _is_valid_fullversion_string(char *s)
{

    assert(s);

    if (!*s)
       return 0;

    if (!isdigit(*s))
       return 0;

    if (_contains_invalid_chars(s, _BEE_ACCEPT_FULLVERSION))
       return 0;

    return 1;
}

/*
static short _is_valid_fullname_string(char *s)
{

    assert(s);

    if (!*s)
        return 0;

    if (!isalpha(*s))
        return 0;

    if (_contains_invalid_chars(s, _BEE_ACCEPT_FULLNAME))
        return 0;

    return 1;
}
*/

/*
   AxBxC

   REV(C) -Y-> VER(B) -Y-> NAME(A) =Y=> N-V-R => N(A) V(B) R(C)
    |           |          |
    |           |          N-> SYNTAX ERROR => N() V() R()
    |           |
    N-----------N-> VER(C) -Y-> NAME(AxB) =Y=> N-V => N(AxB) V(C) R()
                     |          |
    (restore AxBxC)->|          N-> SYNTAX ERROR => N() V() R()
                     |
                     N-> NAME(AxBxC) =Y=> N => N(AxBxC) V() R()
                         |
                         N-> SYNTAX ERROR => N() V() R()
*/

static int _parse_partialpkg(struct bee_version *v)
{
    char *revision = NULL;
    char *version  = NULL;

    assert(v);
    assert(v->name);

    revision = strrchr(v->name, '-');

    /* N(AxBxC) V(NULL) R(NULL) */
    if (!revision) /* N(ABC) V(NULL) R(NULL) */
        goto finish_parsename;

    /* N(AxB-C) V(NULL) R(-C) */
    if (_is_valid_revision_string(revision+1)) {
        *revision++ = 0;
        version     = strrchr(v->name, '-');
        /* (2) N(AxB) V(?) R(C) */
    } else {
        version  = revision;
        revision = NULL;
        /* (1) N(AB-C) V(-C) R(NULL) */
    }

    /* (1) N(AB-C) V(-C) R(NULL) */
    /* (2) N(AxB)  V(?)  R(C)    */
    if (!version) {
        /* (2) N(AB)   V(NULL) R(C)    */
        *--revision = '-';
        version    = revision;
        revision   = NULL;
        /* (2 => 1) N(AB-C) V(-C) R(NULL) */
    }
    /* else: (2) N(A-B)  V(-B) R(C) */

    /* (1) N(AB-C) V(-C) R(NULL) */
    /* (2) N(A-B)  V(-B) R(C) */
    if (_is_valid_fullversion_string(version+1)) {
        *version++ = 0;
        /* (1) N(AB) V(C) R(NULL) */
        /* (2) N(A)  V(B) R(C) */
    } else if (revision && _is_valid_fullversion_string(revision)) {
        version  = revision;
        revision = NULL;
        /* (2) N(A-B)  V(C) R(NULL) */
    } else {
        version  = NULL;
        /* (1) N(AB-C) V(NULL) R(NULL) */
        /* (2) N(A-B)  V(NULL) R(C) */
        if (revision) {
            *--revision = '-';
            revision    = NULL;
            /* (2) N(A-B-C) V(NULL) R(NULL) */
        }
    }

finish_parsename:

    /* N(A)     V(B)    R(C)    */
    /* N(AxB)   V(C)    R(NULL) */
    /* N(AxBxC) V(NULL) R(NULL) */

    if (!_verify_fullname_string(v->name)) {
        /* not valid restore everything */
        if (version)
            *--version = '-';

        if (revision)
            *--revision = '-';

        v->name = v->extraname;
        return 0;
    }

    if (version) {
        v->version = version;
        _parse_pkgfullversion(v);
    }

    if (revision)
        v->revision = revision;

    _parse_pkgfullname(v);
    return 1;
}

static int _parse_nobee(struct bee_version *v)
{
    assert(0);
    return 0;
}

/*

beeversion [parse] <pkg> [<pkg> ..]
   --mode auto (default)
   --mode bee-pkg
   --mode bee-version
   --mode bee-file
   --mode bee-partial
   --mode no-bee

mode==auto:
  suffix starts with 'bee.tar.' => mode = bee-pkg -> [prefix/]PKGALLPKG.suffix
  suffix eq 'bee' => mode = bee-file -> assume [prefix/]PKGFULLPKG[.arch].suffix
  arch is valid => mode = bee-version  -> assume PKGFULLPKG.arch[.suffix]

  contains no -[0-9] => mode = bee-partial -> assume PKGFULLNAME
  contains  1 -[0-9] => mode = bee-partial -> assume PKGFULLNAME-PKGFULLVERSION
  contains  2 -[0-9] =>
     PKGREVISION contains no '_' => mode = bee-version -> assume PKGFULLNAME-PKGFULLVERSION-PKGREVISION
     PKGREVISION contains    '_' => mode = bee-partial -> assume PKGFULLNAME-PKGFULLVERSION

*/

#define BEE_VERSION_MODE_AUTO       0
#define BEE_VERSION_MODE_BEEPKG     1
#define BEE_VERSION_MODE_BEEVERSION 2
#define BEE_VERSION_MODE_BEEFILE    3
#define BEE_VERSION_MODE_BEEPARTIAL 4
#define BEE_VERSION_MODE_NOBEE      5

int bee_version_parse(struct bee_version *v, char *input, int mode)
{
    int res;

    assert(v);
    assert(input);

    res = _parse_setup(v, input);
    if(!res) {
        errno = ENOMEM;
        return 0;
    }

    _parse_prefix(v);
    _parse_suffix(v);
    _parse_arch(v);

    if (mode == BEE_VERSION_MODE_AUTO) {
        if (*v->suffix && !strncmp(v->suffix+1, "ee.tar.", 7)) {
            mode = BEE_VERSION_MODE_BEEPKG;
        } else if (!strcmp(v->suffix, "bee")) {
            mode = BEE_VERSION_MODE_BEEFILE;
        } else if (*v->arch) {
            mode = BEE_VERSION_MODE_BEEVERSION;
        } else {
            mode = BEE_VERSION_MODE_BEEPARTIAL;
        }
    }

    switch (mode) {
        case BEE_VERSION_MODE_BEEPKG:
            if (!(*v->arch))
                fprintf(stderr, "beeversion: WARNING no arch defined when parsing bee-pkg (%s).\n", input);
            if (strncmp(v->suffix+1, "ee.tar.", 7))
                fprintf(stderr, "beeversion: WARNING wrong suffix defined when parsing bee-pkg (%s).\n", input);
            if(_parse_pkgfullpkg(v))
                return 1;
            break;

        case BEE_VERSION_MODE_BEEFILE:
            if (strcmp(v->suffix, "bee"))
                fprintf(stderr, "beeversion: WARNING wrong suffix defined when parsing bee-file (%s).\n", input);
            if(_parse_pkgfullpkg(v))
                return 1;
            break;

        case BEE_VERSION_MODE_BEEVERSION:
            if (*v->prefix)
                fprintf(stderr, "beeversion: WARNING prefix defined when parsing bee-version (%s).\n", input);
            if (*v->suffix)
                fprintf(stderr, "beeversion: WARNING suffix defined when parsing bee-version (%s).\n", input);
            if(_parse_pkgfullpkg(v))
                return 1;
            break;

        case BEE_VERSION_MODE_BEEPARTIAL:
            if (*v->arch)
                if(_parse_pkgfullpkg(v))
                    return 1;
            if(_parse_partialpkg(v))
                return 1;
            break;

        case BEE_VERSION_MODE_NOBEE:
            if(_parse_nobee(v))
                return 1;
            break;

        default:
            assert(0);
    }

    bee_version_parse_finish(v);

    return 0;
}
