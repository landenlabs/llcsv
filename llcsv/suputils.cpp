//-------------------------------------------------------------------------------------------------
//
//  llcsv      4/20/2020       Dennis Lang
//
//  CSV command line tool
//
//-------------------------------------------------------------------------------------------------
//
// Author: Dennis Lang - 2020
// http://landenlabs.com/
//
// This file is part of llcsv project.
//
// ----- License ----
//
// Copyright (c) 2020 Dennis Lang
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// 4291 - No matching operator delete found
#pragma warning( disable : 4291 )

#include <stdio.h>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdlib.h>     // stroul
#include <ctype.h>      // islower, isupperre

#include "util/ll_stdhdr.h"

#include "util/directory.h"
#include "util/split.h"
#include "util/filters.h"



#include <vector>
#include <map>
#include <algorithm>
#include <regex>
#include <exception>
#include <typeinfo>
using namespace std;


// Helper types
typedef std::vector<lstring> StringList;
typedef std::vector<std::regex> PatternList;
typedef unsigned int uint;

#if 0
// Runtime options
std::regex fromPat;
lstring toPat;
lstring backupDir;
PatternList includeFilePatList;
PatternList excludeFilePatList;
StringList fileDirList;

bool showPattern = false;
bool inverseMatch = false;
uint optionErrCnt = 0;
uint patternErrCnt = 0;

lstring printPosFmt = "%f(%o)\n";
lstring cwd;    // current working directory

// Working values
bool doReplace = false;
std::vector<char> buffer;

Filter nopFilter;
LineFilter lineFilter;
Filter* pFilter = &nopFilter;


// ---------------------------------------------------------------------------
// Dump String, showing non-printable has hex value.
static void DumpStr(const char* label, const std::string& str) {
    if (showPattern)
    {
        std::cerr << "Pattern " << label << " length=" << str.length() << std::endl;
        for (int idx = 0; idx < str.length(); idx++)
        {
            std::cerr << "  [" << idx << "]";
            if (isprint(str[idx]))
                std::cerr << str[idx] << std::endl;
            else
                std::cerr << "(hex) " << std::hex << (unsigned)str[idx] << std::dec << std::endl;
        }
        std::cerr << "[end-of-pattern]\n";
    }
}

// ---------------------------------------------------------------------------
// Convert special characters from text to binary.
static std::string& ConvertSpecialChar(std::string& inOut)
{
    uint len = 0;
    int x, n;
    const char *inPtr = inOut.c_str();
    char* outPtr = (char*)inPtr;
    while (*inPtr)
    {
        if (*inPtr == '\\')
        {
            inPtr++;
            switch (*inPtr)
            {
                case 'n': *outPtr++ = '\n'; break;
                case 't': *outPtr++ = '\t'; break;
                case 'v': *outPtr++ = '\v'; break;
                case 'b': *outPtr++ = '\b'; break;
                case 'r': *outPtr++ = '\r'; break;
                case 'f': *outPtr++ = '\f'; break;
                case 'a': *outPtr++ = '\a'; break;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                    sscanf(inPtr,"%3o%n",&x,&n);
                    inPtr += n-1;
                    *outPtr++ = (char)x;
                    break;
                case 'x':                                // hexadecimal
                    sscanf(inPtr+1,"%2x%n",&x,&n);
                    if (n>0)
                    {
                        inPtr += n;
                        *outPtr++ = (char)x;
                        break;
                    }
                    // seep through
                default:
                    throw( "Warning: unrecognized escape sequence" );
                case '\\':
                case '\?':
                case '\'':
                case '\"':
                    *outPtr++ = *inPtr;
                    break;
            }
            inPtr++;
        }
        else
            *outPtr++ = *inPtr++;
        len++;
    }
    
    inOut.resize(len);
    return inOut;;
}

#ifdef WIN32
const char SLASH_CHAR('\\');
#else
const char SLASH_CHAR('/');
#endif

// ---------------------------------------------------------------------------
// Extract name part from path.
lstring& getName(lstring& outName, const lstring& inPath)
{
    size_t nameStart = inPath.rfind(SLASH_CHAR) + 1;
    if (nameStart == 0)
        outName = inPath;
    else
        outName = inPath.substr(nameStart);
    return outName;
}

// ---------------------------------------------------------------------------
// Return true if inPath (filename part) matches pattern in patternList
bool FileMatches(const lstring& inName, const PatternList& patternList, bool emptyResult)
{
    if (patternList.empty() || inName.empty())
        return emptyResult;
    
    for (size_t idx = 0; idx != patternList.size(); idx++)
        if (std::regex_match(inName.begin(), inName.end(), patternList[idx]))
            return true;
    
    return false;
}

// ---------------------------------------------------------------------------
void printParts(
        const char* customFmt,
        const char* filepath,
        size_t fileOffset,
        size_t matchLen,
        const lstring& matchStr)
{
    // TODO - handle custom printf syntax to get to path parts:
    //    %#.#s    s=fullpath,  p=path only, n=name only, e=extension only f=filename name+ext
    //    %0#d     o=offset,  l=length
    // printf(filepath, fileOffset, len, filepath);
    
    const int NONE = 12345;
    lstring itemFmt;
    
    char* fmt = (char*)customFmt;
    while (*fmt) {
        char c = *fmt;
        if (c != '%') {
            putchar(c);
            fmt++;
        } else {
            const char* begFmt = fmt;
            int precision = NONE;
            // int width = (int)strtol(fmt+1, &fmt, 10);
            if (*fmt == '.') {
                precision = (int)strtol(fmt+1, &fmt, 10);
            }
            c = *fmt;
        
            itemFmt = begFmt;
            itemFmt.resize(fmt - begFmt);
           
            switch (c) {
                case 's':
                    itemFmt += "s";
                    printf(itemFmt, filepath);
                    break;
                case 'p':
                   itemFmt += "s";
                   printf(itemFmt, Directory_files::parts(filepath, true, false, false).c_str());
                   break;
                case 'r':   // relative path
                    itemFmt += "s";
                    printf(itemFmt, Directory_files::parts(filepath, true, false, false).replaceStr(cwd, "").c_str());
                    break;
                case 'n':
                    itemFmt += "s";
                    printf(itemFmt, Directory_files::parts(filepath, false, true, false).c_str());
                    break;
                case 'e':
                    itemFmt += "s";
                    printf(itemFmt, Directory_files::parts(filepath, false, false, true).c_str());
                    break;
                case 'f':
                    itemFmt += "s";
                    printf(itemFmt, Directory_files::parts(filepath, false, true, true).c_str());
                    break;
                case 'o':
                    itemFmt += "lu";    // unsigned long formatter
                    printf(itemFmt, fileOffset);
                    break;
                case 'l':
                    itemFmt += "lu";    // unsigned long formatter
                    printf(itemFmt, matchLen);
                    break;
                case 'm':
                    itemFmt += "s";
                    printf(itemFmt, matchStr.c_str());
                    break;
                default:
                    putchar(c);
                    break;
            }
            fmt++;
        }
    }
}


// ---------------------------------------------------------------------------
static size_t ActOnFile(const lstring& fullname)
{
    size_t fileCount = 0;
    lstring name;
    getName(name, fullname);
    
    if (!name.empty()
        && !FileMatches(name, excludeFilePatList, false)
        && FileMatches(name, includeFilePatList, true))
    {
        if (doReplace)
        {
#if 0
            if (ReplaceFile(fullname, name))
            {
                fileCount++;
                printParts(printPosFmt, fullname, 0, 0, "");
            }
#endif
        }
        else
        {
#if 0
            unsigned matchCnt = FindGrep(fullname);
            if (matchCnt != 0)
            {
                fileCount++;
                printParts(printPosFmt, fullname, 0, 0, "");
            }
#endif
        }
    }
    
    return fileCount;
}

 
// ---------------------------------------------------------------------------
static size_t ActOnFiles(const lstring& dirname)
{
    Directory_files directory(dirname);
    lstring fullname;
    
    size_t fileCount = 0;
    
    struct stat filestat;
    try {
        if (stat(dirname, &filestat) == 0 && S_ISREG(filestat.st_mode))
        {
            fileCount += ActOnFile(dirname);
        }
    }
    catch (exception ex)
    {
        // Probably a pattern, let directory scan do its magic.
    }

    while (directory.more())
    {
        directory.fullName(fullname);
        if (directory.is_directory())
        {
            fileCount += ActOnFile(fullname);
        }
        else if (fullname.length() > 0)
        {
            fileCount += ActOnFile(fullname);
        }
    }
    
    return fileCount;
}


// ---------------------------------------------------------------------------
// Return compiled regular expression from text.
std::regex getRegEx(const char* value)
{
    try {
        std::string valueStr(value);
        ConvertSpecialChar(valueStr);
        DumpStr("From", valueStr);
        return std::regex(valueStr);
        // return std::regex(valueStr, regex_constants::icase);
    }
    catch (const std::regex_error& regEx)
    {
        std::cerr << regEx.what() << ", Pattern=" << value << std::endl;
    }
    
    patternErrCnt++;
    return std::regex("");
}
#endif


