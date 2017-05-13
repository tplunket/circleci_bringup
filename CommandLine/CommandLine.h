#ifndef CommandLine_h
#define CommandLine_h

class CommandLineArgument;

class CommandLine
{
public:
    CommandLine();
    ~CommandLine();

    void AddArgument(bool*, char const* name);

    bool Parse(int argc, char const** argv);

    char const* GetApplicationName() const { return appName; }

private:
    char const* appName;
    CommandLineArgument* arguments;
};

#endif // ndef CommandLine_h
