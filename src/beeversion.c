/*
** beeversion - compare bee package versionnumbers
**
** Copyright (C) 2009-2013
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

#include <sysexits.h>

#include "bee_getopt.h"
#include "bee_version.h"

void print_version(void) {
    printf("beeversion v%d.%d.%d - "
           "by Marius Tolzmann <tolzmann@molgen.mpg.de> 2009-2013\n",
           BEEVERSION_MAJOR, BEEVERSION_MINOR, BEEVERSION_PATCHLVL);
}

void print_full_usage(void) {

    printf("usage:\n\n");
    printf("  beeversion [options] <package1> [..] [packageN]\n\n");
}

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
    char *format  = NULL;
    char *modestr = NULL;

    int mode = BEE_VERSION_MODE_AUTO;

    struct bee_getopt_ctl optctl;

    struct bee_option opts[] = {
        BEE_OPTION_NO_ARG("help",         'h'),
        BEE_OPTION_NO_ARG("version",      'V'),
        BEE_OPTION_REQUIRED_ARG("mode",   'm'),
        BEE_OPTION_REQUIRED_ARG("format", 'f'),
        BEE_OPTION_NO_ARG("pkgfullname",  'P'),
        BEE_OPTION_NO_ARG("pkgfullpkg",   'F'),
        BEE_OPTION_NO_ARG("pkgallpkg",    'A'),
        BEE_OPTION_END
    };

    int i;
    int opt;

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

    bee_getopt_init(&optctl, argc, argv, opts);

    optctl.flags = 0;

    while((opt=bee_getopt(&optctl, &i)) != BEE_GETOPT_END) {
        if (opt == BEE_GETOPT_ERROR) {
            exit(EX_USAGE);
        }

        switch(opt) {
           case 'm':
               modestr = optctl.optarg;
               break;
           case 'f':
               format = optctl.optarg;
               break;
           case 'P':
               format = "%P\n";
               break;
           case 'F':
               format = "%F\n";
               break;
           case 'A':
               format = "%A\n";
               break;
        }
    }

    BEE_GETOPT_FINISH(optctl, argc, argv);

    if (modestr) {
        if (strcmp(modestr, "auto") == 0) {
            mode = BEE_VERSION_MODE_AUTO;
        } else if (strcmp(modestr, "bee-pkg") == 0) {
            mode = BEE_VERSION_MODE_BEEPKG;
        } else if (strcmp(modestr, "beepkg") == 0) {
            mode = BEE_VERSION_MODE_BEEPKG;
        } else if (strcmp(modestr, "beeversion") == 0) {
            mode = BEE_VERSION_MODE_BEEVERSION;
        } else if (strcmp(modestr, "bee-file") == 0) {
            mode = BEE_VERSION_MODE_BEEFILE;
        } else if (strcmp(modestr, "beefile") == 0) {
            mode = BEE_VERSION_MODE_BEEFILE;
        } else if (strcmp(modestr, "partial") == 0) {
            mode = BEE_VERSION_MODE_BEEPARTIAL;
        } else if (strcmp(modestr, "other") == 0) {
            mode = BEE_VERSION_MODE_NOBEE;
        } else {
            fprintf(stderr, "ERROR: unknown mode '%s'.\n", modestr);
            return 0;
        }
    }

    return _beeversion_do_parse(argc, argv, format, mode);
}

int main(int argc, char *argv[])
{
    return !beeversion_parse(argc-1, &argv[1]);
}
