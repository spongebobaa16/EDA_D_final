#include "Module.h"
#include <iostream>






vector<vector<Width_Height>> L{
    {},
    {Width_Height(1, 1)},
    {Width_Height(1, 2)},
    {Width_Height(2, 2)},
    {Width_Height(2, 2)},
    {Width_Height(2, 3)},
    {Width_Height(2, 3)},
    {Width_Height(2, 4)},
    {Width_Height(2, 4)},
    {Width_Height(3, 3)},
    {Width_Height(3, 4)},
    {Width_Height(3, 4)},
    {Width_Height(3, 4)},
    {Width_Height(3, 5), Width_Height(4, 4)},
    {Width_Height(3, 5), Width_Height(4, 4)},
    {Width_Height(3, 5), Width_Height(4, 4)},
    {Width_Height(4, 4), Width_Height(3, 6)},
    {Width_Height(3, 6), Width_Height(4, 5)},
    {Width_Height(3, 6), Width_Height(4, 5)},
    {Width_Height(4, 5)},
    {Width_Height(4, 5)},
    {Width_Height(4, 6), Width_Height(5, 5)},
    {Width_Height(4, 6), Width_Height(5, 5)},
    {Width_Height(4, 6), Width_Height(5, 5)},
    {Width_Height(4, 6), Width_Height(5, 5)},
    {Width_Height(5, 5), Width_Height(4, 7)},
    {Width_Height(4, 7), Width_Height(5, 6)},
    {Width_Height(4, 7), Width_Height(5, 6)},
    {Width_Height(4, 7), Width_Height(5, 6)},
    {Width_Height(5, 6), Width_Height(4, 8)},
    {Width_Height(5, 6), Width_Height(4, 8)},
    {Width_Height(4, 8), Width_Height(5, 7), Width_Height(6, 6)},
    {Width_Height(4, 8), Width_Height(5, 7), Width_Height(6, 6)},
    {Width_Height(5, 7), Width_Height(6, 6)},
    {Width_Height(5, 7), Width_Height(6, 6)},
    {Width_Height(5, 7), Width_Height(6, 6)},
    {Width_Height(6, 6), Width_Height(5, 8)},
    {Width_Height(5, 8), Width_Height(6, 7)},
    {Width_Height(5, 8), Width_Height(6, 7)},
    {Width_Height(5, 8), Width_Height(6, 7)},
    {Width_Height(5, 8), Width_Height(6, 7)},
    {Width_Height(6, 7), Width_Height(5, 9)},
    {Width_Height(6, 7), Width_Height(5, 9)},
    {Width_Height(5, 9), Width_Height(6, 8), Width_Height(7, 7)},
    {Width_Height(5, 9), Width_Height(6, 8), Width_Height(7, 7)},
    {Width_Height(5, 9), Width_Height(6, 8), Width_Height(7, 7)},
    {Width_Height(6, 8), Width_Height(7, 7), Width_Height(5, 10)},
    {Width_Height(6, 8), Width_Height(7, 7), Width_Height(5, 10)},
    {Width_Height(6, 8), Width_Height(7, 7), Width_Height(5, 10)},
    {Width_Height(7, 7), Width_Height(5, 10), Width_Height(6, 9)},
    {Width_Height(5, 10), Width_Height(6, 9), Width_Height(7, 8)},
    {Width_Height(6, 9), Width_Height(7, 8)},
    {Width_Height(6, 9), Width_Height(7, 8)},
    {Width_Height(6, 9), Width_Height(7, 8)},
    {Width_Height(6, 9), Width_Height(7, 8)},
    {Width_Height(7, 8), Width_Height(6, 10)},
    {Width_Height(7, 8), Width_Height(6, 10)},
    {Width_Height(6, 10), Width_Height(7, 9), Width_Height(8, 8)},
    {Width_Height(6, 10), Width_Height(7, 9), Width_Height(8, 8)},
    {Width_Height(6, 10), Width_Height(7, 9), Width_Height(8, 8)},
    {Width_Height(6, 10), Width_Height(7, 9), Width_Height(8, 8)},
    {Width_Height(7, 9), Width_Height(8, 8), Width_Height(6, 11)},
    {Width_Height(7, 9), Width_Height(8, 8), Width_Height(6, 11)},
    {Width_Height(7, 9), Width_Height(8, 8), Width_Height(6, 11)},
    {Width_Height(8, 8), Width_Height(6, 11), Width_Height(7, 10)},
    {Width_Height(6, 11), Width_Height(7, 10), Width_Height(8, 9)},
    {Width_Height(6, 11), Width_Height(7, 10), Width_Height(8, 9)},
    {Width_Height(7, 10), Width_Height(8, 9), Width_Height(6, 12)},
    {Width_Height(7, 10), Width_Height(8, 9), Width_Height(6, 12)},
    {Width_Height(7, 10), Width_Height(8, 9), Width_Height(6, 12)},
    {Width_Height(7, 10), Width_Height(8, 9), Width_Height(6, 12)},
    {Width_Height(8, 9), Width_Height(6, 12), Width_Height(7, 11)},
    {Width_Height(8, 9), Width_Height(6, 12), Width_Height(7, 11)}

};


void Module::calculate_w_l(int area){
    float ratio = 1.0;
    if(area <= 72){
        for(int i=0; i<L[area].size(); i++){
            validWH.push_back(L[area][i]);
        }
        // for(Width_Height i: validWH){
        //     cout << "targetarea: " << area <<  "\tw: " <<  i.width << "\th: " << i.height << "\tarea: " << i.width*i.height << endl;
        // }
    }
    else{
        vector<Width_Height> tmp;
        for(int i=0; i<10; i++){
            float w = sqrt(float(area)/ratio);
            tmp.push_back(Width_Height(int(w)+1, int(float(area)/w)+1));
            ratio = ratio + 0.1;
        }
        sort(tmp.begin(), tmp.end(), [](Width_Height a, Width_Height b){
            return a.width*a.height < b.width*b.height;
        });

        //test
        // for(Width_Height i: tmp){
        //     cout << "targetarea: " << area <<  "\tw: " <<  i.width << "\th: " << i.height << "\tarea: " << i.width*i.height << endl;
        // }

        validWH.push_back(tmp[0]);
        for(int i=1; i<tmp.size(); i++){
            if(tmp[i].height != tmp[i-1].height && tmp[i].width != tmp[i-1].width){
                validWH.push_back(tmp[i]);
            }
        }
        // cout << endl;
        // for(Width_Height i: validWH){
        //     cout << "targetarea: " << area <<  "\tw: " <<  i.width << "\th: " << i.height << "\tarea: " << i.width*i.height << endl;
        // }
        // cout << endl;
        // cout << endl;
    }

    
}