/*
 *   Command "fo" : Format Output
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *	@(#)	[MB] fa_fmtout.c	Version 1.7 du 24/04/06 - 
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
#include <string.h>

#define   X                             fprintf(stderr, "%-20s (%4d)\n", __FILE__, __LINE__);
#define   CR_EXIT_ERR_REGCOMP           (  6)

struct fa_re_desc {
     regex_t                   reg[2];
     char                     *regex[2];
     int                       cflags;
};

/******************************************************************************

                         FA_USAGE

******************************************************************************/
void fa_usage(char *progname)
{
     fprintf(stderr, "Usage: %s regexp field_lengths\n", progname);
}

/******************************************************************************

                         MAIN

******************************************************************************/
int main(int argc, char *argv[])
{
#define   CR_SIZE             (65536)
     char                     *_progname, *_regexp, *_lengths, _line[CR_SIZE + 1],
                               _errbuf[256], _matching_str[CR_SIZE + 1], *_ptr,
						*_debug_msg;
     int                       _i, _j, _n, _e, _s, _off, _error, _length,
                               _idx[16], _max_idx;
     int                       _debug;
     struct fa_re_desc        *_re, _reg_struct;
     size_t                    _nmatch;
     regmatch_t                _pmatch[CR_SIZE + 1];
     int                       _eflags = 0;

     _progname                = argv[0];
     _debug                   = 0;
	_debug_msg			= "[DEBUG]";

     if (argc != 3) {
          fa_usage(_progname);
          exit(1);
     }

     _regexp                  = argv[1];
     _lengths                 = argv[2];

     if (_debug) {
          fprintf(stderr, "%s regexp  = '%s'\n", _debug_msg, _regexp);
          fprintf(stderr, "%s lengths = '%s'\n", _debug_msg, _lengths);
		fprintf(stderr, "%s\n", _debug_msg);
     }

     _i                       = 1;
     for (_ptr = strtok(_lengths, ","); _ptr != NULL; _ptr = strtok(NULL,",")) {
#if 0
		if (_debug) {
			fprintf(stderr, "%s%s\n", _debug_msg, _ptr);
		}
#endif	/* 0 */

          _idx[_i]                 = atoi(_ptr);

		if (_debug) {
			fprintf(stderr, "%s    %10d\n", _debug_msg, _idx[_i]);
		}
          _i++;
     }
	_max_idx				= _i;

     if (_debug) {
		fprintf(stderr, "%s\n", _debug_msg);
	}

     _nmatch                  = sizeof(_pmatch) / sizeof(_pmatch[0]);

     _i                       = 0;
     _off                     = 0;
     _re                      = &_reg_struct;
     _re->cflags              = 0;

     _re->regex[0]            = _regexp;
     _re->cflags              |= REG_EXTENDED;

     if ((_error = regcomp(&_re->reg[0], _regexp, _re->cflags)) != 0) {
          (void) regerror(_error, &_re->reg[0], _errbuf, sizeof(_errbuf));
          fprintf(stderr, "%s: regcomp error for \"%s\" : %s\n",
                  _progname, _regexp, _errbuf);
          exit(CR_EXIT_ERR_REGCOMP);
     }

     for (_n = sizeof(_line); fgets(_line, _n, stdin) != 0; _n = sizeof(_line)) {
          _length                  = strlen(_line);
          if (_line[_length - 1] == '\n') {
               _line[_length - 1]            = '\0';
          }
          if (regexec(&_re->reg[_i], _line + _off, _nmatch, _pmatch, _eflags) == 0) {
               if (_debug) {
                    fprintf(stderr, "%s Match    : '%s'\n", _debug_msg, _line);
               }
               for (_j = 1; _j < _max_idx; _j++) {
                    _s   = _pmatch[_j].rm_so;
                    _e   = _pmatch[_j].rm_eo - 1;

                    strncpy(_matching_str, _line + _off + _s, _e - _s + 1);
                    _matching_str[_e -_s + 1]   = 0;

                    if (_debug) {
                         fprintf(stderr, "%s %10d : [%3ld] '%s'\n", _debug_msg, _idx[_j], strlen(_matching_str), _matching_str);
					printf("%s[%*s]", _j == 1 ? "" : " ", _idx[_j], _matching_str);
                    }
                    else {
					// if (_matching_str[0] != '\0') {
						printf("%s%*s", _j == 1 ? "" : " ", _idx[_j], _matching_str);
					// }
                    }
               }
               printf("\n");
          }
          else {
               if (_debug) {
                    fprintf(stderr, "%s NO MATCH : '%s'\n", _debug_msg, _line);
               }
			printf("%s\n", _line);
          }
     }
     return 0;
}
