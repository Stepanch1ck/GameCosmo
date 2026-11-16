#pragma once

#include <glad/glad.h>

class Texture2D
{
public:
    unsigned int ID;
    int Width, Height;
    unsigned int Internal_Format;
    unsigned int Image_Format;
    unsigned int Wrap_S;
    unsigned int Wrap_T;
    unsigned int Filter_Min;
    unsigned int Filter_Max;

    Texture2D();

    void Generate(int width, int height, unsigned char* data);
    void Bind() const;
};