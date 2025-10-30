#include "app/range_selector.hpp"
#include "app/draw_range_hand.hpp"

#include <array>
#include <optional>
#include <utility>
#include <vector>


RangeSelector::RangeSelector(const sf::Font& font)
  : font_{font} {
  for (int rank1 = 14; rank1 >= 2; --rank1) {
    rangeHands_[(14 - rank1) * 13 + (14 - rank1)] = {RangeHand{rank1, rank1}, false}; // Pairs
    for (int rank2 = rank1 - 1; rank2 >= 2; --rank2) {
      rangeHands_[(14 - rank1) * 13 + (14 - rank2)] = {RangeHand{rank1, rank2, true}, false};  // Suited
      rangeHands_[(14 - rank2) * 13 + (14 - rank1)] = {RangeHand{rank1, rank2, false}, false}; // Offsuit
    }
  }
}

void RangeSelector::update(const sf::Vector2f& position,
                           const sf::Vector2f& cardSize,
                           float gap) {
  position_ = position;
  cardSize_ = cardSize;
  gap_ = gap;
}

void RangeSelector::draw(sf::RenderTarget& rt) {
  const auto unSelectedColor = sf::Color(200, 200, 200);  // Light gray
  const auto selectedColor = sf::Color(100, 200, 100);    // Light green

  sf::RectangleShape container;
  container.setOrigin({0.f, 0.f});
  container.setSize({14.f * cardSize_.x + 13 * gap_, cardSize_.y + 13 * gap_});
  container.setPosition(position_);
  container.setFillColor(sf::Color::Black);
  rt.draw(container);

  sf::Vector2f pos;
  for (const auto& [rh, selected] : rangeHands_) {
    if (rh.suited) {
      pos = {
        position_.x + (14 - rh.rank2) * (cardSize_.x + gap_),
        position_.y + (14 - rh.rank1) * (cardSize_.y + gap_)
      };
    } else {
      pos = {
        position_.x + (14 - rh.rank1) * (cardSize_.x + gap_),
        position_.y + (14 - rh.rank2) * (cardSize_.y + gap_)
      };
    }

    const auto& backgroundColor = selected ? selectedColor : unSelectedColor;

    drawRangeHand(rh, rt, font_, pos, cardSize_, backgroundColor);
  }
}

void RangeSelector::toggleSelectHand(const RangeHand& hand) {
  auto row = 14 - hand.rank1;
  auto col = 14 - hand.rank2;
  if (!hand.suited) {
    std::swap(row, col);
  }
  rangeHands_[row * 13 + col].second = !rangeHands_[row * 13 + col].second;
}

std::optional<RangeHand> RangeSelector::getHandAtPosition(const sf::Vector2f& mousePos) const {
  sf::Vector2f relativePos = mousePos - position_;

  if (relativePos.x < 0 || relativePos.y < 0 ||
      relativePos.x >= (cardSize_.x + gap_) * 13.f ||
      relativePos.y >= (cardSize_.y + gap_) * 13.f) {
    return std::nullopt;
  }

  int col = static_cast<int>(relativePos.x / (cardSize_.x + gap_));
  int row = static_cast<int>(relativePos.y / (cardSize_.y + gap_));

  if (col < 0 || col >= 13 || row < 0 || row >= 13) {
    return std::nullopt;
  }

  return rangeHands_[row * 13 + col].first;
}

const std::vector<RangeHand>& RangeSelector::getSelectedHands() const {
  selectedHands_.clear();
  for (const auto& [rh, selected] : rangeHands_) {
    if (selected) {
      selectedHands_.push_back(rh);
    }
  }
  return selectedHands_;
}

void RangeSelector::reset() {
  for (auto& [rh, selected] : rangeHands_) {
    selected = false;
  }
}
