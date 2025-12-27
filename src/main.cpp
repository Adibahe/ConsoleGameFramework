#include<bits/stdc++.h>
#include<stdio.h>
#include<windows.h>
#include<algorithm>
#include<chrono>
#include<vector>


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
        HANDLE hconsolebuffer[2];
        DWORD written = 0;
        uint32_t activeBuffer = 0;


    public:
        int32_t frameTime;
        bool keepBorder = true;
        uint32_t refreshRate = 60; // FPS

        struct vec{
            float x;
            float y;
        };

        ~Engine() { // destructor
            delete[] Primaryscreen;
            delete[] Secondaryscreen;
            CloseHandle(hconsolebuffer[0]);
            CloseHandle(hconsolebuffer[1]);
        }


        void
        writePrimaryScreenBuffer(const COORD coord = {0, 0}){
            int back = 1 - activeBuffer;
            WriteConsoleOutputW(hconsolebuffer[back], Primaryscreen, {(SHORT)primaryScreenWidth, (SHORT)primaryScreenHeight} , coord, &windowSize);
            activeBuffer = back;
            SetConsoleActiveScreenBuffer(hconsolebuffer[activeBuffer]);
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
        construct(const int32_t W = 120, const int32_t H = 30)
        {
            secScreenWidth = W;
            secScreenHeight = H;

            primaryScreenWidth  = W + 2;
            primaryScreenHeight = H + 2;
            secondaryCells = secScreenWidth * secScreenHeight;
            primaryCells   = primaryScreenWidth * primaryScreenHeight;

            Secondaryscreen = new CHAR_INFO[secondaryCells];
            Primaryscreen   = new CHAR_INFO[primaryCells];

            if (GetConsoleWindow() == NULL) {
                AllocConsole();
            }

            // Creates TWO console screen buffers
            for (int i = 0; i < 2; i++) {
                hconsolebuffer[i] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,0,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
                if (hconsolebuffer[i] == INVALID_HANDLE_VALUE) return false;
            }

            SMALL_RECT tempRect = {0, 0, 1, 1};
            for (int i = 0; i < 2; i++) {
                SetConsoleWindowInfo(hconsolebuffer[i], TRUE, &tempRect);
            }

            // Set buffer size
            COORD bufferSize = {
                (SHORT)primaryScreenWidth,
                (SHORT)primaryScreenHeight
            };

            for (int i = 0; i < 2; i++) {
                SetConsoleScreenBufferSize(hconsolebuffer[i], bufferSize);
            }

            SMALL_RECT windowRect = {0,0,(SHORT)(primaryScreenWidth - 1), (SHORT)(primaryScreenHeight - 1)};

            for (int i = 0; i < 2; i++) {
                SetConsoleWindowInfo(hconsolebuffer[i], TRUE, &windowRect);
            }
            windowSize = windowRect;

            activeBuffer = 0;
            SetConsoleActiveScreenBuffer(hconsolebuffer[activeBuffer]);
            clearPrimary();
            clear();

            if (keepBorder) border();

            return true;
        }


        void
        Draw(const vec pos, const wchar_t& sym, const WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE){
            int x = pos.x, y = pos.y;
            if((x < 0 || x >= secScreenWidth) ||(y < 0 || y >= secScreenHeight) ) return;

            int index = y * secScreenWidth + x;
            Secondaryscreen[index].Char.UnicodeChar = sym;
            Secondaryscreen[index].Attributes = color;
        }

        void
        DrawString(const vec pos, const wchar_t *text, const WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE){
            int x = pos.x, y = pos.y;
            if((x < 0 || x >= secScreenWidth) ||(y < 0 || y >= secScreenHeight) ) return;

            int index = (y) * secScreenWidth + (x);

            for(int i = 0; text[i] != '\0'; i++ ){
                Secondaryscreen[index + i].Char.UnicodeChar = text[i];
                Secondaryscreen[index + i].Attributes = color;
            }
        }

        //Overloaded DrawString when using multiple subscreen or layers
        void
        DrawString(const vec pos, const wchar_t *text, CHAR_INFO *screen, const int32_t W, const int32_t H , 
            const WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE){
            int x = pos.x, y = pos.y;
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

        void
        run(){

            if(!LoadState()){std::cerr << "Loading failed" << std::endl;}
            std::chrono::time_point tp1 =  std::chrono::steady_clock::now();

            //Game loop
            while(1){
                if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
                break;

                std::chrono::time_point tp2 = std::chrono::steady_clock::now();
                std::chrono::duration<float> elapsedTime = tp2 - tp1;
                tp1 = tp2;

                update(elapsedTime.count());
                render();

                Sleep(1000/(float)refreshRate);
            }

        }
        
        virtual bool
        LoadState(){return true;}

        virtual bool
        update(float elapsedT){ return true;}

        virtual bool
        render(){ return true;}
};

class dummy: public Engine{

    public:

        vec pos = {0.0f, 0.0f};
        vec velocity = {1,1};

        bool LoadState() override{
            if(!construct()){std::cerr << "Console construction failed!" << std::endl; return false;}
            clear();
            return true;
        }

        void scene(){
            DrawString(pos, L"Hi! From Console");
        }

        bool update(float elapsedT) override{
            scene();
            pos.x += velocity.x * elapsedT;
            pos.y += velocity.y * elapsedT;
            return true;
        }

        bool render() override{
            Compose();
            writePrimaryScreenBuffer();
            clear();
            return true;
        }


        
};

int
main(){

    dummy d;
    d.refreshRate = 60;
    d.run();
    return 0;
}