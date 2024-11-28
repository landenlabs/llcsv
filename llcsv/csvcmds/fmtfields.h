//-------------------------------------------------------------------------------------------------
//
//  llcsv      4/20/2020       Dennis Lang
//
//  CSV command line tool
//
//-------------------------------------------------------------------------------------------------
//
// Author: Dennis Lang - 2020
// https://landenlabs.com/
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


#pragma once

#include <algorithm>
#include "strutils.h"
#include "split.h"

typedef  std::vector<char>  FmtBuf;
typedef  std::vector<std::string> FmtSpecs; // 0=name,1=width, 2=fmt,3=value

struct FmtField {
    std::string colName;
    int colNum = CsvRowData::NO_COL_NUM;
    int width;
    std::string widthStr;
    std::string fmt;
   
    
    FmtField(const FmtSpecs& fmtSpec) {
        colName = fmtSpec[0];
        StrUtils::remove(colName, "[]");
        colNum = atoi(colName.c_str());
        widthStr = fmtSpec[1];
        width = atoi(widthStr.c_str());
        fmt = "%" + widthStr + fmtSpec[2];
    }
    
    virtual
    std::string fmtValue(std::string& outVal, FmtBuf& buf, const std::string& inVal) = 0;
};
struct FmtDecField : public FmtField {
    FmtDecField(const FmtSpecs& fmtSpec) : FmtField(fmtSpec) {
        if (width == 0)
            width = 10;
    }
    std::string fmtValue(std::string& outVal, FmtBuf& buf, const std::string& inVal) override {
        buf.resize(std::max(buf.size(), (size_t)width+1));
        char* nptr;
        long val = strtol(inVal.c_str(), &nptr, 10);
        snprintf(buf.data(), width+1, fmt.c_str(), val);
        outVal.assign(buf.data());
        return outVal;
    }
};

struct FmtFltField : public FmtField {
 
    FmtFltField(const FmtSpecs& fmtSpec) : FmtField(fmtSpec) {
        if (width == 0)
            width = 10;
    }
    std::string fmtValue(std::string& outVal, FmtBuf& buf, const std::string& inVal) override {
        buf.resize(std::max(buf.size(), (size_t)width+1));
        char* nptr;
        double val = strtod(inVal.c_str(), &nptr);
        snprintf(buf.data(), width+1, fmt.c_str(), val);
        outVal.assign(buf.data());
        return outVal;
    }
};
 

struct FmtStrField : public FmtField {
    lstring val;
    lstring fmt;
    bool haveExtra = false;
    bool trim = false;
    bool quote = false;
    char caseChg = 'n';
    const char* QUOTE = "\"";
    
    FmtStrField(const FmtSpecs& fmtSpec) : FmtField(fmtSpec) {
        val = fmtSpec[3];
        StrUtils::remove(val, "()");
        Split parts(val, ",");
        
        for (lstring part : parts) {
            if (part.find("%") != lstring::npos) {
                fmt = part;
            } else {
                switch (tolower(part[0])) {
                    case 't':   // trim
                        trim = strncasecmp("trim", part, part.length());
                        break;
                    case 'q':   // quote
                        quote = strncasecmp("quote", part, part.length());
                        break;
                    case 'c':   // capitialize
                        caseChg = strncasecmp("capitialize", part, part.length()) ? 'c':'n';
                        break;
                    case 'u':   // uppercase
                        caseChg = strncasecmp("uppercase", part, part.length()) ? 'u':'n';
                        break;
                    case 'l':   // lowercase
                        caseChg = strncasecmp("lowercase", part, part.length()) ? 'l':'n';
                        break;
                    default:
                        std::cerr << "Unknown format extra="<< part << std::endl;
                        break;
                }
            }

            haveExtra = true;
        }
    }
    

    std::string fmtValue(std::string& outVal, FmtBuf& buf, const std::string& inVal) override {
        if (haveExtra) {
            lstring tmpVal = outVal;
            if (trim) {
                tmpVal.trim();
            }
            if (caseChg != 'n') {
                switch (caseChg) {
                    case 'c':
                        StrUtils::toCapitalize(tmpVal);
                        break;
                    case 'u':
                        StrUtils::toUpper(tmpVal);
                        break;
                    case 'l':
                        StrUtils::toLower(tmpVal);
                        break;
                }
            }
            if (fmt.length() > 0) {
                size_t bufLen = (width > 0) ? width : (fmt.length() + tmpVal.length());
                buf.resize(std::max(buf.size(), bufLen+1));
                snprintf(buf.data(), buf.size(), fmt, tmpVal.c_str());
                tmpVal = buf.data();
            }
            if (width != 0) {
                tmpVal.resize(width, ' ' );
            }
            if (quote) {
                tmpVal.insert(0, QUOTE);
                tmpVal.append(QUOTE);
            }
            outVal = tmpVal;
        }
        return outVal;
    }
};

