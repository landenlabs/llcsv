//
// CSV - Read(optional buffer),  Parse and Store in std maps
//
// Created by Dennis Lang on 3-Jan-2020
//
// ---
// Copyright (C) 2019 Dennis Lang (landenlabs@gmail.com)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

#define CSVTOOL_VERSION "v20.01.06"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <cstring>          // For strchr()
#include <algorithm>        // find
 

namespace CsvTool {

// Build options (csvtool.h):
//  #define USE_IBUFFER             // Custon input buffer
//  #define BUFFER_ENTIRE_FILE      // Use with USE_IBUFFER to buffer entire input csv file
#define SUPPORT_QUOTED_VALUES

#ifdef USE_IBUFFER
// ===============================================================================================
// Custom buffered input stream.
class IBuffer {
  
    std::ifstream in;
#ifdef BUFFER_ENTIRE_FILE
    size_t BUFFER_SIZE = 0;
    std::unique_ptr<char[]> mAllocBuffer;
    char* mBuffer = nullptr;
#else
    static constexpr size_t BUFFER_SIZE = 1024 * 128;
    char mBuffer[BUFFER_SIZE]{};
#endif
    size_t inPos = BUFFER_SIZE;
    size_t inLen = BUFFER_SIZE;
    bool mEof = false;
    
public:
    IBuffer(const char* file, int flags) {
        in.open(file, flags);
#ifdef BUFFER_ENTIRE_FILE
        struct stat stat_buf;
        int rc = stat(file, &stat_buf);
        if  (rc == 0 &&  stat_buf.st_size > 0) {
            BUFFER_SIZE = stat_buf.st_size;
            mAllocBuffer = std::make_unique<char[]>(stat_buf.st_size);
            mBuffer = mAllocBuffer.get();
        }
#endif
    }
    void close() {
        in.close();
    }
    bool eof() {
        return mEof;
    }
    bool get(char& ch) {
        if (inPos < inLen) {
            ch = mBuffer[inPos++];
            return true;
        }
        if (!in.eof()) {
            inPos = 0;
            inLen = in.read(mBuffer, BUFFER_SIZE).gcount();
            return get(ch);
        }
        mEof = true;
        return !mEof;
    }
};
typedef IBuffer CsvStream;
#else
typedef std::ifstream CsvStream;
#endif

typedef std::vector<std::string> CsvCells;

class CsvHeaders : public CsvCells {
public:
    CsvHeaders() = default;
    // CsvHeaders(const CsvHeaders& other) = default;
    CsvHeaders(size_t initSize) : vector(initSize) {
    }
    CsvHeaders& operator=(const CsvHeaders& other) {
        vector::operator=(other);
        return *this;
    }
   //  ~CsvHeaders() = default;
    
    unsigned index(const std::string& item) const {
        for (unsigned idx = 0; idx < size(); idx++) {
            if (item == at(idx))
                return idx;
        }
        return npos;
    }
    const unsigned npos = -1;
};

typedef CsvCells CsvRowColumns;
class CsvRow : public CsvRowColumns {
    
    unsigned rowIdx; // 1...n
    CsvHeaders headers;
    
public:
    const std::string& getColumn(const std::string& colName) const {
        return at(headers.index(colName));
    }
    std::string& getColumn(const std::string& colName)  {
        // std::find(headers.begin(), headers.end(), colName);
        // int index = std::distance(vecOfNums.begin(), it);
        try {
            return at(headers.index(colName));
        }  catch(...) {
            // std::throw_with_nested( std::range_error(colName) );
            // throw std::range_error(colName);
            throw invalid_column(colName);
        }
    }
    
    void setHeaders() {
        headers.swap(*this);
        reserve(std::max(capacity(), headers.size()));
    }
    void setHeaders(const CsvHeaders& _headers) {
        headers = _headers;
        reserve(std::max(capacity(), headers.size()));
    }
    const CsvHeaders& getHeaders() const {
        return headers;
    }
    CsvHeaders& getHeaders() {
        return headers;
    }
};

// ===============================================================================================
// Split string into (comma-separated-values) parts
// Support fields spanning rows, quoted fields and escaped quotes.
class CsvParser
{
  public:
    char delim = ',';
    const char* quotes = "'\"";
    const char space = ' ';
    char inEOL = '\n';
    bool keepQuotes = false;
    char ignoreEOL = '\r';
    
  public:

    // Parse a CSV row from input stream, store in fields using field as working storage.
    bool getRow(CsvStream& istream, std::string& field, CsvRow& fields)  {
        char ch;
        while (!istream.eof()) {
            field.clear();
            
            // Remove unwanted leading characters.
            while (istream.get(ch) && (ch == space || ch == ignoreEOL));
            if (ch == inEOL || ch == '\0')
                return !istream.eof() && ch != '\0';
            
#ifdef SUPPORT_QUOTED_VALUES
            if (strchr(quotes, ch) != nullptr)  {
                // Handle quoted field.
                char quote = ch;
                bool addQuote = keepQuotes;
                do {
                    if (addQuote)
                        field.push_back(quote);
                    while (istream.get(ch) && ch != quote) {
                        field.push_back(ch);
                    }
                    addQuote = true;
                    // Handle escaped field.
                } while (istream.get(ch) && ch == quote);
                if (ch != delim && ch != inEOL) {
                    while (istream.get(ch) && ch != delim && ch != inEOL);
                }
                if (keepQuotes)
                    field.push_back(quote);
            }
            else
#endif
            {
                do {
                    if (ch == delim || ch == inEOL) {
                        break;
                    } else if (ch != ignoreEOL) {
                        field.push_back(ch);
                    }
                } while (istream.get(ch));
            }
            fields.push_back(field);
            if (ch == inEOL)
                break;
        }

        return !fields.empty(); //  !istream.eof();
    }
};

} // namespace
