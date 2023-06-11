#include "Solver.h"

void Solver::readFile(const char *filename)
{
    ifstream fin;
    fin.open(filename);
    string CHIP, SOFTMODULE, FIXEDMODULE, CONNECTION, moduleName, n1, n2;
    int width, height, num_softmodule, num_fixedmodule, area, num_connection;
    int x_c, y_c, fixwidth, fixheight, pN;

    fin >> CHIP >> width >> height;
    chip_width = width;
    chip_height = height;

    fin >> SOFTMODULE >> num_softmodule;
    for (int i = 0; i < num_softmodule; i++)
    {
        fin >> moduleName >> area;
        U_Name_Index[moduleName] = i;
        Module *newModule = new Module();
        newModule->index = i;
        newModule->name = moduleName;
        newModule->area = area;
        Width_Height w_l = calculate_w_l(area);
        newModule->width = w_l.width;
        newModule->height = w_l.height;
        newModule->fixed = false;
        Modules.push_back(newModule);
    }
    fin >> FIXEDMODULE >> num_fixedmodule;
    for (int i = 0; i < num_fixedmodule; i++)
    {
        fin >> moduleName >> x_c >> y_c >> fixwidth >> fixheight;
        U_Name_Index[moduleName] = i + num_softmodule;
        Module *newModule = new Module();
        newModule->index = i + num_softmodule;
        newModule->name = moduleName;
        newModule->area = fixwidth * fixheight;
        Coord fixloc;
        fixloc.x = x_c;
        fixloc.y = y_c;
        newModule->fix_location = fixloc;
        newModule->width = fixwidth;
        newModule->height = fixheight;
        newModule->fixed = true;
        Modules.push_back(newModule);
    }
    fin >> CONNECTION >> num_connection;
    for (int i = 0; i < num_connection; i++)
    {
        fin >> n1 >> n2 >> pN;
        U_Name_Index[n1];
        Connection c(n1, n2, U_Name_Index[n1], U_Name_Index[n2], pN);
        Connections.push_back(c);
    }

    Contour_horizontal initContour = Contour_horizontal(chip_width, 0);
    Contour_H.push_back(initContour);
}

void Solver::readFile_givenWL(const char *filename)
{
    ifstream fin;
    fin.open(filename);
    string CHIP, SOFTMODULE, FIXEDMODULE, CONNECTION, moduleName, n1, n2;
    int width, height, num_softmodule, num_fixedmodule, area, num_connection;
    int x_c, y_c, fixwidth, fixheight, pN;

    fin >> CHIP >> width >> height;
    chip_width = width;
    chip_height = height;

    fin >> SOFTMODULE >> num_softmodule;
    for (int i = 0; i < num_softmodule; i++)
    {
        fin >> moduleName >> width >> height;
        U_Name_Index[moduleName] = i;
        Module *newModule = new Module();
        newModule->index = i;
        newModule->name = moduleName;
        newModule->area = width * height;
        newModule->width = width;
        newModule->height = height;
        newModule->fixed = false;
        Modules.push_back(newModule);
    }
    fin >> FIXEDMODULE >> num_fixedmodule;
    for (int i = 0; i < num_fixedmodule; i++)
    {
        fin >> moduleName >> x_c >> y_c >> fixwidth >> fixheight;
        U_Name_Index[moduleName] = i + num_softmodule;
        Module *newModule = new Module();
        newModule->index = i + num_softmodule;
        newModule->name = moduleName;
        newModule->area = fixwidth * fixheight;
        Coord fixloc;
        fixloc.x = x_c;
        fixloc.y = y_c;
        newModule->fix_location = fixloc;
        newModule->width = fixwidth;
        newModule->height = fixheight;
        newModule->fixed = true;
        Modules.push_back(newModule);
    }
    fin >> CONNECTION >> num_connection;
    for (int i = 0; i < num_connection; i++)
    {
        fin >> n1 >> n2 >> pN;
        U_Name_Index[n1];
        Connection c(n1, n2, U_Name_Index[n1], U_Name_Index[n2], pN);
        Connections.push_back(c);
    }
    Contour_horizontal initContour = Contour_horizontal(chip_width, 0);
    Contour_H.push_back(initContour);
}

Width_Height Solver::calculate_w_l(int area)
{
    // int square_root = sqrt(area);
    // if(float(area)/float(square_root*(square_root+1)) > 0.8){
    //     Width_Height ans;
    //     ans.height = square_root;
    //     ans.width = square_root+1;

    //     return ans;
    // }
}

void Solver::floorplan(B_Tree t)
{
    placeBlock(t.root, 0);
}

void Solver::placeBlock(Node *node, int type)
{
    if (node == NULL)
        return;

    if (type == 0)
    {
        Coord root_loc(0, 0);
        Modules[node->index]->location = root_loc;
        int Yloc = findYandUpdateContour_H(node->index, 0, Modules[node->index]->width);
    }
    else if (type == 1)
    {
        int from_x = Modules[node->parent->index]->location.x + Modules[node->parent->index]->width;
        int to_x = from_x + Modules[node->index]->width;
        int Yloc = findYandUpdateContour_H(node->index, from_x, to_x);
        Coord loc(from_x, Yloc);
        Modules[node->index]->location = loc;
    }
    else if (type == 2)
    {
        int from_x = Modules[node->parent->index]->location.x;
        int to_x = from_x + Modules[node->index]->width;
        int Yloc = findYandUpdateContour_H(node->index, from_x, to_x);
        Coord loc(from_x, Yloc);
        Modules[node->index]->location = loc;
    }
    placeBlock(node->left, 1);
    placeBlock(node->right, 2);
}

int Solver::findYandUpdateContour_H(int index, int from_x, int to_x)
{ // not sure all correct??
    cout << "\nindex: " << index << " from_x: " << from_x << " to_x: " << to_x << endl;
    int max_height = 0;
    int insert_place = 0;
    bool init = false;
    for (int i = 0; i < Contour_H.size(); i++)
    {
        if (!init && Contour_H[i].til_x >= to_x)
        {
            max_height = Contour_H[i].height;
            init = true;
        }
    }
    cout << "pre_max_height: " << max_height << endl;
    for (int i = Contour_H.size() - 1; i >= 0; i--)
    {
        if (Contour_H[i].til_x > from_x && Contour_H[i].til_x <= to_x)
        {
            if (Contour_H[i].height <= max_height)
            {
                Contour_H.erase(Contour_H.begin() + i);
            }
            else
            {
                max_height = Contour_H[i].height;
                Contour_H.erase(Contour_H.begin() + i);
            }
        }
    }

    for (int i = 0; i < Contour_H.size(); i++)
    {
        if (to_x > Contour_H[i].til_x)
            insert_place++;
    }
    cout << "insert_place: " << insert_place << " max_height: " << max_height << endl;
    Contour_horizontal a(to_x, Modules[index]->height + max_height);
    Contour_H.insert(Contour_H.begin() + insert_place, a);

    for (int i = 0; i < Contour_H.size(); i++)
    {
        cout << " til_x: " << Contour_H[i].til_x << " height: " << Contour_H[i].height << endl;
    }

    return max_height;
    /*
        int update_from_index = -1, update_to_index = -1;
        int max_height = 0;
        for(int i=0; i<Contour_H.size(); i++){
            cout << "index: " << index << " til_x: " <<  Contour_H[i].til_x << " height: " << Contour_H[i].height << endl;

            if (update_from_index == -1){
                if(Contour_H[i].til_x > from_x && Contour_H[i].til_x <= to_x){
                    update_from_index   = i;
                    if(Contour_H[i].height > max_height){
                        max_height = Contour_H[i].height;
                        cout << "index: " << index << " mh: " <<  max_height << endl;
                    }
                }
                else{

                }
            }
            else{
                if(Contour_H[i].til_x > from_x && Contour_H[i].til_x <= to_x){
                    if(Contour_H[i].height > max_height){
                        max_height = Contour_H[i].height;
                    }
                }
                else{
                    update_to_index     = i-1;
                    break;
                }
            }
        }
        cout << "fromIDX: " << update_from_index << " toIDX: " << update_to_index << " maxHeight: " << max_height << endl;
        if(update_from_index != 0){
            if     (Modules[index]->height + max_height == Contour_H[update_from_index-1].height && Modules[index]->height + max_height == Contour_H[update_to_index+1].height){
                Contour_H.erase(Contour_H.begin()+update_from_index-1, Contour_H.begin()+update_to_index+1);
            }
            else if(Modules[index]->height + max_height == Contour_H[update_from_index-1].height && Modules[index]->height + max_height != Contour_H[update_to_index+1].height){
                Contour_H.erase(Contour_H.begin()+update_from_index-1, Contour_H.begin()+update_to_index+1);
                Contour_horizontal a(to_x, Modules[index]->height + max_height);
                Contour_H.insert(Contour_H.begin()+update_from_index, a);
            }
            else if(Modules[index]->height + max_height != Contour_H[update_from_index-1].height && Modules[index]->height + max_height == Contour_H[update_to_index+1].height){
                Contour_H.erase(Contour_H.begin()+update_from_index, Contour_H.begin()+update_to_index+1);
            }
            else{
                Contour_H.erase(Contour_H.begin()+update_from_index, Contour_H.begin()+update_to_index+1);
                Contour_horizontal a(to_x, Modules[index]->height + max_height);
                Contour_H.insert(Contour_H.begin()+update_from_index, a);
            }
        }
        else{
            if(Modules[index]->height + max_height == Contour_H[update_to_index+1].height){
                Contour_H.erase(Contour_H.begin()+update_from_index, Contour_H.begin()+update_to_index+1);
            }
            else{
                Contour_H.erase(Contour_H.begin()+update_from_index, Contour_H.begin()+update_to_index+1);
                Contour_horizontal a(to_x, Modules[index]->height + max_height);
                Contour_H.insert(Contour_H.begin()+update_from_index, a);
            }
        }
        return max_height;
    */
}

double Solver::calculate_totalcost()
{
    double A = 0; // area of the current floorplan
    double HPWL = 0;
    int prev_til_x = 0;

    int l = Contour_H.size();
    for (int i = 0; i < l; i++)
    {
        // cout<<Contour_H[i].til_x<<" "<<Contour_H[i].height<<endl;
        A += (Contour_H[i].til_x - prev_til_x) * Contour_H[i].height;
        prev_til_x = Contour_H[i].til_x;
    }

    l = Connections.size();
    for (int i = 0; i < l; i++)
    {
        HPWL += (abs((Modules[Connections[i].index_name1]->location.x + double(Modules[Connections[i].index_name1]->width) / 2) - (Modules[Connections[i].index_name2]->location.x + double(Modules[Connections[i].index_name2]->width) / 2)) + abs((Modules[Connections[i].index_name1]->location.y + double(Modules[Connections[i].index_name1]->height) / 2) - (Modules[Connections[i].index_name2]->location.y + double(Modules[Connections[i].index_name2]->height) / 2))) * Connections[i].pin_Number;
    }

    return A + HPWL; //////////////////////////////////////////////
}

void Solver::printModules()
{
    for (int i = 0; i < Modules.size(); i++)
    {
        cout << " index: " << Modules[i]->index << " name: " << Modules[i]->name;
        cout << " area: " << Modules[i]->area << " width: " << Modules[i]->width;
        cout << " height: " << Modules[i]->height << " fixed: " << Modules[i]->fixed << endl;
    }
}

void Solver::printLocations()
{
    for (int i = 0; i < Modules.size(); i++)
    {
        cout << "index: " << i;
        cout << " x: " << Modules[i]->location.x;
        cout << " y: " << Modules[i]->location.y;
        cout << " w: " << Modules[i]->width;
        cout << " h: " << Modules[i]->height << endl;
    }
}
void Solver::outputFloorPlan()
{
    ofstream fout;
    fout.open("floorplan.txt");
    fout << chip_width << " " << chip_height << " " << Modules.size() << endl;
    for (int i = 0; i < Modules.size(); i++)
    {
        fout << Modules[i]->name << " " << Modules[i]->location.x << " " << Modules[i]->location.y << " " << Modules[i]->width << " " << Modules[i]->height << endl;
    }
    fout.close();
}