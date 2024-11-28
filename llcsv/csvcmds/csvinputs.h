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

#include "csvfiles.h"
#include "csvtool.h"
#include "csvrowdata.h"


// TODO - make parallel (horz) and vertical input cmds.
class CsvInputs : public CsvFiles {
    
    virtual
    std::string getName() const = 0;
    virtual
    bool init(CsvCmds& csvCmds, CsvError& csvError) = 0;
   
    // typedef std::vector<CsvTool::CsvRow> CsvTable;
    // CsvTable csvTable;
    
public:
    std::vector<CsvRowData> rowData;
    bool parallel = false;  // Read all files in parallel
    size_t fileIdx = 0;
    std::string file;
    std::string MISSING_COLUMN;

public:
    CsvInputs(Order_t order) : CsvFiles(order) {
        actionType = IN;
    }
    
    // Read all files in parallel (join or append columns)
    void setParallel(bool _parallel, Order_t _order) {
        parallel = _parallel;
    }
    
    const CsvRowData& getRowData() const  {
         rowData[fileIdx].pInputs = this;
        return rowData[fileIdx];
    }
    CsvRowData& getRowData()  {
        rowData[fileIdx].pInputs = this;
        return rowData[fileIdx];
    }
    size_t getRowNum() const {
        return rowData[fileIdx].inRowCount;
    }
    void setRowNum(size_t rowNum)  {
        rowData[fileIdx].inRowCount = rowNum;
    }
    
    // -- Parallel
    size_t getParallelCnt() const {
        return rowData.size();
    }
    CsvRowData& getParallelData(size_t _fileIdx)  {
         rowData[_fileIdx].pInputs = this;
        return rowData[_fileIdx];
    }
    
    virtual
    bool nextFile(CsvError& csvError) = 0;
    virtual
    bool nextRow() = 0;
};

