#include <gui/gameoverscreen_screen/GameOverScreenView.hpp>

GameOverScreenView::GameOverScreenView()
{

}

void GameOverScreenView::setupScreen()
{
    GameOverScreenViewBase::setupScreen();
}

void GameOverScreenView::tearDownScreen()
{
    GameOverScreenViewBase::tearDownScreen();
}

void GameOverScreenView::setFinalScore(int score)
{
    Unicode::snprintf(scoreBuffer, SCORE_SIZE, "%d", score);
    this->score.setWildcard(scoreBuffer);
    this->score.invalidate();
}
