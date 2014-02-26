#include <stdio.h>
#include <lib_core/StdTypes.h>

extern "C" {
#include "cJSON.h"
}

#include "Mirror.h"
#include "MirrorFactory.h"
#include "lib_core/StringUtil.h"

#include <string>
#include <map>

using namespace Reflection;

static ObjectMirror *spRegisteredMirrors = NULL;
static EnumRegistrant *spEnumRegistrants = NULL;

Mirror::Mirror(const char *pName, U32 offset)
{
    if(pName)
        strcpy(mName, pName);
    else
        mName[0] = 0;
    mOffset = offset;
}

// ----------------------------------------------------------------------------

#define JSON_ENUM_FRIENDLY(SRC) ((SRC->type == cJSON_String) || (SRC->type == cJSON_Number))

static int getMirrorIntegerValue(cJSON *src)
{
    if(src->type == cJSON_String)
        return Reflection::EnumRegistry::Find(src->valuestring);

    return src->valueint;
}

// ----------------------------------------------------------------------------

IntegerMirror::IntegerMirror(const char *pName, U32 offset)
: Mirror(pName, offset)
{
}

bool IntegerMirror::Populate(void *dst, cJSON *src)
{
    if(!JSON_ENUM_FRIENDLY(src))
        return false;

    int *pDst = (int*)(((UINTPTR)dst) + ((UINTPTR)mOffset));
    *pDst = getMirrorIntegerValue(src);

    return true;
}

// ----------------------------------------------------------------------------

IntegerVectorMirror::IntegerVectorMirror(const char *pName, U32 offset)
: Mirror(pName, offset)
{
}

bool IntegerVectorMirror::Populate(void *dst, cJSON *src)
{
    if(src->type != cJSON_Array)
        return false;

    void *arrayPtr = (void*)(((UINTPTR)dst) + ((UINTPTR)mOffset));
    std::vector<int> *pArray = (std::vector<int>*) arrayPtr;

    cJSON *child = src->child;
    while(child != NULL)
    {
        if(child->type == cJSON_Number)
        {
            pArray->push_back(child->valueint);
        }

        child = child->next;
    }

    return true;
}

// ----------------------------------------------------------------------------

ByteMirror::ByteMirror(const char *pName, U32 offset)
: Mirror(pName, offset)
{
}

bool ByteMirror::Populate(void *dst, cJSON *src)
{
    if(!JSON_ENUM_FRIENDLY(src))
        return false;

    U8 *pDst = (U8*)(((UINTPTR)dst) + ((UINTPTR)mOffset));
    *pDst = (U8)getMirrorIntegerValue(src);

    return true;
}

// ----------------------------------------------------------------------------

ShortMirror::ShortMirror(const char *pName, U32 offset)
: Mirror(pName, offset)
{
}

bool ShortMirror::Populate(void *dst, cJSON *src)
{
    if(!JSON_ENUM_FRIENDLY(src))
        return false;

    U16 *pDst = (U16*)(((UINTPTR)dst) + ((UINTPTR)mOffset));
    *pDst = (U16)getMirrorIntegerValue(src);

    return true;
}

// ----------------------------------------------------------------------------

FloatMirror::FloatMirror(const char *pName, U32 offset)
: Mirror(pName, offset)
{
}

bool FloatMirror::Populate(void *dst, cJSON *src)
{
    if(src->type != cJSON_Number)
        return false;

    F32 *pDst = (F32*)(((UINTPTR)dst) + ((UINTPTR)mOffset));
    *pDst = (F32)src->valuedouble;

    return true;
}

// ----------------------------------------------------------------------------

StringMirror::StringMirror(const char *pName, U32 offset)
: Mirror(pName, offset)
{
}

bool StringMirror::Populate(void *dst, cJSON *src)
{
    if(src->type != cJSON_String)
        return false;

    char **pDst = (char**)(((UINTPTR)dst) + ((UINTPTR)mOffset));
    *pDst = strdup(src->valuestring);

    return true;
}

// ----------------------------------------------------------------------------

StringVectorMirror::StringVectorMirror(const char *pName, U32 offset)
: Mirror(pName, offset)
{
}

bool StringVectorMirror::Populate(void *dst, cJSON *src)
{
    if(src->type != cJSON_Array)
        return false;

    void *arrayPtr = (void*)(((UINTPTR)dst) + ((UINTPTR)mOffset));
    std::vector<std::string> *pArray = (std::vector<std::string>*) arrayPtr;

    cJSON *child = src->child;
    while(child != NULL)
    {
        if(child->type == cJSON_String)
        {
            pArray->push_back(child->valuestring);
        }

        child = child->next;
    }

    return true;
}

// ----------------------------------------------------------------------------

RefMirror::RefMirror(const char *pName, U32 offset, ObjectMirror *pTypeMirror)
: Mirror(pName, offset)
, mpTypeMirror(pTypeMirror)
{
}

bool RefMirror::Populate(void *dst, cJSON *src)
{
    if(src->type == cJSON_String)
    {
        char temp[1024];
        strncpy(temp, src->valuestring, 1024);
        temp[1023] = 0;
        char *type = temp;
        char *name = strchr(type, ':');
        if(name)
        {
            *name = 0;
            name++;
            while(*name == ':') name++;
        }

        void **pDst = (void**)(((UINTPTR)dst) + ((UINTPTR)mOffset));
        *pDst = Reflection::MirrorFactory::FindObject(type, name);
        return true;
    }
    else if(src->type == cJSON_Object)
    {
        void *obj = Reflection::MirrorFactory::CreateObject(src, mpTypeMirror);
        mpTypeMirror->Populate(obj, src);

        void **pDst = (void**)(((UINTPTR)dst) + ((UINTPTR)mOffset));
        *pDst = obj;
        return true;
    }

    return false;
}

// ----------------------------------------------------------------------------

BoolMirror::BoolMirror(const char *pName, U32 offset)
: Mirror(pName, offset)
{
}

bool BoolMirror::Populate(void *dst, cJSON *src)
{
    if(src->type != cJSON_True && src->type != cJSON_False)
        return false;

    bool *pDst = (bool*)(((UINTPTR)dst) + ((UINTPTR)mOffset));
    *pDst = (src->type == cJSON_True);

    return true;
}

// ----------------------------------------------------------------------------

ArrayMirror::ArrayMirror(const char *pName, U32 offset, ObjectMirror *pTypeMirror)
: Mirror(pName, offset)
, mpTypeMirror(pTypeMirror)
{
}

bool ArrayMirror::Populate(void *dst, cJSON *src)
{
    if(src->type != cJSON_Array)
        return false;

    void *arrayPtr = (void*)(((UINTPTR)dst) + ((UINTPTR)mOffset));

    cJSON *child = src->child;
    while(child != NULL)
    {
        if(child->type == cJSON_String)
        {
            void *tempPtr = NULL;
            Reflection::MirrorFactory::ConnectRef(child->valuestring, &tempPtr);
            mpTypeMirror->ArrayPush(arrayPtr, tempPtr);
        }
        else if(child->type == cJSON_Object)
        {
            void *pChildObj = Reflection::MirrorFactory::CreateObject(child, mpTypeMirror);
            mpTypeMirror->ArrayPush(arrayPtr, pChildObj);
        }

        child = child->next;
    }

    return true;
}

// ----------------------------------------------------------------------------

ObjectMirror::ObjectMirror(const char *pName, U32 offset, bool selfRegister)
: Mirror(pName, offset)
, mbAware(false)
{
    mpNext = NULL;
    if(selfRegister)
        Register();
}

bool ObjectMirror::Populate(void *dst, cJSON *src)
{
    if(src->type != cJSON_Object)
        return false;

    void *pDst = (void*)(((UINTPTR)dst) + ((UINTPTR)mOffset));

    int count = GetParentCount();
    Mirror **mpParents = GetParents();
    for(int i=0; i<count; i++)
    {
        mpParents[i]->Populate(dst, src);
    }

    cJSON *child = src->child;
    Mirror **mpMember = GetChildren();
    count = GetMemberCount();
    while(child != NULL)
    {
        if(child->string && stricmp(child->string, "_type_") && stricmp(child->string, "_name_"))
        {
            for(int i=0; i<count; i++)
            {
                if(!stricmp(mpMember[i]->mName, child->string))
                {
                    mpMember[i]->Populate(pDst, child);
                    break;
                }
            }
        }
        child = child->next;
    }

    if(mbAware)
    {
        Aware *pAwareObj = (Aware *)dst;
        pAwareObj->OnPopulate();
    }
    return true;
}

void ObjectMirror::Register()
{
    printf("Registering %s\n", mName);
    mpNext = spRegisteredMirrors;
    spRegisteredMirrors = this;
}

bool ObjectMirror::Init()
{
    ObjectMirror *curr = spRegisteredMirrors;
    while(curr != NULL)
    {
        printf("Initializing %s\n", curr->mName);
        curr->Construct();
        curr = curr->mpNext;
    }
    return true;
}

ObjectMirror * ObjectMirror::Find(const char *name)
{
    // TODO: Replace with hash table lookup

    ObjectMirror *curr = spRegisteredMirrors;
    while(curr != NULL)
    {
        if(!strcmp(curr->mName, name))
        {
            return curr;
        }
        curr = curr->mpNext;
    }
    return NULL;
}

// ----------------------------------------------------------------------------

std::map<std::string, int> *spEnumTable = NULL;

EnumRegistrant::EnumRegistrant()
{
    mpNext = spEnumRegistrants;
    spEnumRegistrants = this;
}

EnumRegistrant::~EnumRegistrant()
{
}

void Reflection::EnumRegistry::Init()
{
    spEnumTable = new std::map<std::string, int>;
    EnumRegistrant *pReg = spEnumRegistrants;
    while(pReg != NULL)
    {
        pReg->RegisterAll();
        pReg = pReg->mpNext;
    }
}

void Reflection::EnumRegistry::Register(const char *pName, int value)
{
    printf("** Registering [%s] : %d\n", pName, value);
    spEnumTable->insert(std::pair<std::string,int>(pName, value));
}

int Reflection::EnumRegistry::Find(const char *pName)
{
    void *temp = NULL;

    std::map<std::string, int>::iterator it = spEnumTable->find(pName);

    if(it != spEnumTable->end())
    {
        return (*it).second;
    }

    printf("ERROR: EnumRegistry failed to find '%s'\n", pName);
    return 0;
}
