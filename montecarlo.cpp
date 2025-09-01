#include "montecarlo.h"
#include "types.h"
#include "utils.h"
#include "bitset_rankindex.h"
#include "eval.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <random>

static std::random_device rd;
static std::mt19937 g(rd());
static BitsetRankIndex hash{MAX_HASH_KEY, KEYS};

MonteCarlo::MonteCarlo()
  : m_deck{initialize_deck()} {}

void MonteCarlo::simulate(uint16_t* results, size_t num_simulations) {
  assert(!m_hands.empty());

  // Copy board cards to each hand
  for (auto& hand : m_hands) {
    for (size_t j = 0; j < m_board.size(); ++j) {
      hand[2 + j] = m_board[j];
    }
  }

  for (size_t i = 0; i < num_simulations; ++i) {
    // Shuffle the deck
    // TODO: Check if a simpler shuffle is significantly faster
    // TODO: Maybe use the whole deck before reshuffling
    std::shuffle(m_deck.begin(), m_deck.end(), g);
    int cards_to_deal = 5 - m_board.size();
    int num_cards = 2 + m_board.size();
    int deck_idx = 0;

    // Deal cards from shuffled deck
    while (cards_to_deal > 0) {
      uint32_t card = m_deck[deck_idx++];
      // Ensure no duplicate cards
      bool is_duplicate = std::any_of(m_hands.begin(), m_hands.end(), [&, card](const auto& hand) {
        return std::find(hand.begin(), hand.begin() + 2 + m_board.size(), card) != hand.begin() + 2 + m_board.size();
      });

      if (!is_duplicate) {
        // Add card to all hands
        for (auto& hand : m_hands) {
          hand[num_cards] = card;
        }
        --cards_to_deal;
        ++num_cards;
      }
    }

    // Evaluate each hand
    for (const auto& hand : m_hands) {
      *results++ = eval7(hash, hand);
    }
  }
}
