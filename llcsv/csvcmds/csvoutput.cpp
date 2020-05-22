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

#include "csvoutput.h"



bool CsvOutput::init(CsvCmds& csvCmds, CsvError& csvError) {
    bool ok = true;
    ArgList::iterator iter = args.begin();
    while (iter != args.end()) {
        const std::string& arg = *iter;
        size_t argLen = arg.length()-1;
        if (arg[0] == '-' || arg[0] == '#') {
            if (strcasecmp("headers", arg.c_str()+1) == 0) {
                outHeaders = true;
                outHeader = false;
            } else if (strcasecmp("header", arg.c_str()+1) == 0) {
                outHeader = true;
                outHeaders = false;
            } else if (strncasecmp("tab-separator", arg.c_str()+1, argLen) == 0) {
                outDelim = '\t';
            }  else if (strncasecmp("cr-eol", arg.c_str()+1, argLen) == 0) {
                outEOL = '\r';
            } else {
                ok = false;
                csvError.append(getName() + " unknown argument " + arg);
            }
            args.erase(iter);
        } else {
            iter++;
        }
    }
    
    return ok;
}

bool CsvOutput::writeRow(CsvCmds& csvCmds, const CsvInputs& inputs)  {
    const CsvTool::CsvRow& row = inputs.rowData[inputs.fileIdx].csvRow;
    if (outHeader || (outHeaders && &inputs != pInputs)) {
        outHeader = false;
        pInputs = &inputs;
        writeRow1(csvCmds, row.getHeaders());
    }
    writeRow1(csvCmds, row);
    return !getOut().fail();
}

bool CsvOutput::writeRow1(CsvCmds& csvCmds, const CsvTool::CsvRowColumns& row) {
    bool sep = false;
    for (auto &col : row) {
       if (sep) getOut() << outDelim;
       getOut() << col;
       sep = true;
    }
    getOut() << CsvOutput::outEOL;
    return !getOut().fail();
}
