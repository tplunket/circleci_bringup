#include "CommandLine.h"

#include <cstring>

#include <iostream>

class CommandLineOption
{
protected:
    CommandLineOption(char const* name) : name(name), next(nullptr) {}

public:
    virtual ~CommandLineOption() { delete next; }

    virtual bool GetNumParams() const=0;
    char const* GetName() const { return name; }

    virtual bool LoadParams(char const** p)=0;

    void SetNext(CommandLineOption* n) { next = n; }
    CommandLineOption* GetNext() const { return next; }

private:
    char const* name;
    CommandLineOption* next;
};

class BoolOption : public CommandLineOption
{
public:
    BoolOption(char const* name, bool* value) : CommandLineOption(name), value(value)
        { *value = false; }

    virtual bool GetNumParams() const override { return 0; }
    virtual bool LoadParams(char const** p) override;

private:
    bool* value;
};

CommandLine::CommandLine() : appName(""), options(nullptr)
{
}

CommandLine::~CommandLine()
{
    delete options;
}

void CommandLine::AddOption(bool* output, char const* name)
{
    BoolOption* bo = new BoolOption(name, output);
    bo->SetNext(options);
    options = bo;
}

bool CommandLine::Parse(int argc, char const** argv)
{
    appName = argv[0];

    for (int i = 0; i < argc; ++i)
    {
        char const* cla = argv[i];
        if ((cla[0] == '-') || (cla[0] == '/'))
        {
            CommandLineOption* o = options;
            while ((o != nullptr) && (std::strcmp(o->GetName(), cla+1) != 0))
                o = o->GetNext();

            if (o != nullptr)
                o->LoadParams(nullptr);
        }
    }

    return true;
}

bool BoolOption::LoadParams(char const** params)
{
    *value = true;
    return true;
}

