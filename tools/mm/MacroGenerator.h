#ifndef MACROGENERATOR_H
#define MACROGENERATOR_H

#include <stdio.h>
#include <vector>

#define DEBUG_MM(...)
//#define DEBUG_MM printf

class MacroMemberDef
{
public:
    MacroMemberDef(const char *pType, const char *pName, int starCount, int flags);
    ~MacroMemberDef();

    char *mpType;
    char *mpName;
    int   mFlags;
    int   mStarCount;
};

class MacroParentDef
{
public:
    MacroParentDef(const char *pName);
    ~MacroParentDef();

    char *mpName;
};

class MacroClassDef
{
public:
    MacroClassDef(const char *pName);
    ~MacroClassDef();

    char *mpName;
    std::vector<MacroMemberDef*> mMembers;
    std::vector<MacroParentDef*> mParents;
};

class MacroEnumDef
{
public:
    MacroEnumDef(const char *pName, const char *pEnum);
    ~MacroEnumDef();

    char *mpName;
    char *mpEnum;
};

class MacroGenerator
{
public:
    MacroGenerator();
    ~MacroGenerator();

    enum
    {
        FLAG_ARRAY = (1<<0)
    };

    void AddClass(const char *pClass);
    void DerivesFrom(const char *pClass, const char *pParent);
    void AddMember(const char *pClass, const char *pType, const char *pName, int starCount, int flags);
    void AddEnum(const char *pName, const char *pFullScopeName);

    MacroClassDef *Find(const char *pName);

    void GenerateH(FILE *out, const char *blockName);
    void GenerateCPP(FILE *out, const char *blockName);

private:
    std::vector<MacroClassDef*> mClassDefs;
    std::vector<MacroEnumDef*>  mEnumDefs;
};

#endif
