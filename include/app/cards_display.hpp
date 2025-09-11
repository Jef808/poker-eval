#ifndef CARDS_DISPLAY_H_
#define CARDS_DISPLAY_H_

#include <SFML/Graphics.hpp>
#include <array>
#include <optional>
#include "app/card.hpp"
#include "app/draw_card.hpp"


template <size_t N>
class CardsDisplay {
  public:
    CardsDisplay(const sf::Vector2f& position, const sf::Vector2f& cardSize);

    void draw(sf::RenderTarget& rt) const;
    void setCard(int index, const Card& card);
    const std::optional<Card>& getCard(size_t i) const;
    size_t size() const;
    void reset();

    const std::array<std::optional<Card>, N>& getCards() const { return cards_; }

  private:
    sf::Vector2f position_;
    sf::Vector2f cardSize_;
    std::array<std::optional<Card>, N> cards_;
};


using Hand = CardsDisplay<2>;
using Board = CardsDisplay<5>;


template <size_t N>
CardsDisplay<N>::CardsDisplay(const sf::Vector2f& position, const sf::Vector2f& cardSize)
  : position_(position), cardSize_(cardSize) {
    reset();
}

template <size_t N>
void CardsDisplay<N>::draw(sf::RenderTarget& rt) const {
  const sf::Vector2f size_ = {cardSize_.x * static_cast<float>(N) + 5.f * static_cast<float>(N - 1), cardSize_.y};
  sf::RectangleShape handRect(size_);
  handRect.setPosition(position_);
  handRect.setFillColor(sf::Color(0, 100, 0)); // Dark green
  rt.draw(handRect);

  for (size_t i = 0; i < N; ++i) {
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

template <size_t N>
void CardsDisplay<N>::setCard(int index, const Card& card) {
  if (index < 0 || static_cast<size_t>(index) >= N) return;
  cards_[index] = card;
}

template <size_t N>
const std::optional<Card>& CardsDisplay<N>::getCard(size_t i) const {
  return cards_[i];
}

template <size_t N>
size_t CardsDisplay<N>::size() const {
  for (size_t i = 0; i < N; ++i) {
    if (!cards_[i].has_value()) {
      return i;
    }
  }
}

template <size_t N>
void CardsDisplay<N>::reset() {
  for (size_t i = 0; i < N; ++i) {
    cards_[i].reset();
  }
}


#endif // CARDS_DISPLAY_H_
