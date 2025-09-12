#ifndef EVALUATION_H_
#define EVALUATION_H_

#include <array>
#include <vector>
#include <cstdint>
#include <iterator>
#include <cassert>

class Evaluator {
public:
  Evaluator();
  /**
   * Simulate num_simulations poker hands and store results in results array.
   *
   * The results array must be large enough to hold num_simulations * number_of_hands values.
   * Return the actual number of simulations performed, so probabilities can be computed.
   */
  size_t simulate(uint16_t* results, size_t num_simulations);

  const auto& hands() const { return m_hands; }
  const auto& board() const { return m_board; }

  /**
   * Set the hands for the simulation.
   *
   * InputIterator should be an iterator to a sequence of uint32_t card values,
   * where each pair of values represents a player's hand (2 cards per player).
   */
  template <typename InputIterator>
  void set_hands(InputIterator begin, InputIterator end) {
    assert(std::distance(begin, end) >= 2 && std::distance(begin, end) % 2 == 0);
    m_hands.clear();
    for (auto it = begin; it != end; it += 2) {
      m_hands.emplace_back(std::array<uint32_t, 7>{*it, *(it + 1), 0, 0, 0, 0, 0});
    }
  };

  template <typename InputIterator>
  void set_board(InputIterator begin, InputIterator end) {
    assert(std::distance(begin, end) <= 5);
    m_board.clear();
    for (auto it = begin; it != end; ++it) {
      m_board.push_back(*it);
    }
  }

  void set_board() {
    m_board.clear();
  }

private:
  std::array<uint32_t, 52> m_deck;
  std::vector<std::array<uint32_t, 7>> m_hands;
  std::vector<uint32_t> m_board;
  std::vector<uint32_t> m_deck_nodup;

  void simulate_flop(uint16_t* results);
  void simulate_turn(uint16_t* results);
};

#endif // EVALUATION_H_
