/**
 * Convert an arbitrary data file into a "C string" and an associated length.
 *
 * \author Tom Plunket <tom@mightysprite.com>
 * \copyright (c) 2010-2017 Tom Plunket, all rights reserved
 *
 * The data will be named k_<TranslatedName> and k_<TranslatedName>_length.
 * <TranslatedName> is the filename with dots converted to underscores by default.
 *
 *     ConvertToCString inputfile.dat inputfile.c
 *
 * The above line will yield two data members; k_inputfile_dat will be the file
 * contents as a const char array and k_inputfile_dat_length will be the length
 * of the file.
 *
 * The character array will be zero terminated for safety's sake (as it is often
 * used for text files and prevents many circumstances of user error) but the
 * given length will be the actual length of the input file.
 *
 * Additional command line options are:
 *     -b[in[ary]]: Force a "binary" export as an array of numbers instead of a
 *                  zero-terminated string.
 *     -h[ex]: Force binary export but use hexadecimal instead of decimal numbers
 *             (this yields about 20% larger files).
 *     -n[ame]: Set the name that gets used for the variables that are created
 *              instead of using the default <TranslatedName> as defined above.
 *
 * Licensed under the MIT/X license. Do with these files what you will but leave this header intact.
 */

#include "CommandLine/CommandLine.hpp"
#include "Log/StdStreamLogTarget.hpp"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

// Visual Studio supports strings up to about 16k IIRC. Other compilers may have different limits.
static int const MAX_STRING_LENGTH = 15000;

std::map<char, std::string> CHARACTER_TRANSLATIONS;
enum ExportType
{
    AS_STRING,
    AS_CHARS,
    AS_NUMBERS,
    AS_NUMBERS_AND_CHARS,
};

void GenerateTranslations(ExportType style, bool asHex=false)
{
    if (style == AS_STRING)
    {
        // default each character to the \x00 through \xff
        for (int i = 0; i < 256; ++i)
        {
            std::ostringstream oss;
            oss << "\\x" << std::setw(2) << std::setfill('0') << std::hex << i;
            CHARACTER_TRANSLATIONS[i] = oss.str();
        }

        // but translate the letters and numbers and punctuation to themselves
        for (char i = 32; i < 127; ++i)
            CHARACTER_TRANSLATIONS[i] = std::string(1, i);

        // and then handle these characters specially.
        CHARACTER_TRANSLATIONS[' '] = " ";
        CHARACTER_TRANSLATIONS['"'] = "\\\"";
        CHARACTER_TRANSLATIONS['\\'] = "\\\\";
        CHARACTER_TRANSLATIONS['\n'] = "\\n";
        CHARACTER_TRANSLATIONS['\r'] = "\\r";
        CHARACTER_TRANSLATIONS['\t'] = "\\t";
        CHARACTER_TRANSLATIONS['\0'] = "\\0";
    }
    else
    {
        // default each character to the \x00 through \xff
        if (style == AS_CHARS)
        {
            for (int i = 0; i < 256; ++i)
            {
                std::ostringstream oss;
                oss << "\\x" << std::setw(2) << std::setfill('0') << std::hex << i << ',';
                CHARACTER_TRANSLATIONS[i] = oss.str();
            }
        }
        else if (asHex)
        {
            for (int i = 0; i < 256; ++i)
            {
                std::ostringstream oss;
                oss << "0x" << std::setw(2) << std::setfill('0') << std::hex << i << ',';
                CHARACTER_TRANSLATIONS[i] = oss.str();
            }
        }
        else if (style == AS_NUMBERS)
        {
            for (int i = 0; i < 256; ++i)
            {
                std::ostringstream oss;
                oss << std::setw(3) << i << ',';
                CHARACTER_TRANSLATIONS[i] = oss.str();
            }
        }
        else // NUMBERS_AND_CHARS !asHex
        {
            for (int i = 0; i < 256; ++i)
            {
                std::ostringstream oss;
                oss << std::setw(4) << i << ',';
                CHARACTER_TRANSLATIONS[i] = oss.str();
            }
        }

        if (style == AS_NUMBERS_AND_CHARS)
        {
            // but translate the letters and numbers and punctuation to themselves
            for (char c = 32; c < 127; ++c)
            {
                std::ostringstream oss;
                oss << " '" << c << "',";
                CHARACTER_TRANSLATIONS[c] = oss.str();
            }

            // and then handle these characters specially.
            CHARACTER_TRANSLATIONS[' '] = " ' ',";
            CHARACTER_TRANSLATIONS['"'] = " '\"',";
            CHARACTER_TRANSLATIONS['\''] = "'\\'',";
            CHARACTER_TRANSLATIONS['\\'] = "'\\\\',";
            CHARACTER_TRANSLATIONS['\n'] = "'\\n',";
            CHARACTER_TRANSLATIONS['\r'] = "'\\r',";
            CHARACTER_TRANSLATIONS['\t'] = "'\\t',";
            CHARACTER_TRANSLATIONS['\0'] = "'\\0',";
        }
    }
}

std::string DefaultDataName(std::string name)
{
    size_t lastSlash = name.find_last_of("/\\");
    if (lastSlash != std::string::npos)
        name.erase(0, lastSlash+1);
    std::replace(name.begin(), name.end(), '.', '_');
    return "k_" + name;
}

std::string FormatAsString(std::string const& data, std::string const& dataName)
{
    std::vector<std::string> output;
    std::string currentLine;
    size_t startOfAscii = std::string::npos;
    for (int8_t c : data)
    {
        if ((c != '\n') && (c != '\r'))
        {
            if ((c == '\t') || ((c >= 32) && (c < 127)))
            {
                if (startOfAscii == std::string::npos)
                    startOfAscii = currentLine.length();
            }
            else
                startOfAscii = std::string::npos;
        }

        currentLine.append(CHARACTER_TRANSLATIONS[c]);
        if (((startOfAscii != 0) && (currentLine.size() > 90)) || (currentLine.size() > 140))
        {
            if ((startOfAscii == 0) || (startOfAscii == std::string::npos))
            {
                output.push_back(currentLine);
                currentLine.erase();
                startOfAscii = std::string::npos;
            }
            else
            {
                output.push_back(currentLine.substr(0, startOfAscii));
                currentLine.erase(0, startOfAscii);
                startOfAscii = 0;
            }
        }

        if ((startOfAscii == 0) && (c == '\n'))
        {
            output.push_back(currentLine);
            currentLine.erase();
        }
    }
    if (!currentLine.empty())
        output.push_back(currentLine);

    std::ostringstream oss;

    for (std::string& line : output)
    {
        if (oss.str().empty())
            oss << "\t\"";
        else
            oss << "\"\n\t\"";

        // Substitute pairs of question marks to avoid errant trigraph interpretation by the
        // compiler. See http://en.wikipedia.org/wiki/Digraphs_and_trigraphs
        size_t match = line.find("??");
        std::string trigraphs = "=/'()!<>-";
        while (match != std::string::npos)
        {
            if (trigraphs.find(line[match+2]) != std::string::npos)
                line.replace(match, 2, "?\\?");
            match = line.find("??", match+1);
        }

        oss << line;
    }

    oss << "\";" << std::endl;
    return oss.str();
}

std::string FormatAsData(std::string data, std::string const& dataName)
{
    std::vector<std::string> output { "{" };

    // put a NULL at the end in case the string is used as a string; it doesn't
    // count into the length that we printed before though.
    data.push_back('\0');

    size_t dataLen = data.size();
    size_t current = 0;
    while (current < dataLen)
    {
        std::ostringstream oss;
        if ((current % 1024) == 0)
            oss << "\t/* byte " << current << " */" << std::endl;
        oss << "\t";
        for (int i = 0; (i < 16) && ((current + i) < dataLen); ++i)
        {
            if (i > 0)
                oss << ' ';
            oss << CHARACTER_TRANSLATIONS[data[current+i]];
        }
        output.push_back(oss.str());
        current += 16;
    }

    output.push_back("};");
    std::ostringstream actual_output;
    for (auto line : output)
    {
        actual_output << line << std::endl;
    }
    return actual_output.str();
}

int Main(std::string const& inputFile, std::string const& outputFile, bool asBinary, bool asHex,
         bool forceString, std::string const& dataName)
{
    std::ifstream file(inputFile, std::ios::binary | std::ios::ate);
    if (!file.good())
    {
        Error("Couldn't open file %s.", inputFile.c_str());
        return 3;
    }
    size_t size = file.tellg();
    file.seekg(0);
    std::string contents(size, 0);
    file.read(&contents[0], size);

    std::string output;

    asBinary |= asHex;

    if (!forceString && asBinary)
    {
        GenerateTranslations(AS_NUMBERS, asHex);
        output = FormatAsData(contents, dataName);
    }
	else if (forceString || (size < MAX_STRING_LENGTH))
    {
        GenerateTranslations(AS_STRING);
        output = FormatAsString(contents, dataName);
    }
    else
    {
        GenerateTranslations(AS_NUMBERS_AND_CHARS);
        output = FormatAsData(contents, dataName);
    }

    std::ofstream outfile(outputFile, std::ios::binary | std::ios::trunc);
    outfile << "/* This file was generated by a script and probably shouldn't be modified by hand. */" << std::endl
            << std::endl
            << "const unsigned int " << dataName << "_length = " << size << ";" << std::endl
            << "const " << (asBinary ? "unsigned char " : "char ") << dataName << "[] =" << std::endl;

    outfile << output;

    Info("%s -> %s (%d bytes)", inputFile.c_str(), outputFile.c_str(), size);
    return 0;
}

int main(int argc, char const** argv)
{
    StdStreamLogTarget lt;

    std::string infile, outfile, dataName;
    bool asBinary, asHex, forceString;

    {
        char const* in, *out;
        int b, h, x;
        char const* no;

        CommandLine cl;
        cl.AddArgument(&in);
        cl.AddArgument(&out);
        cl.AddCountingOption(&b, "b");
        cl.AddCountingOption(&b, "bin");
        cl.AddCountingOption(&b, "binary");
        cl.AddCountingOption(&h, "h");
        cl.AddCountingOption(&h, "hex");
        cl.AddStringOption(&no, "n");
        cl.AddStringOption(&no, "name");
        cl.AddCountingOption(&x, "x");

        if (!cl.Parse(argc, argv))
        {
            // if Parse fails, the log target gets the messages.
            return 1;
        }

        if (in == nullptr)
        {
            Error("Need to give a filename.");
            return 2;
        }

        infile = in;
        if (out != nullptr)
            outfile = out;
        else
            outfile = infile + ".c";

        forceString = x != 0;
        asBinary = b != 0;
        asHex = h != 0;
        if (no != nullptr)
            dataName = no;
        else
            dataName = DefaultDataName(infile);
    }

    return Main(infile, outfile, asBinary, asHex, forceString, dataName);
}
