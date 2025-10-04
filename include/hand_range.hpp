#ifndef HAND_RANGE_H_
#define HAND_RANGE_H_

#include <cassert>
#include <cstdint>
#include <vector>

struct WeightedHand {
  std::pair<uint32_t, uint32_t> hand;
  float weight;
};

class HandRange {
public:
  HandRange();

  void addHand(uint32_t card1, uint32_t card2, float weight = 1.f) {
    assert(0.f <= weight && weight <= 1.f && "Weight must be between 0 and 1");

    m_hands.push_back({{card1, card2}, weight});
  }

  void clear() {
    m_hands.clear();
  }

  const auto& hands() const { return m_hands; }

private:
  std::vector<WeightedHand> m_hands;
};


#endif // HAND_RANGE_H_
