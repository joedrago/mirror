#ifndef REFLECTION_H
#define REFLECTION_H

#include "lib_core/StdTypes.h"

#include <vector>
#include <string>

namespace {
    typedef std::vector<std::string > StringVector;
    typedef std::vector<int> IntegerVector;
}

struct cJSON;

namespace Reflection
{
    enum Type
    {
        TYPE_NONE = 0,
        TYPE_INTEGER,
        TYPE_OBJECT,

        TYPE_COUNT
    };

    // Offers a single reflection... get it?

    class Mirror
    {
        public:
            Mirror(const char *pName, U32 offset);
            virtual ~Mirror() {};

            virtual bool Populate(void *dst, cJSON *src) = 0;

            static const U32 MAX_NAME_LENGTH = 31;

            char mName[MAX_NAME_LENGTH+1];
            U32  mOffset;
    };

// ----------------------------------------------------------------------------

    class IntegerMirror : public Mirror
    {
        public:
            IntegerMirror(const char *pName, U32 offset);
            virtual bool Populate(void *dst, cJSON *src);
    };

// ----------------------------------------------------------------------------

    class IntegerVectorMirror : public Mirror
    {
        public:
            IntegerVectorMirror(const char *pName, U32 offset);
            virtual bool Populate(void *dst, cJSON *src);
    };

// ----------------------------------------------------------------------------

    class ByteMirror : public Mirror
    {
        public:
            ByteMirror(const char *pName, U32 offset);
            virtual bool Populate(void *dst, cJSON *src);
    };

// ----------------------------------------------------------------------------

    class ShortMirror : public Mirror
    {
        public:
            ShortMirror(const char *pName, U32 offset);
            virtual bool Populate(void *dst, cJSON *src);
    };

// ----------------------------------------------------------------------------

    class FloatMirror : public Mirror
    {
        public:
            FloatMirror(const char *pName, U32 offset);
            virtual bool Populate(void *dst, cJSON *src);
    };

// ----------------------------------------------------------------------------

    class StringMirror : public Mirror
    {
        public:
            StringMirror(const char *pName, U32 offset);
            virtual bool Populate(void *dst, cJSON *src);
    };

// ----------------------------------------------------------------------------

    class StringVectorMirror : public Mirror
    {
        public:
            StringVectorMirror(const char *pName, U32 offset);
            virtual bool Populate(void *dst, cJSON *src);
    };

// ----------------------------------------------------------------------------

    class BoolMirror : public Mirror
    {
        public:
            BoolMirror(const char *pName, U32 offset);
            virtual bool Populate(void *dst, cJSON *src);
    };

// ----------------------------------------------------------------------------

    class ObjectMirror : public Mirror
    {
        public:
            ObjectMirror(const char *pName, U32 offset, bool selfRegister = false);
            virtual bool Populate(void *dst, cJSON *src);

            virtual void Construct() = 0;

            virtual Mirror ** GetChildren() = 0;
            virtual Mirror ** GetParents() = 0;
            virtual U32 GetParentCount() const = 0;
            virtual U32 GetMemberCount() const = 0;

            virtual void * Create() = 0;
            virtual void Destroy(void *ptr) = 0;
            virtual int ArrayPush(void *arrayPtr, void *obj) = 0;

            void Register();

            static bool Init();
            static ObjectMirror * Find(const char *name);

            bool mbAware;
            ObjectMirror *mpNext;
    };

// ----------------------------------------------------------------------------

    class ArrayMirror : public Mirror
    {
        public:
            ArrayMirror(const char *pName, U32 offset, ObjectMirror *pTypeMirror);
            virtual bool Populate(void *dst, cJSON *src);

            int PushString(void *arrayPtr, const std::string &s);
        private:
            ObjectMirror *mpTypeMirror;
    };

// ----------------------------------------------------------------------------

    class RefMirror : public Mirror
    {
        public:
            RefMirror(const char *pName, U32 offset, ObjectMirror *pTypeMirror);
            virtual bool Populate(void *dst, cJSON *src);
        private:
            ObjectMirror *mpTypeMirror;
    };

// ----------------------------------------------------------------------------

    // Aware -- Special interface class. Things that derive from
    //          Reflection::Aware are automatically notified when interesting
    //          Mirror-related things occur.
    class Aware
    {
    public:
        virtual void OnPopulate() = 0;
    };

// ----------------------------------------------------------------------------
    namespace EnumRegistry
    {
        void Init();
        void Register(const char *pName, int value);

        static const int NOT_FOUND = -1;
        int Find(const char *pName);
    };

    class EnumRegistrant
    {
    public:
        EnumRegistrant();
        virtual ~EnumRegistrant();

        virtual void RegisterAll() = 0;
        EnumRegistrant *mpNext;
    };
}

#include "MirrorMacros.h"

#endif
