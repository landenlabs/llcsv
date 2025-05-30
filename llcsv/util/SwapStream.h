//-------------------------------------------------------------------------------------------------
//
//  SwapStream.cpp      5/8/2020        DALang
//
//  Swap iostream, example cout with a file.
//
//-------------------------------------------------------------------------------------------------
//
// Author: Dennis Lang - 2020
// https://landenlabs.com
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

#include <iostream>

// ---------------------------------------------------------------------------
class SwapStream
{
public:
    SwapStream(std::ostream& inOriginal) :
        mOriginal(inOriginal),
        mOldBuffer(NULL)
    { }

    ~SwapStream()
    {
        restore();
    }

    void restore()
    {
        if (mOldBuffer)
            mOriginal.rdbuf(mOldBuffer);
        mOldBuffer = NULL;
    }

    void swap(std::ostream& inRedirect)
    {
        mOldBuffer = mOriginal.rdbuf(inRedirect.rdbuf());
    }

private:
    SwapStream(const SwapStream&);
    SwapStream& operator=(const SwapStream&);

    std::ostream&   mOriginal;
    std::streambuf* mOldBuffer;
};
