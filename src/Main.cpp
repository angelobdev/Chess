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
    char fenBuffer[256] = STANDARD_FEN;
    Chess::Game chess(gameWindow, fenString);
    if (!ImGui::SFML::Init(controlWindow))
    {
        std::cerr << "Failed to initialize ImGui!" << std::endl;
    }

    // GAME LOOP // TODO: Optimize rendering (only on event)
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
            else if (event->is<sf::Event::MouseButtonPressed>())
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

        // RESET
        ImGui::TextColored(ImColor(255, 255, 128), "Reset");
        if (ImGui::Button("Reset", ImVec2(240, 24)))
        {
            chess.restart(fenString);
        };
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // RESET WITH FEN
        ImGui::TextColored(ImColor(255, 255, 128), "Restart with FEN");
        ImGui::InputText("FEN", fenBuffer, IM_ARRAYSIZE(fenBuffer));
        if (ImGui::Button("Restart", ImVec2(240, 24)))
        {
            sf::String newFen = fenBuffer;
            // TODO: Validate
            chess.restart(newFen);
        };
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // INFORMATIONS
        ImGui::TextColored(ImColor(255, 255, 128), "Informations:");
        ImGui::Text("Turn: %s", chess.getCurrentTurn() == Chess::Piece::Color::White ? "White" : "Black");
        ImGui::TextColored(ImColor(255, 255, 128), "Scores:");
        ImGui::Text("White: %u", chess.getWhiteScore());
        ImGui::Text("Black: %u", chess.getBlackScore());
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::End();

        controlWindow.clear();
        ImGui::SFML::Render(controlWindow);
        controlWindow.display();
    }
}
