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


#include "csvjoincolumns.h"
#include "csvcmds.h"

CsvJoinColumns::CsvJoinColumns(Order_t order) : CsvSelectColunns(order) {
}

bool CsvJoinColumns::init(CsvCmds& csvCmds, CsvError& csvError)  {
    return CsvSelectColunns::init(csvCmds, csvError);
}

bool CsvJoinColumns::action(CsvCmds& csvCmds, CsvInputs& inputs, CsvInputs*& pipe) {
    //  outPipe.nextFile(CsvCmds::CSV_ERROR);
    CsvTool::CsvRow& outRow = outPipe.getRowData().csvRow;
    outRow.clear();
    outRow.getHeaders().clear();
    for (unsigned fileIdx = 0; fileIdx < inputs.getParallelCnt(); fileIdx++) {
        appendColumns(outRow.getHeaders(), inputs.getParallelData(fileIdx).csvRow.getHeaders());
        appendColumns(outRow, inputs.getParallelData(fileIdx).csvRow);
        outPipe.setRowNum(inputs.getRowNum());
        pipe = &outPipe;
    }
    return true;
}

bool CsvJoinColumns::appendColumns(CsvTool::CsvCells& outCells, const CsvTool::CsvCells& inCells) {
   outCells.insert(outCells.end(), inCells.begin(), inCells.end());
    
    /*
    for (iter = colRanges.cbegin(); iter != colRanges.cend(); iter++) {
        const ColRange& colRange = *iter;
        for (unsigned colIdx = colRange.from; colIdx <= colRange.to; colIdx++) {
            outCells.push_back(inCells[colIdx]);
        }
    }
     */
    return true;
}

