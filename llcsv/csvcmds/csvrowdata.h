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

#include <string>
#include <algorithm>    // std::max
#include "utils.h"

// Forward
class CsvInputs;

struct CsvRowData {
    CsvTool::CsvRow csvRow;
    size_t inRowCount;
    static const unsigned NO_COL_NUM = 0;
    
    mutable
    const CsvInputs* pInputs;  
    
    const std::string& getColumn(const std::string& name, unsigned colNum) const {
        return (colNum > NO_COL_NUM) ? csvRow.at(colNum-1) : csvRow.getColumn(name);
    }
    
    std::string& getColumn(const std::string& name, unsigned colNum)  {
        return (colNum > NO_COL_NUM) ? csvRow.at(colNum-1) : csvRow.getColumn(name);
    }
    
    void appendCol(const std::string& colName, size_t colNum, const char* val) {
        if (colName.length() != 0) {
            size_t pos = indexOf(csvRow.getHeaders(), colName);
            if (pos == NPOS) {
                csvRow.getHeaders().push_back(colName);
                colNum = csvRow.getHeaders().size()-1;
            } else {
                colNum = pos;
            }
        }
        csvRow.resize(std::max(csvRow.size(), colNum+1));
        csvRow[colNum] = val;
    }
};
