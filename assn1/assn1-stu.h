#ifndef __ASSN1_STU_H__
#define __ASSN1_STU_H__
#include <iostream>
#include "assn1.h"

using namespace std;
template<size_t N>
void NANDGate<N>::advanceCycle() {
  /* FIXME */
  _output->reset();
  // _inputs[0]->flip();
  // _inputs[1]->flip();

  *_output = ~(*_inputs[0] & *_inputs[1]);
}

template<size_t N>
void SignedAdder<N>::advanceCycle() {
  /* FIXME */
  _output->reset();

  unsigned tmp = 0;
  for(size_t i = 0; i < N; i++) {
    if (_inputs[0]->test(i)) { tmp++; }
    if (_inputs[1]->test(i)) { tmp++; }

    _output->set(i, (tmp % 2) == 1 ? true : false);

    tmp /= 2;
  }
}

template<size_t N>
void SignedSubtractor<N>::advanceCycle() {
  /* FIXME */
  Wire<N> data[2];
  _output->reset();
  data[0] = *_inputs[0];
  data[1] = *_inputs[1];
  data[1].flip();

  unsigned tmp = 1;
  for(size_t i = 0; i < N; i++) {
    if (data[1].test(i)) { tmp++; }
    data[1].set(i, (tmp % 2) == 1 ? true : false);
    tmp /= 2;
  }

  tmp = 0;
  for(size_t i = 0; i < N; i++) {
    if (data[0].test(i)) { tmp++; }
    if (data[1].test(i)) { tmp++; }

    _output->set(i, (tmp % 2) == 1 ? true : false);

    tmp /= 2;
  }
}

template<size_t N>
void LogicalUnit<N>::advanceCycle() {
  /* FIXME */
  _output->reset();

  if (*_operation == 0b000) {
    *_output = *_inputs[0] & *_inputs[1];
  } else if (*_operation == 0b001) {
    *_output = *_inputs[0] | *_inputs[1];
  } else if (*_operation == 0b010) {
    *_output = *_inputs[0] ^ *_inputs[1];
  } else if (*_operation == 0b011) {
    *_output = ~(*_inputs[0]);
  } else if (*_operation == 0b100) {
    *_output = ~(*_inputs[0] & *_inputs[1]);
  } else if (*_operation == 0b101) {
    *_output = ~(*_inputs[0] | *_inputs[1]);
  } else if (*_operation == 0b110) {
    *_output = ~(*_inputs[0] ^ *_inputs[1]);
  }
}

template<size_t N, size_t M>
void HashTable<N, M>::advanceCycle() {
  /* FIXME */
  if(*_isWrite == 0b0){
    *_output = _entries[_index->to_ullong()];
  }
  else{
    _entries[_index->to_ullong()] = *_input;
    _output->reset();
  }
}

template<size_t N, size_t D>
void DelayQueue<N, D>::advanceCycle() {
  /* FIXME */
  *_output = _entries[D - 1];
  for(int i = D - 1; i > 0; i--){
    _entries[i] = _entries[i - 1];
  }

  if(*_isPush == 0b0){
    _entries[0].reset();
  }
  else{
    _entries[0] = *_input;
  }
}

#endif

