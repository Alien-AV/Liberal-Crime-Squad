/*
 * Copyright (c) 2004 by Kevin Sadler
 *
 * This file is part of Liberal Crime Squad.
 *
 * Liberal Crime Squad is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

/**
* compat.h
*
* Compatibility Functions to support ports to non-Windows Platforms
*
* Liberal Crime Squad
*
* Abstract
*
* Portability Functions
*
* These functions are intended to replace explicit calls to Windows API.
*
* We can do the following:
*
* (a) Write alternative calls for the ports, keep Windows calls.
* (b) Write portable alternatives for use by Windows and ports.
* (c) Do (a) and (b) and decide what Windows does (API or portable)
*     based on the value of a MACRO GO_PORTABLE.
*
* compat.cpp is the place for non-trivial or more global functions,
*
* History
*
* LiteralKa's changes:
* 1) Added Window's mostly nonsensical custom type definitions.
*
* Changes for portability...
* 1) For Visual C++ 6.0 don't use namespace std.
* 2) MINGW32 had a name clash between the UNIX-like time() function and
* the time variable used for WIN32. Changed time to ptime.
*
* Revision 1.3  2004/06/30 22:46:33  sadler
* Moved itoa() from game into compat.cpp
*
* itoa() removed 2014/07/01 yetisyny
* - it's unnecessary, use tostring() or toCstring() instead
* - for exact same functionality as itoa(value,str,10) use strcpy(str,value)
* - another alternative is sprintf() variants or addstr_f() or mvaddstr_f() variants with "%d"
* - many functions like addstr(), mvaddstr(), strcpy(), strcat(), etc. have been overloaded to accept integers directly
*/

#ifndef HAS_STRICMP
// Portable equivalent of Windows stricmp() function.
// This is strcmp() on lowercase versions of the string.
int stricmp(const char *str1, const char *str2);
#endif

void pause_ms(int t);
void alarmset(int t);
void alarmwait();

//#ifndef HAS_SNPRINTF // this was never defined, and the next line does the check for it anyway
#if defined(__BORLANDC__) || defined(_MSC_VER)
#define snprintf _snprintf
#endif
//#endif
