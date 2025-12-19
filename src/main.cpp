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
        int32_t newScreenWidth;
        int32_t newScreenHeight;
        int32_t Cells;


        void
        border(const wchar_t borderSymCols = '|', const wchar_t borderSymRows = '_'){
            Cells = screenWidth * screenHeight;

            //Horizontal rows border
            for(int i = 1; i < screenWidth - 1; i++) {screen[i] = borderSymRows;}
            for(int i = Cells - screenWidth; i < Cells; i++ ){screen[i] = borderSymRows;}
            
            //Vertical cols border
            for(int i = screenWidth, j = 2*screenWidth - 1; i < Cells || j < Cells; i += screenWidth, j += screenWidth){screen[i] = borderSymCols; 
                screen[j] = borderSymCols;
            }

            newScreenWidth = screenWidth - 2; newScreenHeight = screenHeight - 2;
            
        }

        HANDLE
        construct(const int32_t W = 143, const int32_t H = 40){
            screenWidth = W; screenHeight = H;

            // Allocating a console and a scrren buffer
            screen = new wchar_t[screenWidth * screenHeight];
            
            if (GetConsoleWindow() == NULL) {
                AllocConsole();
            }
            HANDLE hConsoleBuffer = CreateConsoleScreenBuffer(GENERIC_READ| GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
            SetConsoleActiveScreenBuffer(hConsoleBuffer);

            SMALL_RECT rect = {0, 0, 1, 1};
            SetConsoleWindowInfo(hConsoleBuffer, TRUE, &rect);

            COORD bufferSize = {(SHORT)screenWidth, (SHORT)screenHeight};
            SetConsoleScreenBufferSize(hConsoleBuffer, bufferSize);

            SMALL_RECT windowSize = {0, 0, (SHORT)(screenWidth - 1),(SHORT)(screenHeight - 1)};
            SetConsoleWindowInfo(hConsoleBuffer, TRUE, &windowSize);

            clear();
            border();

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
        HANDLE hConsoleBuffer = game.construct();

        // getting HANDLE from std_input_buffer
        HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

        PINPUT_RECORD record;
        LPDWORD read;
        DWORD written = 0;
        COORD writecoord = {0, 0};
        COORD mycoord = {1, 1};

        game.Draw(mycoord,'#');
        WriteConsoleOutputCharacterW(hConsoleBuffer, game.screen, game.Cells, writecoord, &written);
        sleep(1000);

        CloseHandle(hConsoleBuffer);

    return 0;
}