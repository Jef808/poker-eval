#ifndef DRAW_RANGE_HAND_H_
#define DRAW_RANGE_HAND_H_

#include <SFML/Graphics.hpp>

struct RangeHand {
  int rank1;  // 2..14
  int rank2;  // 2..14
  bool suited{false};
};

void drawRangeHand(const RangeHand& hand,
                   sf::RenderTarget& rt,
                   const sf::Font& font,
                   const sf::Vector2f& position,
                   const sf::Vector2f& size,
                   const sf::Color& backgroundColor);

#endif // DRAW_RANGE_HAND_H_
