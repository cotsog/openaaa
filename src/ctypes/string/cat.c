/*
 *	UCW Library -- Character Classes
 *
 *	(c) 1998--2004 Martin Mares <mj@ucw.cz>
 *
 *	This software may be freely distributed and used according to the terms
 *	of the GNU Lesser General Public License.
 */

#include <core/lib.h>
#include <core/string/chartype.h>

const byte _c_cat[256] = {
#define CHAR(code,upper,lower,cat) cat,
#include <core/string/map.h>
#undef CHAR
};
