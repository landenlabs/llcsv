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

#include "csvcmd.h"
// #include "csvinputs.h"

#include <iostream>
#include <vector>

// Fowward
class CsvRowData;

 
class FindFilter {
    Order_t master;
public:
    FindFilter(Order_t _master) {
        master = _master;
    }
    bool best(Order_t param) const {
        return param < master;
    }
    bool first() const {
        return false;
    }
};
 
typedef std::shared_ptr<CsvCmd> CsvItem;
class CsvCmds : public std::vector<CsvItem> {
    
public:
    static CsvError CSV_ERROR;
    
public:
    int init(CsvError& csvError);
    
    void dump(std::ostream& out);
    
    template <class TT>
    TT* findCmd(const CsvCmd::ActionType& action, const FindFilter& filter) {
        iterator iter = begin();
        TT* result = nullptr;
        while (iter != end()) {
            if ((*iter)->actionType == action && filter.best((*iter)->order)) {
                result = dynamic_cast<TT*>((*iter).get());
                if (filter.first()) {
                    break;
                }
            }
            iter++;
        }
        return result;
    }
    
    template <class TT>
    TT* findCmd(const std::type_info& tinfo, const FindFilter& filter) {
        iterator iter = begin();
        TT* result = nullptr;
        while (iter != end()) {
            if (typeid(*(*iter)) == tinfo && filter.best((*iter)->order)) {
                result = (TT*)(*iter).get();
                if (filter.first()) {
                    break;
                }
            }
            iter++;
        }
        return result;
    }
  
    bool setParallel(bool parallel, Order_t order);
  
    // CsvRowData& getRowData();
};

