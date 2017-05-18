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

#if CL_USE_wchar_t
#define STRCMP wcscmp
#define STR "%ls"
#else
#define STRCMP strcmp
#define STR "%s"
#endif

/**
 * The types of options.
 */
enum OptionType
{
    OT_COUNTER,
    OT_INTEGER,
    OT_FLOAT,
    OT_STRING,
    OT_NUM_TYPES
};

/**
 * The controller for one option.
 */
struct CommandLineOption
{
    enum OptionType type;
    CL_StringType name;
    void* value;
    struct CommandLineOption* next;
};

/**
 * The functions that load option parameters.
 */
typedef int (*LoadParametersFn)(struct CommandLineOption*, CL_StringType*);
static int LoadCountingParameters(struct CommandLineOption*, CL_StringType*);
static int LoadIntegerParameters(struct CommandLineOption*, CL_StringType*);
static int LoadFloatParameters(struct CommandLineOption*, CL_StringType*);
static int LoadStringParameters(struct CommandLineOption*, CL_StringType*);

/**
 * Static data that hooks option types up to their handlers.
 */
struct OptionTypeData
{
    int numParameters;
    LoadParametersFn loadParameters;
};

static struct OptionTypeData s_optionTypeData[] =
{
    { 0, LoadCountingParameters },
    { 1, LoadIntegerParameters },
    { 1, LoadFloatParameters },
    { 1, LoadStringParameters },
};

/**
 * Command line arguments.
 */
struct CommandLineArgument
{
    CL_StringType* value;
    struct CommandLineArgument* next;
};

/**
 * The core opaque structure that gets returned.
 */
struct CommandLineProcessor_
{
    CL_StringType appName;
    struct CommandLineOption* options;
    struct CommandLineArgument* arguments;
    CL_StringType* overflow;
};

/**
 * Create a CommandLineProcessor.
 */
CommandLineProcessor CL_Create()
{
    CommandLineProcessor clp = malloc(sizeof(struct CommandLineProcessor_));
    clp->appName = NULL;
    clp->options = NULL;
    clp->arguments = NULL;
    clp->overflow = NULL;
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

    struct CommandLineArgument* cla = clp->arguments;
    while (cla != NULL)
    {
        struct CommandLineArgument* delete_me = cla;
        cla = cla->next;
        free(delete_me);
    }

    if (clp->overflow != NULL)
    {
        free((void*)clp->overflow);
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
void CL_AddCountingOption(CommandLineProcessor clp, int* value, CL_StringType name)
{
    struct CommandLineOption* clo = malloc(sizeof(struct CommandLineOption));
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
void CL_AddIntegerOption(CommandLineProcessor clp, int* value, CL_StringType name)
{
    struct CommandLineOption* clo = malloc(sizeof(struct CommandLineOption));
    clo->type = OT_INTEGER;
    clo->name = name;
    clo->value = value;
    clo->next = clp->options;
    clp->options = clo;

    *value = 0;
}

/**
 * Add an floating point option to the CommandLineProcessor.
 *
 * The value following the option is loaded into \c *value.
 */
void CL_AddFloatOption(CommandLineProcessor clp, float* value, CL_StringType name)
{
    struct CommandLineOption* clo = malloc(sizeof(struct CommandLineOption));
    clo->type = OT_FLOAT;
    clo->name = name;
    clo->value = value;
    clo->next = clp->options;
    clp->options = clo;

    *value = 0;
}

/**
 * Add a string option to the CommandLineProcessor.
 *
 * The value following the option is loaded into \c *value.
 */
void CL_AddStringOption(CommandLineProcessor clp, CL_StringType* value, CL_StringType name)
{
    struct CommandLineOption* clo = malloc(sizeof(struct CommandLineOption));
    clo->type = OT_STRING;
    clo->name = name;
    clo->value = (void*)value;
    clo->next = clp->options;
    clp->options = clo;

    *value = NULL;
}

/**
 * Add a argument for the command line.
 */
void CL_AddArgument(CommandLineProcessor clp, CL_StringType* value)
{
    struct CommandLineArgument* cla = malloc(sizeof(struct CommandLineArgument));
    cla->value = value;
    cla->next = NULL;

    struct CommandLineArgument** end = &clp->arguments;
    while (*end != NULL)
    {
        end = &((*end)->next);
    }
    *end = cla;

    *value = NULL;
}

/**
 * Let the system track non-option arguments.
 */
void CL_EnableOverflowArguments(CommandLineProcessor clp)
{
    if (clp->overflow == NULL)
    {
        clp->overflow = malloc(sizeof(CL_StringType));
        *(clp->overflow) = NULL;
    }
}

/**
 * Get the overflow arguments.
 */
CL_StringType* CL_GetOverflowArguments(CommandLineProcessor clp)
{
    return clp->overflow;
}

/**
 * Get the application's name.
 */
CL_StringType CL_GetAppName(CommandLineProcessor clp)
{
    return clp->appName;
}

/**
 * Do the parse.
 */
int CL_Parse(CommandLineProcessor clp, int argc, CL_StringType* argv)
{
    clp->appName = argv[0];
    int numErrors = 0;

    struct CommandLineArgument* cla = clp->arguments;

    if (clp->overflow != NULL)
    {
        free((void*)clp->overflow);
        clp->overflow = malloc(sizeof(CL_StringType) * argc);
    }
    CL_StringType* overflow = clp->overflow;

    for (int i = 1; i < argc; ++i)
    {
        CL_StringType arg = argv[i];
        if ((arg[0] == '-') || (arg[0] == '/'))
        {
            struct CommandLineOption* o = clp->options;
            while ((o != NULL) && (STRCMP(o->name, arg+1) != 0))
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
                    Error("Command line option '" STR "' requires %d parameters but only %d are "
                          "available.", arg, otd->numParameters, numArgsLeft);
                    ++numErrors;
                }
            }
            else
            {
                Error("Unknown option '" STR "'.", arg);
                ++numErrors;
            }
        }
        else if (cla != NULL)
        {
            *(cla->value) = arg;
            cla = cla->next;
        }
        else
        {
            if (overflow != NULL)
            {
                *overflow = arg;
                ++overflow;
            }
            else
            {
                Error("Argument '" STR "' can't be handled.", arg);
                ++numErrors;
            }
        }
    }

    if (overflow != NULL)
    {
        *overflow = NULL;
    }

    return numErrors == 0;
}

/**
 * Load parameters into the OT_COUNTER command line option.
 */
int LoadCountingParameters(struct CommandLineOption* o, CL_StringType* params)
{
    *(int*)(o->value) += 1;
    return 0;
}

/**
 * Load parameters into the OT_INTEGER command line option.
 */
int LoadIntegerParameters(struct CommandLineOption* o, CL_StringType* params)
{
    int *v = o->value;
    CL_StringType p = params[0];

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
        Error("'" STR "' is not a valid parameter to '-" STR "'.", params[0], o->name);

    *v *= negator;
    return (*p == '\0') ? 1 : 0;
}

/**
 * Load parameters into the OT_FLOAT command line option.
 */
int LoadFloatParameters(struct CommandLineOption* o, CL_StringType* params)
{
    float *v = o->value;
    CL_StringType p = params[0];

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

    if (*p == '.')
    {
        float accumulator = 0.0f;
        float divider = 1.0f;

        ++p;

        while ((*p != '\0') && (*p >= '0') && (*p <= '9'))
        {
            accumulator = (accumulator * 10.0f) + (*p - '0');
            divider *= 10.0f;
            ++p;
        }

        *v += (accumulator / divider);
    }

    if (*p != '\0')
        Error("'" STR "' is not a valid parameter to '-" STR "'.", params[0], o->name);

    *v *= negator;
    return (*p == '\0') ? 1 : 0;
}

/**
 * Load parameters into the OT_STRING command line option.
 */
int LoadStringParameters(struct CommandLineOption* o, CL_StringType* params)
{
    CL_StringType* v = o->value;
    CL_StringType p = params[0];
    *v = p;
    return 1;
}

