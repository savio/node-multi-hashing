/*
   BLAKE2 reference source code package - optimized C implementations
   Written in 2012 by Samuel Neves <sneves@dei.uc.pt>
   To the extent possible under law, the author(s) have dedicated all copyright
   and related and neighboring rights to this software to the public domain
   worldwide. This software is distributed without any warranty.
   You should have received a copy of the CC0 Public Domain Dedication along with
   this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/
#pragma once
#ifndef __BLAKE2B_H__
#define __BLAKE2B_H__

#include <stddef.h>
#include <stdint.h>

#if defined(_WIN32) || defined(__CYGWIN__)
    #define BLAKE2_DLL_IMPORT __declspec(dllimport)
    #define BLAKE2_DLL_EXPORT __declspec(dllexport)
    #define BLAKE2_DLL_PRIVATE
#elif __GNUC__ >= 4
  #define BLAKE2_DLL_IMPORT   __attribute__ ((visibility ("default")))
  #define BLAKE2_DLL_EXPORT   __attribute__ ((visibility ("default")))
  #define BLAKE2_DLL_PRIVATE  __attribute__ ((visibility ("hidden")))
#else
  #define BLAKE2_DLL_IMPORT
  #define BLAKE2_DLL_EXPORT
  #define BLAKE2_DLL_PRIVATE
#endif

#if defined(BLAKE2_DLL)
  #if defined(BLAKE2_DLL_EXPORTS) // defined if we are building the DLL
    #define BLAKE2_API BLAKE2_DLL_EXPORT
  #else
    #define BLAKE2_API BLAKE2_DLL_IMPORT
  #endif
  #define BLAKE2_PRIVATE BLAKE2_DLL_PRIVATE // must only be used by hidden logic
#else
  #define BLAKE2_API
  #define BLAKE2_PRIVATE
#endif

#if defined(__cplusplus)
extern "C" {
#elif defined(_MSC_VER) && !defined(inline)
#define inline __inline
#endif


#define BLAKE2_IMPL_CAT(x,y) x ## y
#define BLAKE2_IMPL_EVAL(x,y)  BLAKE2_IMPL_CAT(x,y)
#define BLAKE2_IMPL_NAME(fun)  BLAKE2_IMPL_EVAL(fun, SUFFIX)

#if defined(__SSE2__)
#define HAVE_SSE2
#endif

#if defined(HAVE_SSE2)
#include <emmintrin.h>
// MSVC only defines  _mm_set_epi64x for x86_64...
#if defined(_MSC_VER) && !defined(_M_X64)
static inline __m128i _mm_set_epi64x( const uint64_t u1, const uint64_t u0 )
{
  return _mm_set_epi32( u1 >> 32, u1, u0 >> 32, u0 );
}
#endif
#endif

enum blake2b_constant
  {
    BLAKE2B_BLOCKBYTES = 128,
    BLAKE2B_OUTBYTES   = 64,
    BLAKE2B_KEYBYTES   = 64,
    BLAKE2B_SALTBYTES  = 16,
    BLAKE2B_PERSONALBYTES = 16
  };

typedef struct __blake2b_param
  {
    uint8_t  digest_length; // 1
    uint8_t  key_length;    // 2
    uint8_t  fanout;        // 3
    uint8_t  depth;         // 4
    uint32_t leaf_length;   // 8
    uint64_t node_offset;   // 16
    uint8_t  node_depth;    // 17
    uint8_t  inner_length;  // 18
    uint8_t  reserved[14];  // 32
    uint8_t  salt[BLAKE2B_SALTBYTES]; // 48
    uint8_t  personal[BLAKE2B_PERSONALBYTES];  // 64
  } blake2b_param;

typedef struct __blake2b_state
  {
    uint64_t h[8];
    uint64_t t[2];
    uint64_t f[2];
    uint8_t  buf[2 * BLAKE2B_BLOCKBYTES];
    uint32_t buflen;
    uint8_t  outlen;
    uint8_t  last_node;
  } blake2b_state;

/* prevents compiler optimizing out memset() */
static inline void secure_zero_memory(void *v, size_t n)
{
	volatile uint8_t *p = ( volatile uint8_t * )v;

	while( n-- ) *p++ = 0;
}


// Streaming API
BLAKE2_API int blake2b_init( blake2b_state *S, size_t outlen );
BLAKE2_API int blake2b_init_key( blake2b_state *S, size_t outlen, const void *key, size_t keylen );
BLAKE2_API int blake2b_init_param( blake2b_state *S, const blake2b_param *P );
BLAKE2_API int blake2b_update( blake2b_state *S, const uint8_t *in, size_t inlen );
BLAKE2_API int blake2b_final( blake2b_state *S, uint8_t *out, size_t outlen );

// Simple API
BLAKE2_API int blake2b( uint8_t *out, const void *in, const void *key, size_t outlen, size_t inlen, size_t keylen );

#if defined(__cplusplus)
}
#endif

#endif
