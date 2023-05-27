#include "Solver.h"

Solver::Solver(){

}

void Solver::readFile(const char* filename){
    ifstream fin;
    fin.open(filename);
    string CHIP, SOFTMODULE, moduleName;
    int width, height, num_module, area;
    fin >> width >> height;
    chip_width  = width;
    chip_height = height;

    fin >> SOFTMODULE >> num_module;
    for(int i=0; i<num_module; i++){
        fin >> moduleName >> area;
        U_Name_Index[moduleName] = i;
        Module* newModule = new Module();
        newModule->area = area;
        newModule->fixed = false;

        
    }

}