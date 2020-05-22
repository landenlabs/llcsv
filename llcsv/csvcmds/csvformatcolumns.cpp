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

#include <regex>

#include "csvformatcolumns.h"
#include "csvtool.h"
using namespace CsvTool;

// static bool USE_CR_EOL  = false;
// static bool USE_KEEP_QUTOES = false;

// <column>:<width><formatType>(options)
//
// [count]:3d(),[name]:0s(quote),[price]:5.2f()
//
//  formatType:
//      d=decimal,  ex: d, 08d, 8d
//      x=hex
//      f=float,    ex: 3.2f, .2f
//      s=string,   ex: s, 10s, .10s 10.10s
//
//  options:
//      quote
//      comma           (with d or f)
//      lowercase, lc   (with s)
//      uppercase, uc   (with s)
//      capitalize cap  (with s)
//      trim            (with s)
//
//  ex:
//      [count]:d(comma),[name]:s(quote,trim,cap),[price]:.2f()
//
// #nextFile
// -nextFile
CsvFormatColumns::CsvFormatColumns(Order_t order) :
    CsvModify(order) {
    argREstr = "([#-][a-zA-Z]+|([0-9+]+|\\[[^]]+]):[0-9.]*[dsfx]\\(.*\\))";
    argRE = std::regex(argREstr, std::regex::extended);
}


// Format columns
//
// column:format(extra)...
//
// column = [name] or number, ex:[count] or 1
// format = %w.pX
//          Where X is s=string, f=float, d=decimal, x=hex
// extra  = optional actions, such as trim, quote, Capitalize, Lowercase, Uppercase
// Example:
//          [count]:3d(),[name]:10s(trim,cap,quote),[price]:5.2f()
bool CsvFormatColumns::init(CsvCmds& csvCmds, CsvError& csvError) {
    numFiles = 1;
    fileFields.resize(numFiles);
    std::vector<std::shared_ptr<FmtFields>>* pFields = &fileFields.back();
    FmtSpecs genSpec(4);
    
    const char* rxStr = "([#-][^,]+|([0-9+]+|\\[[^]]+]):([0-9.]*)([dsfx])(\\(.*\\)))";
    std::regex rx = std::regex(rxStr, std::regex::extended);
    std::smatch match;
    
    for (unsigned idx = 0; idx < args.size(); idx++) {
        const std::string& aa = args[idx];
        if (std::regex_match(aa, match, rx)) {
            // const std::string& bigName = match[1];
            size_t mlen = match.size();
            if (mlen >= 6 && match[5].length() > 0) {
               
                const std::string& colname = genSpec[0] = match[2];
                const std::string& widthStr = genSpec[1] = match[3];
                const std::string& fieldTyp = genSpec[2] = match[4];
                const std::string& value = genSpec[3] = match[5];
        
                // std::cerr << colname << widthStr << fieldTyp << value << std::endl;
                
                switch (fieldTyp[0]) {
                    case 'x':
                    case 'd':
                        pFields->push_back(std::make_shared<FmtDecField>(genSpec));
                        break;
                    case 'f':
                        pFields->push_back(std::make_shared<FmtFltField>(genSpec));
                        break;
                    case 's':
                        pFields->push_back(std::make_shared<FmtStrField>(genSpec));
                        break;
                }
            } else {
                // #nextfile or -nextflie
                numFiles++;
                fileFields.resize(numFiles);
                pFields = &fileFields.back();
            }
        }
    }
    
    //nextFileIdx = 0;  // TODO - support multiple files
    return fileFields.size() > 0;
}

bool CsvFormatColumns::action(CsvCmds& csvCmds, CsvInputs& inputs, CsvInputs*& pipe) {
    return true;
}

/*
const CsvTool::CsvHeaders& CsvGenerateColumns::genHeaders(size_t fileNum) {
    const RowFields& rowFields = fileFields[fileNum];
    if (rowData[fileNum].csvRow.getHeaders().empty()) {
        CsvTool::CsvHeaders headers(rowFields.size());
        for (unsigned colIdx = 0; colIdx < rowFields.size(); colIdx++) {
            headers[colIdx] = rowFields[colIdx]->name;
        }
        rowData[fileNum].csvRow.setHeaders(headers);
    }
    return rowData[fileNum].csvRow.getHeaders();
}

bool CsvGenerateColumns::nextFile(CsvError& csvError) {
    
    if (nextFileIdx < numFiles) {
        fileIdx = nextFileIdx++;
        file = "gen.csv";
        
        // TODO - isParallel keep all rows, else only keep one row
        // rowData.resize(nextFileIdx+1);
        return true;
    }
    
    return false;
}

bool CsvGenerateColumns::nextRow() {
    rowData[fileIdx].inRowCount++;
    fillRow(rowData[fileIdx]);
    // return !pSelRows->end(*pCsvCmds, *this);
    // return rowData[fileIdx].inRowCount <= maxRows;  // inRowCount 1..n
    return true;
}

void CsvGenerateColumns::fillRow(CsvRowData& rowData) {
    
    GenBuf genBuf;
    std::vector<std::shared_ptr<GenFields>>& fields = fileFields[fileIdx];
    size_t nlen = fields.size();
    rowData.csvRow.resize(nlen);
    
    for (unsigned idx = 0; idx < fields.size(); idx++) {
        // [count]:3d(1,1000,2),[name]:10s(hello),[price]:5.2f(-123.45,123.45)
        try {
            fields[idx]->genValue(rowData.csvRow[idx], genBuf, idx, (unsigned)rowData.inRowCount);
        } catch (std::exception ex) {
            std::cerr << ex.what() << std::endl;
        }
    }
}
*/
