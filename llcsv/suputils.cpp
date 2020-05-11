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


typedef unsigned int uint;

#if 0
// Runtime options
std::regex fromPat;
lstring toPat;
lstring backupDir;


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

void opendir() {
            char cwdTmp[256];
            cwd = getcwd(cwdTmp, sizeof(cwdTmp));
            cwd += Directory_files::SLASH;
            
            if (!toPat.empty() && pFilter == &lineFilter) {
                cerr << "\a\nRange filter does not work for replacement only searching\a\n" << std::endl;
            }
            
            if (fileDirList.size() == 1 && fileDirList[0] == "-") {
                string filePath;
                while (std::getline(std::cin, filePath)) {
                    std::cerr << ScanFiles(<#const lstring &fullname#>)(filePath) << std::endl;
                }
            } else {
                for (auto const& filePath : fileDirList)
                {
                    std::cerr << ScanFiles(filePath) << std::endl;
                }
            }
}
#endif


