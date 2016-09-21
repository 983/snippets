#pragma once

struct Color {
    static uint32_t white(){
        return 0xffffffff;
    }

    static uint32_t green(){
        return 0xff00ff00;
    }

    static void to_float(uint32_t color, float *rgba){
        for (int i = 0; i < 4; i++){
            rgba[i] = 1.0f/256.0f*((color >> i*8) & 0xff);
        }
    }
};
