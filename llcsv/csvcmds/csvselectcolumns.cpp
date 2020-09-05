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


#include "csvselectcolumns.h"
#include "csvcmds.h"
#include "strutils.h"

CsvSelectColunns::CsvSelectColunns(Order_t order) : CsvSelect(order), outPipe(0) {
}

bool CsvSelectColunns::init(CsvCmds& csvCmds, CsvError& csvError) {
    // 1,2-,-3,[hello],[a]-[b]
    //  argREstr = "(([0-9+]+|\\[[^]]+])[,-]{0,1})+";
    minCol = -1;
    maxCol = 0;
    
    ArgList::iterator iter = args.begin();
    while (iter != args.end()) {
        bool validDigit = false;
        char* nptr = (char*)(*iter).c_str();
        while (isdigit(*nptr)) {
            ColIdx_t low = strtoul(nptr, &nptr, 10) -1;   // make zero based
            ColIdx_t high = low;
            if (*nptr == '-') {
                if (nptr[1] == '\0') {
                    high = -1;
                } else {
                    high = strtoul(nptr+1, &nptr, 10) -1;   // make zero based
                    high = std::max(low, high);
                }
            }
            if (*nptr == ',') {
                nptr++;
            }
            
            ColRange colRange(low, high);
            colRanges.push_back(colRange);
            minCol = std::min(minCol, low);
            maxCol = std::max(maxCol, high);
            validDigit = true;
        }
       
        if (validDigit)
            args.erase(iter);
        else
            iter++;
    }
    
    outPipe.init(csvCmds, csvError);
    return true;
}

static std::regex skipPat("^-");

// [colname] or [-skipname]
bool CsvSelectColunns::initNames(CsvCmds& csvCmds) {
    bool okay = true;
    std::string missing;
    if (args.size() > 0) {
        okay = false;
        // CsvInputs* pInputs = csvCmds.findCmd<CsvInputs>(CsvCmd::ActionType::IN, FindFilter(order));
        if (pRowData != nullptr) {
            ColIdx_t colNum = -1;
            do {
            ArgList::iterator iter = args.begin();
            while (iter != args.end()) {
               std::string colName(*iter);
                StrUtils::remove(colName, "[]");  // TODO - support partial match
                bool skip = StrUtils::remove(colName, skipPat);
                colNum = indexOf(pRowData->csvRow.getHeaders(), colName, colNum+1);
               if (colNum != NPOS || skip) {
                   // do {
                       okay = true;
                       ColRange colRange(colNum);
                       colRanges.push_back(colRange);
                       minCol = std::min(minCol, colNum);
                       maxCol = std::max(maxCol, colNum);
                   // } while ((colNum = indexOf(pRowData->csvRow.getHeaders(), colName, colNum+1)) != NPOS);
               } else {
                   missing += *iter + " ";
                   break;
               }
                iter++;
           }
            } while (colNum != NPOS);
       }
        args.clear();
    }
    
    okay |= colRanges.size() > 0;
    if (!okay) {
        if (missing.empty()) {
            CsvCmds::CSV_ERROR.append(getName() + " No valid column selection defined");
        } else {
            CsvCmds::CSV_ERROR.append(getName() + " Failed to find columns:" + missing);
        }
    }
    return okay;
}
    
bool CsvSelectColunns::action(CsvCmds& csvCmds,  CsvInputs& inputs, CsvInputs*& pipe)  {
  
    bool okay = true;
    const CsvRowData* pNextRowData = &inputs.getRowData();
    if (pNextRowData != pRowData) {
        pRowData = pNextRowData;
        okay = initNames(csvCmds);
        outPipe.nextFile(CsvCmds::CSV_ERROR);
        copySelectColumns(outPipe.getRowData().csvRow.getHeaders(), pRowData->csvRow.getHeaders());
    }

    copySelectColumns(outPipe.getRowData().csvRow, pRowData->csvRow);
    outPipe.setRowNum(inputs.getRowNum());
    pipe = &outPipe;
    return okay;
}

bool CsvSelectColunns::copySelectColumns(CsvTool::CsvCells& outCells, const CsvTool::CsvCells& inCells) {
    // const CsvTool::CsvRow& inRow = pRowData->csvRow;

    ColRanges::const_iterator iter;
    outCells.clear();
    for (iter = colRanges.cbegin(); iter != colRanges.cend(); iter++) {
        const ColRange& colRange = *iter;
        for (ColIdx_t colIdx = colRange.from; colIdx <= colRange.to; colIdx++) {
            outCells.push_back(inCells.at(colIdx, EMPTY));
        }
    }

    return true;
}


bool CsvSelectColunns::end(CsvCmds& csvCmds, const CsvInputs& inputs, CsvInputs*& pipe) const {
    return false;
}

