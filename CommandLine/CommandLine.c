/**
 * A simple command line processing system.
 *
 * \author Tom Plunket <tom@mightysprite.com>
 * \copyright (c) 2017 Tom Plunket, all rights reserved
 *
 * Licensed under the MIT/X license. Do with these files what you will.
 */

#include "CommandLine.h"

#include "Log/Log.h"

#include <string.h>
#include <stdlib.h>

/**
 * The types of options.
 */
enum OptionType
{
    OT_COUNTER,
    OT_INTEGER,
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
static int LoadIntegerParameters(struct CommandLineOption*, const char**);

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
    { 1, LoadIntegerParameters },
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
 * Add a counting option to the CommandLineProcessor.
 *
 * A counting option adds one to \c *value each time it appears on the command line. This is
 * typically used to implement boolean options but a count of the number of times the option appears
 * is an easy extension.
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
 * Add an integer option to the CommandLineProcessor.
 *
 * The value following the option is loaded into \c *value.
 */
void CL_AddIntegerOption(CommandLineProcessor clp, int* value, char const* name)
{
    struct CommandLineOption* clo = (struct CommandLineOption*)malloc(sizeof(struct CommandLineOption));
    clo->type = OT_INTEGER;
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
    int numErrors = 0;

    for (int i = 1; i < argc; ++i)
    {
        char const* arg = argv[i];
        if ((arg[0] == '-') || (arg[0] == '/'))
        {
            struct CommandLineOption* o = clp->options;
            while ((o != NULL) && (strcmp(o->name, arg+1) != 0))
                o = o->next;

            if (o != NULL)
            {
                struct OptionTypeData const* otd = &s_optionTypeData[o->type];
                int numArgsLeft = argc - (i + 1);
                if (numArgsLeft >= otd->numParameters)
                {
                    int paramsLoaded = otd->loadParameters(o, &argv[i+1]);
                    if (paramsLoaded != otd->numParameters)
                        ++numErrors;
                    i += paramsLoaded;
                }
                else
                {
                    Error("Command line option -%s requires %d parameters but only %d are "
                          "available.", arg, otd->numParameters, numArgsLeft);
                    ++numErrors;
                }
            }
            else
            {
                Error("Unknown option '%s'.", arg);
                ++numErrors;
            }
        }
        else
        {
            Error("Parameter '%s' can't be handled.", arg);
            ++numErrors;
        }
    }

    return numErrors == 0;
}

/**
 * Load parameters into the OT_COUNTER command line option.
 */
int LoadCountingParameters(struct CommandLineOption* o, char const** params)
{
    *(int*)(o->value) += 1;
    return 0;
}

/**
 * Load parameters into the OT_INTEGER command line option.
 */
int LoadIntegerParameters(struct CommandLineOption* o, char const** params)
{
    int *v = (int*)(o->value);
    char const* p = params[0];

    int negator = 1;
    if (*p == '-')
    {
        negator = -1;
        ++p;
    }

    while ((*p != '\0') && (*p >= '0') && (*p <= '9'))
    {
        *v = (*v * 10) + (*p - '0');
        ++p;
    }

    if (*p != '\0')
        Error("'%s' is not a valid parameter to '-%s'.", params[0], o->name);

    *v *= negator;
    return (*p == '\0') ? 1 : 0;
}

