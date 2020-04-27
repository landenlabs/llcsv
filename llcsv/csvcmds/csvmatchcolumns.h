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

#include "csvselect.h"
#include "matfields.h"

class CsvMatchColumns : public CsvSelect {
    
    typedef std::shared_ptr<MatchField> MatItem;
    typedef std::vector<MatItem> MatchFields;
    MatchFields matchFields;
    
    bool init(CsvCmds& csvCmds, CsvError& cscvError) override;
    
public:
    std::string getName() const  override { return "MatchColumns" ;}
    CsvMatchColumns(Order_t order) : CsvSelect(order) {
        // 2~"text"i, 2=/regex/, [hello]ge[1.0]f
        // i=ignorecase, f=float, d=decimal, x=hexDecimal
        argREstr = "([0-9]+|\\[[^]]+])([=<>!~]|eq|ne|gt|ge|lt|le)([0-9.-]+|['\"/].*['\"/])[ifdx]*";
        argRE = std::regex(argREstr, std::regex::extended);
    }
    
    bool selected(CsvCmds& csvCmds, const CsvInputs& inputs) const override;
    
    // Past end of selection range
    bool end(CsvCmds& csvCmds, const CsvInputs& inputs) const override;
};
