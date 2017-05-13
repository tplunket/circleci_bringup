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

class CommandLineOption;

class CommandLine
{
public:
    CommandLine() : processor(CL_Create()) {}
    ~CommandLine() { CL_Destroy(processor); }

    void AddCountingOption(int* value, char const* name)
        { CL_AddCountingOption(processor, value, name); }

    bool Parse(int argc, char const** argv) { return CL_Parse(processor, argc, argv) == argc; }

    char const* GetApplicationName() const { return CL_GetAppName(processor); }

private:
    CommandLineProcessor processor;
};
#endif // def __cplusplus

#endif // ndef CommandLine_h
