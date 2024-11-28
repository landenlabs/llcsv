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

typedef  std::vector<char>  GenBuf;
typedef  std::vector<std::string> GenSpecs; // 0=name,1=width, 2=fmt,3=value

struct GenFields {
    std::string name;
    int width;
    std::string widthStr;
    std::string fmt;
    GenFields(const GenSpecs& genSpec) {
        name = genSpec[0];
        StrUtils::remove(name, "[]");
        widthStr = genSpec[1];
        width = atoi(widthStr.c_str());
        fmt = "%" + widthStr + genSpec[2];
    }
    
    virtual
    std::string genValue(std::string& dst, GenBuf& buf, unsigned col, unsigned row) = 0;
};
struct GenDecField : public GenFields {
    long fromVal;
    long toVal;
    long stepVal = 1;
    long val;

    GenDecField(const GenSpecs& genSpec) : GenFields(genSpec) {
        std::string valStr = genSpec[3];
        StrUtils::remove(valStr, "()");
        char* nptr;
        toVal = fromVal = strtol(valStr.c_str(), &nptr, 10);
        if (*nptr == ',')
            toVal = strtol(nptr+1, &nptr, 10);
        else
            stepVal = 0;
        if (*nptr == ',') stepVal = strtol(nptr+1, &nptr, 10);
        val = fromVal;
    }
    std::string genValue(std::string& dst, GenBuf& buf, unsigned col, unsigned row) override {
        buf.resize(std::max(buf.size(), (size_t)width+1));
        snprintf(buf.data(), width+1, fmt.c_str(), (int)val);
        val += stepVal;
        if (val == toVal) {
            val = fromVal;
        }
        dst.assign(buf.data());
        return dst;
    }
};

struct GenRowField : public GenDecField {
     GenRowField(const GenSpecs& genSpec) : GenDecField(genSpec) {
         fmt = "%" + genSpec[1] + "d";
     }
    std::string genValue(std::string& dst, GenBuf& buf, unsigned col, unsigned row) override {
        buf.resize(std::max(buf.size(), (size_t)width+1));
        snprintf(buf.data(), width+1, fmt.c_str(), (int)row);
        dst.assign(buf.data());
        return dst;
    }
};

struct GenFltField : public GenFields {
    double fromVal;
    double toVal;
    double stepVal = 1;
    double val;
 
    GenFltField(const GenSpecs& genSpec) : GenFields(genSpec) {
        std::string valStr = genSpec[3];
        StrUtils::remove(valStr, "()");
        char* nptr;
        toVal = fromVal = strtod(valStr.c_str(), &nptr);
        if (*nptr == ',')
            toVal = strtod(nptr+1, &nptr);
        else
            stepVal = 0;
        if (*nptr == ',') stepVal = strtod(nptr+1, &nptr);
        val = fromVal;
    }
    std::string genValue(std::string& dst, GenBuf& buf, unsigned col, unsigned row) override {
        buf.resize(std::max(buf.size(), (size_t)width+1));
        snprintf(buf.data(), width+1, fmt.c_str(), val);
        val += stepVal;
        if (val == toVal) {
            val = fromVal;
        }
        dst.assign(buf.data());
        return dst;
    }
};
 

struct GenStrField : public GenFields {
    std::string val;
    GenStrField(const GenSpecs& genSpec) : GenFields(genSpec) {
        val = genSpec[3];
        StrUtils::remove(val, "()");
        if (val.find(",") != std::string::npos) {
            val.resize(val.find(","));
        }
    }
    std::string genValue(std::string& dst, GenBuf& buf, unsigned col, unsigned row) override {
        dst.assign(val);
        for (int idx = (int)val.length()-1; idx > 0; idx--) {
            char c = ++val[idx];
            if (c < 'z') {
                break;
            }
            val[idx] = '0';
        }
        return dst;
    }
};

