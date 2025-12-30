#include<bits/stdc++.h>
#include<stdio.h>
#include<algorithm>
#include<chrono>
#include<vector>
#include "keys.h"
#include <cmath>
// #include <windows.h>

//Macros
#define PI 22/7


//classes

class vec2f{
    public:
        float x;
        float y;

        vec2f(){}
        vec2f(float X, float Y){ x = X; y = Y;}

        vec2f operator+(const vec2f &vec) const{
            return vec2f(x + vec.x, y + vec.y);
        }

        vec2f operator-(const vec2f &vec) const{
            return vec2f(x - vec.x, y - vec.y);
        }

        vec2f operator*(const float k) const{
            return vec2f(x*k, y*k);
        }

        vec2f operator/(const float k) const{
            return vec2f(x/k, y/k);
        }

        static float mag(const vec2f &vec){
            return (float)sqrt((vec.x * vec.x) + (vec.y * vec.y));
        }

        static vec2f normalize(const vec2f &vec){
            float len = mag(vec);
            return vec2f(vec.x/len, vec.y/len);
        }

        static float dot(const vec2f &vec1, const vec2f &vec2){
            return (vec1.x * vec1.x) + (vec2.y * vec2.y);
        }

        static float dis(const vec2f vec1, const vec2f vec2){
            return (float)sqrt((vec1.x - vec2.x)*(vec1.x - vec2.x) + (vec1.y - vec2.y)*(vec1.y - vec2.y));
        }
};

class Angle{

    float rad;
    float deg;

    Angle(float r, float d): rad(r), deg(d){}
    Angle(){}

    static float toRad(const float deg){
        return deg * PI/180;
    }
    static float toDeg(const float rad){
        return rad * 180/PI;
    }

};

class Sprite {
public:
    uint32_t H = 0, W = 0;
    vec2f point{0.0f, 0.0f};
    CHAR_INFO* body = nullptr;

    Sprite() = default;

    Sprite(uint32_t h, uint32_t w) : H(h), W(w) {
        body = new CHAR_INFO[W * H]{};
    }

    ~Sprite() {
        delete[] body;
    }

    bool createBody(uint32_t x, uint32_t y, const CHAR_INFO& sym) {
        if (x >= W || y >= H) return false;
        body[y * W + x] = sym;
        return true;
    }
};


class Engine : public Sprite{
    
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
        struct key{
            bool pressed = false;
            bool released = false;
            bool held = false;
        } keys[256];

        vec2f pos = {0.0f, 0.0f};

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
        Draw(const vec2f pos, const CHAR_INFO &sym){
            int x = pos.x, y = pos.y;
            if((x < 0 || x >= secScreenWidth) ||(y < 0 || y >= secScreenHeight) ) return;

            int index = y * secScreenWidth + x;
            Secondaryscreen[index].Char.UnicodeChar = sym.Char.UnicodeChar;
            Secondaryscreen[index].Attributes = sym.Attributes;
        }

        void
        DrawString(const vec2f pos, const std::wstring text, const WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE){
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
        DrawString(const vec2f pos, const std::wstring text, CHAR_INFO *screen, const int32_t W, const int32_t H , 
            const WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE){
            int x = pos.x, y = pos.y;
            if((x < 0 || x >= secScreenWidth) ||(y < 0 || y >= secScreenHeight) ) return;
            
            int index = (y) * W + (x);

            for(int i = 0; text[i] != '\0'; i++ ){
                screen[index + i].Char.UnicodeChar = text[i];
                screen[index + i].Attributes = color;
            }
        }

        bool DrawSprite(const Sprite& sprite) {
            vec2f pos;

            for (uint32_t y = 0; y < sprite.H; y++) {
                for (uint32_t x = 0; x < sprite.W; x++) {
                    pos.x = sprite.point.x + x;
                    pos.y = sprite.point.y + y;
                    Draw(pos, sprite.body[y * sprite.W + x]);
                }
            }
            return true;
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

        bool updateInput(){
            for(int i = 0; i < 256; i++){
                bool state = GetAsyncKeyState(i) & 0x8000;
                keys[i].pressed = state && !keys[i].held;
                keys[i].released = !state && keys[i].held;
                keys[i].held = state;

            }
            return true;
        }

        void
        run(){

            if(!create()){std::cerr << "creation failed!" << std::endl;}
            std::chrono::time_point tp1 =  std::chrono::steady_clock::now();

            //Game loop
            while(1){
                if (GetAsyncKeyState(KEY_ESCAPE) & 0x8000) // hit escape to close gameloop and exit
                break;

                std::chrono::time_point tp2 = std::chrono::steady_clock::now();
                std::chrono::duration<float> elapsedTime = tp2 - tp1;
                tp1 = tp2;

                updateInput();
                update(elapsedTime.count());
                render();

                Sleep(1000/(float)refreshRate);
            }
        }
        

        virtual bool
        create(){return true;}

        virtual bool
        update(float elapsedT){ return true;}

        virtual bool
        render(){ return true;}
};

class dummy: public Engine{

    public:

        vec2f pos = {0.0f, 0.0f};
        vec2f coord = {0,29};
        vec2f velocity = {23.0,20.0};
        Sprite square = Sprite(3,3);

        wchar_t box[9] = {'#', '#' , '#'
                         ,'#',  ' ', '#'
                         ,'#', '#', '#'};

        bool create() override {
            if (!construct()) {
                std::cerr << "Console construction failed!\n";
                return false;
            }

            square.point = {1, 1};

            for (int y = 0; y < 3; y++) {
                for (int x = 0; x < 3; x++) {
                    CHAR_INFO temp{};
                    temp.Char.UnicodeChar = box[y * 3 + x];
                    temp.Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
                    square.createBody(x, y, temp);
                }
            }

            clear();
            return true;
        }


        void scene(){
            DrawString(pos, L"Hi! From Console");
        }

        // bool update(float elapsedT) override{

        //     std::string str = "X: " + std::to_string(velocity.x) + " Y: " + std::to_string(velocity.y);
        //     std::wstring wstr(str.begin(), str.end());

            
        //     DrawString({0,28}, wstr);

        //     if(keys['W'].held) {pos.y -= velocity.y * elapsedT; DrawString({0,29},L"pressed w");}
        //     if(keys['S'].held) {pos.y += velocity.y * elapsedT; DrawString({0,29},L"pressed s");}
        //     if(keys['A'].held) {pos.x -= velocity.x * elapsedT; DrawString({0,29},L"pressed a");}
        //     if(keys['D'].held) {pos.x += velocity.x * elapsedT; DrawString({0,29},L"pressed d");}
        //     scene();
        //     return true;
        // }

        bool update(float elapsedT) override{
            if(keys[MOUSE_LEFT].held){
                velocity.x = 100;
            }
            else velocity.x = 23;
            if(keys['W'].held) {square.point.y -= velocity.y * elapsedT; DrawString(coord,L"pressed w");}
            if(keys['S'].held) {square.point.y += velocity.y * elapsedT; DrawString(coord,L"pressed s");}
            if(keys['A'].held) {square.point.x -= velocity.x * elapsedT; DrawString(coord,L"pressed a");}
            if(keys['D'].held) {square.point.x += velocity.x * elapsedT; DrawString(coord,L"pressed d");}

            DrawSprite(square);
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