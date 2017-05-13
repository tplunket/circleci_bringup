#ifndef CommandLine_h
#define CommandLine_h

class CommandLineOption;

class CommandLine
{
public:
    CommandLine();
    ~CommandLine();

    void AddOption(bool*, char const* name);

    bool Parse(int argc, char const** argv);

    char const* GetApplicationName() const { return appName; }

private:
    char const* appName;
    CommandLineOption* options;
};

#endif // ndef CommandLine_h
