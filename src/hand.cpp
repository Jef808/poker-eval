#include "app/hand.hpp"
#include "app/draw_card.hpp"

Hand::Hand(const sf::Vector2f& position, const sf::Vector2f& cardSize)
  : position_(position), cardSize_(cardSize) {}

void Hand::draw(sf::RenderTarget& rt) const {
  const sf::Vector2f size_ = {cardSize_.x * 2.f + 5.f, cardSize_.y};
  sf::RectangleShape handRect(size_);
  handRect.setPosition(position_);
  handRect.setFillColor(sf::Color(0, 100, 0)); // Dark green
  rt.draw(handRect);

  for (int i = 0; i < 2; ++i) {
    if (cards_[i].has_value()) {
      drawCard(*cards_[i], rt, position_ + sf::Vector2f{i * (cardSize_.x + 5.f), 0.f}, cardSize_);
    } else {
      sf::RectangleShape cardBack(cardSize_);
      cardBack.setPosition(position_ + sf::Vector2f{i * (cardSize_.x + 5.f), 0.f});
      cardBack.setFillColor(sf::Color::Blue);
      rt.draw(cardBack);
    }
  }
}

void Hand::setCard(int index, const Card& card) {
  if (index < 0 || index >= 2) return;
  cards_[index] = card;
}

const std::optional<Card>& Hand::getCard(size_t i) const {
  return cards_[i];
}

void Hand::reset() {
  cards_[0].reset();
  cards_[1].reset();
}
