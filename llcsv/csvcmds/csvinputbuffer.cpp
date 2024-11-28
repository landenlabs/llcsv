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

#include <regex>

#include "csvinputbuffer.h"
#include "csvcmds.h"
#include "csvselectrows.h"
#include "csvtool.h"
using namespace CsvTool;

// static bool USE_CR_EOL  = false;
// static bool USE_KEEP_QUTOES = false;


bool CsvInputBuffer::init(CsvCmds& csvCmds, CsvError& csvError) {
    pCsvCmds = &csvCmds;
    nextFileIdx = fileIdx = 0;
    numFiles = 1;
    nextFileIdx = 0;
    return fileIdx < numFiles; // TODO - support multiple files
}


bool CsvInputBuffer::nextFile(CsvError& csvError) {
    
    if (nextFileIdx < numFiles) {
        fileIdx = nextFileIdx++;
        file = "gen.csv";
        
        // TODO - isParallel keep all rows, else only keep one row
        // rowData.resize(nextFileIdx+1);
        return true;
    }
    
    return false;
}

bool CsvInputBuffer::nextRow() {
    rowData[fileIdx].inRowCount++;
    fillRow(rowData[fileIdx]);
    return true;
}

void CsvInputBuffer::fillRow(CsvRowData& rowData) {

}
