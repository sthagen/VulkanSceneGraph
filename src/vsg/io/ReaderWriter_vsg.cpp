/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/core/Version.h>
#include <vsg/io/AsciiInput.h>
#include <vsg/io/AsciiOutput.h>
#include <vsg/io/BinaryInput.h>
#include <vsg/io/BinaryOutput.h>
#include <vsg/io/ReaderWriter_vsg.h>

#include <cstring>
#include <iostream>

using namespace vsg;

// use a static handle that is initialized once at start up to avoid multi-threaded issues associated with calling std::locale::classic().
auto s_class_locale = std::locale::classic();

static VsgVersion parseVersion(std::string version_string)
{
    VsgVersion version{0, 0, 0, 0};

    for (auto& c : version_string)
    {
        if (c == '.') c = ' ';
    }

    std::stringstream str(version_string);

    str >> version.major;
    str >> version.minor;
    str >> version.patch;
    str >> version.soversion;

    return version;
}

ReaderWriter_vsg::ReaderWriter_vsg()
{
    _objectFactory = ObjectFactory::instance();
}

ReaderWriter_vsg::FormatInfo ReaderWriter_vsg::readHeader(std::istream& fin) const
{
    fin.imbue(s_class_locale);

    const char* match_token_ascii = "#vsga";
    const char* match_token_binary = "#vsgb";
    char read_token[5];
    fin.read(read_token, 5);

    FormatType type = NOT_RECOGNIZED;
    if (std::strncmp(match_token_ascii, read_token, 5) == 0)
        type = ASCII;
    else if (std::strncmp(match_token_binary, read_token, 5) == 0)
        type = BINARY;

    if (type == NOT_RECOGNIZED)
    {
        std::cout << "Header token not matched" << std::endl;
        return FormatInfo(NOT_RECOGNIZED, VsgVersion{0, 0, 0, 0});
    }

    std::string version_string;
    std::getline(fin, version_string);

    auto version = parseVersion(version_string);

    return FormatInfo(type, version);
}

void ReaderWriter_vsg::writeHeader(std::ostream& fout, const FormatInfo& formatInfo) const
{
    if (formatInfo.first == NOT_RECOGNIZED) return;

    fout.imbue(s_class_locale);
    if (formatInfo.first == BINARY)
        fout << "#vsgb";
    else
        fout << "#vsga";

    auto version = formatInfo.second;
    fout << " " << version.major << "." << version.minor << "." << version.patch << "\n";
}

vsg::ref_ptr<vsg::Object> ReaderWriter_vsg::read(const vsg::Path& filename, ref_ptr<const Options> options) const
{
    auto ext = vsg::fileExtension(filename);
    if (ext == "vsga" || ext == "vsgt" || ext == "vsgb")
    {
        vsg::Path filenameToUse = options ? findFile(filename, options) : filename;
        if (filenameToUse.empty()) return {};

        std::ifstream fin(filenameToUse, std::ios::in | std::ios::binary);
        if (!fin) return {};

        auto [type, version] = readHeader(fin);
        if (type == BINARY)
        {
            vsg::BinaryInput input(fin, _objectFactory, options);
            input.filename = filenameToUse;
            input.version = version;
            return input.readObject("Root");
        }
        else if (type == ASCII)
        {
            vsg::AsciiInput input(fin, _objectFactory, options);
            input.filename = filenameToUse;
            input.version = version;
            return input.readObject("Root");
        }
    }

    // return null as no means for loading file has been found
    return {};
}

vsg::ref_ptr<vsg::Object> ReaderWriter_vsg::read(std::istream& fin, vsg::ref_ptr<const vsg::Options> options) const
{
    auto [type, version] = readHeader(fin);
    if (type == BINARY)
    {
        vsg::BinaryInput input(fin, _objectFactory, options);
        input.version = version;
        return input.readObject("Root");
    }
    else if (type == ASCII)
    {
        vsg::AsciiInput input(fin, _objectFactory, options);
        input.version = version;
        return input.readObject("Root");
    }

    return vsg::ref_ptr<vsg::Object>();
}

bool ReaderWriter_vsg::write(const vsg::Object* object, const vsg::Path& filename, ref_ptr<const Options> options) const
{
    auto version = vsgGetVersion();

    if (options)
    {
        std::string version_string;
        if (options->getValue("version", version_string))
        {
            version = parseVersion(version_string);
        }
    }

    auto ext = vsg::fileExtension(filename);
    if (ext == "vsgb")
    {
        std::ofstream fout(filename, std::ios::out | std::ios::binary);
        writeHeader(fout, FormatInfo{BINARY, version});

        vsg::BinaryOutput output(fout, options);
        output.version = version;
        output.writeObject("Root", object);
        return true;
    }
    else if (ext == "vsga" || ext == "vsgt")
    {
        std::ofstream fout(filename);
        writeHeader(fout, FormatInfo{ASCII, version});

        vsg::AsciiOutput output(fout, options);
        output.version = version;
        output.writeObject("Root", object);
        return true;
    }
    else
    {
        return false;
    }
}

bool ReaderWriter_vsg::write(const vsg::Object* object, std::ostream& fout, ref_ptr<const Options> options) const
{
    auto version = vsgGetVersion();

    if (options)
    {
        std::string version_string;
        if (options->getValue("version", version_string))
        {
            version = parseVersion(version_string);
        }
    }

#if 0
    if (fout.openmode() & std::ios_base::openmode::binary)
    {
        std::cout<<"Binary outputstream"<<std::endl;
        writeHeader(fout, BINARY);

        vsg::BinaryOutput output(fout, options);
        output.version = version;
        output.writeObject("Root", object);
        return true;
    }
    else
#endif
    {
        std::cout << "Ascii outputstream" << std::endl;
        writeHeader(fout, FormatInfo(ASCII, version));

        vsg::AsciiOutput output(fout, options);
        output.version = version;
        output.writeObject("Root", object);
        return true;
    }
}
