#include "Chess.h"

constexpr auto WINDOW_WIDTH = 856u;
constexpr auto WINDOW_HEIGHT = 600u;

int main()
{
    // Game window (SQUARED)
    auto window = sf::RenderWindow(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Chess!", sf::Style::Close | sf::Style::Resize);
    window.setMinimumSize(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT));
    window.setFramerateLimit(60);

    // Initializing Game and UI
    Chess::Game chess(window);
    if (!ImGui::SFML::Init(window))
    {
        std::cerr << "Failed to initialize ImGui!" << std::endl;
    }

    // GAME LOOP
    sf::Clock deltaClock;
    while (window.isOpen())
    {
        // Handling events
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
                break;
            }
            else if (event->is<sf::Event::MouseButtonPressed>())
            {
                chess.handleClick(sf::Mouse::getPosition(window));
            }
            else if (event->is<sf::Event::Resized>())
            {
                window.setView(sf::View(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(window.getSize().x, window.getSize().y))));
                chess.resize(window.getSize());
                sf::String newTitle = "Chess " + std::to_string(window.getSize().x) + 'x' + std::to_string(window.getSize().y);
                window.setTitle(newTitle);
            }

            ImGui::SFML::ProcessEvent(window, *event);
        }

        // Preparing GUI
        ImGui::SFML::Update(window, deltaClock.restart());
        chess.prepareGUI();

        // Rendering
        window.clear();
        window.draw(chess);
        ImGui::SFML::Render(window);
        window.display();
    }
}
