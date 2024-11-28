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


#include "csvoutputfile.h"
#include "csvcmds.h"
#include "csvtool.h"
#include "fileutils.h"

#include <fstream>


bool CsvOutputFile::init(CsvCmds& csvCmds, CsvError& csvError) {
    CsvOutput::init(csvCmds, csvError);
    
    if (args.size() == 1) {
        outFilename = args[0];
    }
    return (args.size() == 1) ? true : csvError.setFalse("OutputFile not set correctly ");
}

bool CsvOutputFile::open(CsvCmds& csvCmds) {
    if (!out.is_open()) {
        outFilename = getNextOutFilename(csvCmds);
        out.open(outFilename, std::ofstream::out);  // defaults to appending ?
        if (out.fail()) {
            int err = errno;
            CsvCmds::CSV_ERROR.append("OutputFile - Failed to open for write ").append(strerror(err));
            CsvCmds::CSV_ERROR.arg = outFilename;
        }
    }

    return out.is_open();
}

//
// Get output filename, either provided "as is" or derived from the input file.
// The following special formats will extract parts from the current input file.
//    %s=fullpath,  %p=path, %n=name, %e=extension %f=filename name+ext
//
std::string& CsvOutputFile::getNextOutFilename(CsvCmds& csvCmds) {
    nextOutFilename = outFilename;
    CsvInputs* pInputs = csvCmds.findCmd<CsvInputs>(CsvCmd::ActionType::IN, FindFilter(order));
    if (pInputs != nullptr && args[0].find( "%") != std::string::npos) {
        std::vector<char> buf;
        nextOutFilename = getParts(args[0].c_str(), pInputs->file.c_str(), buf);
    }
    return nextOutFilename;
}

std::ostream& CsvOutputFile::getOut() {
    return out;
}

// TODO - remove this method
bool CsvOutputFile::writeRow(CsvCmds& csvCmds, const CsvInputs& inputs)  {
    return open(csvCmds) && CsvOutput::writeRow(csvCmds, inputs);
}

void CsvOutputFile::endInputFile(CsvCmds& csvCmds, const CsvInputs& inputs) {
    out.close();
    CsvOutput::endInputFile(csvCmds, inputs);
}

void CsvOutputFile::close() {
    out.close();
    outFilename.clear();
}
