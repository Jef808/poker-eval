#ifndef RESET_BUTTON_H_
#define RESET_BUTTON_H_

#include <SFML/Graphics.hpp>

class ResetButton {
  public:
    ResetButton(const sf::Vector2f& position, const sf::Vector2f& size);

    void draw(sf::RenderTarget& rt) const;
    bool isClicked(const sf::Vector2i& mousePos) const;

  private:
    sf::Vector2f position_;
    sf::Vector2f size_;
};

#endif // RESET_BUTTON_H_
