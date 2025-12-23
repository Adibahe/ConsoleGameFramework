#include<bits/stdc++.h>
#include<stdio.h>
#include<windows.h>
#include<algorithm>
#include<unistd.h>


class Engine{
    
    private:
        CHAR_INFO *Primaryscreen;
        CHAR_INFO *Secondaryscreen;
        int32_t secScreenWidth;
        int32_t secScreenHeight;
        int32_t primaryScreenWidth;
        int32_t primaryScreenHeight;
        int primaryCells;
        int secondaryCells;
        SMALL_RECT windowSize;
        HANDLE hOriginal;
        CONSOLE_SCREEN_BUFFER_INFO originalInfo;

        HANDLE hconsolebuffer;
        DWORD written = 0;


    public:

        bool keepBorder = true;

        ~Engine() { // destructor
            delete[] Primaryscreen;
            delete[] Secondaryscreen;
            CloseHandle(hconsolebuffer);
        }


        void
        writePrimaryScreenBuffer(const COORD coord = {0, 0}){
            WriteConsoleOutputW(hconsolebuffer, Primaryscreen, {(SHORT)primaryScreenWidth, (SHORT)primaryScreenHeight} , coord, &windowSize);
        }

        // creates border for console screen
        void
        border(const wchar_t borderSymCols = '|', const wchar_t borderSymRows = '_'){

            //Horizontal rows border
            for(int i = 1; i < primaryScreenWidth - 1; i++) {Primaryscreen[i].Char.UnicodeChar = borderSymRows;}
            for(int i = primaryCells - primaryScreenWidth; i < primaryCells; i++ ){Primaryscreen[i].Char.UnicodeChar = borderSymRows;}
            
            //Vertical cols border
            for(int i = primaryScreenWidth, j = 2 * primaryScreenWidth - 1; i < primaryCells || j < primaryCells; i += primaryScreenWidth, j += primaryScreenWidth){Primaryscreen[i].Char.UnicodeChar = borderSymCols; 
                Primaryscreen[j].Char.UnicodeChar = borderSymCols;
            }
        }

        // constructs allocates console ,ConsoleScreen buffers, constructs primary, secondary screen buffers 
        // and initializes variables
        bool 
        construct(const int32_t W = 120, const int32_t H = 30){
            secScreenWidth = W; secScreenHeight = H;
            primaryScreenWidth = W + 2; primaryScreenHeight = H + 2;
            primaryCells = primaryScreenWidth * primaryScreenHeight;
            secondaryCells = secScreenWidth * secScreenHeight;

            // Allocating a console and a scrren buffer
            Secondaryscreen = new CHAR_INFO[secScreenWidth * secScreenHeight];
            Primaryscreen = new CHAR_INFO[primaryScreenWidth * primaryScreenHeight];
            
            if (GetConsoleWindow() == NULL) {
                AllocConsole();
            }
            
            // hconsolebuffer = CreateConsoleScreenBuffer(GENERIC_READ| GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
            hconsolebuffer = GetStdHandle(STD_OUTPUT_HANDLE);
            // SetConsoleActiveScreenBuffer(hconsolebuffer);

            // sets console window, consoleScreenBuffer sizes
            SMALL_RECT rect = {0,0,1,1};
            SetConsoleWindowInfo(hconsolebuffer, TRUE, &rect);

            COORD bufferSize = {(SHORT)primaryScreenWidth, (SHORT)primaryScreenHeight};
            SetConsoleScreenBufferSize(hconsolebuffer, bufferSize);

            windowSize = {0, 0, (SHORT)(primaryScreenWidth - 1),(SHORT)(primaryScreenHeight - 1)};
            SetConsoleWindowInfo(hconsolebuffer, TRUE, &windowSize);

            clearPrimary(); // clears primaryScreen(containing border)
            clear(); // clears secondaryScreen(containing actual game pieces)
            if(keepBorder) border(); 

            return true;
        }

        void
        Draw(const COORD& coord, const wchar_t& sym, const WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE){
            int x = coord.X, y = coord.Y;
            if((x < 0 || x >= secScreenWidth) ||(y < 0 || y >= secScreenHeight) ) return;

            int index = y * secScreenWidth + x;
            Secondaryscreen[index].Char.UnicodeChar = sym;
            Secondaryscreen[index].Attributes = color;
        }

        void
        DrawString(const COORD& coord, const wchar_t *text, const WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE){
            int x = coord.X, y = coord.Y;
            if((x < 0 || x >= secScreenWidth) ||(y < 0 || y >= secScreenHeight) ) return;

            int index = (y) * secScreenWidth + (x);

            for(int i = 0; text[i] != '\0'; i++ ){
                Secondaryscreen[index + i].Char.UnicodeChar = text[i];
                Secondaryscreen[index + i].Attributes = color;
            }
        }

        //Overloaded DrawString when using multiple subscreen or layers
        void
        DrawString(const COORD& coord, const wchar_t *text, CHAR_INFO *screen, const int32_t W, const int32_t H , 
            const WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE){
            int x = coord.X, y = coord.Y;
            if((x < 0 || x >= secScreenWidth) ||(y < 0 || y >= secScreenHeight) ) return;
            
            int index = (y) * W + (x);

            for(int i = 0; text[i] != '\0'; i++ ){
                screen[index + i].Char.UnicodeChar = text[i];
                screen[index + i].Attributes = color;
            }
        }

        void clear(WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, wchar_t clear_sym = ' ')
        {
            int total = secScreenWidth * secScreenHeight;
            for (int i = 0; i < total; i++)
            {
                Secondaryscreen[i].Char.UnicodeChar = clear_sym;
                Secondaryscreen[i].Attributes = color;
            }
        }

        //Overloaded clear() when using a subScreen or multiple layers
        void clear( CHAR_INFO* screen, const int32_t W, const int32_t H, WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, wchar_t clear_sym = ' ')
        {
            int total = W * H;
            for (int i = 0; i < total; i++)
            {
                screen[i].Char.UnicodeChar = clear_sym;
                screen[i].Attributes = color;
            }
        }


        void clearPrimary(wchar_t clear_sym = ' ', WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
        {
            int total = primaryScreenWidth * primaryScreenHeight;
            for (int i = 0; i < total; i++)
            {
                Primaryscreen[i].Char.UnicodeChar = clear_sym;
                Primaryscreen[i].Attributes = color;
            }
        }


        void 
        Compose(const std::vector<CHAR_INFO*> layers = {}, const std::vector<std::pair<COORD, int>> pos_size = {}) { // adds secdonaryScreen data over PrimaryScreen with a offset of (1, 1)
            
            int index = 0, layersSize = layers.size();

            for(int i = 0; i < layersSize; i++){
                index = pos_size[i].first.Y * secScreenWidth + pos_size[i].first.X;
                for(int j = 0; j < pos_size[i].second; j++ ){
                    Secondaryscreen[j + index] = layers[i][j];
                }
            }
            
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
    if(!game.construct()) return 0;
    COORD coord = {0, 0};
    CHAR_INFO *subScreen = new CHAR_INFO[120 * 2];
    //Game Loop
    while(1){
        game.DrawString(coord, L"Hi! From Console", FOREGROUND_GREEN);
        game.Compose();
        game.writePrimaryScreenBuffer();
        coord.X += 2; coord.Y += 1;
        game.clear();
        Sleep(250);
    }
    return 0;
}