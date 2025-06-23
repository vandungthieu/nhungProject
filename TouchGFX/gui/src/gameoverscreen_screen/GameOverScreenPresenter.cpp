#include <gui/gameoverscreen_screen/GameOverScreenView.hpp>
#include <gui/gameoverscreen_screen/GameOverScreenPresenter.hpp>

GameOverScreenPresenter::GameOverScreenPresenter(GameOverScreenView& v)
    : view(v)
{

}

void GameOverScreenPresenter::activate()
{
	 view.setFinalScore(model->getGameScore());
}

void GameOverScreenPresenter::deactivate()
{

}
