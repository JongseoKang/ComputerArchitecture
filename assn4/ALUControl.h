#ifndef __ALU_CONTROL_H__
#define __ALU_CONTROL_H__

#include "DigitalCircuit.h"

#include <cassert>

class ALUControl : public DigitalCircuit
{

public:
  ALUControl(const Wire<2> *iALUOp,
             const Wire<6> *iFunct,
             Wire<4> *oOperation) : DigitalCircuit("ALUControl")
  {
    _iALUOp = iALUOp;
    _iFunct = iFunct;
    _oOperation = oOperation;
  }

  virtual void advanceCycle()
  {
    _oOperation->reset();
    if (!(_iALUOp->test(0)) && !(_iALUOp->test(1)))
      _oOperation->set(1);
    else if (_iALUOp->test(0))
    {
      _oOperation->set(1);
      _oOperation->set(2);
    }
    else if (!(_iFunct->test(0)) && !(_iFunct->test(1)) &&
      !(_iFunct->test(2)) && !(_iFunct->test(3)))
      _oOperation->set(1);
    else if (!(_iFunct->test(0)) && _iFunct->test(1) &&
      !(_iFunct->test(2)) && !(_iFunct->test(3)))
    {
      _oOperation->set(1);
      _oOperation->set(2);
    }
    else if (!(_iFunct->test(0)) && !(_iFunct->test(1)) &&
      _iFunct->test(2) && !(_iFunct->test(3)));
    
    else if (_iFunct->test(0) && !(_iFunct->test(1)) &&
      _iFunct->test(2) && !(_iFunct->test(3)))
      _oOperation->set(0);
    else if (!(_iFunct->test(0)) && _iFunct->test(1) &&
      !(_iFunct->test(2)) && _iFunct->test(3))
    {
      _oOperation->set(0);
      _oOperation->set(1);
      _oOperation->set(2);
    }
  }

private:
  const Wire<2> *_iALUOp;
  const Wire<6> *_iFunct;
  Wire<4> *_oOperation;
};

#endif
