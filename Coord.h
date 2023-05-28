#ifndef COORD_H
#define COORD_H

class Coord{
public:
    Coord (int _x, int _y):x(_x), y(_y) {}
    Coord (){}
    ~Coord(){}

    int x;
    int y;
};

class Contour_horizontal{
public:
    Contour_horizontal (int _x, int _l):til_x(_x),height(_l) {}
    Contour_horizontal (){}
    ~Contour_horizontal(){}

    int til_x;           //
    int height;
};

class Contour_vertical{
public:
    Contour_vertical (int _y, int _l):til_y(_y), width(_l) {}
    Contour_vertical (){}
    ~Contour_vertical(){}

    int til_y;
    int width;
};

class Width_Height{
public:
    Width_Height(int w, int h): width(w), height(h){}
    Width_Height(){}
    ~Width_Height(){}
    int width;
    int height;
};

#endif