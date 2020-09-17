#ifndef __STRING_HELPER__
#define __STRING_HELPER__

#include <string>

//convert a character to upper case
inline char toUpper(char c) {
  return static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
}

//check if two strings are equal, ignoring case and asssuming only ascii
inline bool iStrEqual(std::string a, std::string b) {
  if(a.size() != b.size()) return false;
  for(size_t i = 0; i < a.size(); i++) {
    if(toUpper(a[i]) != toUpper(b[i])) return false;
  }
  return true;
}

#endif