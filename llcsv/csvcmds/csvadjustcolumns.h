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
#include "csvcmds.h"


class AdjustAction {
protected:
    CsvCmds& csvCmds;
    std::string action;
    unsigned colNum;
    std::string val1;
    std::string val2;
    std::smatch matchInfo;
   
public:
    AdjustAction(
             CsvCmds& _csvCmds,
             const std::string& _action,
             const std::string& _val1,
             const std::string& _val2) :
        csvCmds(_csvCmds),
        action(_action),
        val1(_val1),
        val2(_val2)
    {
        remove(val1, "[]");
        colNum = (unsigned)strtoul(val1.c_str(), nullptr, 10);
    }
    
    virtual
    bool adjust(CsvRowData& rowData) = 0;
};

class CsvAdjustColumns : public CsvModify {
    
    // std::regex fromPat;
    typedef std::shared_ptr<AdjustAction> AdjItem;
    typedef std::vector<AdjItem> AdjustActions;
    AdjustActions adjustActions;
    
    bool init(CsvCmds& csvCmds, CsvError& cscvError) override;
    
public:
    std::string getName() const override { return "AdjustColumns"; }
    CsvAdjustColumns(Order_t order) :  CsvModify(order) {
        // append:[filename](filename)
        argREstr = "([^:]+:[^(]+[(][^)]+[)])";
        argRE = std::regex(argREstr, std::regex::extended);
    }
    
    bool action(CsvCmds& csvCmds, CsvInputs& inputs, CsvInputs*& pipe) override;
};
