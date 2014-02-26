#include "lib_core/StdTypes.h"
#include "MacroGenerator.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tokens.h"

#include <vector>

#ifdef PLATFORM_WIN32
#pragma warning(disable:4996)
#endif

#define YYCTYPE char
#define YYCURSOR s->cur
#define YYMARKER s->marker
#define YYLIMIT s->end
#define BOOST_WAVE_RET(r) return r;
#define BOOST_WAVE_UPDATE_CURSOR()

#define LEX_ERROR 1
#define LEX_WARNING 2

typedef void (*ErrorFunc)(class Parser *s, int type, const char *format, int val);

struct Scope
{
    char *mpName;
    int braceDepth;
    bool isClass;
    bool isExposed;

    Scope() {}
    ~Scope()
    {
        free(mpName);
    }
};

struct Identifier
{
    char *mpName;

    int  starCount;
    bool isConst : 1;
    bool isKeyword : 1;
    bool isBracketed : 1;

    Identifier()
        : starCount(0)
        , isConst(0)
        , isKeyword(0)
        , mpName(NULL)
        {};
    ~Identifier()
    {
        if(mpName)
            free(mpName);
    }
};

class Parser
{
    public:
        void PopScope();
        void ProcessNextToken(int token, const char *text);
        void ProcessStatement();
        void FlushDerived();

    // lexer related
    const char *text;
    const char *marker;
    const char *cur;
    const char *token;
    const char *end;
    int line;
    int col;
    int c99;
    int single_line_only;
    bool wait_for_semicolon;
    ErrorFunc errorFunc;
    bool error;

    std::vector<Scope*> mScopes;
    std::vector<Identifier*> mIdentifiers;

    int mStarCount;
    int mBraceDepth;
    int mBracketCount;
    bool mIgnore;

    enum
    {
        FLAG_NAMESPACE       = (1<<0),
        FLAG_CLASS           = (1<<1),
        FLAG_UNOPENED        = (1<<2), // Declaring something, but haven't hit the open curly brace yet
        FLAG_NEXT_IS_CONST   = (1<<3),
        FLAG_PAREN           = (1<<4),
        FLAG_BRACKET         = (1<<5),
        FLAG_NEXT_IS_EXPOSED = (1<<6),
        FLAG_COLON_COLON     = (1<<7),
        FLAG_FRIEND          = (1<<8),
        FLAG_ENUM            = (1<<8),
    };

    inline void SetFlag(U32 flag)
    {
        mFlags |= flag;
    }

    inline void ClearFlag(U32 flag)
    {
        mFlags &= ~flag;
    }

    inline bool IsSet(U32 flag)
    {
        return ((mFlags & flag) != 0);
    }

    inline void ClearFlags()
    {
        mFlags = 0;
    }

    bool IsExposed()
    {
        if(mIgnore)
            return false;

        if(IsSet(FLAG_ENUM) && IsSet(FLAG_NEXT_IS_EXPOSED) && !IsSet(FLAG_UNOPENED))
            return true;

        for(unsigned int i=0; i<mScopes.size(); i++)
        {
            Scope *pScope = mScopes[i];
            if(pScope->isExposed)
                return true;
        }
        return false;
    }

    char mFullName[2048]; // Temporary storage for GetFullScopeName
    char * GetFullScopeName()
    {
        mFullName[0] = 0;
        for(unsigned int i=0; i<mScopes.size(); i++)
        {
            Scope *pPathScope = mScopes[i];
            if(mFullName[0] != 0)
                strcat(mFullName, "::");
            strcat(mFullName, pPathScope->mpName);
        }
        return mFullName;
    }

    U32 mFlags;
    MacroGenerator *mpGenerator;
};

void errorFunc(Parser *s, int type, const char *format, int val)
{
    printf(format, val);
    printf("\n");
    s->error = true;
}

int getNextToken(Parser *s)
{
#include "cpp.re.cpp"
    return T_EOF;
}

void Parser::PopScope()
{
    int size = mScopes.size();
    if(size > 0)
    {
        Scope *pScope = mScopes[size-1];
        if(mBraceDepth == pScope->braceDepth)
        {
            delete pScope;
            mScopes.erase(mScopes.begin() + (size-1));
        }
    }
}

void DeleteIdentifier(Identifier **pIdentifier)
{
    delete *pIdentifier;
}

void Parser::ProcessStatement()
{
    if(IsSet(FLAG_UNOPENED|FLAG_FRIEND))
    {
        int size = mScopes.size();
        if(size > 0)
        {
            Scope *pScope = mScopes[size-1];

            // Most likely this is a pre-declare or a friend declare
            //printf("Throwing out uninteresting scope: %s\n", pScope->mpName);
            PopScope();
        }
    }
    else if(!IsSet(FLAG_PAREN))
    {
        int ssize = mScopes.size();
        if(ssize > 0)
        {
            Scope *pScope = mScopes[ssize-1];
            if(pScope->isClass)
            {
                int isize = mIdentifiers.size();
                if(isize >= 2)
                {
                    Identifier *varType = mIdentifiers[isize-2];
                    Identifier *varName = mIdentifiers[isize-1];
                    int flags = 0;
                    if(varType->isBracketed && isize > 2)
                    {
                        Identifier *templateType = mIdentifiers[isize-3];
                        if(!strcmp(templateType->mpName, "std::vector"))
                        {
                            flags |= MacroGenerator::FLAG_ARRAY;
                        }
                    }

                    if(!mIgnore)
                        mpGenerator->AddMember(pScope->mpName, varType->mpName, varName->mpName, varType->starCount, flags);
                }
            }
        }
    }

    mIdentifiers.clear();
    ClearFlags();
}

void Parser::FlushDerived()
{
    if(IsSet(FLAG_UNOPENED))
    {
        int size = mIdentifiers.size();
        if(size > 0)
        {
            int ssize = mScopes.size();
            if(ssize > 0)
            {
                Scope *pScope = mScopes[ssize-1];
                if(pScope->isClass)
                {
                    Identifier *pIdentifier = mIdentifiers[size-1];
                    mpGenerator->DerivesFrom(GetFullScopeName(), pIdentifier->mpName);
                    mIdentifiers.clear();
                }
            }
        }
    }
}

void Parser::ProcessNextToken(int token, const char *text)
{
    //printf("           * Loop %s [%s]\n", get_token_name(ID_FROM_TOKEN(token)), text);

    switch(token)
    {
    case T_LEFTBRACE:
        {
            mBraceDepth++;
            FlushDerived();
            ClearFlag(FLAG_UNOPENED);
            break;
        }

    case T_COMMA:
        {
            FlushDerived();
            break;
        }

    case T_COLON_COLON:
        {
            SetFlag(FLAG_COLON_COLON);
            break;
        }

    case T_FRIEND:
        {
            SetFlag(FLAG_FRIEND);
            break;
        }

    case T_ENUM:
        {
            SetFlag(FLAG_ENUM|FLAG_UNOPENED);
            break;
        }

    case T_MM_IGNORE_START:
        {
            mIgnore = 1;
            break;
        }

    case T_MM_IGNORE_END:
        {
            mIgnore = 0;
            break;
        }

    case T_RIGHTBRACE:
        {
            mBraceDepth--;
            if(!IsSet(FLAG_ENUM))
            {
                PopScope();
                ProcessStatement();
            }
            break;
        }

    case T_CLASS:
    case T_STRUCT:
    case T_NAMESPACE:
        {
            Scope *pScope      = new Scope;
            pScope->braceDepth = mBraceDepth;
            pScope->isClass    = (token != T_NAMESPACE);
            pScope->mpName     = NULL;
            pScope->isExposed  = IsSet(FLAG_NEXT_IS_EXPOSED);
            mScopes.push_back(pScope);
            SetFlag(pScope->isClass ? FLAG_CLASS|FLAG_UNOPENED : FLAG_NAMESPACE);
            ClearFlag(FLAG_NEXT_IS_EXPOSED);
            break;
        }

    case T_BOOL:
    case T_CHAR:
    case T_DOUBLE:
    case T_FLOAT:
    case T_INT:
    case T_LONG:
    case T_SHORT:
    case T_VOID:
    case T_WCHART:
        {
            Identifier *pIdentifier = new Identifier;
            pIdentifier->mpName = strdup(text);
            pIdentifier->isKeyword = true;
            if(IsSet(FLAG_NEXT_IS_CONST))
            {
                pIdentifier->isConst = true;
                ClearFlag(FLAG_NEXT_IS_CONST);
            }
            mIdentifiers.push_back(pIdentifier);
            break;
        }

    case T_IDENTIFIER:
        {
            if(IsSet(FLAG_NAMESPACE|FLAG_CLASS))
            {
                int size = mScopes.size();
                if(size > 0)
                {
                    Scope *pScope = mScopes[size-1];
                    pScope->mpName = strdup(text);
                    if(IsSet(FLAG_CLASS))
                    {
                        if(IsExposed())
                        {
                            mpGenerator->AddClass(text); //(GetFullScopeName());
                        }
                    }
                    ClearFlag(FLAG_NAMESPACE|FLAG_CLASS);
                }
            }
            else if(IsExposed())
            {
                if(!IsSet(FLAG_PAREN))
                {
                    if(IsSet(FLAG_COLON_COLON))
                    {
                        int size = mIdentifiers.size();
                        if(size > 0)
                        {
                            Identifier *pIdentifier = mIdentifiers[size-1];
                            char temp[2048];
                            sprintf(temp, "%s::%s", pIdentifier->mpName, text);
                            free(pIdentifier->mpName);
                            pIdentifier->mpName = strdup(temp);
                        }
                        ClearFlag(FLAG_COLON_COLON);
                    }
                    else
                    {
                        Identifier *pIdentifier = new Identifier;
                        pIdentifier->mpName = strdup(text);
                        pIdentifier->isBracketed = IsSet(FLAG_BRACKET);
                        if(IsSet(FLAG_NEXT_IS_CONST))
                        {
                            pIdentifier->isConst = true;
                            ClearFlag(FLAG_NEXT_IS_CONST);
                        }
                        mIdentifiers.push_back(pIdentifier);
                    }
                }
            }
            break;
        }

    case T_STAR:
        {
            int size = mIdentifiers.size();
            if(size > 0)
            {
                Identifier *pIdentifier = mIdentifiers[size-1];
                pIdentifier->starCount++;
            }
            break;
        }

    case T_LEFTPAREN:
        {
            SetFlag(FLAG_PAREN);
            break;
        }

    case T_MM_EXPOSE:
    case T_MM_EXPOSED:
        {
            SetFlag(FLAG_NEXT_IS_EXPOSED);
            break;
        }

    case T_LESS:
        {
            mBracketCount++;
            SetFlag(FLAG_BRACKET);
            break;
        }

    case T_GREATER:
        {
            mBracketCount--;
            if(mBracketCount == 0)
            {
                ClearFlag(FLAG_BRACKET);
            }
            break;
        }

    case T_CONST:
        {
            int size = mIdentifiers.size();
            if(size > 0)
            {
                Identifier *pIdentifier = mIdentifiers[size-1];
                pIdentifier->isConst = true;
            }
            else
            {
                SetFlag(FLAG_NEXT_IS_CONST);
            }
            break;
        }

    case T_SEMICOLON:
        {
            if(IsSet(FLAG_ENUM))
            {
                for(unsigned int i=0; i<mIdentifiers.size(); i++)
                {
                    Identifier *pIdentifier = mIdentifiers[i];
                    mpGenerator->AddEnum(pIdentifier->mpName, GetFullScopeName());
                }

                mIdentifiers.clear();
                ClearFlag(FLAG_ENUM|FLAG_PAREN);
            }
            else
            {
                ProcessStatement();
            }
            break;
        }
    }
}

bool scan(const char *text, MacroGenerator *g)
{
    Parser s = {0};
    s.mpGenerator = g;

    s.text   = text;
    s.end    = s.text + strlen(s.text);
    s.cur    = s.text;
    s.token  = s.cur;
    s.errorFunc  = errorFunc;

    int id;
    while((id = getNextToken(&s)) != T_EOF)
    {
        if(s.error)
            break;

        int token_len = (int)((UINTPTR)s.cur - (UINTPTR)s.token);
        if(token_len > 0)
        {
            char token[1024];
            if(token_len > 1023)
                token_len = 1023;
            strncpy(token, s.token, token_len);
            token[token_len] = 0;

            s.ProcessNextToken(id, token);
        }

        s.token = s.cur;
    }

    return true;
}

int main(int argc, char **argv)
{
    for(int i=1; i<argc; i++)
    {
        DEBUG_MM("ARG%d: %s\n", i, argv[i]);
    }
    if(argc > 3)
    {
        FILE *f = fopen(argv[1], "rb");
        if(f)
        {
            //printf("Parsing %s\n", argv[1]);

            int size;
            char *buffer;

            fseek(f, 0, SEEK_END);
            size = ftell(f);
            fseek(f, 0, SEEK_SET);

            buffer = (char*)malloc(size+1);
            fread(buffer, 1, size, f);
            buffer[size] = 0;

            fclose(f);

            MacroGenerator g;
            scan(buffer, &g);

            {
                f = fopen(argv[2], "wb");
                g.GenerateCPP(f, argv[4]);
                fclose(f);
                f = fopen(argv[3], "wb");
                g.GenerateH(f, argv[4]);
                fclose(f);
            }
        }
        else
        {
            printf("Cannot read %s\n", argv[1]);
        }
    }

//    scan("int main() {} class AwesomeTown { public: AwesomeTown(); static int wut = 32.3432; }");
    return 0;
}
