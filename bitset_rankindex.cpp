#include "bitset_rankindex.h"

BitsetRankIndex::BitsetRankIndex(uint32_t max_value, const std::vector<uint32_t>& keys) {
  size_t words = (static_cast<size_t>(max_value) + 64) / 64;  // make room for bit 63
  bits_.assign(words, 0);

  // Set membership bits
  for (uint32_t k : keys) {
    size_t w = k >> 6;
    uint32_t b = k & 63;
    bits_[w] |= (uint64_t(1) << b);
  }

  // Build prefix popcounts (store number of bits turned on in 0..i-1)
  prefix_.resize(words);
  uint32_t running = 0;
  for (size_t i = 0; i < words; ++i) {
    prefix_[i] = running;
    running += std::popcount(bits_[i]);
  }
  n_ = running;  // Should equal keys.size()
}

bool BitsetRankIndex::contains(uint32_t k) const {
  size_t w = k >> 6;
  uint32_t b = k & 63;
  if (w >= bits_.size()) return false;
  return (bits_[w] >> b) & 1u;
}

uint32_t BitsetRankIndex::operator()(uint32_t k) const {
  size_t w = k >> 6;
  uint32_t b = k & 63;
  uint64_t mask = (b == 0) ? 0ull : ((uint64_t(1) << b) - 1);
  return prefix_[w] + std::popcount(bits_[w] & mask);
}

uint32_t BitsetRankIndex::size() const { return n_; }
