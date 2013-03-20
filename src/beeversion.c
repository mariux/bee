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

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <assert.h>

#include "bee_version.h"

/*
#define TEST_BITS 3
#define TYPE_BITS 2

#define USED_BITS (TEST_BITS+TYPE_BITS)

#define TEST_TYPE_MASK           (((1<<TYPE_BITS)-1)<<TEST_BITS)

#define TEST_MASK                ((1<<TEST_BITS)-1)

#define TEST_FULL_MASK           (TEST_TYPE_MASK|TEST_MASK)

#define TEST_WITH_2_ARGS         (1<<TEST_BITS)
#define TEST_WITH_1_OR_MORE_ARGS (2<<TEST_BITS)
#define TEST_WITH_2_OR_MORE_ARGS (3<<TEST_BITS)

#define T_LESS_THAN     0
#define T_LESS_EQUAL    1
#define T_GREATER_THAN  2
#define T_GREATER_EQUAL 3
#define T_EQUAL         4
#define T_NOT_EQUAL     5

#define T_IS_MAX     0
#define T_IS_MIN     1
#define T_IS_NOT_MAX 2
#define T_IS_NOT_MIN 3

#define T_MAX 0
#define T_MIN 1

#define OPT_FORMAT   128
#define OPT_KEYVALUE 129
#define OPT_VERSION  130
#define OPT_HELP     131

#define MODE_TEST   1
#define MODE_PARSE  2
*/

void print_version(void) {
    printf("beeversion v%d.%d.%d - "
           "by Marius Tolzmann <tolzmann@molgen.mpg.de> 2010-2012\n",
           BEEVERSION_MAJOR, BEEVERSION_MINOR, BEEVERSION_PATCHLVL);
}

void print_full_usage(void) {

    printf("usage:\n\n");


    printf("   test: beeversion <packageA> -{lt|le|gt|ge|eq|ne} <packageB>\n");
    printf(" filter: beeversion [filter-options] -{min|max} <package1> [.. <packageN>]\n");
    printf("  parse: beeversion [parse-options] <package>\n\n");
}

/*
int parse_argument(char *s, struct bee_version *v)
{	
    int res;

    res = bee_version_parse(v, s, 0);

    if(!res) {
        fprintf(stderr, "beeversion: can't parse '%s'\n", s);
        return 0;
    }
    return 1;
}
*/
/*
int compare_beepackages_gen(const void *a, const void *b) {
    return((int)bee_version_compare((struct bee_version *)a, (struct bee_version *)b));
}
*/

/*
int do_test(int argc, char *argv[], char test) {
    int i;

    struct bee_version v[2];
    struct bee_version *a, *b, *va;

    int ret;
    char t;

    a = &v[0];
    b = &v[1];

    t = (test & TEST_MASK);

    bee_version_parse_start(a);
    bee_version_parse_start(b);

    if((test & TEST_TYPE_MASK) == TEST_WITH_2_ARGS) {
        if(argc != 2) {
            fprintf(stderr, "usage: beeversion <packageA> -[lt|le|gt|ge|eq|ne] <packageB>\n");
            return(255);
        }

        for(i=0; i<2; i++) {
            if(!parse_argument(argv[i], &v[i]))
               return(0);
        }

        ret = bee_version_compare(a, b);

        bee_version_parse_finish(a);
        bee_version_parse_finish(b);

        switch(t) {
            case T_LESS_THAN:
                return(ret < 0);
            case T_LESS_EQUAL:
                return(ret <= 0);
            case T_GREATER_THAN:
                return(ret > 0);
            case T_GREATER_EQUAL:
                return(ret >= 0);
            case T_EQUAL:
                return(ret == 0);
            case T_NOT_EQUAL:
                return(ret != 0);
        }
        fprintf(stderr, "beeversion: YOU HIT A BUG #004\n");
        assert(0);
        return 0;
    }

    if((test & TEST_TYPE_MASK) == TEST_WITH_2_OR_MORE_ARGS) {

        if(argc < 1) {
            fprintf(stderr, "usage: beeversion -[min|max] <package1> [<package2> .. <packageN>]\n");
            return(255);
        }

        if(!(va = calloc(sizeof(*va), argc))) {
            perror("va=calloc()");
            exit(255);
        }

        for(i=0;i<argc;i++) {
            bee_version_parse_start(va+i);
            if(!parse_argument(argv[i], va+i))
                return(0);
        }

        qsort(va, argc, sizeof(*va), compare_beepackages_gen);

        for(a=va,i=1;i<argc;i++) {
            b=va+i;

            if(bee_version_compare_pkgfullname(a, b)) {
                bee_version_print("%A\n", a);
                a = b;
            }

            if(t == T_MAX)
               a = b;
        }
        bee_version_print("%A\n", a);

        for (i=0;i<argc;i++) {
            bee_version_parse_finish(va+i);
        }

        free(va);
        return(1);
    }

    fprintf(stderr, "beeversion: YOU HIT A BUG #006\n");

    return(0);
}
*/
/*
int do_parse(int argc, char *argv[], char *format) {
    struct bee_version v;

    if(argc != 1) {
        fprintf(stderr, "usage: beeversion <package>\n");
        return(255);
    }

    bee_version_parse_start(&v);

    if(!parse_argument(argv[0], &v))
        return(0);

    bee_version_print(format, &v);

    bee_version_parse_finish(&v);

    return(1);
}
*/

static int _beeversion_do_parse(int argc, char *argv[], char *format, int mode)
{
    struct bee_version v;
    int res;
    int i;

    bee_version_parse_start(&v);

    for (i=0; i < argc; i++) {
        res = bee_version_parse(&v, argv[i], mode);
        if (!res)
            return 0;
        bee_version_print_indexed(format, &v, i);
    }

    bee_version_parse_finish(&v);
    return 1;
}

int beeversion_parse(int argc, char *argv[])
{
    char *format;
    int mode = 0;

    format   = "PKGNAME%_i=( @P )\n"
               "PKGEXTRANAME%_i=( @X )\n"
               "PKGEXTRANAME_UNDERSCORE%_i=%_x\n"
               "PKGEXTRANAME_DASH%_i=%-x\n"
               "PKGVERSIONEPOCH%_i=%h\n"
               "PKGVERSION%_i=( @v )\n"
               "PKGEXTRAVERSION%_i=( @E )\n"
               "PKGEXTRAVERSION_UNDERSCORE%_i=%_e\n"
               "PKGEXTRAVERSION_DASH%_i=%-e\n"
               "PKGREVISION%_i=%r\n"
               "PKGARCH%_i=%a\n"
               "PKGFULLNAME%_i=%P\n"
               "PKGFULLVERSION%_i=%V\n"
               "PKGFULLPKG%_i=%F\n"
               "PKGALLPKG%_i=%A\n"
               "PKGSUFFIX%_i=%s\n";

    return _beeversion_do_parse(argc, argv, format, mode);
}


int main(int argc, char *argv[])
{
    if (argc > 1) {
        if (strcmp(argv[1], "parse") == 0) {
            return !beeversion_parse(argc-2, &argv[2]);
        } else if(0) {
            return !beeversion_parse(argc-1, &argv[1]);
        }
    }
    fprintf(stderr, "UNIMPLEMENTED..\n");
    return 1;
}


/*
    int option_index = 0;
    int c = 0;

    char test_to_do   = 0;
    char *format      = NULL;
    int  test_index   = 0;
    int  build_format = 0;
    char mode         = 0;

    char *keyvalue;
*/
/*
    struct option long_options[] = {
        {"lt",    no_argument, 0, TEST_WITH_2_ARGS|T_LESS_THAN},
        {"le",    no_argument, 0, TEST_WITH_2_ARGS|T_LESS_EQUAL},
        {"gt",    no_argument, 0, TEST_WITH_2_ARGS|T_GREATER_THAN},
        {"ge",    no_argument, 0, TEST_WITH_2_ARGS|T_GREATER_EQUAL},
        {"eq",    no_argument, 0, TEST_WITH_2_ARGS|T_EQUAL},
        {"ne",    no_argument, 0, TEST_WITH_2_ARGS|T_NOT_EQUAL},

        {"max",   no_argument, 0, TEST_WITH_2_OR_MORE_ARGS|T_MAX},
        {"min",   no_argument, 0, TEST_WITH_2_OR_MORE_ARGS|T_MIN},

        {"format",   required_argument, 0, OPT_FORMAT},

        {"pkgfullname",    no_argument,  0, 'P'},
        {"pkgfullversion", no_argument,  0, 'V'},
        {"pkgfullpkg",     no_argument,  0, 'F'},
        {"pkgallpkg",      no_argument,  0, 'A'},

        {"pkgname",         no_argument, 0, 'p'},
        {"pkgarch",         no_argument, 0, 'a'},
        {"pkgversion",      no_argument, 0, 'v'},
        {"pkgextraversion", no_argument, 0, 'e'},
        {"pkgrevision",     no_argument, 0, 'r'},
        {"pkgsuffix",       no_argument, 0, 's'},

        {"pkgextraname",    no_argument, 0, 'x'},
        {"pkgsubname",      no_argument, 0, 'x'},

        {"version",     no_argument, 0, OPT_VERSION},
        {"help",        no_argument, 0, OPT_HELP},

        {0, 0, 0, 0}
    };
*/
/*
    keyvalue = "PKGNAME=%p\n"
               "PKGEXTRANAME=%x\n"
               "PKGEXTRANAME_UNDERSCORE=%_x\n"
               "PKGEXTRANAME_DASH=%-x\n"
               "PKGVERSION=( @v )\n"
               "PKGEXTRAVERSION=%e\n"
               "PKGEXTRAVERSION_UNDERSCORE=%_e\n"
               "PKGEXTRAVERSION_DASH=%-e\n"
               "PKGREVISION=%r\n"
               "PKGARCH=%a\n"
               "PKGFULLNAME=%P\n"
               "PKGFULLVERSION=%V\n"
               "PKGFULLPKG=%F\n"
               "PKGALLPKG=%A\n"
               "PKGSUFFIX=%s\n";
*/

/*
    while ((c = getopt_long_only(argc, argv, "PAVFpaversx", long_options, &option_index)) != -1) {

        if( (c & TEST_TYPE_MASK) && ! (c & ~TEST_FULL_MASK)) {
            if(mode && mode == MODE_PARSE) {
                fprintf(stderr, "beeversion: skipping test-option --%s since already running in parse mode\n",
                          long_options[option_index].name);
                continue;
            }
            if(test_to_do) {
                fprintf(stderr, "beeversion: skipping test-option --%s since --%s is already set\n",
                          long_options[option_index].name, long_options[test_index].name);
                continue;
            }
            mode       = MODE_TEST;
            test_index = option_index;
            test_to_do = c;
            continue;
        }

        if(mode && mode == MODE_TEST) {
            fprintf(stderr, "beeversion: skipping parse-option --%s since already running in test mode\n",
                      long_options[option_index].name);
            continue;
        }

        mode = MODE_PARSE;

        if((c >= 'A' && c <= 'z')) {
            if(format && ! build_format) {
                fprintf(stderr, "beeversion: --%s ignored\n", long_options[option_index].name);
                continue;
            }

            if(!format) {
                format = calloc(sizeof(char), argc * 3 + 2);
                if(!format) {
                    perror("calloc(format)");
                    exit(255);
                }
            }

            if(build_format)
                format[build_format++] = ' ';

            format[build_format++] = '%';
            format[build_format++] = c;
            continue;
        }

        if(c == OPT_FORMAT) {
            if(format) {
                fprintf(stderr, "beeversion: --%s ignored\n", long_options[option_index].name);
                continue;
            }
            format = optarg;
            continue;
        }

        if(c == OPT_KEYVALUE) {
            if(format) {
                fprintf(stderr, "beeversion: --%s ignored\n", long_options[option_index].name);
                continue;
            }
            format = keyvalue;
            continue;
        }

        if(c == OPT_HELP) {
            printf("\n");
            print_version();
            printf("\n");
            print_full_usage();
            exit(0);
        }

        if(c == OPT_VERSION) {
            print_version();
            exit(0);
        }

        if(opterr)
           continue;

        fprintf(stderr, "beeversion: YOU HIT A BUG #003 opterr=%d\n", opterr);
    }

    if(build_format)
        format[build_format++] = '\n';

    if(mode == MODE_TEST)
        return(!do_test(argc-optind, argv+optind, test_to_do));

    if(!mode || mode == MODE_PARSE) {
        if(!format)
            format = keyvalue;

        return(!do_parse(argc-optind, argv+optind, format));
    }

    return(0);
*/
