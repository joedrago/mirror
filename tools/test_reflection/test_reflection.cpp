#include <stdio.h>
#include <stdlib.h>

#include "lib_core/StringUtil.h"
#include "lib_reflection/Mirror.h"
#include "cJSON.h"

struct Awesome
{
	int a;
	int b;
};

struct Embedded
{
	int c;
	Awesome awesome;
};

// ---------------------------------------------------------------------------------
// This section is what would be auto-generated from the above structs

MIRROR_IMPLEMENT_BEGIN(Awesome, 0, 2);
    MIRROR_IMPLEMENT_ADD(new Reflection::IntegerMirror("a", MOFFSET(Awesome, a)));
    MIRROR_IMPLEMENT_ADD(new Reflection::IntegerMirror("b", MOFFSET(Awesome, b)));
MIRROR_IMPLEMENT_END(Awesome);

MIRROR_IMPLEMENT_BEGIN(Embedded, 0, 2);
    MIRROR_IMPLEMENT_ADD(new Reflection::IntegerMirror("c", MOFFSET(Embedded, c)));
    MIRROR_IMPLEMENT_ADD(CreateObjectMirror_Awesome("awesome", MOFFSET(Embedded, awesome)));
MIRROR_IMPLEMENT_END(Embedded);

// ---------------------------------------------------------------------------------

using namespace Reflection;

int main(int argc, char* argv[])
{
	printf("main()\n");

    Reflection::ObjectMirror::Init();

    cJSON_Hooks hooks = { malloc, free };
    cJSON_InitHooks(&hooks);

    ObjectMirror *mirror;

    cJSON *src = cJSON_Parse("{ \"a\":4, \"b\":5 }");
    Awesome dst = {0};
    mirror = ObjectMirror::Find("Awesome");
    mirror->Populate(&dst, src);
    printf("Awesome! [%d, %d]\n", dst.a, dst.b);

    cJSON *s2 = cJSON_Parse("{ \"c\":6, \"awesome\":{\"a\":7, \"b\" : 8} }");
    Embedded d2 = {0};
    mirror = ObjectMirror::Find("Embedded");
    mirror->Populate(&d2, s2);
    printf("Embedded! [%d, [%d, %d]]\n", d2.c, d2.awesome.a, d2.awesome.b);

    return 0;
}
