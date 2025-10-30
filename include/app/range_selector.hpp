#ifndef RANGE_SELECTOR_H_
#define RANGE_SELECTOR_H_

#include "app/draw_range_hand.hpp"

#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>

class RangeSelector {
public:
  RangeSelector(const sf::Font& font);

  void update(const sf::Vector2f& position,
              const sf::Vector2f& cardSize,
              float gap = 1.f);

  void draw(sf::RenderTarget& rt);

  void toggleSelectHand(const RangeHand& hand);

  std::optional<RangeHand> getHandAtPosition(const sf::Vector2f& mousePos) const;

  const std::vector<RangeHand>& getSelectedHands() const;

  void reset();

private:
  const sf::Font& font_;
  std::array<std::pair<RangeHand, bool>, 13 * 13> rangeHands_;
  sf::Vector2f position_;
  sf::Vector2f cardSize_;
  float gap_;
  mutable std::vector<RangeHand> selectedHands_;
};

#endif // RANGE_SELECTOR_H_
