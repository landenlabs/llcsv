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

typedef  std::vector<std::string> MatchSpecs; // 0=name,1=operator, 2=type, 3=pattern or value

class MatchField {
protected:
    std::string name;
    std::string op;
    std::string typ;
    std::string val;
    std::string opt;
    unsigned colNum;
    
public:
    MatchField(const MatchSpecs& matSpec) {
        name = matSpec[0];
        StrUtils::remove(name, "[]");
        colNum = (unsigned)strtoul(name.c_str(), nullptr, 10);
        op = matSpec[1];
        typ = matSpec[2];
        val = matSpec[3];
        opt = matSpec[4];
    }
    
    virtual
    bool match(const CsvRowData& rowData) = 0;
};

class MatTxtField : public MatchField {
    bool (*matchPtr)(const std::string& str, const std::string& val);
    bool invert;
    
public:
    MatTxtField(const MatchSpecs& matSpec) : MatchField(matSpec) {
        unsigned tidx = 0;
        bool invert = (typ[tidx] == '!');
        if (invert && typ.length() > 1)
            tidx++;
          
        if (typ.length() > 0)
        switch (typ[tidx]) {
          case '~':
              matchPtr = (opt=="i") ? &StrUtils::containsNoCase : &StrUtils::containsCase;
              break;
          case '=':
              matchPtr = (opt=="i") ? &StrUtils::equalsNoCase : &StrUtils::equalsCase;
              break;
          }
    }
    
    // 2~"text"i   or  [name]="hello"  or [date]!~"2020"
    bool match(const CsvRowData& rowData) override {
        const std::string& str = rowData.getColumn(name, colNum);
        bool isMatch = (*matchPtr)(str, val);
        return invert ? !isMatch : isMatch;
    }
};
class MatRegField : public MatchField {
    std::regex pat;
    bool invert;
    std::smatch matchInfo;
    
public:
    MatRegField(const MatchSpecs& matSpec) : MatchField(matSpec), pat(val) {
        unsigned tidx = 0;
        bool invert = (typ[tidx] == '!');
        if (invert && typ.length() > 1)
            tidx++;
        pat = std::regex(val);
    }
    // TODO - currently only support =
    // 2=/regex/  [name]!=/(tom|bill|sue)/
    bool match(const CsvRowData& rowData) override {
        const std::string& str = rowData.getColumn(name, colNum);
        bool isMatch = std::regex_search (str, matchInfo, pat);
        // bool isMatch = std::regex_match(str, pat);
        return invert ? !isMatch : isMatch;
    }
};

template<typename TT>
class MatNumField : public MatchField {
    bool (*matchPtr)(TT srcVal, TT matVal);
    TT (*cnvPtr)(const std::string& src);
    TT matVal;
    
public:
    MatNumField(const MatchSpecs& matSpec, TT (*cnv)(const std::string& src)) : MatchField(matSpec) {
        cnvPtr = cnv;
        matVal = (*cnvPtr)(val);
        
        //  =<>!~
        // eq|ne|gt|ge|lt|le
        if (op.length() > 0)
        switch (op[0]) {
             case '!':
             case 'n':   // ne
                matchPtr = &StrUtils::notEqualNum<TT>;
                break;
            case '=':
            case 'e':   // eq
                matchPtr = &StrUtils::equalNum<TT>;
                break;
            case 'l':   // maybe lt
                if (op[1] == 'e') {
                    matchPtr = &StrUtils::lessEqualNum<TT>;
                    break;
                }
            case '<':
                matchPtr = &StrUtils::lessNum<TT>;
                break;
            case 'g':
                  if (op[1] == 'e') {
                      matchPtr = &StrUtils::greaterEqualNum<TT>;
                      break;
                  }
            case '>':
                matchPtr = &StrUtils::greaterNum<TT>;
                break;
        }
    }
    
    // [hello]ge[1.0]f
    bool match(const CsvRowData& rowData) override {
        const std::string& str = rowData.getColumn(name, colNum);
        TT srcVal = (*cnvPtr)(str);
        bool isMatch = (*matchPtr)(srcVal, matVal);
        return isMatch;
    }
};

