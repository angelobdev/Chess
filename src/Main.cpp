#include "Chess.h"

int main()
{
    // Game window (SQUARED)
    auto window = sf::RenderWindow(sf::VideoMode({856u, 600u}), "Chess!");
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
