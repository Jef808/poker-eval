#ifndef BITSET_RANKINDEX_H_
#define BITSET_RANKINDEX_H_

#include <bit>
#include <cstdint>
#include <array>
#include <vector>

class BitsetRankIndex {
public:
  BitsetRankIndex(uint32_t max_value, const std::vector<uint32_t>& keys);
  bool contains(uint32_t k) const;
  uint32_t index(uint32_t k) const;
  uint32_t size() const;

private:
  std::vector<uint64_t> bits_;
  std::vector<uint32_t> prefix_;
  uint32_t n_{0};
};

#endif // BITSET_RANKINDEX_H_
