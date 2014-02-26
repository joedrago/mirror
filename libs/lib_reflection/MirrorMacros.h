#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include <vector>

// do not include directly; include Mirror.h

#define MIRROR_DECLARE(NAME) Reflection::ObjectMirror * CreateObjectMirror_ ## NAME (const char *pName, U32 offset);

#define MIRROR_IMPLEMENT_BEGIN(NAME, PARENTCOUNT, MEMBERCOUNT) \
class ObjectMirror_ ## NAME : public Reflection::ObjectMirror \
{ \
    public: \
        ObjectMirror_ ## NAME (const char *pName, U32 offset, bool sr = false) \
        : Reflection::ObjectMirror(pName, offset, sr) {}; \
\
        virtual const char * GetName()      const { return #NAME; } \
        virtual U32          GetParentCount() const { return PARENTCOUNT; } \
        virtual U32          GetMemberCount() const { return MEMBERCOUNT; } \
\
        static Mirror **mpParents; \
        virtual Mirror ** GetParents()  { return mpParents; } \
        static Mirror **mpChildren; \
        virtual Mirror ** GetChildren() { return mpChildren; } \
\
        virtual void * Create() \
        { \
            return new NAME; \
        } \
\
        virtual void Destroy(void *ptr) \
        { \
            NAME *p = (NAME*)ptr;\
            delete p; \
        } \
\
        virtual int ArrayPush(void *arrayPtr, void *obj) \
        { \
            std::vector<NAME*> *pArray = (std::vector<NAME*>*) arrayPtr; \
            pArray->push_back((NAME*)obj); \
            return pArray->size(); \
        } \
\
        virtual void Construct() \
        { \
            int pidx = 0; \
            int midx = 0; \
            if(GetParentCount()) mpParents  = new Reflection::Mirror*[GetParentCount()]; \
            if(GetMemberCount()) mpChildren = new Reflection::Mirror*[GetMemberCount()];

#define MOFFSET(PARENT, CHILD) offsetof(PARENT, CHILD)
#define MIRROR_IMPLEMENT_PARENT_CLASS(PARENTCLASSMIRROR) mpParents[pidx++] = PARENTCLASSMIRROR(NULL, 0);
#define MIRROR_IMPLEMENT_ADD(STUFF) mpChildren[midx++] = STUFF ;
#define MIRROR_IMPLEMENT_END(NAME)        } \
} StaticObjectMirror_ ## NAME (#NAME, 0, true); \
Reflection::Mirror **ObjectMirror_##NAME::mpParents; \
Reflection::Mirror **ObjectMirror_##NAME::mpChildren; \
Reflection::ObjectMirror * CreateObjectMirror_ ## NAME (const char *pName, U32 offset) \
{ \
    Reflection::ObjectMirror *pMirror = new ObjectMirror_ ## NAME (pName, offset); \
    pMirror->Construct(); \
    return pMirror; \
}
#define MIRROR_AWARE() mbAware = true;


// GENERATED EXAMPLE:

// MIRROR_IMPLEMENT_BEGIN(Awesome, 2);
//     MIRROR_IMPLEMENT_ADD(Reflection::IntegerMirror("a", 0));
//     MIRROR_IMPLEMENT_ADD(Reflection::IntegerMirror("b", 4));
// MIRROR_IMPLEMENT_END(Awesome);

#define MIRROR_ENUM_BEGIN(SECTION) \
    class RegEnum_ ## SECTION : public Reflection::EnumRegistrant \
    { \
    public: \
        RegEnum_ ## SECTION() : Reflection::EnumRegistrant() {} \
        ~RegEnum_ ## SECTION() {} \
        virtual void RegisterAll() {
#define MIRROR_ENUM_REGISTER(NAME, ENUM) Reflection::EnumRegistry::Register(NAME, ENUM);
#define MIRROR_ENUM_END(SECTION) } }; \
    RegEnum_ ## SECTION SECTION; \
    void enum_ref_func_ ## SECTION() { int x = 5; if(x == 7) RegEnum_ ## SECTION *p = new RegEnum_ ## SECTION; }

#define MIRROR_REFFUNC_DECLARE(SECTION) void ref_func_ ## SECTION();
#define MIRROR_REFFUNC_BEGIN(SECTION)   void ref_func_ ## SECTION() { int x = 5; if(x == 7) {
#define MIRROR_REFFUNC_ENUM(SECTION)        { RegEnum_ ## SECTION *p = new RegEnum_ ## SECTION; }
#define MIRROR_REFFUNC_MIRROR(NAME)         { ObjectMirror_ ## NAME *p = new ObjectMirror_ ## NAME(0, 0); }
#define MIRROR_REFFUNC_END(SECTION)     } }

#define MIRROR_REFFUNC_DECLARE(SECTION) void ref_func_ ## SECTION();

// Use this in your startup code to force an entire H or CPP's worth of mirrors/enums into your EXE
// Syntax : MIRROR_BLOCK_REFERENCE(LIBNAME_FILENAME_CPPORH);
// Example: MIRROR_BLOCK_REFERENCE(lib_input_inputgamepadenums_h);
#define MIRROR_FORCE_REFERENCE(SECTION) ref_func_ ## SECTION();
