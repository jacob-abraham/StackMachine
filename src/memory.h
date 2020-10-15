#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdlib.h>
#include <stdint.h>
#include <map>
#include "stringhelper.h"
#include <exception>
#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>

#if defined(__i386__) || defined(__x86_64__)
#include <immintrin.h>
  #if defined(__AVX512F__)
  #define _USEAVX512
  #endif
  #if defined(__AVX2__)
  #define _USEAVX2
  #endif
  #if defined(__SSE2__)
  #define _USESSE2
  #endif
#endif



typedef size_t ADDRESS;
typedef int64_t WORD;
typedef int64_t IMMEDIATE;
typedef int64_t LABEL;
typedef std::map<LABEL, ADDRESS> AddressMap;

class MemoryError: public std::exception {
public:
  std::string message;

  MemoryError(std::string msg) : message(msg) {}

  MemoryError() : message("Unknown") {}

  virtual const char* what() const throw()
  {
    //const ref trick to allow for copy so string isnt local stack memory
    const std::string& ret = getString();
    return ret.c_str();
  }

  friend std::ostream& operator<<(std::ostream& out, const MemoryError& me) {
    out << me.getString();
    return out;
  }
  std::string getString() const {
    std::stringstream str;
    str << "Memory Error: " << this->message;
    return str.str();
  }
  std::string getString() {
    return const_cast<const MemoryError*>(this)->getString();
  }
};



namespace MemoryParser {
  IMMEDIATE getImmediate(std::string);
  WORD getWord(std::string);

  extern std::vector<std::string> stringTable;
  LABEL getLabel(std::string);
  std::string getLabelName(LABEL);
}


class Memory {

public:
  explicit Memory(size_t, ADDRESS, ADDRESS);
  ~Memory();

  ADDRESS addData(WORD);
  ADDRESS addText(WORD);


  bool setData(ADDRESS, WORD);
  WORD getData(ADDRESS);
  WORD getText(ADDRESS);


  bool setData(LABEL, WORD);
  WORD getData(LABEL);
  WORD getText(LABEL);

  bool addLabel(LABEL, ADDRESS);
  bool addDataLabel(LABEL);
  bool addTextLabel(LABEL);

  ADDRESS getAddress(LABEL);

  std::string getDataString();
  std::string getTextString();
  std::string getMemString();
  std::string getMemSectionString(size_t,size_t);


private:
  size_t size;
  WORD* mem;

  ADDRESS data_start_addr;
  ADDRESS text_start_addr;
  ADDRESS data_offset_addr;
  ADDRESS text_offset_addr;

  size_t data_size;
  size_t text_size;

  AddressMap labels;

  bool inAddressMap(LABEL);

};


#endif
