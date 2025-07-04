/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/homescreen_screen/HomeScreenViewBase.hpp>
#include <touchgfx/Color.hpp>
#include <images/BitmapDatabase.hpp>

HomeScreenViewBase::HomeScreenViewBase() :
    buttonCallback(this, &HomeScreenViewBase::buttonCallbackHandler)
{
    __background.setPosition(0, 0, 240, 320);
    __background.setColor(touchgfx::Color::getColorFromRGB(0, 0, 0));
    add(__background);

    homeBackground.setXY(0, 0);
    homeBackground.setBitmap(touchgfx::Bitmap(BITMAP_BACKGROUND_START_ID));
    add(homeBackground);

    startBtn.setXY(70, 146);
    startBtn.setBitmaps(touchgfx::Bitmap(BITMAP_START_BUTTON_GAME_ID), touchgfx::Bitmap(BITMAP_START_BUTTON_GAME_NHAN_ID));
    startBtn.setAction(buttonCallback);
    add(startBtn);
}

HomeScreenViewBase::~HomeScreenViewBase()
{

}

void HomeScreenViewBase::setupScreen()
{

}

void HomeScreenViewBase::buttonCallbackHandler(const touchgfx::AbstractButton& src)
{
    if (&src == &startBtn)
    {
        //Interaction1
        //When startBtn clicked change screen to GamePlayScreen
        //Go to GamePlayScreen with screen transition towards North
        application().gotoGamePlayScreenScreenSlideTransitionNorth();
    }
}
