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
  : m_deck{initialize_deck()} {
  m_deck_nodup.reserve(52);
}

void MonteCarlo::simulate_flop(uint16_t* results) {
  // Loop over all combos of turn and river
  for (const auto& c : c45_2) {
    uint32_t turn = m_deck_nodup[c[0]];
    uint32_t river = m_deck_nodup[c[1]];

    // Add turn and river to each hand
    for (auto& hand : m_hands) {
      hand[5] = turn;
      hand[6] = river;
    }

    // Evaluate each hand
    for (const auto& hand : m_hands) {
      *results++ = eval7(hash, hand);
    }
  }
}

void MonteCarlo::simulate_turn(uint16_t* results) {
  for (size_t i = 0; i < m_deck_nodup.size(); ++i) {
    uint32_t river = m_deck_nodup[i];

    // Add river to each hand
    for (auto& hand : m_hands) {
      hand[6] = river;
    }

    // Evaluate each hand
    for (const auto& hand : m_hands) {
      *results++ = eval7(hash, hand);
    }
  }
}

void MonteCarlo::simulate(uint16_t* results, size_t num_simulations) {
  assert(!m_hands.empty());

  // Copy board cards to each hand
  for (auto& hand : m_hands) {
    for (size_t j = 0; j < m_board.size(); ++j) {
      hand[2 + j] = m_board[j];
    }
  }

  // Populate the deck without known cards
  m_deck_nodup.clear();
  std::copy_if(m_deck.begin(), m_deck.end(), std::back_inserter(m_deck_nodup), [&](uint32_t card) {
    return std::none_of(m_hands.begin(), m_hands.end(), [&, card](const auto& hand) {
      return std::find(hand.begin(), hand.begin() + 2 + m_board.size(), card) != hand.begin() + 2 + m_board.size();
    });
  });

  if (m_board.size() == 3) {
    return simulate_flop(results);
  } else if (m_board.size() == 4) {
    return simulate_turn(results);
  }

  for (size_t i = 0; i < num_simulations; ++i) {
    // Shuffle the deck
    // TODO: Check if a simpler shuffle is significantly faster
    // TODO: Maybe use the whole deck before reshuffling
    std::shuffle(m_deck_nodup.begin(), m_deck_nodup.end(), g);
    int cards_to_deal = 5 - m_board.size();
    int num_cards = 2 + m_board.size();

    // Deal cards from shuffled deck
    for (int j = 0; j < cards_to_deal; ++j) {
      uint32_t card = m_deck_nodup[j];
      for (auto& hand : m_hands) {
        hand[num_cards] = card;
      }
      ++num_cards;
    }

    // Evaluate each hand
    for (const auto& hand : m_hands) {
      *results++ = eval7(hash, hand);
    }
  }
}
