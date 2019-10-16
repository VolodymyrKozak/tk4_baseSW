/*
 * time64.h
 *
 *  Created on: 5 ρεπο. 2018 π.
 *      Author: Volodymyr.Kozak
 */

#ifndef DISPLAY_TIME64_H_
#define DISPLAY_TIME64_H_

/*
 * https://github.com/evalEmpire/y2038
This is an implementation of POSIX time.h which solves the year 2038
bug on systems where time_t is only 32 bits.  It is implemented in
bog-standard ANSI C.  The latest version can be found at
https://github.com/evalEmpire/y2038

It makes use of the system's native 32 bit functions to perform time
zone and daylight savings time calculations and thus does *not* need
to ship its own time zone table.

Here is a list of the currently implemented functions in time64.h and
their POSIX time.h equivalents.

  time64.h                      time.h
  --------                      ------
  localtime64_r                 localtime_r
  localtime64                   localtime
  gmtime64_r                    gmtime_r
  gmtime64                      gmtime
  asctime64_r                   asctime_r
  asctime64                     asctime
  ctime64_r                     ctime_r
  ctime64                       ctime
  timelocal64                   mktime
  mktime64                      mktime
  timegm64                      timegm (a GNU extension)

To install, simply copy time64.c and time64.h into your project and
make use of the functions.

To test, run "make test".  You must have Perl, prove (which comes with
a recent version of the Test::Harness Perl module) and bzdiff
installed to run the full test suite.  It will do a number of unit
tests, plus test against a large table of known good values in
different time zones.


Limitations, Issues, etc...
---------------------------

localtime64_r() gets its time zone and daylight savings time
information by mappping the future year back to a similar one between
2010 and 2037, safe for localtime_r().  The calculations are accurate
according to current time zone and daylight savings information, but
may become inaccurate if a change is made that takes place after 2010.
But its better to be off by an hour than 137 years.

Future versions will probe for a 64 bit safe system localtime_r() and
gmtime_r() and use that.  You can manually set the safe range of your
system in time64_config.h.

The maximum date is still limited by your tm struct.  Most 32 bit
systems use a signed integer tm_year which means the practical upper
limit is the year 2147483647 which is somewhere around 2**54.  You can
use a 64 bit clean tm struct by setting USE_TM64 in time64_config.h
*/

#ifndef TIME64_H
#define TIME64_H

#include <time.h>
#include "time64_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Set our custom types */
typedef INT_64_T        Int64;
typedef Int64           Time64_T;
typedef Int64           Year;

#ifndef PRId64
# if (__WORDSIZE == 64) && !defined(__APPLE__)
#  define PRId64 "ld"
# else
#  define PRId64 "lld"
# endif
#endif

/* A copy of the tm struct but with a 64 bit year */
struct TM64 {
        int     tm_sec;
        int     tm_min;
        int     tm_hour;
        int     tm_mday;
        int     tm_mon;
        Year    tm_year;
        int     tm_wday;
        int     tm_yday;
        int     tm_isdst;

#ifdef HAS_TM_TM_GMTOFF
        long    tm_gmtoff;
#endif

#ifdef HAS_TM_TM_ZONE
        const char *tm_zone;
#endif
};


/* Decide which tm struct to use */
#ifdef USE_TM64
#define TM      TM64
#else
#define TM      tm
#endif


/* Declare public functions */
struct TM *gmtime64_r    (const Time64_T *, struct TM *);
struct TM *localtime64_r (const Time64_T *, struct TM *);
struct TM *gmtime64      (const Time64_T *);
struct TM *localtime64   (const Time64_T *);

char *asctime64          (const struct TM *);
char *asctime64_r        (const struct TM *, char *);

char *ctime64            (const Time64_T*);
char *ctime64_r          (const Time64_T*, char*);

Time64_T   timegm64      (const struct TM *);
Time64_T   mktime64      (struct TM *);
Time64_T   timelocal64   (struct TM *);


/* Not everyone has gm/localtime_r(), provide a replacement */
#ifdef HAS_LOCALTIME_R
#    define LOCALTIME_R(clock, result) localtime_r(clock, result)
#else
#    define LOCALTIME_R(clock, result) fake_localtime_r(clock, result)
#endif
#ifdef HAS_GMTIME_R
#    define GMTIME_R(clock, result)    gmtime_r(clock, result)
#else
#    define GMTIME_R(clock, result)    fake_gmtime_r(clock, result)
#endif


/* Use a different asctime format depending on how big the year is */
#ifdef USE_TM64
    #define TM64_ASCTIME_FORMAT "%.3s %.3s%3d %.2d:%.2d:%.2d %"PRId64"\n"
#else
    #define TM64_ASCTIME_FORMAT "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n"
#endif

#ifdef __cplusplus
  };
#endif

#endif


#endif /* DISPLAY_TIME64_H_ */
