/*
 *	UCW Library -- Lowercase Map
 *
 *	(c) 1997--2004 Martin Mares <mj@ucw.cz>
 *
 *	This software may be freely distributed and used according to the terms
 *	of the GNU Lesser General Public License.
 */

#include <core/lib.h>
#include <core/string/chartype.h>

const byte _c_lower[256] = {
#define CHAR(code,upper,lower,cat) lower,
#include <core/string/map.h>
#undef CHAR
};
