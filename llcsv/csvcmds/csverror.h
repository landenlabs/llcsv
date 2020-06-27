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

#include <string>
#include <regex>
#include <sstream>


class CsvError {
public:
    std::string arg;
    std::string msg;
    
    bool setFalse(const std::string& _msg) {
        msg = _msg;
        return false;
    }
    
    CsvError& append(const char* _msg)
    {
        if (_msg != nullptr && *_msg != '\0') {
            if (msg.find(_msg) == std::string::npos) {
                if (!msg.empty()) {
                    msg += "\n";
                }
                msg += _msg;
            }
        }
        return *this;
    }

    CsvError& append(const std::string _msg ) {
       return append(_msg.c_str());
    }
    
    std::string stringer(const std::exception& ex) {
        std::regex fromPat(".*[0-9]([a-z_]+).*");
        std::string errType = typeid(ex).name();
        errType = std::regex_replace(errType, fromPat, " $1 ", std::regex_constants::format_first_only);
        errType = std::regex_replace(errType, std::regex("_"), " ");
        std::string msg = errType + ex.what();
        try {
            std::rethrow_if_nested(ex);
        } catch (const std::exception& nested) {
            msg += stringer(nested);
        }
        return msg;
    }
    
    template<typename TT>
    std::string stringer(const TT& value)
    {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    template<typename TT, typename ... Args >
    std::string stringer(const TT& value, const Args& ... args)
    {
        return stringer(value) + " " + stringer(args...);
    }
    
    template<typename TT, typename... Args>
    CsvError& append(TT _msg, const Args& ... args) {
        std::string msg = stringer(_msg, args...);
        return append(msg.c_str());
    }
};

struct invalid_column: public std::range_error
{
    invalid_column(const std::string& what) : range_error(what) {}
    invalid_column(const char* what) : range_error(what) {}
    // InvalidColumn_error(const char* what) : range_error(std::string("Invalid column ") + what)   { }
};
