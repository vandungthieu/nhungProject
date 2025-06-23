#ifndef GAMEPLAYSCREENVIEW_HPP
#define GAMEPLAYSCREENVIEW_HPP

#include <gui_generated/gameplayscreen_screen/GamePlayScreenViewBase.hpp>
#include <gui/gameplayscreen_screen/GamePlayScreenPresenter.hpp>
#include <touchgfx/mixins/MoveAnimator.hpp>
#include <touchgfx/widgets/ScalableImage.hpp>
#include <touchgfx/widgets/TextureMapper.hpp>
#include <touchgfx/mixins/MoveAnimator.hpp>
#include <touchgfx/Callback.hpp>

class GamePlayScreenView : public GamePlayScreenViewBase
{
public:
    GamePlayScreenView();
    virtual ~GamePlayScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();


//    void prepareShootEgg();
//    void animateShootEgg(const touchgfx::MoveAnimator<touchgfx::Image> &animator);
//
    void updateJoystick(uint16_t value);
    float currentAngle = 0.0f;


    // ghi đè handleTickEvent()
    virtual void handleTickEvent();

protected:
//    touchgfx::Callback<GamePlayScreenView, const touchgfx::MoveAnimator<touchgfx::Image> &> moveEndCallback;

private:
    static const int ROWS = 5;
    static const int COLS = 10;
    static const int eggWidth = 20;
    static const int eggHeight = 21;

    float shootDX = 0.0f;
    float shootDY = 0.0f;
    bool shooting = false;

    bool isStart = false;

    static const int eggSequence[];
    static const int EGG_SEQ_SIZE;
    int eggSeqIndex; // thành viên class



    int eggMap[ROWS][COLS] = {
        {0, 1, 2, 0, 0, 1, 2, 3, 0, 1},
        {0, 1, 3, 2, 1, 0, 3, 1, 2, 2},
        {2, 2, 0, 0, 2, 0, 2, 1, 2, 3},
        {3, 0, 1, 3, 3, 0, 3, 0, 1, 0},
        {0, 3, 2, 2, 0, 1, 2, 3, 0, 3}
    };

//    bool firstPress = true;

    bool shouldInitEggs = false;

    static const int FALL_STEP = 5;
    static const int ROWS_PER_GROUP = 2;

    int currentFallGroup = 0;
    int pixelsMoved = 0;
    bool isGroupedFalling = false;

    bool gameOverTriggered = false ;

    int tickCounter = 0;
    const int fallTickDelay = 2;  // Rơi 1 pixel mỗi 2 tick (30px/s)

    int score = 0;  // Tổng điểm
    //Unicode::UnicodeChar scoreBuffer[20]; // đủ để hiển thị "Score: 99999"
    void updateScoreDisplay();  // Cập nhật điểm lên UI

    void checkAndClearMatchingEggs(int row, int col);
    int countMatchingDirection(int row, int col, int dRow, int dCol);



//
//    int arrowAngle = 0; // Góc xoay hiện tại (độ)
//
    // mảng trứng
    MoveAnimator<touchgfx::Image> eggGrid[10][COLS];
//
//    touchgfx::MoveAnimator<touchgfx::Image> shootEgg;
//    touchgfx::MoveAnimator<touchgfx::Image> nextEgg;

    touchgfx::MoveAnimator<touchgfx::Image> nextEgg;
    touchgfx::MoveAnimator<touchgfx::Image> shootEgg;

    touchgfx::Container eggContainer;

    touchgfx::TextureMapper arrow;

//
//    void generateRandomEggGrid();
//    void generateNextEgg();
    void initializeEggs();
    void generateNewNextEgg();
    void initializeEggGrid();
    void pushEggGridDown();
    void addTopRowEggs();
    void handleShootEggCollision();
    bool checkCollision(const touchgfx::Image& a, const touchgfx::Image& b);
    void attachEggToGrid(int x, int y);

    void onRowFallStep(const touchgfx::MoveAnimator<touchgfx::Image>& img); // ⬅️ Có tham số!

    touchgfx::Callback<GamePlayScreenView, const touchgfx::MoveAnimator<touchgfx::Image>&> rowFallCallback;

 //   void testMoveRowDown();
//    void animateShootEgg();
//    void animateNextEggEntry();
//    void checkEggGridPosition();

    void shoot();

    touchgfx::Callback<GamePlayScreenView, const touchgfx::MoveAnimator<touchgfx::Image>&> shootEggEndedCallback;
    void onShootEggEnded(const touchgfx::MoveAnimator<touchgfx::Image>& e);
};

#endif // GAMEPLAYSCREENVIEW_HPP
