#ifndef __ALU_H__
#define __ALU_H__

#include "DigitalCircuit.h"

#include <cassert>

class ALU : public DigitalCircuit
{

public:
  ALU(const Wire<4> *iALUControl,
      const Wire<32> *iInput0,
      const Wire<32> *iInput1,
      Wire<32> *oOutput,
      Wire<1> *oZero) : DigitalCircuit("ALU")
  {
    _iALUControl = iALUControl;
    _iInput0 = iInput0;
    _iInput1 = iInput1;
    _oOutput = oOutput;
    _oZero = oZero;
  }

  virtual void advanceCycle()
  {
    /* FIXME */
    unsigned long aluControlValue = _iALUControl->to_ulong();

    if (aluControlValue == 0)
    { // AND
      *_oOutput = *_iInput0;
      *_oOutput &= *_iInput1;
      if(_oOutput->to_ulong() == 0) _oZero->set();
      else _oZero->reset();
    }
    else if (aluControlValue == 1)
    {
      *_oOutput = *_iInput0;
      *_oOutput |= *_iInput1;
      if(_oOutput->to_ulong() == 0) _oZero->set();
      else _oZero->reset();
    }
    else if (aluControlValue == 2)
    {
      _oOutput->reset();

      unsigned tmp = 0;
      for (size_t i = 0; i < 32; i++)
      {
        if (_iInput0->test(i))
          tmp++;
        if (_iInput1->test(i))
          tmp++;

        _oOutput->set(i, (tmp % 2) == 1 ? true : false);
        tmp /= 2;
      }
      if(_oOutput->to_ulong() == 0) _oZero->set();
      else _oZero->reset();
    }
    else if (aluControlValue == 6)
    {
      Wire<32> complement;
      _oOutput->reset();
      complement = *_iInput1;
      complement.flip();

      unsigned tmp = 1;
      for (size_t i = 0; i < 32; i++)
      {
        if (complement.test(i))
        {
          tmp++;
        }
        complement.set(i, (tmp % 2) == 1 ? true : false);
        tmp /= 2;
      }

      tmp = 0;
      for (size_t i = 0; i < 32; i++)
      {
        if (_iInput0->test(i))
          tmp++;
        if (complement.test(i))
          tmp++;

        _oOutput->set(i, (tmp % 2) == 1 ? true : false);
        tmp /= 2;
      }

      if(_oOutput->to_ulong() == 0) _oZero->set();
      else _oZero->reset();
    }
    else if (aluControlValue == 7)
    {
      int i;
      for(i = 31; i >= 0 && _iInput0->test(i) == _iInput1->test(i); i--);

      if(i == -1){
        _oOutput->reset();
        _oOutput->set(0);
        _oZero->reset();
      }
      else if(_iInput0->test(i)){
        _oOutput->reset();
        _oZero->set();
      }
      else{
        _oOutput->reset();
        _oOutput->set(0);
        _oZero->reset();
      }
    }
    else if (aluControlValue == 12)
    {
      *_oOutput = (*_iInput0 | *_iInput1).flip();
      if(_oOutput->to_ulong() == 0) _oZero->set();
      else _oZero->reset();
    }
  }

private:
  const Wire<4> *_iALUControl;
  const Wire<32> *_iInput0;
  const Wire<32> *_iInput1;
  Wire<32> *_oOutput;
  Wire<1> *_oZero;
};

#endif
