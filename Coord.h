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
    Contour_horizontal (int _x, int _l):to_x(_x),height_base(_l) {}
    Contour_horizontal (){}
    ~Contour_horizontal(){}

    int to_x;           //
    int height_base;
};

class Contour_vertical{
public:
    Contour_vertical (int _y, int _l):to_y(_y), width_base(_l) {}
    Contour_vertical (){}
    ~Contour_vertical(){}

    int to_y;
    int width_base;
};

#endif