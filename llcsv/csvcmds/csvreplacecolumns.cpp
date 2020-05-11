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


#include "csvreplacecolumns.h"
#include "csvcmds.h"

bool CsvReplaceColumns::init(CsvCmds& csvCmds, CsvError& cscvError) {
    // 2/old/new/  [date]/2020/2021/
    //  "([0-9]+|\\[[^]]+])/[^/]+/[^/]*/";
    
    //  MatchSpecs matchSpec(5);
    
    const char* rxStr = "([0-9]+|\\[[^]]+])/([^/]+)/([^/]*)/";
    std::regex rx = std::regex(rxStr, std::regex::extended);
    std::smatch match;

    for (unsigned idx = 0; idx < args.size(); idx++) {
        const std::string& aa = args[idx];
        if (std::regex_match(aa, match, rx)) {
            // const std::string& bigName = match[1];
            // size_t mlen = match.size();
            const std::string& colname = match[1];
            const std::string& fromPatStr = match[2];
            const std::string& toPat = match[3];
            
            std::cout << "colName=" << colname
                << " fromPat=" << fromPatStr
                << " toPat=" << toPat
                << std::endl;
            
            replaceFields.push_back(std::make_shared<ReplaceField>(colname, fromPatStr, toPat));
        }
    }
                
    return true;
}

bool CsvReplaceColumns::action(CsvCmds& csvCmds, CsvInputs& inputs, CsvInputs*& pipe)   {
    CsvRowData& rowData = inputs.getRowData();
    ReplaceFields::const_iterator iter;
    
    bool matched = true;
    try {
        for (iter = replaceFields.cbegin(); iter != replaceFields.cend(); iter++) {
            (*iter)->replace(rowData);
        }
    } catch (std::exception ex) {
        CsvCmds::CSV_ERROR.append(ex.what());
        matched = false;
    }
    return matched;
}
