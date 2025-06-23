#include <gui/gameplayscreen_screen/GamePlayScreenView.hpp>
#include <gui/gameplayscreen_screen/GamePlayScreenPresenter.hpp>

GamePlayScreenPresenter::GamePlayScreenPresenter(GamePlayScreenView& v)
    : view(v)
{

}

void GamePlayScreenPresenter::activate()
{

}

void GamePlayScreenPresenter::deactivate()
{
	 model->setGameScore(view.getScore());
}
