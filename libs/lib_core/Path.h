#ifndef PATH_H
#define PATH_H

#include "lib_core/StdTypes.h"

namespace Path
{
	typedef bool (*ForEachCallback)(const char *fullpath, const char *basename, void *userData);
	void ForEachFile(const char *path, ForEachCallback cb, void *userData);

	U8 *LoadRaw(const char *path, int *pOutSize);
};

#endif

