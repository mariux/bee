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

#ifndef _BEE_BEEVERSION_H_
#define _BEE_BEEVERSION_H_

#define BEEVERSION_MAJOR    2
#define BEEVERSION_MINOR    0
#define BEEVERSION_PATCHLVL 0

struct bee_version {
    char *_input;

    char *prefix;

    char *name;
    char *extraname;

    char *version;
    char *extraversion;

    char *revision;

    char *arch;
    char *suffix;
};

#define SUPPORTED_ARCHITECTURES \
            "noarch", "any", \
            "x86_64", "i686", "i386", "i486", "i586", \
            "alpha", "arm", "m68k", "sparc", "mips", "ppc"


#define BEE_DEPRECATED __attribute__((__deprecated__))

void bee_version_parse_start(struct bee_version *v);
void bee_version_parse_finish(struct bee_version *v);
void bee_version_parse_reset(struct bee_version *v);
struct bee_version *bee_version_alloc(void);
void bee_version_free(struct bee_version *v);

int  bee_version_parse(struct bee_version *v, char *input, int mode);

int bee_version_compare(struct bee_version *a, struct bee_version *b);
int bee_version_compare_pkgfullname(struct bee_version *a, struct bee_version *b);

void bee_version_print(char *format, struct bee_version *v);
void bee_version_print_indexed(char *format, struct bee_version *v, int index);


#endif
