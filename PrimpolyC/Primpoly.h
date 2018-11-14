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
#   define bigintOutputFormat "%llu"

//constexpr auto SBIGINT_MAX  = std::numeric_limits<__int128>::max;
//constexpr auto SBIGINT_MIN  = std::numeric_limits<__int128>::min;
//constexpr auto BIGINT_MAX = std::numeric_limits<unsigned __int128>::max;
//constexpr auto BIGINT_MIN = 0;

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
#   define bigintOutputFormat "%llu"
//constexpr auto SBIGINT_MAX  = std::numeric_limits<std::int64_t>::max;
//constexpr auto SBIGINT_MIN  = std::numeric_limits<std::int64_t>::min;
//constexpr auto BIGINT_MAX = std::numeric_limits<std::uint64_t>::max;
//constexpr auto BIGINT_MIN = 0;
#endif

#define YES 1                      /*  Imitate boolean values. */
#define NO  0

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


/*==============================================================================
|                            CONTROL PARAMETERS
==============================================================================*/

#define NUMOPTIONS 3         /*  Number of command line arguments including
                                 the program name "pp".                       */

#define NUMTERMSPERLINE 16    /*  How many terms of a polynomial to
                                 write before starting a new line.            */

/*==============================================================================
|                            F U N C T I O N S
==============================================================================*/
enum PRINT_MODE
{
    PM_poly,        // old style
    PM_hex,         // n x byte in hex
    PM_mask,        // 0xHHH...Hull (0xHHH...Hull 0xHHH...Hull for > 64 bit)
    PM_maskinv,     // 0xHHH...Hull (0xHHH...Hull 0xHHH...Hull for > 64 bit)
};

/* ppIO.c */
//int parse_command_line( int    argc,
//					    char * argv[],
//                        int *  testPolynomialForPrimitivity,
//                        int *  listAllPrimitivePolynomials,
//                        int *  printStatistics,
//                        int *  printHelp,
//                        int *  selfCheck,
//                        int *  p,
//                        int *  n,
//                        int *  testPolynomial ) ;
//void write_poly       ( int *  a, int n ) ;


///* ppArith.c */
//int    mod              ( int   n, int p ) ;
//bigint power            ( int   x, int y ) ;
//int    power_mod        ( int   a, int n, int p ) ;
//int    is_primitive_root( int   a, int p ) ;
//int    inverse_mod_p    ( int n, int p ) ;


///* ppPolyArith.c */
//int  eval_poly            ( int *  f, int x, int n, int p ) ;
//int  linear_factor        ( int *  f, int n, int p ) ;
//int  is_integer           ( int *  t, int n ) ;
//void construct_power_table( int power_table[][ MAXDEGPOLY ], int * f,
//                            int    n, int   p ) ;
//int  auto_convolve        ( int  * t, int   k, int   lower, int upper, int p ) ;
//int  convolve             ( int  * s, int * t, int   k, int   lower, int upper, int p ) ;
//int  coeff_of_square      ( int  * t, int   k, int   n, int p ) ;
//int  coeff_of_product     ( int  * s, int * t, int   k, int   n, int p ) ;
//void square               ( int  * t, int   power_table[][ MAXDEGPOLY ], int n, int p ) ;
//void product              ( int  * s, int * t, int   power_table[][ MAXDEGPOLY ], int n, int p ) ;
//void times_x              ( int  * t, int   power_table[][ MAXDEGPOLY ], int n, int p ) ;
//void x_to_power           ( bigint m, int * g, int power_table[][ MAXDEGPOLY ], int n, int p ) ;


///* ppFactor.c */
//int    factor                ( bigint   n, bigint * primes, int * count ) ;
//int    is_probably_prime     ( int      n, int      x ) ;
//int    is_almost_surely_prime( int      n ) ;
//bigint EulerPhi              ( bigint   n ) ;


///* ppHelperFunc.c */
//void initial_trial_poly   ( int * f, int   n ) ;
//void next_trial_poly      ( int * f, int   n, int p ) ;
//int  const_coeff_test     ( int * f, int n, int p, int a ) ;
//int  const_coeff_is_primitive_root(  int * f, int n, int p ) ;
//int  skip_test            ( int   i, bigint * primes, int p ) ;
//void generate_Q_matrix    ( int **q, int power_table[][ MAXDEGPOLY ], int n, int p ) ;
//int  find_nullity         ( int ** Q, int n, int p ) ;
//int  has_multi_irred_factors ( int power_table[][ MAXDEGPOLY ], int n, int p ) ;



///*  pporder.c */
//int  order_m      ( int power_table[][ MAXDEGPOLY ], int n, int p, bigint r,
//                    bigint * primes, int prime_count ) ;
//int  order_r      ( int power_table[][ MAXDEGPOLY ], int n, int p, bigint r, int * a ) ;
//int  maximal_order( int * f, int n, int p ) ;

#endif  /*  End of wrapper for header. */
