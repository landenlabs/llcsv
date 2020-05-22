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
#include "csvinputpipe.h"
#include "csvoutput.h"
#include "csvtool.h"
using namespace CsvTool;

#include <iostream>


// static bool DUMP_TABLE = false;

CsvError CsvCmds::CSV_ERROR;

int CsvCmds::init(CsvError& csvError) {
    
    typedef std::shared_ptr<CsvInputs> CsvIn;
    std::vector<CsvIn> inCmds;
    // std::vector<CsvIn>::iterator inIter;
    
    typedef std::shared_ptr<CsvAction> CsvAct;
    std::vector<CsvAct> actionCmds;
    std::vector<CsvAct>::iterator actIter;
    
    typedef std::shared_ptr<CsvOutput> CsvOut;
    std::vector<CsvOut> outCmds;
    // std::vector<CsvOut>::iterator outIter;
    
    std::vector<CsvItem>::iterator iter = begin();
    while (iter != end()) {
        CsvItem item = *iter++;
        if (!item->init(*this, csvError)) {
            return -1;
        }
        switch (item->actionType) {
            case CsvCmd::IN:
                inCmds.push_back(std::dynamic_pointer_cast<CsvInputs>(item));
                break;
            case CsvCmd::MODIFY:
            case CsvCmd::SELECT:
                actionCmds.push_back(std::dynamic_pointer_cast<CsvAction>(item));
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
                CsvInputs* pipe = &inputs;
                actIter = actionCmds.begin();
                while (rowValid && actIter != actionCmds.end()) {
                    CsvAction& actItem = *(*actIter++);
                    if (actItem.order > inOrder && actItem.order < endOrder) {
                        try {
                            rowValid = actItem.action(*this, *pipe, pipe);
                        } catch (std::exception ex) {
                            std::cerr << actItem.getName() << " " << ex.what() << std::endl;
                        }
                    }
                }
                
                if (rowValid /* pipe->getRowData().csvRow.size() > 0 */) {
                    for (unsigned outIdx=0; outIdx < outCmds.size(); outIdx++) {
                        CsvOutput& output = *outCmds[outIdx];
                        output.writeRow(*this, *pipe);
                    }
                }
                
                // SelectRows can return false.
                for (actIter = actionCmds.begin();
                     !endInput && actIter != actionCmds.end();
                     actIter++) {
                    endInput =(*actIter)->end(*this, *pipe, pipe);
                }
            }
            
            for (unsigned outIdx=0; outIdx < outCmds.size(); outIdx++) {
               CsvOutput& output = *outCmds[outIdx];
               output.endInputFile(*this, inputs);
            }
        }
    }
    return result;
}

bool CsvCmds::setParallel(bool parallel, Order_t order) {
    CsvInputs* pInputs = findCmd<CsvInputs>(CsvCmd::ActionType::IN, FindFilter(order));

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
 


