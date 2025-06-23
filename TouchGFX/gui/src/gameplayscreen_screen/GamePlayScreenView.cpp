#include <gui/gameplayscreen_screen/GamePlayScreenView.hpp>
#include <touchgfx/mixins/MoveAnimator.hpp>
#include <BitmapDatabase.hpp>
#include <stdlib.h>
#include <algorithm>
#include <cmath>
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"


extern osMessageQueueId_t buttonQueueHandle;
extern osMessageQueueId_t shootQueueHandle;
extern void SendChar(char c);

const int GamePlayScreenView::eggSequence[] = {0, 1, 2, 3, 2, 0, 1, 3};
const int GamePlayScreenView::EGG_SEQ_SIZE = sizeof(GamePlayScreenView::eggSequence) / sizeof(int);


GamePlayScreenView::GamePlayScreenView()
    : shootEggEndedCallback(this, &GamePlayScreenView::onShootEggEnded),
	  rowFallCallback(this, &GamePlayScreenView::onRowFallStep)
{}



void GamePlayScreenView::setupScreen()
{
    GamePlayScreenViewBase::setupScreen();

    //testMoveRowDown();

   //srand(HAL_GetTick());

    // === Mũi tên căn giữa đáy tại (120, 235) ===
    const int centerX = 120;
    const int bottomY = 255; // 255

    arrow.setBitmap(touchgfx::Bitmap(BITMAP_ARROW1_ID));
    arrow.setXY(centerX - arrow.getBitmap().getWidth() / 2,
                bottomY - arrow.getBitmap().getHeight());


    // Xoay quanh tâm đáy mũi tên
    arrow.setOrigo(arrow.getBitmap().getWidth() / 2,
                   arrow.getBitmap().getHeight());



    arrow.setCameraDistance(6000); // Khoảng cách ảo cho phép xoay 2D, giá trị càng nhỏ càng méo
    arrow.setZAngle(0.0f);


    add(arrow);



    initializeEggGrid(); // gọi sau khi add container

    // Thiết lập eggContainer theo kích thước từ ảnh
    eggContainer.setPosition(20, 30, 199, 365);
    eggContainer.setTouchable(false); // nếu không cần bắt sự kiện chạm

    add(eggContainer); // Thêm container sau arrow nếu muốn trứng nằm trên mũi tên

    eggContainer.invalidate();


    shouldInitEggs = true; // Đợi tick rồi mới gọi initializeEggs


}

void GamePlayScreenView::tearDownScreen()
{
    GamePlayScreenViewBase::tearDownScreen();
}



void GamePlayScreenView::initializeEggs()
{

	// Mảng chứa các ID bitmap của 4 quả trứng
	const touchgfx::BitmapId eggBitmaps[] = {
	    BITMAP_EGG1_ID,
	    BITMAP_EGG2_ID,
	    BITMAP_EGG3_ID,
	    BITMAP_EGG4_ID
	};
    // Trứng bắn
	int randomIndex =   1;
	shootEgg.setBitmap(touchgfx::Bitmap(BITMAP_EGG1_ID));
	shootEgg.setXY(90, 215);  // Vì 70 + 20 = 90, 185 + 30 = 215
    shootEgg.setVisible(true);
    eggContainer.add(shootEgg);
    shootEgg.invalidate();

    // Trứng chờ
    //nextEgg.setBitmap((selected));
    nextEgg.setBitmap(touchgfx::Bitmap(BITMAP_EGG2_ID));
    //nextEgg.setXY(40, 240);
    nextEgg.setXY(90, 215);
    nextEgg.setVisible(true);
    eggContainer.add(nextEgg);
    nextEgg.invalidate();


}



// tạo lưới trứng
void GamePlayScreenView::initializeEggGrid()
{
    const touchgfx::BitmapId eggBitmaps[] = {
        BITMAP_EGG1_ID, // = 3
        BITMAP_EGG2_ID, // = 4
        BITMAP_EGG3_ID, // = 5
        BITMAP_EGG4_ID  // = 6
    };

    int eggWidth = 20;
    int eggHeight = 21;
    int padding = 0;

    int startX = 0; // Tọa độ bắt đầu vẽ lưới
    int startY = 0;

    for (int row = 0; row < ROWS; ++row)
    {
        for (int col = 0; col < COLS; ++col)
        {
            int eggType = eggMap[row][col]; // từ 0 đến 3

            eggGrid[row][col].setBitmap(touchgfx::Bitmap(eggBitmaps[eggType]));
            eggGrid[row][col].setXY(startX + col * (eggWidth + padding),
                                    startY + row * (eggHeight + padding));
            eggGrid[row][col].setVisible(true);
            eggContainer.add(eggGrid[row][col]);
        }
    }
}


void GamePlayScreenView::pushEggGridDown()
{
    // Khởi động rơi từng nhóm
    currentFallGroup = 0;
    pixelsMoved = 0;
    isGroupedFalling = true;
}


void GamePlayScreenView::onRowFallStep(const touchgfx::MoveAnimator<touchgfx::Image>& img)
{
    int x = img.getX();
    int y = img.getY();
    const int moveDistance = 21;

    // Nếu đã chạm tới 200px hoặc hơn → GAME OVER
    if (y + moveDistance >= 200)
    {
        static_cast<FrontendApplication *>(Application::getInstance())
            ->gotoGameOverScreenScreenSlideTransitionNorth();
    }
    else
    {
        // Chưa đến 200 → tiếp tục rơi
        const_cast<touchgfx::MoveAnimator<touchgfx::Image>&>(img).startMoveAnimation(
            x,
            y + moveDistance,
            150,
            touchgfx::EasingEquations::linearEaseNone
        );
    }
}


// hàm bắn trứng
void GamePlayScreenView::shoot()
{
    int shootStartX = nextEgg.getX();
    int shootStartY = nextEgg.getY();

    shootEgg.setBitmap(nextEgg.getBitmap());
    shootEgg.setXY(shootStartX, shootStartY);
    shootEgg.setVisible(true);
    shootEgg.invalidate();

    float angleRad = currentAngle *69 * 3.14f / 180.0f;
    float speed = 4.0f; // tốc độ bay

    shootDX = speed * sinf(angleRad);
    shootDY = -speed * cosf(angleRad); // trục Y hướng xuống

    shooting = true; // bật trạng thái đang bắn

    // ===== TẠO BÓNG KẾ TIẾP (KHÔNG GỌI HÀM RIÊNG) =====
        // Mảng ID các loại trứng
        const touchgfx::BitmapId eggBitmaps[] = {
            BITMAP_EGG1_ID,
            BITMAP_EGG2_ID,
            BITMAP_EGG3_ID,
            BITMAP_EGG4_ID
        };

        // Lấy chỉ số từ mảng định sẵn
        int randIndex = eggSequence[eggSeqIndex];
        eggSeqIndex = (eggSeqIndex + 1) % EGG_SEQ_SIZE;

        touchgfx::BitmapId selected = eggBitmaps[randIndex];

        nextEgg.setBitmap(touchgfx::Bitmap(selected));
        nextEgg.setXY(90, 215);
        nextEgg.setVisible(true);
        nextEgg.invalidate();

}


// va chạm tường
void GamePlayScreenView::handleShootEggCollision()
{
    if (!shooting) return;

    // 1. Lấy vị trí hiện tại của quả trứng bắn
    int x = shootEgg.getX();
    int y = shootEgg.getY();
    int w = shootEgg.getWidth();


    // 2. Kiểm tra va chạm tường trái/phải
    if (x <= 0 || (x + w) >= eggContainer.getWidth()) {
        shootDX = -shootDX; // Đảo chiều ngang
        shootEgg.setX(std::max(0, std::min(x, eggContainer.getWidth() - w)));
    }

    // 3. Kiểm tra va trúng trứng khác trong lưới
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (!eggGrid[row][col].isVisible()) continue;

            if (checkCollision(shootEgg, eggGrid[row][col])) {
                attachEggToGrid(x, y);  // gắn trứng vào lưới
                shooting = false;
                return;
            }
        }
    }

    // 4. Kiểm tra va chạm với tường trên
    if (y <= 0) {
        attachEggToGrid(x, y); // gắn vào hàng đầu
        shooting = false;
    }
}

//kiểm tra va chạm
bool GamePlayScreenView::checkCollision(const touchgfx::Image& a, const touchgfx::Image& b)
{
    int ax = a.getX(), ay = a.getY(), aw = a.getWidth(), ah = a.getHeight();
    int bx = b.getX(), by = b.getY(), bw = b.getWidth(), bh = b.getHeight();

    return (ax < bx + bw && ax + aw > bx &&
            ay < by + bh && ay + ah > by);
}

// ánh xạ x y
void GamePlayScreenView::attachEggToGrid(int x, int y)
{
    int col = (x - eggContainer.getX()) /eggWidth;
    int row = (y - eggContainer.getY()) / eggHeight;

    if (row < 0) row = 0;
    if (row >= ROWS) row = ROWS - 1;
    if (col < 0) col = 0;
    if (col >= COLS) col = COLS - 1;

    eggGrid[row][col].setBitmap(shootEgg.getBitmap());
    eggGrid[row][col].setXY(col * eggWidth + eggContainer.getX(),
                            row * eggHeight + eggContainer.getY());
    eggGrid[row][col].setVisible(true);
    eggGrid[row][col].invalidate();

    shootEgg.setVisible(false);
    shootEgg.invalidate();

    checkAndClearMatchingEggs(row, col);
}


// check khi trùng trứng
void GamePlayScreenView::checkAndClearMatchingEggs(int row, int col)
{
    touchgfx::BitmapId target = eggGrid[row][col].getBitmap();
    int totalMatch = 1; // Bản thân quả trứng

    // Kiểm tra tất cả 4 hướng chính + 2 chéo
    const int dirs[4][2] = {
        {0, 1},  // ngang →
        {1, 0},  // dọc ↓
        {1, 1},  // chéo ↘
        {1, -1}  // chéo ↙
    };

    for (int i = 0; i < 4; ++i) {
        int count = 1; // chính giữa

        count += countMatchingDirection(row, col, dirs[i][0], dirs[i][1]);
        count += countMatchingDirection(row, col, -dirs[i][0], -dirs[i][1]);

        if (count >= 3) {
            // Có đủ cụm ≥3 → xóa & ghi điểm
            // Xóa từng ô theo hướng đó
            int dr = dirs[i][0], dc = dirs[i][1];

            // Chính giữa
            eggGrid[row][col].setVisible(false);
            eggGrid[row][col].invalidate();

            // Hai hướng
            for (int d = 1; d < count; ++d) {
                int r1 = row + d * dr, c1 = col + d * dc;
                int r2 = row - d * dr, c2 = col - d * dc;

                if (r1 >= 0 && r1 < ROWS && c1 >= 0 && c1 < COLS &&
                    eggGrid[r1][c1].getBitmap() == target)
                {
                    eggGrid[r1][c1].setVisible(false);
                    eggGrid[r1][c1].invalidate();
                }

                if (r2 >= 0 && r2 < ROWS && c2 >= 0 && c2 < COLS &&
                    eggGrid[r2][c2].getBitmap() == target)
                {
                    eggGrid[r2][c2].setVisible(false);
                    eggGrid[r2][c2].invalidate();
                }
            }

            // Ghi điểm
            score += count * 10;
            //updateScoreDisplay();
            return; // Nếu xóa rồi thì không kiểm tra các hướng khác nữa
        }
    }
}

// đếm trứng giống nhau theo hướng
int GamePlayScreenView::countMatchingDirection(int row, int col, int dRow, int dCol)
{
    int count = 0;
    touchgfx::BitmapId target = eggGrid[row][col].getBitmap();

    while (true)
    {
        row += dRow;
        col += dCol;

        if (row < 0 || row >= ROWS || col < 0 || col >= COLS) break;
        if (!eggGrid[row][col].isVisible()) break;
        if (eggGrid[row][col].getBitmap() != target) break;

        count++;
    }
    return count;
}


// hàm tạo hàng trứng mới
void GamePlayScreenView::addTopRowEggs()
{
    const touchgfx::BitmapId eggBitmaps[] = {
        BITMAP_EGG1_ID,
        BITMAP_EGG2_ID,
        BITMAP_EGG3_ID,
        BITMAP_EGG4_ID
    };

    // Dịch toàn bộ lưới xuống một dòng (từ dưới lên để tránh ghi đè)
    for (int row = ROWS - 1; row > 0; --row)
    {
        for (int col = 0; col < COLS; ++col)
        {
            eggGrid[row][col].setBitmap(eggGrid[row - 1][col].getBitmap());
            eggGrid[row][col].setVisible(eggGrid[row - 1][col].isVisible());

            int x = eggGrid[row - 1][col].getX();
            int y = eggGrid[row - 1][col].getY();
            eggGrid[row][col].setXY(x, y);
            eggGrid[row][col].invalidate();
        }
    }

    // Sinh hàng mới ở hàng 0 (trên cùng) bằng mảng định sẵn
    for (int col = 0; col < COLS; ++col)
    {
        int randIndex = eggSequence[eggSeqIndex];
        eggSeqIndex = (eggSeqIndex + 1) % EGG_SEQ_SIZE;

        eggGrid[0][col].setBitmap(touchgfx::Bitmap(eggBitmaps[randIndex]));
        eggGrid[0][col].setVisible(true);

        // Đặt trứng hơi ra ngoài màn hình trên
        int x = eggGrid[1][col].getX();         // dùng vị trí của hàng 1
        int y = eggGrid[1][col].getY() - 21;    // cao hơn đúng 1 dòng

        eggGrid[0][col].setXY(x, y);
        eggGrid[0][col].invalidate();
    }
}




void GamePlayScreenView::onShootEggEnded(const touchgfx::MoveAnimator<touchgfx::Image>& e)
{
    // Tùy ý xử lý sau khi trứng bắn xong
    shootEgg.setVisible(false);
    shootEgg.invalidate();

    // Ví dụ: kiểm tra va chạm, xếp lưới, chơi âm thanh,...
}



void GamePlayScreenView::handleTickEvent()
{

	GamePlayScreenViewBase::handleTickEvent();

	// ============= RƠI TRỨNG ========
	if (isGroupedFalling)
	    {
	        tickCounter++;
	        if (tickCounter < fallTickDelay)
	            return; // Chưa đến thời điểm rơi → bỏ qua

	        tickCounter = 0; // Reset để rơi 1px

	        if (pixelsMoved < FALL_STEP)
	        {
	            for (int row = currentFallGroup * ROWS_PER_GROUP;
	                 row < (currentFallGroup + 1) * ROWS_PER_GROUP && row < ROWS;
	                 ++row)
	            {
	                for (int col = 0; col < COLS; ++col)
	                {
	                    if (eggGrid[row][col].isVisible())
	                    {
	                        eggGrid[row][col].invalidate();
	                        int x = eggGrid[row][col].getX();
	                        int y = eggGrid[row][col].getY();

	                        if (y + 1 >= 167 && !gameOverTriggered)
	                        {
	                            gameOverTriggered = true;
	                            static_cast<FrontendApplication *>(Application::getInstance())
	                                ->gotoGameOverScreenScreenSlideTransitionNorth();
	                            return;
	                        }

	                        eggGrid[row][col].setXY(x, y + 1);
	                        eggGrid[row][col].invalidate();
	                    }
	                }
	            }

	            pixelsMoved++;
	        }
	        else
	        {
	            currentFallGroup++;
	            pixelsMoved = 0;

	            if (currentFallGroup * ROWS_PER_GROUP >= ROWS)
	            {
	                isGroupedFalling = false;

	                //addTopRowEggs();
	                handleShootEggCollision();
	            }
	        }
	    }

	 // ===============BẮN TRỨNG============
	if (shooting)
	{
	    // 1. Xóa vùng cũ
	    shootEgg.invalidate();

	    // 2. Tính vị trí mới
	    int x = shootEgg.getX();
	    int y = shootEgg.getY();

	    shootEgg.setXY(x + shootDX, y + shootDY);

	    // 3. Vẽ lại vùng mới
	    shootEgg.invalidate();

	    handleShootEggCollision();

	    // 4. Kiểm tra biên
	    if (x < 0 || x > 240 || y < 0 || y > 320) {
	        shooting = false;
	        shootEgg.setVisible(false);


	    }
	}

	    if (shouldInitEggs)
	    {
	        shouldInitEggs = false;
	        initializeEggs();
	    }


// =========== Đọc lệnh từ buttonqueue ==========
	    uint8_t direction = 0;
	    bool handledShoot = false;
	    if (osMessageQueueGetCount(buttonQueueHandle) > 0)
	    {
	        osMessageQueueGet(buttonQueueHandle, &direction, NULL, osWaitForever);


	        if (direction == 'L'){

	        		currentAngle -= 0.20f;
	        		//testMoveRowDown();


	        }
	        else if (direction == 'R'){

	        		currentAngle += 0.20f;
	        		//testMoveRowDown();

	        }
	        else if (direction == 'B' && !handledShoot) {
	            shoot();
	            handledShoot = true;
	            if(!isStart){
	            	//testMoveRowDown();
	            	pushEggGridDown();

	            	isStart = false;
	            }
	        }



	        if (currentAngle > 180.0f)  currentAngle = 180.0f;
	        else if (currentAngle < -180.0f)  currentAngle = -180.0f;

	        arrow.setZAngle(currentAngle);
	        arrow.invalidate();
	    }




}
