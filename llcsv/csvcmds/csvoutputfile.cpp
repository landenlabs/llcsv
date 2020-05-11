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


#include "csvoutputfile.h"
#include "csvcmds.h"
#include "csvtool.h"

#include <fstream>


bool CsvOutputFile::init(CsvCmds& csvCmds, CsvError& csvError) {
    CsvOutput::init(csvCmds, csvError);
    if (args.size() == 1) {
        out.open(args[0], std::ofstream::out);
        if (out.fail()) {
            int err = errno;
            csvError.append("OutputFile - Failed to open for write ").append(strerror(err));
            csvError.arg = args[0];
        }
    } else {
        csvError.append("OutputFile - Missing filename");
    }
    return out.is_open();
}

std::ostream& CsvOutputFile::getOut() {
    return out;
}

// TODO - remove this method
bool CsvOutputFile::writeRow(CsvCmds& csvCmds, const CsvInputs& inputs)  {
    return CsvOutput::writeRow(csvCmds, inputs);
}
