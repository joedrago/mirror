#include "lib_core/Path.h"
#include "lib_core/StringUtil.h"
#include "Mirror.h"
#include "MirrorFactory.h"

#include <vector>

#pragma warning(disable:4996)

#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>

using namespace Reflection;

class RegisteredObject
{
public:
    RegisteredObject(ObjectMirror *m, const char *pType, const char *pName, void *ptr)
    {
        mpMirror = m;
        mpType   = strdup(pType);
        mpName   = (pName) ? strdup(pName) : NULL;
        mpObj    = ptr;
    }

    ~RegisteredObject()
    {
        free(mpType);
        if(mpName)
            free(mpName);
        mpMirror->Destroy(mpObj);
    }

    ObjectMirror *mpMirror;
    char *mpType;
    char *mpName;
    void *mpObj;
};

static std::vector<RegisteredObject*> *spObjects = NULL;

void Reflection::MirrorFactory::Init()
{
    cJSON_Hooks hooks = { malloc, free };
    cJSON_InitHooks(&hooks);

    spObjects = new std::vector<RegisteredObject*>;
}

void* Reflection::MirrorFactory::CreateObject(cJSON *pRoot, ObjectMirror *pMirror)
{
    const char *type = NULL;
    const char *name = NULL;
    void *ret = NULL;

    cJSON *child = pRoot->child;
    while(child != NULL)
    {
        if(child->string)
        {
            if(!stricmp(child->string, "_type_"))
            {
                type = child->valuestring;
            }
            else if(!stricmp(child->string, "_name_"))
            {
                name = child->valuestring;
            }
        }
        child = child->next;
    }

    if(type || pMirror)
    {
        ObjectMirror *mirror = NULL;
        if(type)    mirror = ObjectMirror::Find(type); // Allow the embedded object's type to override
        if(!mirror) mirror = pMirror;                  // Use the built-in as a fallback, if present
        if(mirror)
        {
            void *obj = mirror->Create();
            mirror->Populate(obj, pRoot);
            if(name)
                MirrorFactory::RegisterObject(mirror, type, name, obj);
            ret = obj;

            // I'd warn here about entries where both pMirror and name are NULL, as they 
            // are completely "unreferenced" data, but with classes able to self-register,
            // it'd just be a nuisance.
        }
        else
        {
            printf("ERROR: Failing to create object [%s], unknown type [%s].\n",
                (name) ? name : "<unknown name>",
                (type) ? type : "<unknown type>");
        }
    }
    else
    {
        printf("ERROR: Objects require a '_type_'\n");
    }
    return ret;
}

static bool LoadObjects(const char *fullpath, const char *basename, void *ignored)
{
    printf("Loading Objects [%s]...\n", basename);
    FILE *f = fopen(fullpath, "rb");
    if(f)
    {
        int size;
        char *buffer;

        fseek(f, 0, SEEK_END);
        size = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = (char*)malloc(size+1);
        fread(buffer, 1, size, f);
        buffer[size] = 0;
        fclose(f);

        cJSON *src = cJSON_Parse(buffer);
        if(src)
        {
            if(src->type == cJSON_Object)
            {
                Reflection::MirrorFactory::CreateObject(src, NULL);
            }
            else if(src->type == cJSON_Array)
            {
                cJSON *child = src->child;
                while(child != NULL)
                {
                    Reflection::MirrorFactory::CreateObject(child, NULL);
                    child = child->next;
                }
            }
            else
            {
                printf("ERROR: [%s] Doesn't hold either a valid JSON array or single object.\n", basename);
            }

            cJSON_Delete(src);
        }
        else
        {
            cJSONErrorInfo *pErrorInfo = cJSON_GetError();
            printf("ERROR: Failed to parse JSON in %s: '%s' [Line %d], near '%c'\n",
                fullpath,
                (pErrorInfo->desc) ? pErrorInfo->desc : "<No Reason>",
                pErrorInfo->lineno,
                (pErrorInfo->loc)  ? *pErrorInfo->loc : ' ');
        }

        free(buffer);
    }

    return true;
}

void Reflection::MirrorFactory::LoadPath(const char *pPath)
{
    Path::ForEachFile(pPath, LoadObjects, NULL);
}

void Reflection::MirrorFactory::RegisterObject(ObjectMirror *pMirror, const char *pType, const char *pName, void *ptr)
{
    RegisteredObject *pRegObj = new RegisteredObject(pMirror, pType, pName, ptr);
    spObjects->push_back(pRegObj);
}

void * Reflection::MirrorFactory::FindObject(const char *pType, const char *pName)
{
    // TODO: Use lookup table
    for(unsigned int i=0; i<spObjects->size(); i++)
    {
        RegisteredObject *pRegObj = spObjects->at(i);
        if(pRegObj->mpName && (!stricmp(pRegObj->mpType, pType) && !stricmp(pRegObj->mpName, pName)))
        {
            return pRegObj->mpObj;
        }
    }
    return NULL;
}

void Reflection::MirrorFactory::ConnectRef(const char *pRefString, void **ptr)
{
    char temp[1024];
    strncpy(temp, pRefString, 1024);
    temp[1023] = 0;
    char *type = temp;
    char *name = strchr(type, ':');
    if(name)
    {
        *name = 0;
        name++;
    }

    *ptr = Reflection::MirrorFactory::FindObject(type, name);
}
