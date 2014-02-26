#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include <string.h>

#if defined(PLATFORM_LINUX) || defined(PLATFORM_OSX)
#include <strings.h>
#define stricmp strcasecmp
#endif

#if defined(PLATFORM_WIN32)
// Disable the stupid "strcpy might be unsafe" crap
#pragma warning(disable:4996)
#endif

#endif

