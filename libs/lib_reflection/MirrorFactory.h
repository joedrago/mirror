#ifndef MIRRORFACTORY_H
#define MIRRORFACTORY_H

#include "Mirror.h"

namespace Reflection
{
    namespace MirrorFactory
    {
        void Init();

        void LoadPath(const char *pPath);

        void  RegisterObject(ObjectMirror *pMirror, const char *pType, const char *pName, void *ptr);
        void* FindObject(const char *pType, const char *pName);
        void* CreateObject(cJSON *pRoot, ObjectMirror *pMirror);
        void ConnectRef(const char *pRefString, void **ptr);
    };
};

#define GET_NAMED_OBJECT(CLASS, NAME) (CLASS*)Reflection::MirrorFactory::FindObject(#CLASS, NAME)

#endif
