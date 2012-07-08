/*
** beeversion - compare bee package versionnumbers
**
** Copyright (C) 2010-2012
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

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "bee_version.h"

static void _cut_and_print(char *string, char delimiter, char opt_short)
{
    char *p, *s;

    assert(string);

    p = s = string;

    printf("%s", string);

    while((p=strchr(p, delimiter))) {
        putchar(' ');

        while(s < p)
            putchar(*(s++));

        p++;

        s = (opt_short) ? p : string;
    }

    printf(" %s", s);
}

void bee_version_print(char *format, struct bee_version *v)
{
    bee_version_print_indexed(format, v, 0);
}

void bee_version_print_indexed(char *format, struct bee_version *v, int index)
{
    char *p;

    for(p=format; *p; p++) {
        if(*p == '%') {
            switch(*(++p)) {
                case '%':
                    printf("%%");
                    break;
                case 'i':
                    printf("%d", index);
                    break;
                case 'p':
                    printf("%s", v->name);
                    break;
                case 's':
                    if(*(v->suffix))
                        printf(".%s", v->suffix);
                    break;
                case 'x':
                    printf("%s", v->extraname);
                    break;
                case 'v':
                    printf("%s", v->version);
                    break;
                case 'e':
                    printf("%s", v->extraversion);
                    break;
                case 'r':
                    printf("%s", v->revision);
                    break;
                case 'a':
                    printf("%s", v->arch);
                    break;
                case 'P':
                    printf("%s", v->name);
                    if(*(v->extraname))
                        printf("_%s", v->extraname);
                    break;
                case 'V':
                    printf("%s", v->version);
                    if(*(v->extraversion))
                        printf("_%s", v->extraversion);
                    break;
                case 'F':
                case 'A':
                    printf("%s", v->name);
                    if(*(v->extraname))
                        printf("_%s", v->extraname);
                    if(*(v->version))
                        printf("-%s", v->version);
                    if(*(v->extraversion))
                        printf("_%s", v->extraversion);
                    if(*(v->revision))
                        printf("-%s", v->revision);
                    if(*p == 'A' && *(v->arch))
                        printf(".%s", v->arch);
                    break;
            }
            if (*p) {
                switch(*(p+1)) {
                    case 'x':
                        if (*(v->extraname))
                            printf("%c%s", *p, v->extraname);
                        p++;
                        continue;
                    case 'e':
                        if (*(v->extraversion))
                            printf("%c%s", *p, v->extraversion);
                        p++;
                        continue;
                    case 'i':
                        if (index)
                            printf("%c%d", *p, index);
                        p++;
                        continue;
                }
            }
            continue;
        } /* if '%' */

        if(*p == '@') {
            switch(*(++p)) {
                case 'v':
                    _cut_and_print(v->version, '.', 0);
                    break;
                case 'e':
                    _cut_and_print(v->extraversion, '_', 0);
                    break;
                case 'V':
                    _cut_and_print(v->version, '.', 1);
                    break;
                case 'E':
                    _cut_and_print(v->extraversion, '_', 1);
                    break;
            }
            continue;
        } /* if '@' */

        if(*p == '\\') {
            switch(*(++p)) {
                case 'n':
                    printf("\n");
                    break;
                case 't':
                    printf("\t");
                    break;
                case '0':
                    printf("%c", '\0');
                    break;
                default:
                    printf("%c", *p);
                    break;
            }
            continue;
        } /* if '\' */

        printf("%c", *p);

    } /* for *p */
}
