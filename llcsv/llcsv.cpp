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

// 4291 - No matching operator delete found
#pragma warning( disable : 4291 )

#include <stdio.h>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdlib.h>     // stroul
#include <ctype.h>      // islower, isupperre

#include "util/ll_stdhdr.h"

// #include "util/directory.h"
#include "util/split.h"
// #include "util/filters.h"

#include "csvcmds/csvcmds.h"
#include "csvcmds/csvinputfiles.h"
#include "csvcmds/csvoutputfile.h"
#include "csvcmds/csvoutputtable.h"

#include "csvcmds/csvselectrows.h"
#include "csvcmds/csvselectcolumns.h"

#include "csvcmds/csvappendrows.h"
#include "csvcmds/csvappendcolumns.h"

#include "csvcmds/csvformatcolumns.h"
#include "csvcmds/csvjoincolumns.h"
#include "csvcmds/csvmatchcolumns.h"
#include "csvcmds/csvadjustcolumns.h"
#include "csvcmds/csvreplacecolumns.h"

#include "csvcmds/csvsortrowsbycolumnkeys.h"
#include "csvcmds/csvgeneratecolumns.h"

#include <vector>
#include <map>
#include <algorithm>
#include <regex>
#include <exception>
#include <typeinfo>
using namespace std;

uint optionErrCnt = 0;



// ---------------------------------------------------------------------------
size_t FindCase(const lstring& str, const char* delimList, size_t begIdx) {
 
    while (isupper(str[begIdx])) {
        begIdx++;
    }
    while (islower(str[begIdx])) {
        begIdx++;
    }
    return begIdx < str.length() ? begIdx : string::npos;
}

size_t UpperLen(const lstring& str, size_t begIdx=0) {
    while (isupper(str[begIdx])) {
       begIdx++;
    }
    return begIdx;
}

size_t MatchLen(const lstring& master, const lstring& substr) {
    size_t len = std::min(master.length(), substr.length());
    if ( strncasecmp(master, substr, len) == 0)
        return substr.length();
 
    return 0;
}

// ---------------------------------------------------------------------------
// Validate option matchs and optionally report problem to user.
//
// appendRow  = aR aRow  appRow
//
bool ValidOption(const char* validCmd, const char* possibleCmd, bool reportErr = false)
{
    // Starts with validCmd else mark error
    // size_t validLen = strlen(validCmd);
    size_t possibleLen = strlen(possibleCmd);
  
    if (strncasecmp(validCmd, possibleCmd, possibleLen) == 0)
        return true;
    
    Split validWords(validCmd, NULL, FindCase, 0);
    Split possibleWords(possibleCmd, NULL, FindCase, 0);
    bool good = validWords.size() == possibleWords.size();
    if (good) {
        for (unsigned idx = 0; idx < validWords.size(); idx++) {
            size_t matchLen = MatchLen(validWords[idx], possibleWords[idx]);
            if (matchLen == 0 || matchLen < UpperLen(validWords[idx])) {
                good = false;
                break;
            }
        }
    }
    
    if (!good && reportErr)
    {
        std::cerr << "Unknown option:'" << possibleCmd << "', expect:'" << validCmd << "'\n";
        optionErrCnt++;
    }
    return good;
}

// ---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    int result = -1;
    if (argc == 1)
    {
        cerr << "\n" << argv[0] << "  Dennis Lang v1.1 (landenlabs.com) " __DATE__ << "\n"
            << "\nDes: CSV manipulator command line tool\n"
            "Use: llcsv [actions] ...\n"
            "  General pipeline   input selection modify output"
            "\n"
            "Actions:\n"
            "   Inputs: \n"
            "      inputFiles \n"
            "      generate \n"
            "   Selection:\n"
            "      rows\n"
            "      columns\n"
            "      match\n"
            "   Modify:\n"
            "     appendRows   \n"
            "     appendColumns \n"
            "     formatColumns (na)\n"
            "     joinColumns (na)\n"
            "     replaceColumns \n"
            "     sort (na)\n"
            "     transpose (na)\n"
            "   Output:\n"
            "     outputFiles\n"
            "     outputTable\n"
            "   Help:\n"
            "     help action"
            "\n"
            "Examples\n"
            "\n";
    }
    else
    {
        CsvCmds csvCmds;
        // std::shared_ptr<CsvCmd> pCsvCmd = NULL; // std::make_shared<Derived>();
        
        bool doParseCmds = true;
        string endCmds = "--";
        Order_t order = 0;
        for (int argn = 1; argn < argc; argn++)
        {
            lstring argStr(argv[argn]);
            // Split cmdParts(argStr, " ", 1);
            
            doParseCmds |= !(argStr == endCmds);
            if (doParseCmds)
            {
                // lstring cmd = cmdParts[0];
                bool okay = false;
                CsvError& csvError = CsvCmds::CSV_ERROR;
                
                if (!csvCmds.empty()) {
                    okay = csvCmds.back()->addArgs(argStr, csvError);
                }
                
                if (!okay) switch (argStr[0])
                {
                    case 'a':
                        if ((okay = ValidOption("appendRows", argStr))) {
                            csvCmds.push_back(std::make_shared<CsvAppendRows>(order++));
                            continue;
                        }
                        if ((okay = ValidOption("appendColumns", argStr))) {
                            csvCmds.push_back(std::make_shared<CsvAppendColumns>(order++));
                            continue;
                        }
                        if ((okay = ValidOption("adjustColumns", argStr))) {
                           csvCmds.push_back(std::make_shared<CsvAdjustColumns>(order++));
                           continue;
                        }
                        break;
                    case 'c':
                        if ((okay = ValidOption("columns", argStr))) {
                            csvCmds.push_back(std::make_shared<CsvSelectColunns>(order++));
                            continue;
                        }
                        break;
                        
                    case 'f':
                            if ((okay = ValidOption("format", argStr))) {
                                // TODO - use Generate syntax, include option to add quotes
                                //  [count]:3d,[name]:10s,[price]:5.2f
                                //  1:3d(,")   add commas and inclose in quotes
                                //  2:10.10s   pad and truncate to fixed length 10
                                //  10:1s("t")
                                csvCmds.push_back(std::make_shared<CsvFormatColumns>(order++));
                                continue;
                            }
                            break;
                        
                    case 'g':
                        // [count]:3d(1,1000,2),[name]:10s,[price]:5.2f(-123.45,123.45)
                        if ((okay = ValidOption("generate", argStr))) {
                            csvCmds.push_back(std::make_shared<CsvGenerateColumns>(order++));
                            continue;
                        }
                        break;
                        
                    case 'i':
                        if ((okay = ValidOption("input", argStr))) {
                            // TODO - support #utf-8, #utf-16?
                            csvCmds.push_back(std::make_shared<CsvInputFiles>(order++));
                            continue;
                        }
                        break;
                        
                    case 'j':
                        if ((okay = ValidOption("joinColumns", argStr))) {
                             csvCmds.push_back(std::make_shared<CsvJoinColumns>(order++));
                            continue;
                        }
                        break;
                        
                    case 'm':
                        if ((okay = ValidOption("matchColumns", argStr))) {
                             csvCmds.push_back(std::make_shared<CsvMatchColumns>(order++));
                            continue;
                        }
                        break;
                        
                    case 'o':
                        if ((okay = ValidOption("outputTable", argStr))) {
                            csvCmds.push_back(std::make_shared<CsvOutputTable>(order++));
                            continue;
                        } else if ((okay = ValidOption("outputFile", argStr))) {
                            // TODO - support #append, #binary? #utf-16
                            csvCmds.push_back(std::make_shared<CsvOutputFile>(order++));
                            continue;
                        }
                        break;
                        
                    case 'r':
                        if ((okay = ValidOption("ROws", argStr))) {
                             csvCmds.push_back(std::make_shared<CsvSelectRows>(order++));
                            continue;
                        } else if ((okay = ValidOption("REplace", argStr))) {
                             csvCmds.push_back(std::make_shared<CsvReplaceColumns>(order++));
                            continue;
                        }
                        break;
                        
                        
                    case 's':
                        if ((okay = ValidOption("sort", argStr))) {
                             csvCmds.push_back(std::make_shared<CsvSortRowsByColumnKeys>(order++));
                            continue;
                        }
                        break;
                        
                    case 't':
                       if ((okay = ValidOption("transpose", argStr))) {
                           /*   TODO
                            csvCmds.push_back(std::make_shared<CsvTranspose>(order++));
                           continue;
                            */
                       }
                       break;
                }
                
                if (!okay) {
                    // TODO - use csvError
                    std::cerr <<  csvCmds.back()->getName();
                    std::cerr << " Unknown option:'" << argStr << "'\n";
                    optionErrCnt++;
                }
            }
            else
            {
                // Store file directories
                // fileDirList.push_back(argv[argn]);
            }
        }            
    
        if (!csvCmds.empty() &&  optionErrCnt == 0)
        {
            csvCmds.dump(std::cout);
            result = csvCmds.init(CsvCmds::CSV_ERROR);
        }
        
        if (result == -1) {
            std::cerr << CsvCmds::CSV_ERROR.msg
                << " " << CsvCmds::CSV_ERROR.arg
                << std::endl;
        }
    }

    return result;
}

