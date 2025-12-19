#include<bits/stdc++.h>
#include<stdio.h>
#include<windows.h>
#include<algorithm>
#include<unistd.h>

class Engine{
    
    public:
    wchar_t *screen;
    int32_t screenWidth;
    int32_t screenHeight;


    HANDLE
    construct(const int32_t W, const int32_t H){
        screenWidth = W; screenHeight = H;

        // Allocating a console and a scrren buffer
        screen = new wchar_t[screenWidth * screenHeight];
        AllocConsole();
        HANDLE hConsoleBuffer = CreateConsoleScreenBuffer(GENERIC_READ| GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
        SetConsoleActiveScreenBuffer(hConsoleBuffer);

        return hConsoleBuffer;
    }

    void
    Draw(const COORD& coord, const wchar_t& sym){
        int x = coord.X, y = coord.Y;
        if((x < 0 || x >= screenWidth) ||(y < 0 || y >= screenHeight)) return;

        int index = y * screenWidth + x;
        screen[index] = sym;
    }

    void
    DrawString(const COORD& coord, const wchar_t *text){
        int x = coord.X, y = coord.Y;
        if((x < 0 || x >= screenWidth) ||(y < 0 || y >= screenHeight)) return;

        int index = y * screenWidth + x;

        for(int i = 0; text[i] != '\0'; i++ ){
            screen[index + i] = text[i];
        }
    }

    void
    clear(const wchar_t clear_sym = ' '){
        for(int i = 0; i < screenHeight * screenWidth; i++){
            screen[i] = clear_sym;
        }
    }

};

int
main(){
        Engine game;
        HANDLE hConsoleBuffer = game.construct(80, 30);    

        // getting HANDLE from std_input_buffer
        HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

        PINPUT_RECORD record;
        LPDWORD read;
        LPDWORD written;
        COORD writecoord = {0, 0};
        COORD mycoord = {0,0};

        // game loop
        short x = 0;
        game.clear();
        while(1){
            game.DrawString(mycoord, L"Hi! From Console");

            WriteConsoleOutputCharacterW(hConsoleBuffer, game.screen, game.screenWidth * game.screenHeight ,writecoord, written); // writting onto screen buffer
            mycoord.X++; mycoord.Y++;
            game.clear();
            sleep(1);
        }

        CloseHandle(hConsoleBuffer);

    return 0;
}