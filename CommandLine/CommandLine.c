#include "CommandLine.h"

#ifdef __cplusplus
#error "No."
#endif

#include <string.h>
#include <stdlib.h>

/**
 * The types of options.
 */
enum OptionType
{
    OT_COUNTER,
    OT_NUM_TYPES
};

/**
 * The controller for one option.
 */
struct CommandLineOption
{
    enum OptionType type;
    char const* name;
    void* value;
    struct CommandLineOption* next;
};

/**
 * The functions that load parameters.
 */
typedef int (*LoadParameters)(struct CommandLineOption*, char const**);
static int LoadCountingParameters(struct CommandLineOption*, const char**);

/**
 * Static data that hooks option types up to their handlers.
 */
struct OptionTypeData
{
    int numParameters;
    LoadParameters loadParameters;
};

static struct OptionTypeData s_optionTypeData[] =
{
    { 0, LoadCountingParameters },
};

/**
 * The core opaque structure that gets returned.
 */
struct CommandLineProcessor_
{
    char const* appName;
    struct CommandLineOption* options;
};

/**
 * Create a CommandLineProcessor.
 */
CommandLineProcessor CL_Create()
{
    CommandLineProcessor clp = (CommandLineProcessor)malloc(sizeof(struct CommandLineProcessor_));
    clp->appName = NULL;
    clp->options = NULL;
    return clp;
}

/**
 * Destroy a CommandLineProcessor.
 */
void CL_Destroy(CommandLineProcessor clp)
{
    struct CommandLineOption* clo = clp->options;
    while (clo != NULL)
    {
        struct CommandLineOption* delete_me = clo;
        clo = clo->next;
        free(delete_me);
    }

    free(clp);
}

/**
 * Add a boolean option to the CommandLineProcessor.
 */
void CL_AddCountingOption(CommandLineProcessor clp, int* value, char const* name)
{
    struct CommandLineOption* clo = (struct CommandLineOption*)malloc(sizeof(struct CommandLineOption));
    clo->type = OT_COUNTER;
    clo->name = name;
    clo->value = value;
    clo->next = clp->options;
    clp->options = clo;

    *value = 0;
}

/**
 * Get the application's name.
 */
char const* CL_GetAppName(CommandLineProcessor clp)
{
    return clp->appName;
}

/**
 * Do the parse.
 */
int CL_Parse(CommandLineProcessor clp, int argc, char const** argv)
{
    clp->appName = argv[0];

    for (int i = 0; i < argc; ++i)
    {
        char const* arg = argv[i];
        if ((arg[0] == '-') || (arg[0] == '/'))
        {
            struct CommandLineOption* o = clp->options;
            while ((o != NULL) && (strcmp(o->name, arg+1) != 0))
                o = o->next;

            if (o != NULL)
                s_optionTypeData[o->type].loadParameters(o, NULL);
        }
    }

    return argc;
}

/**
 * Load parameters into the OT_COUNTER command line option.
 */
int LoadCountingParameters(struct CommandLineOption* o, char const** params)
{
    *(int*)(o->value) += 1;
    return 1;
}

