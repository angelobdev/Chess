#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

#include "Chess.h"

int main()
{
    // Game window (SQUARED)
    auto gameWindow = sf::RenderWindow(sf::VideoMode({600u, 600u}), "Chess Game!");
    gameWindow.setFramerateLimit(60);

    // Control Panel / Information window
    auto controlWindow = sf::RenderWindow(sf::VideoMode({256u, 600u}), "Chess Info!");
    controlWindow.setFramerateLimit(60);

    // Initializing Game and UI
    sf::String fenString = STANDARD_FEN;
    Chess::Game chess(gameWindow, fenString);
    if (!ImGui::SFML::Init(controlWindow))
    {
        std::cerr << "Failed to initialize ImGui!" << std::endl;
    }

    // GAME LOOP
    sf::Clock deltaClock;
    while (gameWindow.isOpen() && controlWindow.isOpen())
    {
        // Handling game window events
        while (const std::optional event = gameWindow.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                gameWindow.close();
            }

            if (event->is<sf::Event::MouseButtonPressed>())
            {
                chess.handleClick(sf::Mouse::getPosition(gameWindow));
            }
        }

        // Rendering
        gameWindow.clear();
        gameWindow.draw(chess);
        gameWindow.display();

        // Handling UI window events
        while (const std::optional event = controlWindow.pollEvent())
        {
            ImGui::SFML::ProcessEvent(controlWindow, *event);
            if (event->is<sf::Event::Closed>())
            {
                controlWindow.close();
            }
        }

        ImGui::SFML::Update(controlWindow, deltaClock.restart());

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(256, 600));

        ImGui::Begin("Control Panel", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

        ImGui::TextColored(ImColor(255, 255, 128), "Control Panel");
        if (ImGui::Button("Restart", ImVec2(240, 24)))
        {
            chess.restart(fenString);
        };

        ImGui::Separator();

        ImGui::TextColored(ImColor(255, 255, 128), "Current situation:");
        ImGui::Text("Turn: %s", chess.getCurrentTurn() == Chess::Piece::Color::White ? "White" : "Black");

        ImGui::TextColored(ImColor(255, 255, 128), "Scores:");
        ImGui::Text("White: %u", chess.getWhiteScore());
        ImGui::Text("Black: %u", chess.getBlackScore());

        ImGui::End();

        controlWindow.clear();
        ImGui::SFML::Render(controlWindow);
        controlWindow.display();
    }
}
