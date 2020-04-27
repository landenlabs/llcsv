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


#include "csvcmds.h"
#include "csvmodify.h"
#include "csvselect.h"
#include "csvinputs.h"
#include "csvoutput.h"
#include "csvtool.h"
using namespace CsvTool;

#include <iostream>


// static bool DUMP_TABLE = false;

CsvError CsvCmds::CSV_ERROR;

int CsvCmds::init(CsvError& csvError) {
    
    typedef std::shared_ptr<CsvInputs> CsvIn;
    std::vector<CsvIn> inCmds;
    std::vector<CsvIn>::iterator inIter;
    
    typedef std::shared_ptr<CsvSelect> CsvSel;
    std::vector<CsvSel> selectCmds;
    std::vector<CsvSel>::iterator selIter;
    
    typedef std::shared_ptr<CsvModify> CsvMod;
    std::vector<CsvMod> modifyCmds;
    std::vector<CsvMod>::iterator modIter;
    
    typedef std::shared_ptr<CsvOutput> CsvOut;
    std::vector<CsvOut> outCmds;
    std::vector<CsvOut>::iterator outIter;
    
    std::vector<CsvItem>::iterator iter = begin();
    while (iter != end()) {
        CsvItem item = *iter++;
        if (!item->init(*this, csvError)) {
            return -1;
        }
        switch (item->action) {
            case CsvCmd::IN:
                inCmds.push_back(std::dynamic_pointer_cast<CsvInputs>(item));
                break;
            case CsvCmd::MODIFY:
                modifyCmds.push_back(std::dynamic_pointer_cast<CsvModify>(item));
                break;
            case CsvCmd::SELECT:
                selectCmds.push_back(std::dynamic_pointer_cast<CsvSelect>(item));
                break;
            case CsvCmd::OUT:
                outCmds.push_back(std::dynamic_pointer_cast<CsvOutput>(item));
                break;
            default:
                break;
        }
    }
    
    // std::sort(std::begin(selectCmds), std::end(selectCmds),
    //          [](CsvItem a, CsvItem b) {return a->order > b->order; });
    
    int result = 0;
   
    for (unsigned inIdx=0; inIdx < inCmds.size(); inIdx++) {
        CsvInputs& inputs = *inCmds[inIdx];
        Order_t inOrder = inputs.order;
        Order_t endOrder = (inIdx+1 < inCmds.size())
            ? inCmds[inIdx+1]->order : MAX_ORDER;
    
        while (inputs.nextFile(csvError)) {
            bool endInput = false;
            while (!endInput && inputs.nextRow()) {
                bool rowValid = true;
                selIter = selectCmds.begin();
                while (rowValid && selIter != selectCmds.end()) {
                    CsvSelect& selItem = *(*selIter++);
                    if (selItem.order > inOrder && selItem.order < endOrder) {
                        rowValid = selItem.selected(*this, inputs);
                    }
                }
                
                if (rowValid) {
                    bool modValid = true;
                    modIter = modifyCmds.begin();
                    while (modValid && modIter != modifyCmds.end()) {
                        modValid =(*modIter++)->modify(*this, inputs);
                    }
                    
                    for (unsigned outIdx=0; outIdx < outCmds.size(); outIdx++) {
                        CsvOutput& output = *outCmds[outIdx];
                        output.writeRow(*this, inputs);
                    }
                }
                
                for (selIter = selectCmds.begin();
                     !endInput && selIter != selectCmds.end();
                     selIter++) {
                    endInput =(*selIter)->end(*this, inputs);
                }
            }
        }
    }
    return result;
}

bool CsvCmds::setParallel(bool parallel, Order_t order) {
    CsvInputs* pInputs = findCmd<CsvInputs>(CsvCmd::Action::IN, FindFilter(order));

    if (pInputs != NULL) {
        pInputs->setParallel(parallel, order);
        return true;
    }

    return false;
}

// CsvRowData& CsvCmds::getRowData() { }

void CsvCmds::dump(std::ostream& out) {
    for (size_t idx = 0; idx < size(); idx++) {
        out << idx << ": " << at(idx)->dump() << std::endl;
    }
}
 


