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

#pragma once

#include <iostream>
#include "csvfiles.h"
#include "csvinputs.h"

#ifdef IN
#undef IN
#undef OUT
#endif

class CsvOutput : public CsvFiles {
   
protected:
    bool outHeader = false;
    bool outHeaders = false;
    const CsvInputs* pInputs; 
    bool init(CsvCmds& csvCmds, CsvError& cscvError) = 0;
    bool writeRow1(CsvCmds& csvCmds, const CsvTool::CsvRowColumns& row);
    
public:
    char outEOL = '\n';
    char outDelim = ',';
    
    enum Quotes { NONE, ALWAYS, AUTO };
    Quotes outQuotes = NONE;
    std::string QUOTE = "\"";
    
    template <typename TT>
    std::string withQuotes(TT& col) {
        return QUOTE + col + QUOTE;
    }
    template <typename TT>
    std::string autoQuotes(TT& col) {
        if (needsQuotes(col))  
            return withQuotes(col);
        else
            return col;
    }
    template <typename TT>
    std::string noQuotes(TT& col) {
        return col;
    }
    
    template <typename TT>
    std::string quote(const TT& value);
        
    template <typename TT>
    bool needsQuotes(const TT& value);
     
    
public:
    std::string getName() const = 0;
    virtual std::string getOutput() const = 0;
    
    CsvOutput(Order_t order) : CsvFiles(order) {
        actionType = OUT;
    }
    
    virtual
    void endInputFile(CsvCmds& csvCmds, const CsvInputs& inputs);
    
    virtual
    bool writeRow(CsvCmds& csvCmds, const CsvInputs& inputs) = 0;
    
    virtual
    std::ostream& getOut() = 0;
    
    virtual
    void close() {
    }
};
