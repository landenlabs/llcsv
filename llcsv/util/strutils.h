//
//  strutils.h
//  llcsv
//
//  Created by Dennis Lang on 4/26/20.
//  Copyright © 2020 Dennis Lang. All rights reserved.
//

#pragma once

inline
std::string& remove(std::string& str, const char* find) {
    while (*find) {
        size_t pos = str.find(*find++);
        if (pos != std::string::npos) {
            str.erase(pos, 1);
        }
    }
    return str;
}

inline
std::string& removeWrapper(std::string& str, const char* find) {
    while (*find) {
        if (str[0] == *find++) {
            str.erase(0, 1);
            str.erase(str.length()-1, 1);
            break;
        }
    }
    return str;
}

inline
bool containsCase(const std::string& str, const std::string& val) {
    return strstr(str.c_str(), val.c_str()) != nullptr;
}
inline
bool containsNoCase(const std::string& str, const std::string& val) {
    return strcasestr(str.c_str(), val.c_str()) != nullptr;
}

inline
bool equalsCase(const std::string& str, const std::string& val) {
    return str == val;
}
inline
bool equalsNoCase(const std::string& str, const std::string& val) {
    return strcasecmp(str.c_str(), val.c_str());
}
 
inline
double toDbl(const std::string& str) {
    return strtod(str.c_str(), nullptr);
}

 
template<typename TT>
bool notEqualNum(TT srcVal, TT matVal) {
    return srcVal != matVal;
}
 
template<typename TT>
bool equalNum(TT srcVal, TT matVal) {
    return srcVal = matVal;
}
 
template<typename TT>
bool lessNum(TT srcVal, TT matVal) {
    return srcVal < matVal;
}
 
template<typename TT>
bool lessEqualNum(TT srcVal, TT matVal) {
    return srcVal <= matVal;
}
 
template<typename TT>
bool greaterNum(TT srcVal, TT matVal) {
    return srcVal > matVal;
}
 
template<typename TT>
bool greaterEqualNum(TT srcVal, TT matVal) {
    return srcVal >= matVal;
}
