#include<bits/stdc++.h>
#include <stdlib.h>
#include<stdio.h>
#include<algorithm>
#include<chrono>
#include<vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <clocale>
#include <windows.h>

//Macros
#define PI 22/7

//////////////// KEY MACRO /////////////////////
// MOUSE KEYS
#define MOUSE_LEFT   VK_LBUTTON
#define MOUSE_RIGHT  VK_RBUTTON
#define MOUSE_MIDDLE VK_MBUTTON

//FUNCTION KEYS
#define KEY_F1  VK_F1
#define KEY_F2  VK_F2
#define KEY_F3  VK_F3
#define KEY_F4  VK_F4
#define KEY_F5  VK_F5
#define KEY_F6  VK_F6
#define KEY_F7  VK_F7
#define KEY_F8  VK_F8
#define KEY_F9  VK_F9
#define KEY_F10 VK_F10
#define KEY_F11 VK_F11
#define KEY_F12 VK_F12

//NAVIGATION KEYS
#define KEY_INSERT   VK_INSERT
#define KEY_DELETE   VK_DELETE
#define KEY_HOME     VK_HOME
#define KEY_END      VK_END
#define KEY_PAGEUP   VK_PRIOR
#define KEY_PAGEDOWN VK_NEXT

//MODIFIER KEYS
#define KEY_SHIFT    VK_SHIFT
#define KEY_CTRL     VK_CONTROL
#define KEY_ALT      VK_MENU

#define KEY_LSHIFT   VK_LSHIFT   // 160
#define KEY_RSHIFT   VK_RSHIFT   // 161
#define KEY_LCTRL    VK_LCONTROL
#define KEY_RCTRL    VK_RCONTROL
#define KEY_LALT     VK_LMENU
#define KEY_RALT     VK_RMENU

// ARROW KEYS
#define KEY_UP       VK_UP       // 38
#define KEY_DOWN     VK_DOWN     // 40
#define KEY_LEFT     VK_LEFT     // 37
#define KEY_RIGHT    VK_RIGHT    // 39

//CONTROL KEYS
#define KEY_SPACE    VK_SPACE    // 32
#define KEY_ENTER    VK_RETURN   // 13
#define KEY_ESCAPE   VK_ESCAPE   // 27
#define KEY_TAB      VK_TAB
#define KEY_BACKSPACE VK_BACK
//////////////// END //////////////////////////

// FOREGROUND AND BACKGRUND COLOURS
enum COLOUR
{
	FG_BLACK		= 0x0000,
	FG_DARK_BLUE    = 0x0001,	
	FG_DARK_GREEN   = 0x0002,
	FG_DARK_CYAN    = 0x0003,
	FG_DARK_RED     = 0x0004,
	FG_DARK_MAGENTA = 0x0005,
	FG_DARK_YELLOW  = 0x0006,
	FG_GREY			= 0x0007,
	FG_DARK_GREY    = 0x0008,
	FG_BLUE			= 0x0009,
	FG_GREEN		= 0x000A,
	FG_CYAN			= 0x000B,
	FG_RED			= 0x000C,
	FG_MAGENTA		= 0x000D,
	FG_YELLOW		= 0x000E,
	FG_WHITE		= 0x000F,
	BG_BLACK		= 0x0000,
	BG_DARK_BLUE	= 0x0010,
	BG_DARK_GREEN	= 0x0020,
	BG_DARK_CYAN	= 0x0030,
	BG_DARK_RED		= 0x0040,
	BG_DARK_MAGENTA = 0x0050,
	BG_DARK_YELLOW	= 0x0060,
	BG_GREY			= 0x0070,
	BG_DARK_GREY	= 0x0080,
	BG_BLUE			= 0x0090,
	BG_GREEN		= 0x00A0,
	BG_CYAN			= 0x00B0,
	BG_RED			= 0x00C0,
	BG_MAGENTA		= 0x00D0,
	BG_YELLOW		= 0x00E0,
	BG_WHITE		= 0x00F0,
};

// for printing error msg on to classic consoles
int 
wdie(const char *msg)
{
    fprintf(stderr, "ERROR: %lu\nError: %s\nPress ENTER to continue...\n",GetLastError(), msg);
    getchar();
    return -1;
}

int
die(const char *msg)
{
    fprintf(stderr, "Error: %s\nPress ENTER to continue...\n",msg);
    getchar();
    return -1;
}

//classes
// vector class
class vec2f{
    public:
        float x;
        float y;

        vec2f(){}
        vec2f(float X, float Y){ x = X; y = Y;}

        //overloaded operator
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
            if(!k){ die("vec2f: Division by zero");}
            return vec2f(x/k, y/k);
        }

        static float mag(const vec2f &vec){
            return (float)sqrt((vec.x * vec.x) + (vec.y * vec.y));
        }

        static vec2f normalize(const vec2f &vec){
            float len = mag(vec);
            if (!len) {die("vec2f: Cannot normalize zero-length vector");}
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
    uint32_t H = 0;
    uint32_t W = 0;
    vec2f point{0.0f, 0.0f};
    CHAR_INFO* body = nullptr;

    Sprite() = default;

    Sprite(uint32_t h, uint32_t w) : H(h), W(w) {}

    ~Sprite() {
        delete[] body;
    }

    // bool createBody(uint32_t x, uint32_t y, const CHAR_INFO& sym) {
    //     if (x >= W || y >= H) return false;
    //     body[y * W + x] = sym;
    //     return true;
    // }

};


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
        DWORD written = 0;
        uint32_t activeBuffer = 0;


    public:
        HANDLE hconsolebuffer[2];
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
            if(!WriteConsoleOutputW(hconsolebuffer[back], Primaryscreen, {(SHORT)primaryScreenWidth, (SHORT)primaryScreenHeight} , coord, &windowSize)) 
                wdie("In consoleGameFramework At writePrimaryScreenbuffer()\n Error while writting Primaryscreen through WriteConsoleOutputW");
            activeBuffer = back;
            if(!SetConsoleActiveScreenBuffer(hconsolebuffer[activeBuffer]))
                wdie("In consoleGameFramework At writePrimaryScreenbuffer()\n Error at SetConsoleActiveScreenBuffer");
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

        void SetConsoleFont(HANDLE hConsole, short fontW, short fontH)
        {
            CONSOLE_FONT_INFOEX cfi;
            cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
            cfi.nFont = 0;
            cfi.dwFontSize.X = fontW;   // width of each cell
            cfi.dwFontSize.Y = fontH;   // height of each cell
            cfi.FontFamily = FF_DONTCARE;
            cfi.FontWeight = FW_NORMAL;
            wcscpy_s(cfi.FaceName, L"Consolas");

            if(!SetCurrentConsoleFontEx(hConsole, FALSE, &cfi)) wdie("In ConsoleGameFrame at SetConsoleFont");
        }

        bool CanCreateConsole(HANDLE hConsole, int32_t w, int32_t h, COORD& maxSize)
        {
            maxSize = GetLargestConsoleWindowSize(hConsole);
            
            if (maxSize.X == 0 || maxSize.Y == 0)
                return false;

            return (w <= maxSize.X && h <= maxSize.Y);
        }


        // constructs allocates console ,ConsoleScreen buffers, constructs primary, secondary screen buffers 
        // and initializes variables
        bool 
        construct(short fw, short fh, const int32_t W, const int32_t H)
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
                freopen("CONIN$", "r", stdin);
                freopen("CONOUT$", "w", stdout);
                freopen("CONOUT$", "w", stderr);
            }

            // Creates TWO console screen buffers
            for (int i = 0; i < 2; i++) {
                hconsolebuffer[i] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,0,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
                if (hconsolebuffer[i] == INVALID_HANDLE_VALUE) return false;
                SetConsoleFont(hconsolebuffer[i], fw, fh); // try (6,12), (8,16), (10,20)
            }

            COORD maxSize;
            if (!CanCreateConsole( hconsolebuffer[0], primaryScreenWidth,primaryScreenHeight,maxSize))
            {
                printf("ERROR: Requested console size too large\n");
                printf("Requested: %d x %d \n", secScreenWidth, secScreenHeight);
                printf("Maximum allowed: %d x %d \n", maxSize.X - 2, maxSize.Y-2 );
                printf("Press ESC to quit");
                while(1){if(GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;}
                return false;
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
        
        Sprite LoadSpriteFromFile(const std::string& path, WORD color =  FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
        {
            std::wifstream file(path);

            if (!file.is_open())
                die("In ConsoleGameFramework at LoadSpriteFromFile(): Failed to open sprite file");

            uint32_t W, H;
            file >> W >> H;
            file.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');

            Sprite sprite(H, W);
            sprite.body = new CHAR_INFO[W * H];

            std::wstring line;

            // ─────────────────────────────
            // Skip blank line
            // ─────────────────────────────
            std::getline(file, line);

            // ─────────────────────────────
            // Read character glyphs
            // ─────────────────────────────
            for (uint32_t y = 0; y < H; y++)
            {
                std::getline(file, line);

                std::wstringstream ss(line);

                for (uint32_t x = 0; x < W; x++)
                {
                    unsigned int glyph;
                    ss >> std::hex >> glyph;   // read hex value like 2588

                    sprite.body[y * W + x].Char.UnicodeChar = static_cast<wchar_t>(glyph);
                }
            }

            // ─────────────────────────────
            // Skip blank line
            // ─────────────────────────────
            std::getline(file, line);

            // ─────────────────────────────
            // Read color attributes
            // ─────────────────────────────
            
            for (uint32_t y = 0; y < H; y++)
            {
                std::getline(file, line);
                std::wstringstream ss(line);

                for (uint32_t x = 0; x < W; x++)
                {
                    unsigned int attr;
                    ss >> std::hex >> attr;
                    color = static_cast<short>(attr);
                    sprite.body[y * W + x].Attributes = color;
                }
            }

            return sprite;
        }

        
        void
        run(const short fw = 8, const short fh = 16, const int32_t W = 140, const int32_t H = 40){

            if(!create(fw, fh, W, H)){std::cerr << "creation failed!" << std::endl; return;}
            std::chrono::time_point tp1 =  std::chrono::steady_clock::now();

            if(!load()){die(" In ConsoleGameFramework at run(): load(); loading failed"); return;}

            //Game loop
            while(1){
                if (GetAsyncKeyState(KEY_ESCAPE) & 0x8000) // hit escape to close gameloop and exit
                break;

                std::chrono::time_point tp2 = std::chrono::steady_clock::now();
                std::chrono::duration<float> elapsedTime = tp2 - tp1;
                tp1 = tp2;

                clear();
                updateInput();
                update(elapsedTime.count());
                render();

                Sleep(1000/(float)refreshRate);
            }
        }

        bool create(const short fw = 8, const short fh = 16, const int32_t W = 140, const int32_t H = 40 ) {
            if(!construct(fw, fh, W, H)){die("In ConsoleGameFrame at create(): construct(): construction of console and screen failed");
            return false;}
            return true;
        }

        virtual bool
        load(){return true;}

        virtual bool
        update(float elapsedT){ return true;}

        virtual bool
        render(){ return true;}

};
