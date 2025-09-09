#include "app/reset_button.hpp"

ResetButton::ResetButton(const sf::Vector2f& position, const sf::Vector2f& size)
  : position_(position), size_(size) {}

void ResetButton::draw(sf::RenderTarget& rt) const {
  sf::RectangleShape button(size_);
  button.setPosition(position_);
  button.setFillColor(sf::Color(200, 0, 0)); // Red button
  rt.draw(button);

  sf::Font font("resources/DejaVuSans.ttf");
  sf::Text buttonText(font);
  buttonText.setString("Clear");
  buttonText.setCharacterSize(static_cast<unsigned int>(size_.y * 0.6f));
  buttonText.setFillColor(sf::Color::White);

  const float textWidth = buttonText.getLocalBounds().size.x;
  const float textHeight = buttonText.getLocalBounds().size.y;

  buttonText.setPosition({
      position_.x + (size_.x - textWidth) / 2.f,
      position_.y + (size_.y - textHeight) / 2.f - 5.f
    });

  rt.draw(buttonText);
}

bool ResetButton::isClicked(const sf::Vector2i& mousePos) const {
  return mousePos.x >= position_.x && mousePos.x <= position_.x + size_.x &&
    mousePos.y >= position_.y && mousePos.y <= position_.y + size_.y;
}
