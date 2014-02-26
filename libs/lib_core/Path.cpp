#include "Path.h"

#if (PLATFORM_LINUX || PLATFORM_OSX)

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

void Path::ForEachFile(const char *path, Path::ForEachCallback cb, void *userData)
{
	DIR *dir = opendir(path);
	struct dirent *dp;
	
	if(dir == NULL)
		return;

	while(dp = readdir(dir))
    {
    	if(dp->d_name[0] != '.' && dp->d_name[0] != 0)
        {
            char fullpath[1024];
        	char basename[1024];
        	sprintf(fullpath, "%s/%s", path, dp->d_name);
        	strcpy(basename, dp->d_name);
        	char *period_loc = strrchr(basename, '.');
        	if(period_loc)
        		*period_loc = 0;

        	if(!cb(fullpath, basename, userData))
        		break;
        }
    }
}

#else

#include <windows.h>
#include <stdio.h>
#pragma warning(disable:4996)

void Path::ForEachFile(const char *path, Path::ForEachCallback cb, void *userData)
{
	char findpath[1024];
	WIN32_FIND_DATA wfd;

	sprintf(findpath, "%s/*", path);

	HANDLE h = FindFirstFile(findpath, &wfd);
	if(h != INVALID_HANDLE_VALUE)
	{
		do
		{
			if(wfd.cFileName[0] != '.' && wfd.cFileName[0] != 0)
			{
				char fullpath[1024];
				char basename[1024];

				sprintf(fullpath, "%s/%s", path, wfd.cFileName);
				strcpy(basename, wfd.cFileName);
				char *period_loc = strrchr(basename, '.');
				if(period_loc)
					*period_loc = 0;

				if(!cb(fullpath, basename, userData))
					return;
			}
		} while(FindNextFile(h, &wfd));
	}
}

#endif

U8 * Path::LoadRaw(const char *path, int *pOutSize)
{
	FILE *in = fopen(path, "rb");
	if(!in)
		return NULL;

	fseek(in, 0, SEEK_END);
	int size = ftell(in);
	fseek(in, 0, SEEK_SET);

	if(pOutSize)
	{
		*pOutSize = size;
	}

	U8 *pRawData = (U8 *)calloc(1, size+1);
	int amtRead = fread(pRawData, 1, size, in);
	if(amtRead != size)
	{
		printf("ERROR: Could not read all of %s (read %d/%d bytes).\n", path, amtRead, size);
	}

	pRawData[size] = 0; // Returns actual size of data, but ensures there is a NULL terminator in case it is used as a string
	return pRawData;
}
