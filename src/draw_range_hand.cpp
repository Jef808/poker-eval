#include "app/draw_range_hand.hpp"

#include <sstream>
#include <string>

inline char rank_to_char(int rank) {
  switch (rank) {
    case 14: return 'A';
    case 13: return 'K';
    case 12: return 'Q';
    case 11: return 'J';
    case 10: return 'T';
    default: return '0' + rank; // Assumes rank is between 2 and 9
  }
}

std::string to_string(const RangeHand& rh) {
  std::ostringstream oss;
  oss << rank_to_char(rh.rank1);
  oss << rank_to_char(rh.rank2);
  if (rh.rank1 == rh.rank2) {
    return oss.str();  // pair
  }
  if (rh.suited) {
    oss << 's';
  } else {
    oss << 'o';
  }
  return oss.str();
}

void drawRangeHand(const RangeHand& hand,
                   sf::RenderTarget& rt,
                   const sf::Font& font,
                   const sf::Vector2f& position,
                   const sf::Vector2f& size,
                   const sf::Color& backgroundColor) {
  sf::RectangleShape container{size};
  container.setPosition(position);
  container.setFillColor(backgroundColor);
  rt.draw(container);

  sf::Text text(font);
  text.setFillColor(sf::Color::Black);
  text.setCharacterSize(static_cast<unsigned int>(size.x / 3.f));
  text.setString(to_string(hand));

  const auto textPos = position + size / 2.f - text.getLocalBounds().size / 2.f;
  text.setPosition(textPos - sf::Vector2f{0.f, 4.f});

  rt.draw(text);
}
