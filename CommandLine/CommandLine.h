#ifndef CommandLine_h
#define CommandLine_h

#ifdef __cplusplus
extern "C"
{
#endif

struct CommandLineProcessor_;
typedef struct CommandLineProcessor_* CommandLineProcessor;

CommandLineProcessor CL_Create();
void CL_Destroy(CommandLineProcessor);

void CL_AddCountingOption(CommandLineProcessor, int* value, char const* name);

char const* CL_GetAppName(CommandLineProcessor);
int CL_Parse(CommandLineProcessor, int argc, char const** argv);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ndef CommandLine_h
