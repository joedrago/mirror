#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MacroGenerator.h"

#pragma warning(disable:4996)

// ----------------------------------------------------------------------------
MacroMemberDef::MacroMemberDef(const char *pType, const char *pName, int starCount, int flags)
{
    mpType     = strdup(pType);
    mpName     = strdup(pName);
    mFlags     = flags;
    mStarCount = starCount;
}

MacroMemberDef::~MacroMemberDef()
{
    free(mpType);
    free(mpName);
}

// ----------------------------------------------------------------------------
MacroParentDef::MacroParentDef(const char *pName)
{
    mpName = strdup(pName);
}

MacroParentDef::~MacroParentDef()
{
    free(mpName);
}
// ----------------------------------------------------------------------------

MacroClassDef::MacroClassDef(const char *pName)
{
    mpName = strdup(pName);
}

MacroClassDef::~MacroClassDef()
{
    free(mpName);
}

// ----------------------------------------------------------------------------

MacroEnumDef::MacroEnumDef(const char *pName, const char *pEnum)
{
    mpName = strdup(pName);
    mpEnum = strdup(pEnum);
}

MacroEnumDef::~MacroEnumDef()
{
    free(mpName);
    free(mpEnum);
}

// ----------------------------------------------------------------------------

MacroGenerator::MacroGenerator()
{
}

MacroGenerator::~MacroGenerator()
{
}

void MacroGenerator::AddClass(const char *pName)
{
    if(Find(pName))
        return;

    MacroClassDef *pDef = new MacroClassDef(pName);
    mClassDefs.push_back(pDef);

    DEBUG_MM("NewClass: %s\n", pName);
}

MacroClassDef * MacroGenerator::Find(const char *pName)
{
    for(unsigned int i=0; i<mClassDefs.size(); i++)
    {
        MacroClassDef *pDef = mClassDefs[i];
        if(!strcmp(pDef->mpName, pName))
            return pDef;
    }
    return NULL;
}

void MacroGenerator::DerivesFrom(const char *pClass, const char *pName)
{
    MacroClassDef *pDef = Find(pClass);
    if(pDef)
    {
        MacroParentDef *pParent = new MacroParentDef(pName);
        pDef->mParents.push_back(pParent);

        DEBUG_MM(" * %s derives from %s\n", pDef->mpName, pName);
    }
}

void MacroGenerator::AddMember(const char *pClass, const char *pType, const char *pName, int starCount, int flags)
{
    MacroClassDef *pDef = Find(pClass);
    if(!pDef)
        return;

    if((flags & FLAG_ARRAY) && starCount != 1)
    {
        DEBUG_MM("ERROR: exposed arrays must be pointers to objects.\n");
    }

    MacroMemberDef *pMember = new MacroMemberDef(pType, pName, starCount, flags);
    pDef->mMembers.push_back(pMember);

    const char *pIsArray = (flags & FLAG_ARRAY) ? "Array:" : "";
    DEBUG_MM("Adding variable [type:%s%s] %s to structure %s\n", pIsArray, pType, pName, pClass);
}

void MacroGenerator::AddEnum(const char *pName, const char *pFullScopeName)
{
    const char *colonColon = "";
    if(pFullScopeName[0])
        colonColon = "::";

    char tempEnum[2048];
    sprintf(tempEnum, "%s%s%s", pFullScopeName, colonColon, pName);

    MacroEnumDef *pDef = new MacroEnumDef(pName, tempEnum);
    mEnumDefs.push_back(pDef);

    DEBUG_MM("Adding Enum: %s [%s]\n", pName, tempEnum);
}


static const char *calcMirrorName(const char *pType, int starCount, int flags)
{
    static char mirrorName[2048];

    if(!strcmp(pType, "int")
    || !strcmp(pType, "S32")
    || !strcmp(pType, "U32"))
    {
        strcpy(mirrorName, "new Reflection::IntegerMirror");
    }
    else if(!strcmp(pType, "StringVector") && starCount == 0)
    {
        strcpy(mirrorName, "new Reflection::StringVectorMirror");
    }
    else if(!strcmp(pType, "IntegerVector") && starCount == 0)
    {
        strcpy(mirrorName, "new Reflection::IntegerVectorMirror");
    }
    else if(!strcmp(pType, "char") && starCount == 1)
    {
        strcpy(mirrorName, "new Reflection::StringMirror");
    }
    else if(!strcmp(pType, "U8") || !strcmp(pType, "S8"))
    {
        strcpy(mirrorName, "new Reflection::ByteMirror");
    }
    else if(!strcmp(pType, "U16") || !strcmp(pType, "S16"))
    {
        strcpy(mirrorName, "new Reflection::ShortMirror");
    }
    else if(!strcmp(pType, "F32")
         || !strcmp(pType, "float"))
    {
        strcpy(mirrorName, "new Reflection::FloatMirror");
    }
    else if(!strcmp(pType, "bool"))
    {
        strcpy(mirrorName, "new Reflection::BoolMirror");
    }
    else
    {
        sprintf(mirrorName, "CreateObjectMirror_%s", pType);
    }
    return mirrorName;
}

void MacroGenerator::GenerateH(FILE *out, const char *blockName)
{
    fprintf(out, "// Autogenerated by mm\n#include \"lib_reflection/Mirror.h\"\n\n");

    for(unsigned int i=0; i<mClassDefs.size(); i++)
    {
        MacroClassDef *pDef = mClassDefs[i];
        fprintf(out, "MIRROR_DECLARE(%s);\n", pDef->mpName);
    }
    fprintf(out, "\n");

    if(mClassDefs.size() || mEnumDefs.size())
    {
        fprintf(out, "MIRROR_REFFUNC_DECLARE(%s);\n", blockName);
    }
}

void MacroGenerator::GenerateCPP(FILE *out, const char *blockName)
{
    GenerateH(out, blockName);

    for(unsigned int i=0; i<mClassDefs.size(); i++)
    {
        MacroClassDef *pDef = mClassDefs[i];
        unsigned int parentCount    = pDef->mParents.size();
        unsigned int memberCount    = pDef->mMembers.size();
        unsigned int adjParentCount = parentCount;

        for(unsigned int j=0; j<parentCount; j++)
        {
            MacroParentDef *pParent = pDef->mParents[j];
            if(!strcmp(pParent->mpName, "Reflection::Aware"))
            {
                adjParentCount--;
            }
        }

        fprintf(out, "MIRROR_IMPLEMENT_BEGIN(%s, %d, %d);\n", pDef->mpName, adjParentCount, memberCount);

        for(unsigned int j=0; j<parentCount; j++)
        {
            MacroParentDef *pParent = pDef->mParents[j];

            if(!strcmp(pParent->mpName, "Reflection::Aware"))
            {
                fprintf(out, "    MIRROR_AWARE();\n");
            }
            else
            {
                const char *mirrorName = calcMirrorName(pParent->mpName, 0, 0);
                fprintf(out, "    MIRROR_IMPLEMENT_PARENT_CLASS(%s);\n", mirrorName);
            }
        }

        for(unsigned int j=0; j<memberCount; j++)
        {
            MacroMemberDef *pMember = pDef->mMembers[j];
            const char *mirrorName = calcMirrorName(pMember->mpType, pMember->mStarCount, pMember->mFlags);

            if((pMember->mStarCount == 1)
            && ((pMember->mFlags & MacroGenerator::FLAG_ARRAY) == 0)
            && (strstr(mirrorName, "Reflection::") == 0))
            {
                fprintf(out, "    MIRROR_IMPLEMENT_ADD(new Reflection::RefMirror(\"%s\", MOFFSET(%s, %s), %s(NULL,0)));\n",
                    pMember->mpName,
                    pDef->mpName, pMember->mpName,
                    mirrorName);
            }
            else if(pMember->mFlags & FLAG_ARRAY)
            {
                fprintf(out, "    MIRROR_IMPLEMENT_ADD(new Reflection::ArrayMirror(\"%s\", MOFFSET(%s, %s), %s(NULL,0)));\n",
                        pMember->mpName,
                        pDef->mpName, pMember->mpName,
                        mirrorName);
            }
            else
            {
                fprintf(out, "    MIRROR_IMPLEMENT_ADD(%s(\"%s\", MOFFSET(%s, %s)));\n",
                        mirrorName,
                        pMember->mpName,
                        pDef->mpName, pMember->mpName);
            }
        }
        fprintf(out, "MIRROR_IMPLEMENT_END(%s);\n\n", pDef->mpName);
    }

    int enumCount = mEnumDefs.size();
    if(enumCount > 0)
    {
        fprintf(out, "MIRROR_ENUM_BEGIN(%s);\n", blockName);
        for(int i=0; i<enumCount; i++)
        {
            MacroEnumDef *pDef = mEnumDefs[i];
            fprintf(out, "    MIRROR_ENUM_REGISTER(\"%s\", %s);\n", pDef->mpName, pDef->mpEnum);
        }
        fprintf(out, "MIRROR_ENUM_END(%s);\n\n", blockName);
    }

    if(mClassDefs.size() || mEnumDefs.size())
    {
        fprintf(out, "MIRROR_REFFUNC_BEGIN(%s);\n", blockName);

        for(int i=0; i<mClassDefs.size(); i++)
        {
            MacroClassDef *pDef = mClassDefs[i];
            int parentCount = pDef->mParents.size();
            int memberCount = pDef->mMembers.size();

            fprintf(out, "    MIRROR_REFFUNC_MIRROR(%s);\n", pDef->mpName);
        }

        if(mEnumDefs.size() > 0)
            fprintf(out, "    MIRROR_REFFUNC_ENUM(%s);\n", blockName);

        fprintf(out, "MIRROR_REFFUNC_END(%s);\n\n", blockName);
    }
}
