#pragma once
/*==============================================================================
| 
|  File Name:     
|
|     Primpoly.h
|
|  Description:   
|
|     Global header file for primitive polynomial routines.
|     Constants, data types and algorithm control parameters.
| 
|     For an exhaustive description of the algorithms used in this program,
|     the theory which underlies them, and the software design, visit my
|     web page at http://seanerikoconnor.freeservers.com or
|     http://seanerikoconnor.freeyellow.com and look under the topic
|     Professional Interests->Generating Primitive Polynomials
|
|  LEGAL
|
|     Primpoly Version 13.0 - A Program for Computing Primitive Polynomials.
|     Copyright (C) 1999-2018 by Sean Erik O'Connor.  All Rights Reserved.
|
|     This program is free software: you can redistribute it and/or modify
|     it under the terms of the GNU General Public License as published by
|     the Free Software Foundation, either version 3 of the License, or
|     (at your option) any later version.
|
|     This program is distributed in the hope that it will be useful,
|     but WITHOUT ANY WARRANTY; without even the implied warranty of
|     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
|     GNU General Public License for more details.
|
|     You should have received a copy of the GNU General Public License
|     along with this program.  If not, see <http://www.gnu.org/licenses/>.
|     
|     The author's address is artificer!AT!seanerikoconnor!DOT!freeservers!DOT!com
|     with !DOT! replaced by . and the !AT! replaced by @
|
==============================================================================*/

#ifndef PP_H  /*  Wrap this header file. */
#define PP_H

#include <cstdint>
#include <limits>
#include <iostream>

/*==============================================================================
|                            DATA TYPES
==============================================================================*/

#ifdef BIGINT_128
    typedef unsigned __int128 bigint;
    typedef __int128 sbigint;

    inline std::ostream &operator<<(std::ostream &out, __uint128_t x)
    {
      if(x >= 10)
      {
        out << x / 10;
      }
      return out << static_cast<unsigned>(x % 10);
    }

    inline std::ostream &operator<<(std::ostream &out, __int128_t x)
    {
      if(x < 0)
      {
        out << '-';
        x = -x;
      }
      return out << static_cast<__uint128_t>(x);
    }

#else
    typedef std::uint64_t bigint;
    typedef std::int64_t sbigint;
#endif

#define YES true
#define NO  false

/* In case it's not defined, put something reasonable. */
#ifndef _MAX_PATH
#define _MAX_PATH 100
#endif


/*==============================================================================
|                            CONSTANTS
==============================================================================*/

#define MAXPTON (bigint) \
                pow( (double) 2, \
                     (double) (8 * sizeof( bigint ) - 1) ) - 1

                             /*  Maximum value of p to the nth power.  As high
                                 as possible without causing integer overflow
								 or overflow into the sign bit (if a signed type)
                                 for the bigint data type.  This value will be
                                  b
                                 2   - 1 if bigint = b-bit unsigned integer type.
                                  b-1
                                 2   - 1 if bigint = b-bit signed integer type.*/

#define NUMBITS 8 * \
        sizeof( bigint )     /*  Number of bits in the high precision integer. */

#ifdef BIGINT_128

#   define MAXDEGPOLY 125        /*  Maximum possible degree n of f( x ) when p = 2:
                                 | log ( MAXPTON ) |.
                                 --   2           --                          */

#   define MAXNUMPRIMEFACTORS 63       /*  Maxmimum number of distinct primes =
                                        | log ( MAXPTON ) | + 1.
                                        --               --                   */
#else

#   define MAXDEGPOLY 62        /*  Maximum possible degree n of f( x ) when p = 2:
                                 | log ( MAXPTON ) |.
                                 --   2           --                          */

#   define MAXNUMPRIMEFACTORS 31       /*  Maxmimum number of distinct primes =
                                        | log ( MAXPTON ) | + 1.                                                 --               --                   */
#endif


#define NUM_PRIME_TEST_TRIALS 50 /*  Number of trials to test if a number is
								     probably prime. */

#define NUMOPTIONS 3         /*  Number of command line arguments including
                                 the program name "pp".                       */

#define NUMTERMSPERLINE 16    /*  How many terms of a polynomial to
                                 write before starting a new line.            */

#endif  /*  End of wrapper for header. */
