#ifndef DEBUG_DISPLAY_H_
#define DEBUG_DISPLAY_H_

#include <SFML/Graphics.hpp>
#include <string>

class DebugDisplay {
  public:
    DebugDisplay(const sf::Font& font, unsigned int characterSize = 14)
      : text_(font) {
        text_.setCharacterSize(characterSize);
        text_.setFillColor(sf::Color(200, 200, 200, 180));  // Semi-transparent light gray
        text_.setPosition({.0f, .0f});
    }

    void update(const std::string& msg) {
      text_.setString(msg);
    }

    void draw(sf::RenderTarget& rt) const {
        rt.draw(text_);
    }

  private:
    sf::Text text_;
};

#endif // DEBUG_DISPLAY_H_
