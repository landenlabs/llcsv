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

#pragma once

#include "csvmodify.h"
#include "strutils.h"

class ReplaceField {
    std::string name;
    unsigned colNum;
    std::regex fromPat;
    std::string toText;
    std::smatch matchInfo;
   
public:
    ReplaceField(const std::string& _name, const std::string& _fromPat, const std::string& _toText) {
        name = _name;
        StrUtils::remove(name, "[]");
        colNum = (unsigned)strtoul(name.c_str(), nullptr, 10);
        fromPat = std::regex(_fromPat);
        toText = _toText;
    }
    

    bool replace(CsvRowData& rowData)  {
        std::string& str = rowData.getColumn(name, colNum);
        bool isMatch = std::regex_search(str, matchInfo, fromPat);
        // bool isMatch = std::regex_match(str, pat);
        if (isMatch) {
            size_t pos = matchInfo.position();
            size_t len = matchInfo[0].length();
            str.replace(pos, len, toText.c_str());
        }
        return isMatch;
    }
};

class CsvReplaceColumns : public CsvModify {
    
    // std::regex fromPat;
    typedef std::shared_ptr<ReplaceField> RepItem;
    typedef std::vector<RepItem> ReplaceFields;
    ReplaceFields replaceFields;
    
    
    bool init(CsvCmds& csvCmds, CsvError& cscvError) override;
    
public:
    std::string getName() const override { return "ReplaceColumns"; }
    CsvReplaceColumns(Order_t order) :  CsvModify(order) {
        // 2/old/new/  [date]/2020/2021/
        argREstr = "([0-9]+|\\[[^]]+])/[^/]+/[^/]*/";
        argRE = std::regex(argREstr, std::regex::extended);
    }
    
    bool action(CsvCmds& csvCmds,  CsvInputs& inputs, CsvInputs*& pipe) override;
};
