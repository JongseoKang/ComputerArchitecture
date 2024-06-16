#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "DigitalCircuit.h"

#include <cassert>
#include <cstdio>

#define MEMORY_SIZE (32 * 1024 * 1024) // <-- 32-MB memory

class Memory : public DigitalCircuit {

  public:

    enum Endianness { LittleEndian, BigEndian };

    Memory(const std::string &name,
           const Wire<32> *iAddress,
           const Wire<32> *iWriteData,
           const Wire<1> *iMemRead,
           const Wire<1> *iMemWrite,
           Wire<32> *oReadData,
           const Endianness endianness,
           const char *initFileName = nullptr) : DigitalCircuit(name) {
      _endianness = endianness;
      _iAddress = iAddress;
      _iWriteData = iWriteData;
      _oReadData = oReadData;
      _iMemRead = iMemRead;
      _iMemWrite = iMemWrite;

      _memory = new std::bitset<8>[MEMORY_SIZE];

      if (initFileName != nullptr) {
        // Each line of the memory initialization file consists of:
        //   - the starting memory address of a 32-bit data in hexadecimal value
        //   - the eight-digit hexadecimal value of the data
        // e.g., "1000 ABCD1234" stores 0xABCD1234 from 0x1000 to 0x1003.
        FILE *initFile = fopen(initFileName, "r");
        assert(initFile != NULL);
        std::uint32_t addr, value;
        while (fscanf(initFile, " %x %x", &addr, &value) == 2) {
          printf("INFO: memory[0x%08lx..0x%08lx] <-- 0x%08lx\n",
                 (unsigned long)addr, (unsigned long)(addr + 3),
                 (unsigned long)value);
          if (_endianness == LittleEndian) {
            _memory[addr + 0] = (std::uint8_t)(value % 0x100); value >>= 8;
            _memory[addr + 1] = (std::uint8_t)(value % 0x100); value >>= 8;
            _memory[addr + 2] = (std::uint8_t)(value % 0x100); value >>= 8;
            _memory[addr + 3] = (std::uint8_t)(value % 0x100);
          } else { // _endianness == BigEndian
            _memory[addr + 3] = (std::uint8_t)(value % 0x100); value >>= 8;
            _memory[addr + 2] = (std::uint8_t)(value % 0x100); value >>= 8;
            _memory[addr + 1] = (std::uint8_t)(value % 0x100); value >>= 8;
            _memory[addr + 0] = (std::uint8_t)(value % 0x100);
          }
        }
        fclose(initFile);
      }
    }

    void printMemory() {
      for (size_t i = 0; i < MEMORY_SIZE; i += 4) {
        if (_memory[i].any() || _memory[i + 1].any() || _memory[i + 2].any()
            || _memory[i + 3].any()) {
          std::uint32_t value = 0;
          value += (_memory[i + 3].to_ulong()); value <<= 8;
          value += (_memory[i + 2].to_ulong()); value <<= 8;
          value += (_memory[i + 1].to_ulong()); value <<= 8;
          value += (_memory[i + 0].to_ulong());
          printf("  memory[0x%08lx..0x%08lx] = 0x%08lx\n", i + 3, i,
                 (unsigned long)value);
        }
      }
    }

    virtual void advanceCycle() {
      /* FIXME */
      unsigned long memAddr = _iAddress->to_ulong();
      std::string dataStr(""), str0, str1, str2, str3;
      
      if(_iMemRead->test(0)){
        str0 = _memory[memAddr].to_string();
        str1 = _memory[memAddr + 1].to_string();
        str2 = _memory[memAddr + 2].to_string();
        str3 = _memory[memAddr + 3].to_string();

        if(_endianness == LittleEndian){
          dataStr.append(str3);
          dataStr.append(str2);
          dataStr.append(str1);
          dataStr.append(str0);
        }
        else if(_endianness == BigEndian){
          dataStr.append(str0);
          dataStr.append(str1);
          dataStr.append(str2);
          dataStr.append(str3);
        }
        *_oReadData = Wire<32>(dataStr);
      }
      if(_iMemWrite->test(0)){
        dataStr = _iWriteData->to_string();
        if(_endianness == LittleEndian){
          str0 = dataStr.substr(0, 8);
          str1 = dataStr.substr(8, 8);
          str2 = dataStr.substr(16, 8);
          str3 = dataStr.substr(24, 8);     
        }
        else if(_endianness == BigEndian){
          str3 = dataStr.substr(0, 8);
          str2 = dataStr.substr(8, 8);
          str1 = dataStr.substr(16, 8);
          str0 = dataStr.substr(24, 8);     
        }
        _memory[memAddr] = std::bitset<8>(str3);
        _memory[memAddr + 1] = std::bitset<8>(str2);
        _memory[memAddr + 2] = std::bitset<8>(str1);
        _memory[memAddr + 3] = std::bitset<8>(str0);
      }
    }

    ~Memory() {
      delete[] _memory;
    }

  private:

    const Wire<32> *_iAddress;
    const Wire<32> *_iWriteData;
    const Wire<1> *_iMemRead;
    const Wire<1> *_iMemWrite;
    Wire<32> *_oReadData;

    Endianness _endianness;
    std::bitset<8> *_memory;

};

#endif
