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


#include "csvinputfiles.h"
#include "csvcmds.h"
#include "csvtool.h"
using namespace CsvTool;

bool static USE_BIG_STACK_BUFFER = false;
bool static USE_BIG_HEAP_BUFFER = false;
static bool USE_CR_EOL  = false;
static bool USE_KEEP_QUTOES = false;

bool CsvInputFiles::init(CsvCmds& csvCmds, CsvError& csvError) {
    nextFileIdx = fileIdx = 0;
    
    // Parse and count rows
    if ( USE_CR_EOL) {
        csvParser.inEOL = '\r';
    }
    if ( USE_KEEP_QUTOES ) {
        csvParser.keepQuotes = true;
    }
    
    return fileIdx < args.size();
}

bool CsvInputFiles::nextFile(CsvError& csvError) {
    inFS.close();
    
    if (nextFileIdx < args.size()) {
        fileIdx = nextFileIdx;
        file = args[nextFileIdx++];
        inFS.open(file, std::ifstream::binary);
        if (!inFS.fail()) {
            // Optional input buffering hacks
            if( USE_BIG_STACK_BUFFER){
                static constexpr int BUFFER_SIZE = 1024 * 128;
                char bigBuffer[BUFFER_SIZE];
                inFS.rdbuf()->pubsetbuf(bigBuffer, BUFFER_SIZE);
            } else if (USE_BIG_HEAP_BUFFER ) {
                static constexpr int BUFFER_SIZE = 1024 * 128;
                std::vector<char> bigBuffer(BUFFER_SIZE);
                inFS.rdbuf()->pubsetbuf(&bigBuffer[0], BUFFER_SIZE);
            }
            rowData.resize(nextFileIdx);
        }
    }
    
    return inFS.is_open();
}

bool CsvInputFiles::nextRow() {
    bool okay = csvParser.getRow(inFS, field, rowData[fileIdx].csvRow);
    if (okay) rowData[fileIdx].inRowCount++;
    return okay;
}
