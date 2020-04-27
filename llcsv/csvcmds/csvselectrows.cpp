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


#include "csvselectrows.h"
#include "csvcmds.h"


bool operator<(const RowRange& lhs, const RowRange& rhs)  {
    return lhs.from < rhs.from;
}

CsvSelectRows::CsvSelectRows(Order_t order, const char* defArg) : CsvSelect(order) {
    if (defArg != nullptr) {
        addArgs(defArg, CsvCmds::CSV_ERROR);
    }
}

bool CsvSelectRows::init(CsvCmds& csvCmds, CsvError& csvError) {
    // TODO - support #nextFile to have multiple row selection sets.
    
    // 1, 2-3, 4-
    minRow = -1;
    maxRow = 0;
    for (size_t idx = 0; idx < args.size(); idx++) {
        char* nptr = (char*)args[idx].c_str();
        while (isdigit(*nptr)) {
            size_t low = strtoul(nptr, &nptr, 10);
            size_t high = low;
            if (*nptr == '-') {
                if (nptr[1] == '\0') {
                    high = -1;
                } else {
                   high = strtoul(nptr+1, &nptr, 10);
                    high = std::max(low, high);
                }
            }
            if (*nptr == ',') {
                nptr++;
            }
            rows.insert(RowRange(low, high));
            minRow = std::min(minRow, low);
            maxRow = std::max(maxRow, high);
        }
    }
    
    // TODO - pass over rows and merge overlaps and join consecutive sequences.
    
    if (rows.empty()) {
        csvError.append("SelectRow has no valid parameters ");
    }
    return (rows.size() > 0);
}

// TODO - support #nextFile to have multiple row selection sets.
bool CsvSelectRows::selected(CsvCmds& csvCmds, const CsvInputs& inputs) const {
    size_t rowNum = inputs.getRowNum();
    
    // TODO - init iter and only advance as needed. 
    RowRanges::const_iterator iter;
    for (iter = rows.cbegin(); iter != rows.cend(); iter++) {
        if (iter->contains(rowNum))
            return true;
        if (iter->above(rowNum))
            break;
    }
    return false;
}
 

bool CsvSelectRows::end(CsvCmds& csvCmds, const CsvInputs& inputs) const {
    return  inputs.getRowNum() > maxRow;
}
