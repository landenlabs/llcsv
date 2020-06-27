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


#include "csvadjustcolumns.h"
#include "csvcmds.h"
#include "fileutils.h"

class AdjustAppendFilename : public AdjustAction {
    std::vector<char> buf;
public:
    AdjustAppendFilename(
         CsvCmds& _csvCmds, const std::string& _action, const std::string& _val1, const std::string& _val2) :
        AdjustAction(_csvCmds, _action, _val1, _val2) {
    }
    
    bool adjust(CsvRowData& rowData)  override {
        // std::string& str = rowData.getColumn(val1, colNum);
        buf.clear();
        rowData.appendCol(val2, colNum, getParts("%n", rowData.pInputs->file.c_str(), buf) );
        return true;
    }
};

bool CsvAdjustColumns::init(CsvCmds& csvCmds, CsvError& cscvError) {
    // append:[filename](filename)
    const char* rxStr =  "(([^:]+):([^(]+)[(]([^)]+)[)])";
    std::regex rx = std::regex(rxStr, std::regex::extended);
    std::smatch match;

    for (unsigned idx = 0; idx < args.size(); idx++) {
        const std::string& aa = args[idx];
        if (std::regex_match(aa, match, rx)) {
            // size_t mlen = match.size();
            const std::string& adjAction = match[2];
            const std::string& val1 = match[3];
            const std::string& val2 = match[4];
            
            switch (adjAction[0]) {
                case 'a': // append
                    if (strncasecmp("append", adjAction.c_str(), adjAction.length()) == 0) {
                        switch (val2[0]) {
                            case 'f':   // filename
                                adjustActions.push_back(std::make_shared<AdjustAppendFilename>(csvCmds, adjAction, val1, val2));
                                break;
                        }
                    }
                    break;
            }
        }
    }
                
    return true;
}

bool CsvAdjustColumns::action(CsvCmds& csvCmds, CsvInputs& inputs, CsvInputs*& pipe)  {
    CsvRowData& rowData = inputs.getRowData();
    AdjustActions::const_iterator iter;
    
    bool matched = true;
    try {
        for (iter = adjustActions.cbegin(); iter != adjustActions.cend(); iter++) {
            (*iter)->adjust(rowData);
        }
    } catch (std::exception const& ex) {
        CsvCmds::CSV_ERROR.append(getName(), ex);
        matched = false;
    }
    return matched;
}
