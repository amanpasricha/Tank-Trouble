#include <graphics.h>
#include <windows.h>
#include <conio.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;

bool gameActive = true;
bool resetForNewMap = false;
int playerWon = 0;
int activeMap = 1;
int player1Won=0, player2Won=0;
int lastMatchWon = 0;

class TankTrouble
{
    // CENTRE OF TANK BODY
    int x, y;
    int TANK_COLOR;
    static const double TANK_SPEED = 10.0;

    // ANGLE THAT DIAGONAL OF TANK BODY MAKES WITH X-AXIS
    int degree = -45;
    double angle = -M_PI_4;

    // TANK BODY
    double r = 45.0;    // RADIUS OF CIRCLE ENCLOSING TANK BODY
    int x1, y1, x2, y2, x3, y3, x4, y4;

    // TANK SHOOTER
    double R = 25.0;    // LENGTH OF LARGER SIDE OF SHOOTER
    int X1, Y1, X2, Y2, X3, Y3, X4, Y4;

    // BULLET
    static const int NUM_BULLETS = 5;
    static const double BULLET_SPEED = 10.0;
    struct bulletInfo
    {
        bool isActive=false;
        int x_pos=0, y_pos=0;
        double angle=0.0;;
        clock_t releaseTime=0;
    };
    bulletInfo bullet[NUM_BULLETS];
    clock_t lastBulletFired = clock();


    void findTankCoordinates()
    {
        // TANK
        x1 = x - (int)r*sin(angle), y1 = y - (int)r*cos(angle);
        x2 = x - (int)r*cos(angle), y2 = y + (int)r*sin(angle);
        x3 = x + (int)r*sin(angle), y3 = y + (int)r*cos(angle);
        x4 = x + (int)r*cos(angle), y4 = y - (int)r*sin(angle);

        // SHOOTER
        X1 = x + (int)(20.0*cos(angle - M_PI_4/3.0)), Y1 = y - (int)(20.0*sin(angle - M_PI_4/3.0));
        X2 = x + (int)(20.0*cos(angle - M_PI_4 - M_PI_2/3.0)), Y2 = y - (int)(20.0*sin(angle - M_PI_4 - M_PI_2/3.0));
        X3 = X2 + R*cos(angle-M_PI_4), Y3 = Y2 - R*sin(angle-M_PI_4);
        X4 = X1 + R*cos(angle-M_PI_4), Y4 = Y1 - R*sin(angle-M_PI_4);
    }


public:

    void setTankPosition(int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    void moveTankAhead()
    {
        x += (int) (TANK_SPEED * cos(angle-M_PI_4));
        y -= (int) (TANK_SPEED * sin(angle-M_PI_4));
        findTankCoordinates();
    }

    void moveTankBehind()
    {
        x -= (int) (TANK_SPEED * cos(angle-M_PI_4));
        y += (int) (TANK_SPEED * sin(angle-M_PI_4));
        findTankCoordinates();
    }

    void rotateTankLeft()
    {
        degree += 6;
        angle += M_PI/30.0;
        findTankCoordinates();
    }

    void rotateTankRight()
    {
        degree -= 6;
        angle -= M_PI/30.0;
        findTankCoordinates();
    }


    bool tankCanMove()
    {
        // EXTRA POINTS TO CHECK CONDITIONS
        // 1/4, 1/2, 3/4 BACK POINTS ON BODY BOUNDARY
        int x_mid = (x1+x2)/2, y_mid = (y1+y2)/2;
        int x_mid_1 = (x_mid+x1)/2, y_mid_1 = (y_mid+y1)/2;
        int x_mid_2 = (x_mid+x2)/2, y_mid_2 = (y_mid+y2)/2;

        // 1/2 FRONT POINT ON SHOOTER
        int X_MID = (X3+X4)/2, Y_MID = (Y3+Y4)/2;

        // 1/4 and 3/4 FRONT POINTS ON BODY BOUNDARY
        int X_MID_F = (x3+x4)/2, Y_MID_F = (y3+y4)/2;
        int X_MID_1 = (X_MID_F+x3)/2, Y_MID_1 = (Y_MID_F+y3)/2;
        int X_MID_2 = (X_MID_F+x4)/2, Y_MID_2 = (Y_MID_F+y4)/2;

        // CHECKING THAT THE TANK DOES NOT PASS THROUGH BLACK LINE
        if(getpixel(x1, y1) && getpixel(x2, y2) && getpixel(x3, y3) && getpixel(x4, y4) && getpixel(X3, Y3) && getpixel(X4, Y4))
        {
            if(getpixel(x_mid, y_mid) && getpixel(x_mid_1, y_mid_1) && getpixel(x_mid_2, y_mid_2) &&
               getpixel(X_MID, Y_MID) && getpixel(X_MID_1, Y_MID_1) && getpixel(X_MID_2, Y_MID_2))
            {
                return true;    // NONE OF THE COORDINATES LIE IN BLACK AREA
            }
        }

        return false;
    }

    void drawTank()
    {
        setlinestyle(SOLID_LINE, 0, 2);
        setfillstyle(SOLID_FILL, TANK_COLOR);

        int body[] = {x1, y1, x2, y2, x3, y3, x4, y4, x1, y1};
        fillpoly(5, body);

        int shooter[] = {X1, Y1, X2, Y2, X3, Y3, X4, Y4, X1, Y1};
        fillpoly(5, shooter);
        arc(x, y, degree-15, 285+degree, 20);
    }


    void newBullet()
    {
        clock_t currentTime = clock();
        int timeInterval = (int) ((currentTime-lastBulletFired)/CLOCKS_PER_SEC);
        if(timeInterval < 1)
            return;

        int n;
        for(n=0; n<NUM_BULLETS; n++)
        {
            if(!bullet[n].isActive)
            {
                break;
            }
        }

        if(n==NUM_BULLETS)
            return;

        bullet[n].isActive = true;
        bullet[n].angle = angle - M_PI_4;
        bullet[n].x_pos = x + (int)(40.0*cos(angle-M_PI_4));
        bullet[n].y_pos = y - (int)(40.0*sin(angle-M_PI_4));
        bullet[n].releaseTime = currentTime;
        lastBulletFired = currentTime;
    }

    void moveBullet()
    {
        clock_t currentTime = clock();
        int bulletSurvivalTime;
        setfillstyle(SOLID_FILL, BLACK);

        for(int i=0; i<NUM_BULLETS; i++)
        {
            if(bullet[i].isActive)
            {
                bulletSurvivalTime = (int)((currentTime - bullet[i].releaseTime)/CLOCKS_PER_SEC);
                if(bulletSurvivalTime > 10)
                {
                    bullet[i].isActive = false;
                    bullet[i].angle=0.0;
                    bullet[i].x_pos=0;
                    bullet[i].y_pos=0;
                    bullet[i].releaseTime=0;
                    continue;
                }


                bullet[i].x_pos += (int)(BULLET_SPEED * cos(bullet[i].angle));
                bullet[i].y_pos -= (int)(BULLET_SPEED * sin(bullet[i].angle));


                if(getpixel(bullet[i].x_pos, bullet[i].y_pos) == RED &&
                   (getpixel(bullet[i].x_pos+4, bullet[i].y_pos+4 == RED) || getpixel(bullet[i].x_pos-4, bullet[i].y_pos+4) == RED ||
                    getpixel(bullet[i].x_pos+4, bullet[i].y_pos-4 == RED) || getpixel(bullet[i].x_pos-4, bullet[i].y_pos-4) == RED))
                {
                    if(activeMap == 1)
                    {
                        activeMap = 2;
                        resetForNewMap = true;
                    }
                    else if(activeMap == 2)
                    {
                        activeMap = 3;
                        resetForNewMap = true;
                    }
                    else
                    {
                        gameActive = false;
                    }
                    player2Won++;
                    lastMatchWon = 2;
                }
                else if(getpixel(bullet[i].x_pos, bullet[i].y_pos) == GREEN &&
                   (getpixel(bullet[i].x_pos+4, bullet[i].y_pos+4) == GREEN || getpixel(bullet[i].x_pos-4, bullet[i].y_pos+4) == GREEN ||
                    getpixel(bullet[i].x_pos+4, bullet[i].y_pos-4) == GREEN || getpixel(bullet[i].x_pos-4, bullet[i].y_pos-4) == GREEN))
                {
                    if(activeMap == 1)
                    {
                        activeMap = 2;
                        resetForNewMap = true;
                    }
                    else if(activeMap == 2)
                    {
                        activeMap = 3;
                        resetForNewMap = true;
                    }
                    else
                    {
                        gameActive = false;
                    }
                    player1Won++;
                    lastMatchWon = 1;
                }
                else if(getpixel(bullet[i].x_pos, bullet[i].y_pos) == BLACK &&
                   (getpixel(bullet[i].x_pos+4, bullet[i].y_pos+4) == BLACK || getpixel(bullet[i].x_pos-4, bullet[i].y_pos+4) == BLACK ||
                    getpixel(bullet[i].x_pos+4, bullet[i].y_pos-4) == BLACK || getpixel(bullet[i].x_pos-4, bullet[i].y_pos-4) == BLACK))
                {

                    bullet[i].x_pos -= (int)(BULLET_SPEED*cos(bullet[i].angle));

                    if(getpixel(bullet[i].x_pos, bullet[i].y_pos) == 0)
                    {
                        // DEFLECTING FROM HORIZONTAL SURFACE
                        bullet[i].y_pos += (int)(BULLET_SPEED*sin(bullet[i].angle));
                        bullet[i].angle = -bullet[i].angle;
                    }
                    else
                    {
                        // DEFLEFCTING FROM VERTICAL SURFACE
                        bullet[i].y_pos += (int)(BULLET_SPEED*sin(bullet[i].angle));
                        bullet[i].angle = M_PI - bullet[i].angle;
                    }
                }

                fillellipse( bullet[i].x_pos, bullet[i].y_pos, 4, 4);
            }
        }
    }

    TankTrouble(int x, int y, int TANK_COLOR)
    {
        setbkcolor(WHITE);
        setcolor(BLACK);
        this->TANK_COLOR = TANK_COLOR;
        this->x = x;
        this->y = y;

        for(int i=0; i<NUM_BULLETS; i++)
        {
            bullet[i].isActive = false;
        }

        findTankCoordinates();
    }


    static void drawMap1()
    {
        setlinestyle(SOLID_LINE, 0, 10);
        rectangle(150, 100, 1350, 700);

        setfillstyle(SOLID_FILL, LIGHTGRAY);
        floodfill(750, 400, BLACK);

        line(350, 250, 700, 250);
        line(350, 250, 350, 400);
        line(150, 550, 350, 550);
        line(550, 100, 550, 400);
        line(550, 550, 550, 700);
        line(1150, 400, 1350 ,400);
        line(1150, 550, 1350 ,550);
        rectangle(950, 100, 1150, 250);
        rectangle(750, 400, 950, 550);
    }

    static void drawMap2()
    {
        setlinestyle(SOLID_LINE, 0, 10);
        rectangle(150, 30, 1350, 780);

        setfillstyle(SOLID_FILL, LIGHTGRAY);
        floodfill(750, 400, BLACK);

        line(350, 30, 350, 180);
        line(550, 30, 550, 180);
        line(550, 180, 950, 180);
        line(1150, 180, 1150, 330);

        rectangle(950, 330, 1350, 780);
        rectangle(1150, 630, 1350, 780);
        line(950, 480, 1150, 480);

        line(350, 330, 750, 330);
        line(550, 330, 550, 480);
        line(750, 330, 750, 480);

        rectangle(150, 480, 350, 630);
        line(550, 630, 750, 630);
        line(750, 630, 750, 780);
    }

    static void drawMap3()
    {
        setlinestyle(SOLID_LINE, 0, 10);
        rectangle(150, 30, 1350, 780);

        setfillstyle(SOLID_FILL, LIGHTGRAY);
        floodfill(750, 400, BLACK);

        rectangle(150, 30, 350, 180);
        line(350, 180, 350, 330);
        line(350, 480, 350, 780);

        rectangle(350, 30, 1350, 180);
        line(550, 30, 550, 180);
        rectangle(1150, 180, 1350, 330);
        line(1150, 480, 1350, 480);

        line(950, 330, 950, 480);

        rectangle(550, 330, 750, 630);
        line(750, 630, 750, 780);
        line(750, 630, 1150, 630);
    }

    void resetConditionsForNewMap()
    {
        for(int i=0; i<NUM_BULLETS; i++)
        {
            bullet[i].isActive = false;
            bullet[i].angle = 0.0;
            bullet[i].releaseTime = 0;
            bullet[i].x_pos = 0;
            bullet[i].y_pos = 0;
        }

        findTankCoordinates();
    }

};



int main()
{
    string player1, player2;
    cout<<"Enter Player 1's Name:"<<endl;
    getline(cin, player1);
    //cin>>player1;

    cout<<endl<<"Enter Player 2's Name:"<<endl;
    getline(cin, player2);
    //cin>>player2;

    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    int gameWindow = initwindow(width, height, "Tank Trouble", -3, 0, false, true);

    int page=0;
    TankTrouble tank1(250, 200, RED);
    TankTrouble tank2(1250, 150, GREEN);

    while(gameActive)
    {
        setactivepage(page);
        setvisualpage(1-page);
        cleardevice();

        if(activeMap == 1)
            TankTrouble::drawMap1();
        else if(activeMap == 2)
            TankTrouble::drawMap2();
        else
            TankTrouble::drawMap3();


        if(GetAsyncKeyState(VK_UP))
        {
            tank1.moveTankAhead();
            if(!tank1.tankCanMove())
            {
                tank1.moveTankBehind();
            }
        }

        if(GetAsyncKeyState(VK_DOWN))
        {
            tank1.moveTankBehind();
            if(!tank1.tankCanMove())
            {
                tank1.moveTankAhead();
            }
        }

        if(GetAsyncKeyState(VK_LEFT))
        {
            tank1.rotateTankLeft();

            if(!tank1.tankCanMove())
                tank1.rotateTankRight();
        }

        if(GetAsyncKeyState(VK_RIGHT))
        {
            tank1.rotateTankRight();

            if(!tank1.tankCanMove())
                tank1.rotateTankLeft();
        }

        if(GetAsyncKeyState(VK_RCONTROL))
        {
            tank1.newBullet();
        }

        if(GetAsyncKeyState('W'))
        {
            tank2.moveTankAhead();
            if(!tank2.tankCanMove())
            {
                tank2.moveTankBehind();
            }
        }

        if(GetAsyncKeyState('S'))
        {
            tank2.moveTankBehind();
            if(!tank2.tankCanMove())
            {
                tank2.moveTankAhead();
            }
        }

        if(GetAsyncKeyState('A'))
        {
            tank2.rotateTankLeft();

            if(!tank2.tankCanMove())
                tank2.rotateTankRight();
        }

        if(GetAsyncKeyState('D'))
        {
            tank2.rotateTankRight();

            if(!tank2.tankCanMove())
                tank2.rotateTankLeft();
        }

        if(GetAsyncKeyState(VK_LCONTROL))
        {
            tank2.newBullet();
        }

        if(GetAsyncKeyState(VK_ESCAPE))
        {
            gameActive = false;
            lastMatchWon = 0;
            //break;
        }

        tank1.drawTank();
        tank2.drawTank();

        tank1.moveBullet();
        tank2.moveBullet();

        if(resetForNewMap)
        {
            FlushConsoleInputBuffer( GetStdHandle( STD_INPUT_HANDLE ) );
            setvisualpage(page);
            char output[50];
            sprintf(output, "Player %d has won this match. Press any key to proceed to next map. DO NOT press CNTRL buttons", lastMatchWon);
            outtextxy( (getmaxx()-textwidth(output))/2, (getmaxy()-textheight(output))/2, output);
            Sleep(1000);
            _getch();
            //FlushConsoleInputBuffer( GetStdHandle( STD_INPUT_HANDLE ) );

            if(activeMap == 2)
            {
                tank1.setTankPosition(650, 700);
                tank2.setTankPosition(1250, 250);
            }
            else if(activeMap == 3)
            {
                tank1.setTankPosition(250, 250);
                tank2.setTankPosition(1250, 550);
            }
            tank1.resetConditionsForNewMap();
            tank2.resetConditionsForNewMap();
            resetForNewMap = false;
        }

        page = 1-page;
        delay(15);
    }
    //setvisualpage(1-page);
    //setactivepage(page);

    FlushConsoleInputBuffer( GetStdHandle( STD_INPUT_HANDLE ) );
    char output[50];
    //sprintf(output, "Player %d has won this match. Press any key to proceed to next map.", lastMatchWon);
    //outtextxy( (getmaxx()-textwidth(output))/2, (getmaxy()-textheight(output))/2, output);
    //_getch();
    closegraph();

    cout<<endl;

    if(player1Won > player2Won)
        playerWon = 1;
    else if(player2Won > player1Won)
        playerWon = 2;
    else
        playerWon = 0;

    if(playerWon == 1)
    {
        cout<<"Player "<<playerWon<<" has won the game. Congratulations "<<player1;
    }
    else if(playerWon == 2)
    {
        cout<<"Player "<<playerWon<<" has won the game. Congratulations "<<player2;
    }
    else
    {
        cout<<"Game has tied";
    }

    cout<<endl<<"Press any key to exit"<<endl;
    Sleep(2000);
    _getch();
    return 0;
}
