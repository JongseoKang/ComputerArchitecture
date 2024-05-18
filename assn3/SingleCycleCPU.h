#ifndef __SINGLE_CYCLE_CPU_H__
#define __SINGLE_CYCLE_CPU_H__

#include "DigitalCircuit.h"

#include "ALU.h"
#include "ALUControl.h"
#include "Control.h"
#include "RegisterFile.h"
#include "Memory.h"

template<size_t N>
class MUX : public DigitalCircuit {

  public:

    MUX(const std::string &name,
        const Wire<N> *iInput0,
        const Wire<N> *iInput1,
        const Wire<1> *iSelect,
        Wire<N> *oOutput) : DigitalCircuit(name) {
      _iInput0 = iInput0;
      _iInput1 = iInput1;
      _iSelect = iSelect;
      _oOutput = oOutput;
    }

    virtual void advanceCycle() {
      /* FIXME */
      if(_iSelect->test(0))
        *_oOutput = *_iInput1;
      else
        *_oOutput = *_iInput0;
    }

  private:

    const Wire<N> *_iInput0;
    const Wire<N> *_iInput1;
    const Wire<1> *_iSelect;
    Wire<N> *_oOutput;

};

class SingleCycleCPU : public DigitalCircuit {

  public:

    SingleCycleCPU(const std::string name,
                   const std::uint32_t initialPC,
                   const char *regFileName,
                   const char *instMemFileName,
                   const char *dataMemFileName) : DigitalCircuit(name) {
      _currCycle = 0;

      _alwaysHi = 1;
      _alwaysLo = 0;

      _PC = initialPC;

      /* FIXME: setup various sequential/combinational circuits and wires as needed */

      _instMemory = new Memory("instMemory", 
        &_PC, &_PC, &_alwaysHi, &_alwaysLo, &_instMemInstruction, 
        Memory::LittleEndian, instMemFileName);

      _muxRegFileWriteRegister = new MUX<5>("muxRegFileWriteRegister", 
        &_muxRegFileWriteRegisterInput0, &_muxRegFileWriteRegisterInput1, 
        &_ctrlRegDst, &_muxRegFileWriteRegisterOutput);
      
      _muxRegFileWriteData = new MUX<32>("muxRegFileWriteData",
        &_aluResult, &_dataMemReadData, &_ctrlMemToReg, &_regFileWriteData);

      _registerFile = new RegisterFile(&_regFileReadRegister1, 
        &_regFileReadRegister2, &_muxRegFileWriteRegisterOutput, &_regFileWriteData, 
        &_ctrlRegWrite, &_regFileReadData1, &_regFileReadData2, regFileName);

      _muxALUInput1 = new MUX<32>("muxALUInput1", 
        &_regFileReadData2, &_signExtendOutput,
        &_ctrlALUSrc, &_muxALUInput1Output);

      _alu = new ALU(&_aluCtrlOp, &_regFileReadData1, &_muxALUInput1Output,
        &_aluResult, &_aluZero);

      _dataMemory = new Memory("dataMemory", &_aluResult, 
        &_regFileReadData2, &_ctrlMemRead, &_ctrlMemWrite,
        &_dataMemReadData, Memory::LittleEndian, dataMemFileName);

      _control = new Control(&_ctrlOpcode, &_ctrlRegDst, &_ctrlALUSrc, 
        &_ctrlMemToReg, &_ctrlRegWrite, &_ctrlMemRead, &_ctrlMemWrite,
        &_ctrlBranch, &_ctrlALUOp);

      _aluControl = new ALUControl(&_ctrlALUOp, &_aluCtrlFunct, &_aluCtrlOp);
      
      _muxPC = new MUX<32>("muxPC", &_muxPCInput0, &_muxPCInput1, &_muxPCSelect, &_PC);
    }

    void printPVS() {
      printf("==================== Cycle %lu ====================\n", _currCycle);
      printf("PC = 0x%08lx\n", _PC.to_ulong());
      printf("Registers:\n");
      _registerFile->printRegisters();
      printf("Data Memory:\n");
      _dataMemory->printMemory();
      printf("Instruction Memory:\n");
      _instMemory->printMemory();
    }

    virtual void advanceCycle() {
      _currCycle += 1;

      /* FIXME: implement the single-cycle behavior of the single-cycle MIPS CPU */

      // instruction fetch
      _instMemory->advanceCycle();


      // instruction decode
      std::string instruction = _instMemInstruction.to_string();
      _ctrlOpcode = Wire<6>(instruction.substr(0, 6));
      _regFileReadRegister1 = Wire<5>(instruction.substr(6, 5));
      _regFileReadRegister2 = _muxRegFileWriteRegisterInput0 = Wire<5>(instruction.substr(11, 5));
      _muxRegFileWriteRegisterInput1 = Wire<5>(instruction.substr(16, 5));
      _signExtendInput = Wire<16>(instruction.substr(16, 16));
      _aluCtrlFunct = Wire<6>(instruction.substr(26, 6));


      // register file virtual subcycle
      _ctrlRegWrite.reset();
      _registerFile->advanceCycle();      // one more time after mem load

      // control unit
      _control->advanceCycle();
      _muxRegFileWriteRegister->advanceCycle();


      // alu control
      _aluControl->advanceCycle();

      // alu
      _signExtendOutput = Wire<32>(_signExtendInput.to_ulong());
      _muxALUInput1->advanceCycle();
      _alu->advanceCycle();

      // data memory
      _dataMemory->advanceCycle();
      _muxRegFileWriteData->advanceCycle();

      // fed data to write register
      _registerFile->advanceCycle();

      // determine value of PC
      for (int tmp = 1, i = 2; i < 32; i++)
      {
        if (_PC.test(i))
          tmp++;
        _PC.set(i, (tmp % 2) == 1 ? true : false);
        tmp /= 2;
      }
      _muxPCInput0 = _PC;
      
      _signExtendOutput <<= 2;
      for (int tmp = 0, i = 0; i < 32; i++)
      {
        if (_PC.test(i))
          tmp++;
        if(_signExtendOutput.test(i))
          tmp++;
        _PC.set(i, (tmp % 2) == 1 ? true : false);
        tmp /= 2;
      }

      _muxPCInput1 = _PC;
      _muxPCSelect = _ctrlBranch & _aluZero;
      _muxPC->advanceCycle();
    }

    ~SingleCycleCPU() {
      delete _instMemory;
      delete _registerFile;
      delete _dataMemory;

      delete _control;
      delete _aluControl;
      delete _alu;
      delete _muxRegFileWriteRegister;
      delete _muxALUInput1;
      delete _muxRegFileWriteData;
      delete _muxPC;
    }

  private:

    // Cycle tracker
    std::uint64_t _currCycle;

    // Always-1/0 wires
    Wire<1> _alwaysHi;
    Wire<1> _alwaysLo;

    // Program Counter (PC) register
    Register<32> _PC;

    // Sequential circuits
    Memory *_instMemory;
    RegisterFile *_registerFile;
    Memory *_dataMemory;

    // Combinational circuits
    Control *_control;
    ALUControl *_aluControl;
    ALU *_alu;
    MUX<5> *_muxRegFileWriteRegister;
    MUX<32> *_muxALUInput1;
    MUX<32> *_muxRegFileWriteData;
    MUX<32> *_muxPC;

    // instruction[31-0] from the instruction memory
    Wire<32> _instMemInstruction;
    // the control signals to/from the Control unit
    Wire<6> _ctrlOpcode;
    Wire<1> _ctrlRegDst, _ctrlALUSrc, _ctrlMemToReg, _ctrlRegWrite, _ctrlMemRead, _ctrlMemWrite, _ctrlBranch;
    Wire<2> _ctrlALUOp;
    // the wires to/from the MUX in front of the Register File's Write Register
    Wire<5> _muxRegFileWriteRegisterInput0, _muxRegFileWriteRegisterInput1, _muxRegFileWriteRegisterOutput;
    // the wires to/from the Register File
    Wire<5> _regFileReadRegister1, _regFileReadRegister2;
    Wire<32> _regFileWriteData, _regFileReadData1, _regFileReadData2;
    // the wires to/from the Sign-extend unit
    Wire<16> _signExtendInput;
    Wire<32> _signExtendOutput;
    // the wires from the MUX in front of the ALU's second input
    Wire<32> _muxALUInput1Output;
    // the wires to/from the ALU control unit
    Wire<6> _aluCtrlFunct;
    Wire<4> _aluCtrlOp;
    // the wires from the ALU
    Wire<32> _aluResult;
    Wire<1> _aluZero;
    // the wire from the data memory
    Wire<32> _dataMemReadData;
    // the wires to/from the MUX in front of the PC register
    Wire<32> _muxPCInput0, _muxPCInput1;
    Wire<1> _muxPCSelect; // a.k.a. PCSrc

};

#endif

