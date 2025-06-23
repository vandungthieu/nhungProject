#include <gui/common/FrontendApplication.hpp>
#include <gui/gameplayscreen_screen/GamePlayScreenView.hpp>

extern "C" osMessageQueueId_t joystickQueueHandle;


FrontendApplication::FrontendApplication(Model& m, FrontendHeap& heap)
    : FrontendApplicationBase(m, heap)
{

}
//
//void FrontendApplication::handleTickEvent()
//{
//    model.tick();
//    FrontendApplicationBase::handleTickEvent();
//
//    uint16_t joyValue;
//    if (joystickQueueHandle &&
//        osMessageQueueGetCount(joystickQueueHandle) > 0 &&
//        osMessageQueueGet(joystickQueueHandle, &joyValue, NULL, 0) == osOK)
//    {
//        GamePlayScreenView* view = static_cast<GamePlayScreenView*>(Application::getInstance()->getCurrentScreen());
//        if (view)
//        {
//            view->updateJoystick(joyValue);
//        }
//    }
//}
