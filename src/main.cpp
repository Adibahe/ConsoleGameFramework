#include<bits/stdc++.h>
#include<stdio.h>
#include<windows.h>
#include<algorithm>
#include<unistd.h>


class Engine{
    
    private:
        wchar_t *Primaryscreen;
        wchar_t *Secondaryscreen;
        int32_t secScreenWidth;
        int32_t secScreenHeight;
        int32_t primaryScreenWidth;
        int32_t primaryScreenHeight;
        int primaryCells;
        int secondaryCells;

        HANDLE hconsolebuffer;
        DWORD written = 0;

    public:
        bool keepBorder = true;

        ~Engine() {
            delete[] Primaryscreen;
            delete[] Secondaryscreen;
            CloseHandle(hconsolebuffer);
        }


        void
        writePrimaryScreenBuffer(const COORD coord = {0, 0}){
            WriteConsoleOutputCharacterW(hconsolebuffer, Primaryscreen, primaryCells, coord, &written);
        }

        void
        border(const wchar_t borderSymCols = '|', const wchar_t borderSymRows = '_'){

            //Horizontal rows border
            for(int i = 1; i < primaryScreenWidth - 1; i++) {Primaryscreen[i] = borderSymRows;}
            for(int i = primaryCells - primaryScreenWidth; i < primaryCells; i++ ){Primaryscreen[i] = borderSymRows;}
            
            //Vertical cols border
            for(int i = primaryScreenWidth, j = 2 * primaryScreenWidth - 1; i < primaryCells || j < primaryCells; i += primaryScreenWidth, j += primaryScreenWidth){Primaryscreen[i] = borderSymCols; 
                Primaryscreen[j] = borderSymCols;
            }
        }

        HANDLE
        construct(const int32_t W = 141, const int32_t H = 38){
            secScreenWidth = W; secScreenHeight = H;
            primaryScreenWidth = W + 2; primaryScreenHeight = H + 2;
            primaryCells = primaryScreenWidth * primaryScreenHeight;
            secondaryCells = secScreenWidth * secScreenHeight;


            // Allocating a console and a scrren buffer
            Secondaryscreen = new wchar_t[secScreenWidth * secScreenHeight];
            Primaryscreen = new wchar_t[primaryScreenWidth * primaryScreenHeight];
            
            if (GetConsoleWindow() == NULL) {
                AllocConsole();
            }
            
            hconsolebuffer = CreateConsoleScreenBuffer(GENERIC_READ| GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
            SetConsoleActiveScreenBuffer(hconsolebuffer);

            SMALL_RECT rect = {0, 0, 1, 1};
            SetConsoleWindowInfo(hconsolebuffer, TRUE, &rect);

            COORD bufferSize = {(SHORT)primaryScreenWidth, (SHORT)primaryScreenHeight};
            SetConsoleScreenBufferSize(hconsolebuffer, bufferSize);

            SMALL_RECT windowSize = {0, 0, (SHORT)(primaryScreenWidth - 1),(SHORT)(primaryScreenHeight - 1)};
            SetConsoleWindowInfo(hconsolebuffer, TRUE, &windowSize);

            clearPrimary();
            clear();
            if(keepBorder) border();

            return hconsolebuffer;
        }

        void
        Draw(const COORD& coord, const wchar_t& sym){
            int x = coord.X, y = coord.Y;
            if((x < 0 || x >= secScreenWidth) ||(y < 0 || y >= secScreenHeight) ) return;

            int index = y * secScreenWidth + x;
            Secondaryscreen[index] = sym;
        }

        void
        DrawString(const COORD& coord, const wchar_t *text){
            int x = coord.X, y = coord.Y;
            if((x < 0 || x >= secScreenWidth) ||(y < 0 || y >= secScreenHeight)) return;

            int index = y * secScreenWidth + x;

            for(int i = 0; text[i] != '\0' && x + i < secScreenWidth; i++ ){
                Secondaryscreen[index + i] = text[i];
            }
        }

        void
        clear(const wchar_t clear_sym = ' '){
            for(int i = 0; i < secScreenHeight * secScreenWidth; i++){
                Secondaryscreen[i] = clear_sym;
            }
        }

        void
        clearPrimary(const wchar_t clear_sym = ' '){
            for(int i = 0; i < primaryScreenHeight * primaryScreenWidth; i++){
                Primaryscreen[i] = clear_sym;
            }
        }

        void 
        Compose() {
            for (int y = 0; y < secScreenHeight; y++) {
                for (int x = 0; x < secScreenWidth; x++) {
                    Primaryscreen[(y + 1) * primaryScreenWidth + (x + 1)] = Secondaryscreen[y * secScreenWidth + x];
                }
            }
        }


};

int
main(){

    Engine game;
    game.construct();
    COORD coord = {0, 0};

    //Game Loop
    while(1){
        game.DrawString(coord, L"Hi! From Console");
        game.Compose();
        game.writePrimaryScreenBuffer();
        coord.X += 2; coord.Y += 2;
        game.clear();
        Sleep(1000);
    }

    return 0;
}