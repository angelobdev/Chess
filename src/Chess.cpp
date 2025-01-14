#include "Chess.h"

Chess::Game::Game(sf::RenderWindow &window, sf::Color whiteColor, sf::Color blackColor)
    : m_Pieces(), m_FenBuffer(STANDARD_FEN),
      m_WhiteColor(whiteColor), m_BlackColor(blackColor),
      m_WhiteScore(0), m_BlackScore(0)
{

    m_BoardSize = window.getSize().y;
    m_GUIOffset = window.getSize().x - m_BoardSize;
    float tileSize = m_BoardSize / 8.0f;

    m_Tile = sf::RectangleShape(sf::Vector2f(tileSize, tileSize));

    m_SelectedBox = sf::RectangleShape(sf::Vector2f(tileSize, tileSize));
    m_SelectedBox.setFillColor(sf::Color::Transparent);
    m_SelectedBox.setOutlineColor(sf::Color::Red);
    m_SelectedBox.setOutlineThickness(2.0f);

    m_PossibleMoveBox = sf::RectangleShape(sf::Vector2f(tileSize, tileSize));
    m_PossibleMoveBox.setFillColor(sf::Color(0x00ff0055));

    restart();
}

void Chess::Game::resize(const sf::Vector2u newSize)
{
    m_BoardSize = newSize.y;
    m_GUIOffset = newSize.x - m_BoardSize;
    float tileSize = m_BoardSize / 8.0f;

    m_Tile.setSize(sf::Vector2f(tileSize, tileSize));
    m_SelectedBox.setSize(sf::Vector2f(tileSize, tileSize));
    m_PossibleMoveBox.setSize(sf::Vector2f(tileSize, tileSize));

    for (auto piece : m_Pieces)
    {
        if (piece != nullptr)
            piece->resizeSprite(m_Tile.getSize());
    }
}

void Chess::Game::restart()
{
    // Clearing
    for (int i = 0; i < 64; i++)
    {
        if (m_Pieces[i] != nullptr)
        {
            delete m_Pieces[i];
            m_Pieces[i] = nullptr;
        }
    }

    // Updating board by parsing FEN string
    auto index = 0;
    for (auto symbol : m_FenBuffer)
    {
        if (symbol == '/')
            continue;

        if (symbol == ' ')
            index = 64;

        if (index < 64)
        {
            // Placing pieces
            if (std::isdigit(symbol))
            {
                index += symbol - '0';
            }
            else
            {
                auto posIndex = (7 - index / 8) * 8 + index % 8;
                m_Pieces[posIndex] = new Piece(symbol, m_Tile);
                index++;
            }
        }
        else
        {
            // Parsing game info
            if (symbol == 'w')
            {
                m_CurrentTurn = Piece::Color::White;
            }
            else if (symbol == 'b')
            {
                m_CurrentTurn = Piece::Color::Black;
            }

            // TODO:
        }
    }

    // Resetting game
    m_CurrentSelectedIndex = -1;
    m_WhiteScore = 0;
    m_BlackScore = 0;
}

void Chess::Game::handleClick(sf::Vector2i mousePos)
{
    if (mousePos.x <= m_GUIOffset)
        return;

    int file = (mousePos.x - m_GUIOffset) / static_cast<int>(m_Tile.getSize().x);
    int rank = 7 - (mousePos.y / static_cast<int>(m_Tile.getSize().y));
    int targetIndex = rank * 8 + file;

    Piece *targetPiece = m_Pieces[targetIndex];

    if (currentSelectedPiece() == nullptr)
    {
        // No piece is currently selected
        if (targetPiece != nullptr && targetPiece->getColor() == m_CurrentTurn)
        {
            calculatePossibleMoves(targetIndex);
            m_CurrentSelectedIndex = targetIndex; // Selecting...
        }
    }
    else
    {
        // There's a selected piece
        if (targetPiece == nullptr)
        {
            // Moving to an empty spot
            if (std::find(m_Movements.begin(), m_Movements.end(), targetIndex) == m_Movements.end())
            {
                return; // Target index is not a valid move
            }

            // Check for castling
            if (currentSelectedPiece()->getType() == Piece::Type::King && !currentSelectedPiece()->hasMoved())
            {
                int fileDiff = targetIndex % 8 - m_CurrentSelectedIndex % 8;
                if (std::abs(fileDiff) == 2)
                {
                    // Castling move
                    int rookStartIndex = (fileDiff > 0) ? m_CurrentSelectedIndex + 3 : m_CurrentSelectedIndex - 4;
                    int rookEndIndex = (fileDiff > 0) ? m_CurrentSelectedIndex + 1 : m_CurrentSelectedIndex - 1;

                    if (m_Pieces[rookStartIndex] != nullptr && m_Pieces[rookStartIndex]->getType() == Piece::Type::Rook && !m_Pieces[rookStartIndex]->hasMoved())
                    {
                        swapPieces(rookStartIndex, rookEndIndex);
                        m_Pieces[rookEndIndex]->setMoved(true);
                    }
                }
            }
            else
            {
                // Check for en passant
                if (currentSelectedPiece()->getType() == Piece::Type::Pawn)
                {
                    int startRank = indexToRank(m_CurrentSelectedIndex);
                    int targetRank = indexToRank(targetIndex);

                    if (abs(startRank - targetRank) == 2)
                    {
                        currentSelectedPiece()->setEnPassantVulnerable(true);
                    }
                    else
                    {
                        currentSelectedPiece()->setEnPassantVulnerable(false);
                    }

                    // Capture en passant
                    int enPassantTarget = targetIndex + (m_CurrentTurn == Piece::Color::White ? -8 : 8);
                    if (targetPiece == nullptr && std::find(m_Movements.begin(), m_Movements.end(), targetIndex) != m_Movements.end() && m_Pieces[enPassantTarget] != nullptr && m_Pieces[enPassantTarget]->getType() == Piece::Type::Pawn && m_Pieces[enPassantTarget]->isEnPassantVulnerable())
                    {
                        if (currentSelectedPiece()->getColor() == Piece::Color::White)
                        {
                            m_WhiteScore += m_Pieces[enPassantTarget]->getValue();
                        }
                        else
                        {
                            m_BlackScore += m_Pieces[enPassantTarget]->getValue();
                        }

                        delete m_Pieces[enPassantTarget];
                        m_Pieces[enPassantTarget] = nullptr;
                    }
                }
            }

            currentSelectedPiece()->setMoved(true);
            swapPieces(m_CurrentSelectedIndex, targetIndex);
            m_CurrentSelectedIndex = -1;

            switchTurn();
        }
        else
        {
            // Selecting an occupied spot
            if (targetPiece->getColor() != currentSelectedPiece()->getColor())
            {
                // Different color: capturing
                if (std::find(m_Movements.begin(), m_Movements.end(), targetIndex) == m_Movements.end())
                {
                    return; // Target index is not a valid move
                }

                // Updating scores
                if (currentSelectedPiece()->getColor() == Piece::Color::White)
                {
                    m_WhiteScore += targetPiece->getValue();
                }
                else
                {
                    m_BlackScore += targetPiece->getValue();
                }

                delete m_Pieces[targetIndex];
                m_Pieces[targetIndex] = nullptr;

                currentSelectedPiece()->setMoved(true);

                swapPieces(m_CurrentSelectedIndex, targetIndex);
                m_CurrentSelectedIndex = -1;

                switchTurn();
            }
            else
            {
                // Same color: switching piece
                calculatePossibleMoves(targetIndex);
                m_CurrentSelectedIndex = targetIndex;
            }
        }
    }
}

void Chess::Game::prepareGUI()
{
    // Preparing UI
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(m_GUIOffset, m_BoardSize));

    ImGui::Begin("Control Panel", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    // RESET
    ImGui::TextColored(ImColor(255, 255, 128), "Reset");
    if (ImGui::Button("Reset"))
    {
        m_FenBuffer = STANDARD_FEN;
        restart();
    };
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // RESET WITH FEN
    ImGui::TextColored(ImColor(255, 255, 128), "Restart with FEN");
    ImGui::InputText("FEN", m_FenBuffer.data(), m_FenBuffer.size());
    if (ImGui::Button("Restart"))
    {
        // TODO: Validate
        restart();
    };
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // INFORMATIONS
    ImGui::TextColored(ImColor(255, 255, 128), "Informations:");
    ImGui::Text("Turn: %s", m_CurrentTurn == Chess::Piece::Color::White ? "White" : "Black");
    ImGui::TextColored(ImColor(255, 255, 128), "Scores:");
    ImGui::Text("White: %u", m_WhiteScore);
    ImGui::Text("Black: %u", m_BlackScore);
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::End();
}

void Chess::Game::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform.translate(sf::Vector2f(m_GUIOffset, 0));

    // Rendering Board
    for (int i = 0; i < 64; i++)
    {
        int file = indexToFile(i);
        int rank = indexToRank(i);

        // Drawing tile
        m_Tile.setPosition(sf::Vector2f(file * m_Tile.getSize().x, (7 - rank) * m_Tile.getSize().y));
        m_Tile.setFillColor((file + rank) % 2 == 0 ? m_BlackColor : m_WhiteColor);
        target.draw(m_Tile, states);

        // Drawing piece
        auto piece = m_Pieces[i];
        if (piece != nullptr)
        {
            piece->setPosition(sf::Vector2f(file * m_Tile.getSize().x, (7 - rank) * m_Tile.getSize().y));
            target.draw(*piece, states);
        }
    }

    // Drawing overlays
    if (m_CurrentSelectedIndex != -1)
    {
        int file = indexToFile(m_CurrentSelectedIndex);
        int rank = indexToRank(m_CurrentSelectedIndex);

        // Selected overlay
        m_SelectedBox.setPosition(sf::Vector2f(file * m_SelectedBox.getSize().x, (7 - rank) * m_SelectedBox.getSize().y));
        target.draw(m_SelectedBox, states);

        // Movements overlays
        for (auto movePos : m_Movements)
        {
            int moveFile = indexToFile(movePos);
            int moveRank = indexToRank(movePos);

            m_PossibleMoveBox.setPosition(sf::Vector2f(moveFile * m_PossibleMoveBox.getSize().x, (7 - moveRank) * m_PossibleMoveBox.getSize().y));
            target.draw(m_PossibleMoveBox, states);
        }
    }
}
