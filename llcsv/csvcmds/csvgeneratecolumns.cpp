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

#include "csvgeneratecolumns.h"
#include "csvcmds.h"
#include "csvselectrows.h"
#include "csvtool.h"
using namespace CsvTool;

// static bool USE_CR_EOL  = false;
// static bool USE_KEEP_QUTOES = false;


bool CsvGenerateColumns::init(CsvCmds& csvCmds, CsvError& csvError) {
    pCsvCmds = &csvCmds;
    nextFileIdx = fileIdx = 0;
    pSelRows = csvCmds.findCmd<CsvSelectRows>(typeid(CsvSelectRows), FindFilter(order));
    if (pSelRows == nullptr) {
        // csvError.append("Must proceed Generate with RowSelect to define maximum rows");
    }
    
    numFiles = 1;
    fileFields.resize(numFiles);
    std::vector<std::shared_ptr<GenFields>>* pFields = &fileFields.back();
    GenSpecs genSpec(4);
    
    const char* rxStr = "(#[^,]+|([0-9+]+|\\[[^]]+]):([0-9.]+)([dsfrx])(\\([^,]+(,[^,]+)*\\)))";
    std::regex rx = std::regex(rxStr, std::regex::extended);
    std::smatch match;
    
    for (unsigned idx = 0; idx < args.size(); idx++) {
        const std::string& aa = args[idx];
        if (std::regex_match(aa.begin(), aa.end(), match, rx)) {
            // const std::string& bigName = match[1];
            size_t mlen = match.size();
            if (mlen > 6 && match[5].length() > 0) {
               
                const std::string& colname = genSpec[0] = match[2];
                const std::string& widthStr = genSpec[1] = match[3];
                const std::string& fieldTyp = genSpec[2] = match[4];
                const std::string& value = genSpec[3] = match[5];
        
                // std::cerr << colname << widthStr << fieldTyp << value << std::endl;
                
                switch (fieldTyp[0]) {
                    case 'x':
                    case 'd':
                        pFields->push_back(std::make_shared<GenDecField>(genSpec));
                        break;
                    case 'f':
                        pFields->push_back(std::make_shared<GenFltField>(genSpec));
                        break;
                    case 's':
                        pFields->push_back(std::make_shared<GenStrField>(genSpec));
                        break;
                    case 'r':   // row - TODO - remove this
                        pFields->push_back(std::make_shared<GenRowField>(genSpec));
                        break;
                }
            } else {
                // nextfile
                numFiles++;
                fileFields.resize(numFiles);
                pFields = &fileFields.back();
            }
        }
    }
    
    rowData.resize(numFiles);
    for (size_t idx = 0;  idx < numFiles; idx++) {
        // rowData[idx].csvRow.resize(args.size());
        rowData[idx].csvRow.setHeaders(genHeaders(idx));
    }
    
    nextFileIdx = 0;
    return fileIdx < numFiles; // TODO - support multiple files
}

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
