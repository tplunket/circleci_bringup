#include "CommandLine.h"

#include <cstring>

#include <iostream>

class CommandLineArgument
{
protected:
    CommandLineArgument(char const* name) : name(name), next(nullptr) {}

public:
    virtual ~CommandLineArgument() { delete next; }

    virtual bool GetNumParams() const=0;
    char const* GetName() const { return name; }

    virtual bool LoadParams(char const** p)=0;

    void SetNext(CommandLineArgument* n) { next = n; }
    CommandLineArgument* GetNext() const { return next; }

private:
    char const* name;
    CommandLineArgument* next;
};

class BoolArgument : public CommandLineArgument
{
public:
    BoolArgument(char const* name, bool* value) : CommandLineArgument(name), value(value)
        { *value = false; }

    virtual bool GetNumParams() const override { return 0; }
    virtual bool LoadParams(char const** p) override;

private:
    bool* value;
};

CommandLine::CommandLine() : appName(""), arguments(nullptr)
{
}

CommandLine::~CommandLine()
{
    delete arguments;
}

void CommandLine::AddArgument(bool* output, char const* name)
{
    BoolArgument* ba = new BoolArgument(name, output);
    ba->SetNext(arguments);
    arguments = ba;
}

bool CommandLine::Parse(int argc, char const** argv)
{
    appName = argv[0];

    for (int i = 0; i < argc; ++i)
    {
        char const* cla = argv[i];
        if ((cla[0] == '-') || (cla[0] == '/'))
        {
            CommandLineArgument* a = arguments;
            while ((a != nullptr) && (std::strcmp(a->GetName(), cla+1) != 0))
                a = a->GetNext();

            if (a != nullptr)
                a->LoadParams(nullptr);
        }
    }

    return true;
}

bool BoolArgument::LoadParams(char const** params)
{
    *value = true;
    return true;
}

