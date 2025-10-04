#include "range_evaluation.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <random>

static std::random_device rd;
static std::mt19937 g(rd());
static BitsetRankIndex hash{MAX_HASH_KEY, KEYS};

RangeEvaluator::RangeEvaluator()
  : m_deck{initialize_deck()} {
  m_deck_nodup.reserve(52);
}

void RangeEvaluator::set_num_simulations(size_t n) {
  num_simulations = n;
}

EvalResult RangeEvaluator::evaluate(const std::pair<uint32_t, uint32_t>& hand, const HandRange& range) {
  std::array<uint32_t, 4> hands = {hand.first, hand.second, 0, 0};

  size_t total_hands = 0;
  float total_win_prob = 0.f;
  float total_tie_prob = 0.f;

  for (size_t i = 0; i < range.hands().size(); ++i) {
    const auto& wh = range.hands()[i];
    std::pair<uint32_t, uint32_t> h = wh.hand;
    float w = wh.weight;

    if (w == 0.f) continue;

    hands[2] = h.first;
    hands[3] = h.second;

    m_evaluator.set_hands(hands.begin(), hands.end());

    auto result = m_evaluator.evaluate();

    total_win_prob += result.win_prob * weight;
    total_tie_prob += result.tie_prob * weight;
    total_hands += weight;
  }

  if (total_hands > 0) {
    total_win_prob /= total_hands;
    total_tie_prob /= total_hands;
  }

  EvalResult result;
  result.win_prob = total_win_prob;
  result.tie_prob = total_tie_prob;

  return result;
}
