
#include "memory.h"



IMMEDIATE MemoryParser::getImmediate(std::string x) {
  return std::stoll(x);
}
WORD MemoryParser::getWord(std::string x) {
  return std::stoll(x);
}

std::vector<std::string> MemoryParser::stringTable;
LABEL MemoryParser::getLabel(std::string x) {
  //find existing string
  for(size_t i = 0; i < MemoryParser::stringTable.size(); i++) {
    if(iStrEqual(MemoryParser::stringTable[i],x)) {
      return i;
    }
  }
  //add and return string
  MemoryParser::stringTable.push_back(x);
  return MemoryParser::stringTable.size()-1;
}




Memory::Memory(size_t _size,
               ADDRESS _data_start,
               ADDRESS _text_start): size(_size),
                                    mem(NULL),
                                    data_start_addr(_data_start),
                                    text_start_addr(_text_start),
                                    data_offset_addr(0),
                                    text_offset_addr(0),
                                    data_size(_text_start-_data_start),
                                    text_size(_size-_text_start),
                                    labels() {
  mem = (int64_t *)malloc(sizeof(int64_t)*size);

  //init mem to 0
  size_t i = 0;
#if defined(_USEAVX512)
  __m512i zero_8 = _mm512_set1_epi64(0);
  for(;i < size; i+=16) {
    j =
    _mm512_store_epi64((__m512i*)(mem+i),zero_8);
    _mm512_store_epi64((__m512i*)(mem+i+8),zero_8);
  }
#elif defined(_USEAVX2)
  __m256i zero_4 = _mm256_set1_epi64x(0);
  for(;i < size; i+=8) {
    _mm256_store_si256((__m256i*)(mem+i),zero_4);
    _mm256_store_si256((__m256i*)(mem+i+4),zero_4);
  }
#elif defined(_USESSE2)
  __m128i zero_2 = _mm_set1_epi64x(0);
  for(;i < size; i+=4) {
    _mm_store_si128((__m128i*)(mem+i),zero_2);
    _mm_store_si128((__m128i*)(mem+i+2),zero_2);
  }
#endif
  //handle base case of unalgined or not intel ISA
  for(; i < size; i++) {
    mem[i] = 0;
  }
}
Memory::~Memory() {
  free(mem);
}

//add data to first available address
ADDRESS Memory::addData(WORD x) {
  //if we have space for data
  if(data_start_addr+data_offset_addr < text_start_addr) {
    //set the data and increase the offset
    mem[data_start_addr+data_offset_addr] = x;
    data_offset_addr++;

    //successfully added, return address
    return data_start_addr+data_offset_addr-1;
  }
  //no more space
  throw MemoryError("No more memory");
}
//add text to first available address
ADDRESS Memory::addText(WORD x) {
  //if we have space for text
  if(text_start_addr+text_offset_addr < size) {
    //set the text and increase the offset
    mem[text_start_addr+text_offset_addr] = x;
    text_offset_addr++;

    //successfully added, return address
    return text_start_addr+text_offset_addr-1;
  }
  //no more space
  throw MemoryError("No more instruction space");
}

//set data at address
bool Memory::setData(ADDRESS addr, WORD x) {
  //if address in range
  if(addr > data_start_addr && addr < data_start_addr+data_offset_addr) {
    mem[addr] = x;
    return true;
  }
  throw MemoryError("Invalid memory access");
  return false;
}

WORD Memory::getData(ADDRESS addr) {
  //if address in range, get it
  if(addr > data_start_addr && addr < data_start_addr+data_offset_addr) {
    return mem[addr];
  }
  throw MemoryError("Invalid memory access");
  return 0;
}
WORD Memory::getText(ADDRESS addr) {
  //if address in range, get it
  if(addr > text_start_addr && addr < text_start_addr+text_offset_addr) {
    return mem[addr];
  }
  throw MemoryError("Invalid memory access");
  return 0;
}


//set data at a labeled address
bool Memory::setData(LABEL lbl, WORD x) {
  if(inAddressMap(lbl)) return setData(labels[lbl], x);
  throw MemoryError("No memory label defined");
  return false;
}
//get data at a labeled address
WORD Memory::getData(LABEL lbl) {
  if(inAddressMap(lbl)) return getData(labels[lbl]);
  throw MemoryError("No memory label defined");
  return 0;
}
//get text at labeled address
WORD Memory::getText(LABEL lbl) {
  if(inAddressMap(lbl)) return getText(labels[lbl]);
  throw MemoryError("No memory label defined");
  return 0;
}

//add labled address
bool Memory::addLabel(LABEL lbl, ADDRESS addr) {
  labels[lbl] = addr;
  //always add address, if its a bad address not my problem
  return true;
}
//label top level address
bool Memory::addLabel(LABEL lbl) {
  return addLabel(lbl, text_start_addr+text_offset_addr);
}

//get address for label
ADDRESS Memory::getAddress(LABEL lbl) {
  if(inAddressMap(lbl)) return labels[lbl];
  throw MemoryError("No memory label defined");
  return 0;
}

//private, is label already assigned
bool Memory::inAddressMap(LABEL lbl) {
  return labels.find(lbl) != labels.end();
}

std::string Memory::getDataString() {
  std::stringstream ss;
  ss << "Data Section" << std::endl;
  ss << getMemSectionString(data_start_addr, data_size);
  return ss.str();
}
std::string Memory::getTextString() {
  std::stringstream ss;
  ss << "Text Section" << std::endl;
  ss << getMemSectionString(text_start_addr, text_size);
  return ss.str();
}

std::string Memory::getMemString() {
  std::stringstream ss;
  ss << "Total Memory" << std::endl;
  ss << getMemSectionString(0, size);
  return ss.str();
}
std::string Memory::getMemSectionString(size_t start, size_t length) {
  std::stringstream ss;

  size_t const rowLength = 16;
  size_t const width = 4;

  //print header row
  ss << std::setw(width) << "" << " | ";
  for(size_t i = 0; i < rowLength; i++) {
    ss << std::setw(width) << i;
  }
  ss << std::endl;
  //3 is for " | "
  ss << std::string((width*(rowLength+1)+3), '-');
  ss << std::endl;


  //calculate start and end positions
  size_t index = start;
  size_t end = start + length;

  //keep printing rows until out of range
  for(size_t col = 0; index < end; col++) {

    size_t rowStart = index;
    //print col number
    ss << std::setw(width) << col << " | ";

    //print across the row as long as valid index
    for(; index < rowStart + rowLength &&
          index < end; index++) {
      ss << std::setw(width) << mem[index];
    }
    ss << std::endl;
  }
  return ss.str();
}