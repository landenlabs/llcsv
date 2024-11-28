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


#include "csvinputfiles.h"
#include "csvcmds.h"
#include "csvtool.h"
using namespace CsvTool;
#include "fileutils.h"

bool static USE_BIG_STACK_BUFFER = false;
bool static USE_BIG_HEAP_BUFFER = false;


size_t onFile(const std::string& fullname, FileUtils_t& fileUtils) {
    fileUtils.fileDirList.push(fullname);
    return fileUtils.fileDirList.size();
}

CsvInputFiles::CsvInputFiles(Order_t order) :
    CsvInputs(order),  fileUtils(&onFile, this)
{
}

bool CsvInputFiles::init(CsvCmds& csvCmds, CsvError& csvError) {
    nextArgIdx = nextFileIdx = fileIdx = 0;


    ArgList::iterator iter = args.begin();
    while (iter != args.end()) {
        const std::string& arg = *iter;
        size_t argLen = arg.length()-1;
        if (arg[0] == '-' || arg[0] == '#') {
            if (strcasecmp("header", arg.c_str()+1) == 0) {
                hasHeader = true;
            } else if (strncasecmp("tab-separator", arg.c_str()+1, argLen) == 0) {
                csvParser.delim = '\t';
            } else if (strncasecmp("quotes-keep", arg.c_str()+1, argLen) == 0) {
                csvParser.keepQuotes = true;
            }  else if (strncasecmp("cr-eol", arg.c_str()+1, argLen) == 0) {
                csvParser.inEOL = '\r';
            }
            args.erase(iter);
        } else {
            iter++;
        }
    }

    return args.size() > 0 ? true : csvError.setFalse("InputFile - missing filename");
}

bool CsvInputFiles::getNextFileName()  {
    // TODO - iterate on FileUtil to get filenames
    if (nextArgIdx < args.size()) {
        file = args[nextArgIdx++];
        std::cerr << "Inpfiles::getNextFileName arg=" << file << std::endl;

        fileUtils.ScanFiles(file);   // TODO - use thread
        std::cerr << "Inpfiles::getNextFileName nextArgIdx=" << nextArgIdx << " of " << args.size() << " found=" << fileUtils.fileDirList.size() << std::endl;
    }
    if (fileUtils.fileDirList.empty()) {
        return false;
    } else {
        file = fileUtils.fileDirList.front();
        fileUtils.fileDirList.pop();
        fileIdx = nextFileIdx;
        if (parallel)
           nextFileIdx++;
        return true;
    }
}
    
bool CsvInputFiles::nextFile(CsvError& csvError) {
    std::cerr << "Inpfiles::nextFile fileIdx=" << fileIdx << " of " << inFsList.size() << std::endl;
    
    if (!parallel && fileIdx < inFsList.size()) {
        CsvTool::CsvStream& inFS = inFsList[fileIdx];
        inFS.close();
    }
    
    bool okay = (parallel && inFsList.size() > fileIdx+1);
    if (okay) {
        fileIdx++;  // TODO - update filename
    } else if (getNextFileName()) {
        inFsList.resize(fileIdx+1); // TODO - combine rowData and inFS
        CsvTool::CsvStream& inFS = inFsList[fileIdx];
        inFS.open(file, std::ifstream::binary); // TODO - hande "-" for stdin
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
            
            rowData.resize(fileIdx+1);  // TODO - combine rowData and inFS
            
            if (hasHeader) {
                if (csvParser.getRow(inFS, field, rowData[fileIdx].csvRow)) {
                    rowData[fileIdx].csvRow.setHeaders();
                }
            }
            std::cerr << "InputFile open " << file << std::endl;
        } else {
            std::cerr << "InputFile - Failed to open  " << file << std::endl;
            csvError.append("InputFile - Failed to open ")
                .append(file)
                .append(" ").append(strerror(errno));
        }
        okay = inFS.is_open();
    }
    return okay;
}

bool CsvInputFiles::nextRow() {
    bool anyOkay = false;
    do {
        CsvTool::CsvStream& inFS = inFsList[fileIdx];
        rowData[fileIdx].csvRow.clear();
        bool okay = csvParser.getRow(inFS, field, rowData[fileIdx].csvRow);
        anyOkay |= okay;
        if (okay)
            rowData[fileIdx].inRowCount++;
    } while (parallel && nextFile(CsvCmds::CSV_ERROR));
    if (parallel && anyOkay)
        fileIdx = 0;
    return anyOkay;
}
