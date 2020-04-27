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


#include "csvmatchcolumns.h"
#include "csvcmds.h"
#include "strutils.h"


bool CsvMatchColumns::init(CsvCmds& csvCmds, CsvError& cscvError) {
    // 2~"text"i, 2=/regex/, [hello]ge[1.0]f
    // i=ignorecase, f=float, d=decimal, x=hexDecimal
    
    MatchSpecs matchSpec(5);
    
    const char* rxStr = "([0-9]+|\\[[^]]+])([=<>!~]|eq|ne|gt|ge|lt|le)([0-9.-]+|['\"/].*['\"/])([ifdx]*)";
    std::regex rx = std::regex(rxStr, std::regex::extended);
    std::smatch match;

    for (unsigned idx = 0; idx < args.size(); idx++) {
        const std::string& aa = args[idx];
        if (std::regex_match(aa.begin(), aa.end(), match, rx)) {
            // const std::string& bigName = match[1];
            size_t mlen = match.size();
            const std::string& colname = matchSpec[0] = match[1];
            const std::string& matchOp = matchSpec[1] = match[2];
            matchSpec[3] = match[3];
            const std::string& matchType = matchSpec[2] =matchSpec[3][0];
            const std::string& value = removeWrapper(matchSpec[3], "/'\"");
            const std::string& opt = matchSpec[4] = match[4];
            
            /*
            std::cerr << "name=" << colname
                    << " operator=" << matchOp
                    << " type=" << matchType
                    << " value=" << value
                    << " opt=" << opt
                    << std::endl;
            */
            
            switch (matchType[0]) {
               case '\'':
               case '"':
                    matchFields.push_back(std::make_shared<MatTxtField>(matchSpec));
                   break;
               case '/':
                    matchFields.push_back(std::make_shared<MatRegField>(matchSpec));
                   break;
                default:
                    matchFields.push_back(std::make_shared<MatNumField<double>>(matchSpec, *toDbl));
                    break;
           }
        }
    }
            
    return true;
}

bool CsvMatchColumns::selected(CsvCmds& csvCmds, const CsvInputs& inputs) const {

    const CsvRowData& rowData = inputs.getRowData();
    MatchFields::const_iterator iter;
    bool matched = true;
    try {
        for (iter = matchFields.cbegin(); matched && iter != matchFields.cend(); iter++) {
            matched = (*iter)->match(rowData);
        }
    } catch (std::exception ex) {
        CsvCmds::CSV_ERROR.append(ex.what());
        matched = false;
    }
    return matched;
}

bool CsvMatchColumns::end(CsvCmds& csvCmds, const CsvInputs& inputs) const {
    return false;
}
