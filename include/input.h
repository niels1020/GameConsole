#pragma once

// from left to right with the pin number they are connected
enum Button : uint {
    Button1 = 14,
    Button2 = 13,
    Button3 = 12,
    Button4 = 11,
};

//configure everythin for the input to work
void InputInit();