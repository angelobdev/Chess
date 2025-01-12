#include "Chess.h"

Chess::Game::Game(sf::RenderWindow &window, sf::String &fen, sf::Color whiteColor, sf::Color blackColor)
    : m_Pieces(),
      m_WhiteColor(whiteColor), m_BlackColor(blackColor),
      m_WhiteScore(0), m_BlackScore(0)
{
    auto tileSize = window.getSize().x / 8.0f;
    m_Tile = sf::RectangleShape(sf::Vector2f(tileSize, tileSize));

    m_SelectedBox = sf::RectangleShape(sf::Vector2f(tileSize, tileSize));
    m_SelectedBox.setFillColor(sf::Color::Transparent);
    m_SelectedBox.setOutlineColor(sf::Color::Red);
    m_SelectedBox.setOutlineThickness(2.0f);

    m_PossibleMoveBox = sf::RectangleShape(sf::Vector2f(tileSize, tileSize));
    m_PossibleMoveBox.setFillColor(sf::Color(0x00ff0055));

    restart(fen);
}

void Chess::Game::restart(sf::String &fen)
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

    // Resetting
    m_CurrentTurn = Piece::Color::White;
    m_CurrentSelectedIndex = -1;

    // Updating board
    auto index = 0;
    for (auto symbol : fen)
    {
        if (symbol == '/')
            continue;

        if (std::isdigit(symbol))
        {
            index += symbol - '0';
        }
        else
        {
            auto posIndex = (7 - index / 8) * 8 + index % 8;
            m_Pieces[posIndex] = new Piece(symbol, m_Tile);
            std::cout << "Placed " << m_Pieces[posIndex]->toString() << " at index " << (posIndex) << " : " << indexToFRString(posIndex) << "\n";
            index++;
        }
    }
}

void Chess::Game::handleClick(sf::Vector2i mousePos)
{
    int file = mousePos.x / static_cast<int>(m_Tile.getSize().x);
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

                // Updating scores // TODO: Add captured symbols
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

void Chess::Game::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    // Rendering Board
    for (int i = 0; i < 64; i++)
    {
        int file = indexToFile(i);
        int rank = indexToRank(i);

        // Drawing tile
        m_Tile.setPosition(sf::Vector2f(file * m_Tile.getSize().x, (7 - rank) * m_Tile.getSize().y));
        m_Tile.setFillColor((file + rank) % 2 == 0 ? m_BlackColor : m_WhiteColor);
        target.draw(m_Tile);

        // Drawing piece
        auto piece = m_Pieces[i];
        if (piece != nullptr)
        {
            piece->setPosition(sf::Vector2f(file * m_Tile.getSize().x, (7 - rank) * m_Tile.getSize().y));
            target.draw(*piece);
        }
    }

    // Drawing overlays
    if (m_CurrentSelectedIndex != -1)
    {
        int file = indexToFile(m_CurrentSelectedIndex);
        int rank = indexToRank(m_CurrentSelectedIndex);

        // Selected overlay
        m_SelectedBox.setPosition(sf::Vector2f(file * m_SelectedBox.getSize().x, (7 - rank) * m_SelectedBox.getSize().y));
        target.draw(m_SelectedBox);

        // Movements overlays
        for (auto movePos : m_Movements)
        {
            int moveFile = indexToFile(movePos);
            int moveRank = indexToRank(movePos);

            m_PossibleMoveBox.setPosition(sf::Vector2f(moveFile * m_PossibleMoveBox.getSize().x, (7 - moveRank) * m_PossibleMoveBox.getSize().y));
            target.draw(m_PossibleMoveBox);
        }
    }
}
