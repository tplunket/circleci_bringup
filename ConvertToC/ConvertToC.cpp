/**
 * Convert an arbitrary data file into a "C string" and an associated length.
 *
 * \author Tom Plunket <tom@mightysprite.com>
 * \copyright (c) 2010-2017 Tom Plunket, all rights reserved
 *
 * The data will be named k_<TranslatedName> and k_<TranslatedName>_length.
 * <TranslatedName> is the filename with dots converted to underscores.
 *
 *     python ConvertToCString.py inputfile.dat inputfile.c
 *
 * The above line will yield two data members; k_inputfile_dat will be the file
 * contents as a const char array and k_inputfile_dat_length will be the length of
 * the file.
 *
 * The character array will be zero terminated for safety's sake (as it is often
 * used for text files and prevents many circumstances of user error) but the
 * given length will be the actual length of the input file.
 *
 * Licensed under the MIT/X license. Do with these files what you will but leave this header intact.
 */

#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

// Visual Studio supports strings up to about 16k IIRC. Other compilers may have different limits.
static int const MAX_STRING_LENGTH = 15000;
static bool const BULK_AS_CHARACTERS = false;

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
        CHARACTER_TRANSLATIONS['\n'] = "\\n\"\n\t\"";
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
                oss << "\\x" << std::setw(2) << std::setfill('0') << std::hex << i;
                CHARACTER_TRANSLATIONS[i] = oss.str();
            }
        }
        else if (asHex)
        {
            for (int i = 0; i < 256; ++i)
            {
                std::ostringstream oss;
                oss << "0x" << std::setw(2) << std::setfill('0') << std::hex << i;
                CHARACTER_TRANSLATIONS[i] = oss.str();
            }
        }
        else if (style == AS_NUMBERS)
        {
            for (int i = 0; i < 256; ++i)
            {
                std::ostringstream oss;
                oss << std::setw(3) << i;
                CHARACTER_TRANSLATIONS[i] = oss.str();
            }
        }
        else // NUMBERS_AND_CHARS !asHex
        {
            for (int i = 0; i < 256; ++i)
            {
                std::ostringstream oss;
                oss << std::setw(4) << i;
                CHARACTER_TRANSLATIONS[i] = oss.str();
            }
        }

        if (style == AS_NUMBERS_AND_CHARS)
        {
            // but translate the letters and numbers and punctuation to themselves
            for (char c = 32; c < 127; ++c)
            {
                std::ostringstream oss;
                oss << " '" << c << "'";
                CHARACTER_TRANSLATIONS[c] = oss.str();
            }

            // and then handle these characters specially.
            CHARACTER_TRANSLATIONS[' '] = " ' '";
            CHARACTER_TRANSLATIONS['"'] = " '\"'";
            CHARACTER_TRANSLATIONS['\''] = "'\\''";
            CHARACTER_TRANSLATIONS['\\'] = "'\\\\'";
            CHARACTER_TRANSLATIONS['\n'] = "'\\n'";
            CHARACTER_TRANSLATIONS['\r'] = "'\\r'";
            CHARACTER_TRANSLATIONS['\t'] = "'\\t'";
            CHARACTER_TRANSLATIONS['\0'] = "'\\0'";
        }
    }
}

std::string ConvertDataName(std::string name)
{
    size_t lastSlash = name.find_last_of("/\\");
    if (lastSlash != std::string::npos)
        name.erase(0, lastSlash+1);
    std::replace(name.begin(), name.end(), '.', '_');
    return name;
}

std::string Convert(std::string const& data)
{
    std::string output;
    for (int8_t c : data)
    {
        output.append(CHARACTER_TRANSLATIONS[c]);
    }
    return output;
}

std::string FormatAsString(std::string const& data, std::string const& dataName)
{
    auto lines = Convert(data);
    // substitute pairs of question marks to avoid errant trigraph interpretation by the compiler.
    // http://en.wikipedia.org/wiki/Digraphs_and_trigraphs
    //output = re.sub(r"[?]{2}([=/'()!<>-])", r'?\\?\1', lines)
    //output = '    "%s"' % output
    //return output
    return lines;
}

std::string FormatAsData(std::string const& data, std::string const& dataName)
{
    std::vector<std::string> output { "{" };

    // put a NULL at the end in case the string is used as a string; it doesn't
    // count into the length that we printed before though.
    auto new_data = Convert(data);
    new_data.append(CHARACTER_TRANSLATIONS['\0']);
    size_t dataLen = new_data.size();
    size_t current = 0;
    while (current < dataLen)
    {
        std::ostringstream oss;
        if ((current % 1024) == 0)
            oss << "    /* byte " << current << " */" << std::endl;
        oss << "    ";
        for (int i = 0; i < 16; ++i)
            oss << data[current+i] << ',';
        current += 16;
    }

    output.push_back("}");
    std::ostringstream actual_output;
    for (auto line : output)
    {
        actual_output << line << std::endl;
    }
    return actual_output.str();
}

int Main(std::string const& inputFile, std::string const& outputFile)
{
    std::ifstream file(inputFile, std::ios::binary | std::ios::ate);
    size_t size = file.tellg();
    file.seekg(0);
    std::string contents(size, 0);
    file.read(&contents[0], size);

    std::string dataName = ConvertDataName(inputFile);
    std::string output;

    if (size < MAX_STRING_LENGTH)
    {
        GenerateTranslations(AS_STRING);
        output = FormatAsString(contents, dataName);
    }
    else
    {
        GenerateTranslations(AS_NUMBERS_AND_CHARS, true);
        output = FormatAsData(contents, dataName);
    }

    std::ofstream outfile(outputFile, std::ios::binary | std::ios::trunc);
    outfile << "/* This file was generated by a script and probably shouldn't be modified by hand. */" << std::endl
            << std::endl
            << "const unsigned int k_" << dataName << "_length = " << size << ";" << std::endl
            << "const char k_" << dataName << "[] =" << std::endl;

    outfile << output << ";" << std::endl;

    return 0;
}

int main(int argc, char const** argv)
{
    if (argc == 1)
    {
        std::cerr << "Need to give a filename.";
        return 1;
    }
    else
    {
        std::string infile = argv[1];
        std::string outfile;
        if (argc > 2)
            outfile = argv[2];
        else
            outfile = infile + ".c";
        return Main(infile, outfile);
    }
}