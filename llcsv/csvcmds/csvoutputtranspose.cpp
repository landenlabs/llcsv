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

#include "ll_stdhdr.h"
#include "csvoutputtranspose.h"
#include "csvcmds.h"
#include "csvtool.h"

#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;
#include <cstdio>

#define TMP_DIR "/tmp/";

CsvOutputTranspose::CsvOutputTranspose(Order_t order) : CsvOutput(order) {
}
CsvOutputTranspose::~CsvOutputTranspose() {
}

bool CsvOutputTranspose::init(CsvCmds& csvCmds, CsvError& csvError) {
    CsvOutput::init(csvCmds, csvError);
    // outfilename = std::tmpnam(nullptr);
    // outfilename = fs::temp_directory_path();
    outfilename = TMP_DIR;
    outfilename += getName() + std::to_string(order);
  
    return true;
}

bool CsvOutputTranspose::openOut() {
   out.open(outfilename, std::ofstream::out);
   if (out.fail()) {
       int err = errno;
       CsvCmds::CSV_ERROR.append("OutputFile - Failed to open for write ").append(strerror(err));
       CsvCmds::CSV_ERROR.arg = outfilename;
   }

   return out.is_open();
}

std::ostream& CsvOutputTranspose::getOut() {
    return out;
}

void CsvOutputTranspose::close() {
    out.close();
    std::remove(outfilename.c_str());
}

void CsvOutputTranspose::endInputFile(CsvCmds& csvCmds, const CsvInputs& inputs) {
    openOut();
    // write buffered data tranposed to tmp file.
    close();
}

// TODO - remove this method
bool CsvOutputTranspose::writeRow(CsvCmds& csvCmds, const CsvInputs& inputs)  {
    inRowData.push_back(inputs.getRowData());
    // Store in local buffer to be transposed on endInputFile
    return true; // CsvOutput::writeRow(csvCmds, inputs);
}
