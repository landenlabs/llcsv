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
#include "csvtool.h"
#include "fmtfields.h"

// Format columns
//
// column:format(extra)...
//
// column = [name] or number, ex:[count] or 1
// format = %w.pX
//          Where X is s=string, f=float, d=decimal, x=hex
// Example:
//          [count]:3d(1),[name]:10s(1),[price]:5.2f(1)
class CsvFormatColumns : public CsvModify {
    
    bool init(CsvCmds& csvCmds, CsvError& csvError) override;
    size_t numFiles = 1;
   
    typedef  std::shared_ptr<FmtField> ColItem;
    typedef  std::vector<ColItem> RowFields;
    std::vector<RowFields> fileFields;
 
public:
    std::string getName() const override { return "FormatColumns"; }
    
    CsvFormatColumns(Order_t order);
    
    virtual
    bool action(CsvCmds& csvCmds, CsvInputs& inputs, CsvInputs*& pipe)  override;

};

