#ifndef RANGE_EVALUATION_H_
#define RANGE_EVALUATION_H_

#include "types.h"
#include "hand_range.hpp"
#include "evaluation.hpp"

#include <cstdint>
#include <vector>

class RangeEvaluator {
public:

  RangeEvaluator();

  EvalResult evaluate(const std::pair<uint32_t, uint32_t>& hand, const HandRange& range);

  template <typename InputIterator>
  void set_board(InputIterator begin, InputIterator end) {
    m_evaluator.set_board(begin, end);
  }

private:
  Evaluator m_evaluator;
};

#endif // RANGE_EVALUATION_H_
