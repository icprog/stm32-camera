/**
 * @file    bitset.h
 *
 * @brief   This module provides support for setting and clearing bits in a
 *          16bit variable to be used as flags. Flags are numbered 1 to 16 and
 *          when bitsetHighest returns 0 none of the flags are set.
 *
 * @author  Marco Hess <marcoh@applidyne.com.au>
 *
 * @copyright (c) 2013-2015 Applidyne Pty. Ltd. - All rights reserved.
*/

#ifndef BITSET_H
#define BITSET_H

#ifdef __cplusplus
extern "C" {
#endif

/* ----- System Includes ---------------------------------------------------- */

#include <stdint.h>
#include <stdbool.h>

/* ----- Local Includes ----------------------------------------------------- */

#include "global.h"

/* ----- Types -------------------------------------------------------------- */

typedef uint16_t BitSet_t;      // Up to 16 bits

/* ----------------------- Inline Functions --------------------------------- */

//! Set the indicated bit in the bitPattern (inline macro )
static inline void
bitsetSet( BitSet_t *bitPattern, uint8_t bitNumber )
{
    *bitPattern |= (BitSet_t)_BV( bitNumber - 1 );
}

//! Clear the indicated bit in the bitPattern (inline macro )
static inline void
bitsetClear( BitSet_t *bitPattern, uint8_t bitNumber )
{
    *bitPattern &= (BitSet_t)(~_BV( bitNumber - 1 ));
}

//! Return true when indicated bit in the bitPattern is set
static inline bool
bitsetIsSet( const BitSet_t *bitPattern, uint8_t bitNumber )
{
    return ( ( *bitPattern & (BitSet_t)_BV( bitNumber - 1 ) ) != 0 );
}

/* ----- Public Functions --------------------------------------------------- */

//! Return the highest set bit number in the pattern. 0 when no bits are set.
PUBLIC uint8_t
bitsetHighest( const BitSet_t *bitPattern );

/* ----- End ---------------------------------------------------------------- */

#ifdef    __cplusplus
}
#endif
#endif // BITSET_H


