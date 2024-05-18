#ifndef __CONTROL_H__
#define __CONTROL_H__

#include "DigitalCircuit.h"

class Control : public DigitalCircuit {

  public:

    Control(const Wire<6> *iOpcode,
            Wire<1> *oRegDst,
            Wire<1> *oALUSrc,
            Wire<1> *oMemToReg,
            Wire<1> *oRegWrite,
            Wire<1> *oMemRead,
            Wire<1> *oMemWrite,
            Wire<1> *oBranch,
            Wire<2> *oALUOp) : DigitalCircuit("Control") {
      _iOpcode = iOpcode;
      _oRegDst = oRegDst;
      _oALUSrc = oALUSrc;
      _oMemToReg = oMemToReg;
      _oRegWrite = oRegWrite;
      _oMemRead = oMemRead;
      _oMemWrite = oMemWrite;
      _oBranch = oBranch;
      _oALUOp = oALUOp;
    }

    virtual void advanceCycle() {
      unsigned long opcode = _iOpcode->to_ulong();

      if(opcode == 0b000000){       // R type (add, sub, and, or, nor, slt)
        _oRegDst->set();
        _oALUSrc->reset();
        _oMemToReg->reset();
        _oRegWrite->set();
        _oMemRead->reset();
        _oMemWrite->reset();
        _oBranch->reset();
        _oALUOp->set(1);
        _oALUOp->reset(0);
      }
      else if(opcode == 0b100011){  // lw
        _oRegDst->reset();
        _oALUSrc->set();
        _oMemToReg->set();
        _oRegWrite->set();
        _oMemRead->set();
        _oMemWrite->reset();
        _oBranch->reset();
        _oALUOp->reset(1);
        _oALUOp->reset(0);
      }
      else if(opcode == 0b101011){  // sw
        _oRegDst->reset();
        _oALUSrc->set();
        _oMemToReg->reset();
        _oRegWrite->reset();
        _oMemRead->reset();
        _oMemWrite->set();
        _oBranch->reset();
        _oALUOp->reset(1);
        _oALUOp->reset(0);
      }
      else if(opcode == 0b000100){  // beq
        _oRegDst->reset();
        _oALUSrc->reset();
        _oMemToReg->reset();
        _oRegWrite->reset();
        _oMemRead->reset();
        _oMemWrite->reset();
        _oBranch->set();
        _oALUOp->reset(1);
        _oALUOp->set(0);
      }
      else if(opcode == 0b001000){  // addi
        _oRegDst->reset();    //
        _oALUSrc->set();      //
        _oMemToReg->reset();  //
        _oRegWrite->set();    //
        _oMemRead->reset();   //
        _oMemWrite->reset();  //
        _oBranch->reset();    //
        _oALUOp->set(1);      //
        _oALUOp->reset(0);    //
      }
    }

  private:

    const Wire<6> *_iOpcode;
    Wire<1> *_oRegDst;
    Wire<1> *_oALUSrc;
    Wire<1> *_oMemToReg;
    Wire<1> *_oRegWrite;
    Wire<1> *_oMemRead;
    Wire<1> *_oMemWrite;
    Wire<1> *_oBranch;
    Wire<2> *_oALUOp;

};

#endif

