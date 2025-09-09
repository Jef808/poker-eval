#include "app/card_selector.hpp"
#include "app/draw_card.hpp"
#include <algorithm>

CardSelector::CardSelector(const sf::Vector2f& position, const sf::Vector2f& cardSize)
  : position_(position), cardSize_(cardSize) {}

void CardSelector::draw(sf::RenderTarget& rt) const {
  sf::RectangleShape selectorRect({cardSize_.x * 13.f, cardSize_.y * 4.f});
  selectorRect.setOrigin({0.f, 0.f});
  selectorRect.setPosition(position_);
  selectorRect.setFillColor(sf::Color::Black);
  rt.draw(selectorRect);

  for (auto suit : {'h', 'd', 'c', 's'}) {
    for (int rank = 2; rank <= 14; ++rank) {
      const sf::Vector2f cardPosition = {
        position_.x + (rank - 2) * cardSize_.x,
        position_.y + (suit == 'h' ? 0.f : suit == 'd' ? cardSize_.y : suit == 'c' ? 2 * cardSize_.y : 3 * cardSize_.y)
      };
      drawCard({rank, suit}, rt, cardPosition, cardSize_ - sf::Vector2f{1.f, 1.f});
      if (std::find(selectedCards_.begin(), selectedCards_.end(), (rank - 2) + (suit == 'h' ? 0 : suit == 'd' ? 13 : suit == 'c' ? 26 : 39)) != selectedCards_.end()) {
        sf::RectangleShape highlight(cardSize_);
        highlight.setPosition(cardPosition);
        highlight.setFillColor(sf::Color(0, 40, 0, 200)); // Semi-transparent green
        rt.draw(highlight);
      }
    }
  }
}

std::optional<Card> CardSelector::getClickedCard(const sf::Vector2i& mousePos) const {
  sf::Vector2f relativePos = static_cast<sf::Vector2f>(mousePos) - position_;

  if (relativePos.x < 0 || relativePos.y < 0 || relativePos.x >= cardSize_.x * 13.f || relativePos.y >= cardSize_.y * 4.f) {
    return std::nullopt;
  }

  int col = static_cast<int>(relativePos.x / cardSize_.x);
  int row = static_cast<int>(relativePos.y / cardSize_.y);

  if (col < 0 || col >= 13 || row < 0 || row >= 4) {
    return std::nullopt;
  }

  int rank = col + 2;
  char suit;
  switch (row) {
  case 0: suit = 'h'; break;
  case 1: suit = 'd'; break;
  case 2: suit = 'c'; break;
  case 3: suit = 's'; break;
  default: return std::nullopt;
  }

  if (std::find(selectedCards_.begin(), selectedCards_.end(), (rank - 2) + (suit == 'h' ? 0 : suit == 'd' ? 13 : suit == 'c' ? 26 : 39)) != selectedCards_.end()) {
    return std::nullopt; // Card already selected
  }

  return Card{rank, suit};
}

void CardSelector::selectCard(const Card& card) {
  int index = (card.rank - 2) + (card.suit == 'h' ? 0 : card.suit == 'd' ? 13 : card.suit == 'c' ? 26 : 39);
  selectedCards_.push_back(index);
}

void CardSelector::reset() {
  selectedCards_.clear();
}
