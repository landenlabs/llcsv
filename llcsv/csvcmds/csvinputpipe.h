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

#include "csvinputs.h"
#include "csvtool.h"
#include "csvrowdata.h"
#include "fileutils.h"

typedef FileUtils<size_t> FileUtils_t;

class CsvInputPipe : public CsvInputs {
    
    std::string getName() const override { return "InputPipe"; }
    
public:
    size_t nextFileIdx = 0;
    
public:
    CsvInputPipe(Order_t order);
    bool init(CsvCmds& csvCmds, CsvError& csvError) override;
  
    bool nextFile(CsvError& csvError) override;
    bool nextRow() override;
};

