//-------------------------------------------------------------------------------------------------
//
// File: directory.cpp   Author: Dennis Lang  Desc: Get files from directories
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

#include "directory.h"


#ifdef HAVE_WIN

const lstring ANY("\\*");
lstring Directory_files::SLASH = "\\";
lstring Directory_files::EXTN = ".";

//-------------------------------------------------------------------------------------------------
// Return true if attribute is a Directory
inline static bool isDir(DWORD attr)
{
    return (attr != -1) && ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

//-------------------------------------------------------------------------------------------------
// Return 'clean' full path, remove extra slahes.
static lstring& GetFullPath(lstring& fname)
{
    char fullPath[MAX_PATH];
    DWORD len1 = GetFullPathName(fname, ARRAYSIZE(fullPath), fullPath, NULL);
    fname = fullPath;
    return fname;
}

//-------------------------------------------------------------------------------------------------

Directory_files::Directory_files(const lstring& dirName) :
    my_dir_hnd(INVALID_HANDLE_VALUE),
    my_dirName(dirName)
{ 
}

//-------------------------------------------------------------------------------------------------

Directory_files::~Directory_files()
{
    if (my_dir_hnd != INVALID_HANDLE_VALUE)
        FindClose(my_dir_hnd);
}

//-------------------------------------------------------------------------------------------------

void Directory_files::close()
{
    if (my_dir_hnd != INVALID_HANDLE_VALUE)
    {
        FindClose(my_dir_hnd);
        my_dir_hnd = INVALID_HANDLE_VALUE;
    }
}

//-------------------------------------------------------------------------------------------------

bool Directory_files::begin()
{
    close();

    lstring dir = my_dirName;
    if (dir.empty())    
        dir = ".";    // Default to current directory
   
    DWORD attr = GetFileAttributes(dir);
    if (isDir(attr))
    {
        dir += ANY;
    }
    else // if (attr != INVALID_FILE_ATTRIBUTES)
    {
        GetFullPath(my_dirName);
        // Peel off one subdir from reference name.
        size_t pos = my_dirName.find_last_of(":/\\");
        if (pos != std::string::npos)
            my_dirName.resize(pos);
    }

    my_dir_hnd = FindFirstFile(dir, &my_dirent);
    bool is_more = (my_dir_hnd != INVALID_HANDLE_VALUE);

    while (is_more
        && (isDir(my_dirent.dwFileAttributes)
            && strspn(my_dirent.cFileName, ".") == strlen(my_dirent.cFileName) ))
    {
        is_more = (FindNextFile(my_dir_hnd, &my_dirent) != 0);
    }

    return is_more;
}

//-------------------------------------------------------------------------------------------------

bool Directory_files::more()
{
    if (my_dir_hnd == INVALID_HANDLE_VALUE)
        return begin();

    bool is_more = false;
    if (my_dir_hnd != INVALID_HANDLE_VALUE)
    {
        // Determine if there any more files
        //   skip any dot-directories.
        do
        {
            is_more = (FindNextFile(my_dir_hnd, &my_dirent) != 0);
        } while (is_more
            && (isDir(my_dirent.dwFileAttributes)
            && strspn(my_dirent.cFileName, ".") == strlen(my_dirent.cFileName)));

    }

    return is_more;
}

//-------------------------------------------------------------------------------------------------

bool Directory_files::is_directory() const
{
   return (my_dir_hnd != INVALID_HANDLE_VALUE && isDir(my_dirent.dwFileAttributes));
}

//-------------------------------------------------------------------------------------------------

const char* Directory_files::name() const
{
    return (my_dir_hnd != INVALID_HANDLE_VALUE) ?
        my_dirent.cFileName : NULL; 
}

//-------------------------------------------------------------------------------------------------

lstring& Directory_files::fullName(lstring& fname) const
{
    fname = my_dirName + SLASH + name();
    return GetFullPath(fname);
}


#else

#include <unistd.h>
#include <stdlib.h>

lstring Directory_files::SLASH = "/";
lstring Directory_files::EXTN = ".";

//-------------------------------------------------------------------------------------------------

Directory_files::Directory_files(const lstring& dirName)
{
    realpath(dirName.c_str(), my_fullname);
    my_baseDir = my_fullname;
    errno = 0;
    my_pDir = opendir(my_baseDir);
    my_is_more = (my_pDir != NULL && errno == 0);
}

//-------------------------------------------------------------------------------------------------

Directory_files::~Directory_files()
{
    if (my_pDir != NULL)
        closedir(my_pDir);
}

//-------------------------------------------------------------------------------------------------

bool Directory_files::more()
{
    if (my_is_more)
    {
        my_pDirEnt = readdir(my_pDir);
        my_is_more = my_pDirEnt != NULL;
        if (my_is_more)
        {
            if (my_pDirEnt->d_type == DT_DIR)
            {
                while (my_is_more &&
                       (my_pDirEnt->d_name[0] == '.' && !isalnum(my_pDirEnt->d_name[1])))
                {
                    more();
                }
            }
        }
    }
    
    return my_is_more;
}

//-------------------------------------------------------------------------------------------------

bool Directory_files::is_directory() const
{
    return my_pDirEnt->d_type == DT_DIR;
}

//-------------------------------------------------------------------------------------------------

lstring& Directory_files::fullName(lstring& fname) const
{
    return join(fname, my_baseDir, my_pDirEnt->d_name);
}
#endif

//-------------------------------------------------------------------------------------------------

lstring& Directory_files::join(lstring& outFull, const char* dir, const char* name)
{
    
    outFull = dir;
    outFull += SLASH + name;
    // return realpath(fname.c_str(), my_fullname);
    return outFull;
    // return GetFullPath(fname);
}


//-------------------------------------------------------------------------------------------------
lstring getPartDir(const char* filepath) {
    lstring result = filepath;
    size_t endDir = result.find_last_of(Directory_files::SLASH);
    if (endDir != string::npos)
        result = result.substr(0, endDir);
    else
        result = "";
    return result;
}

lstring getPartName(const char* filepath) {
    lstring result = filepath;
    size_t endDir = result.find_last_of(Directory_files::SLASH);
    if (endDir != string::npos)
        result = result.substr(endDir+1);
    size_t endName = result.find_last_of(Directory_files::EXTN);
    if (endName != string::npos)
        result.resize(endName);
    return result;
}

lstring getPartExt(const char* filepath) {
    lstring result = filepath;
    size_t pos = result.find_last_of(Directory_files::EXTN);
    return (pos != std::string::npos) ? result.substr(pos) : result.substr(0,0);
}


lstring  Directory_files::parts(const char* filepath, bool dir, bool name, bool ext)
{
    // #include <filesystem>    
    // std::filesystem::path pathParts = filepath;
    
    lstring result;
    if (dir) {
        result += getPartDir(filepath);
    }
    if (name) {
        result += getPartName(filepath);
    }
    if (ext) {
        result += getPartExt(filepath);
    }
    return result;
}




