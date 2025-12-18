#include<bits/stdc++.h>
#include<stdio.h>
#include<windows.h>
#include<algorithm>
#include<unistd.h>

int
main(){

        AllocConsole();
        HANDLE hConsoleBuffer = CreateConsoleScreenBuffer(GENERIC_READ| GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
        SetConsoleActiveScreenBuffer(hConsoleBuffer);

        HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

        PINPUT_RECORD record;
        LPDWORD read;
        while(1){
            ReadConsoleInput(hIn, record,1, read);
            if(record->Event.KeyEvent.bKeyDown){
                char c = record ->Event.KeyEvent.uChar.AsciiChar;
                if(c == 'q' || c =='Q') break;
            }
        }

        CloseHandle(hConsoleBuffer);

    return 0;
}