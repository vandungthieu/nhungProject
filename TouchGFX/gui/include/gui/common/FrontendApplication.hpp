#ifndef FRONTENDAPPLICATION_HPP
#define FRONTENDAPPLICATION_HPP

#include <gui_generated/common/FrontendApplicationBase.hpp>
#include "cmsis_os2.h"

class FrontendHeap;

using namespace touchgfx;

class FrontendApplication : public FrontendApplicationBase
{
public:
    FrontendApplication(Model& m, FrontendHeap& heap);
    virtual ~FrontendApplication() { }

    virtual void handleTickEvent()
    {
        model.tick();
        FrontendApplicationBase::handleTickEvent();
    }

   // virtual void handleTickEvent() override;

    // Queue nhận dữ liệu joystick từ FreeRTOS task
    // static osMessageQueueId_t joystickQueueHandle;

private:
};

#endif // FRONTENDAPPLICATION_HPP
