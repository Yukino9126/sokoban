#pragma once
#include <vector>
using std::vector;

#define wall_character 'H'
#define box_character 'B'
#define dest_character ' '
#define worker_character 'W'


class CFloor {
public:
    char form;
    short state;

    CFloor(char a, short b) {
        form = a;
        state = b;
    }
};
