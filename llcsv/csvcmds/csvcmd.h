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


#pragma once

#include "csverror.h"
#include <iostream>
#include <regex>

// Forward
class CsvCmds;
typedef unsigned int Order_t;
const Order_t MAX_ORDER = -1;

class CsvCmd {  // make abstract
public:
    typedef  std::vector<std::string>  ArgList;
    ArgList args;
    std::regex argRE;
    std::smatch match;
    std::string argREstr;
    
    enum ActionType { NONE, IN, SELECT, MODIFY, OUT };
    ActionType actionType = NONE;
    Order_t order = 0;
    
protected:
    CsvCmd(Order_t _order) : order(_order) {
        // 1,2-,-3,[hello],[a]-[b]
        argREstr = "(([0-9+]+|\\[[^]]+])[,-]{0,1})+";
        argRE = std::regex(argREstr, std::regex::extended);
    }
    virtual ~CsvCmd() {
    };
    
public:
    bool addArgs(const std::string& arg, CsvError& error) {
        if (std::regex_match(arg, match, argRE)) {
            if (match.length() == arg.length()) {
                args.push_back(arg);
                return true;
            } else {
                // TODO - show partial match.
                // std::cerr << "unknown arg " << arg << std::endl;
            }
        }
        error.msg = "Invalid arguments";
        error.arg = arg;
        return false;
    }
    
    std::string dump() {
        std::string msg = getName();
        for (size_t idx = 0; idx < args.size(); idx++) {
            msg += "\n  " + args[idx];
        }
        return msg;
    }
   
public:
    virtual std::string getName() const = 0;
    virtual bool init(CsvCmds& csvCmds, CsvError& csvError) = 0;
};
