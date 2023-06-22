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
    num_softmodules = num_softmodule;
    for (int i = 0; i < num_softmodule; i++)
    {
        fin >> moduleName >> area;
        U_Name_Index[moduleName] = i;
        Module *newModule = new Module();
        newModule->index = i;
        newModule->name = moduleName;
        newModule->area = area;
        newModule->calculate_w_l(area);
        newModule->width = newModule->validWH[0].width;
        newModule->height = newModule->validWH[0].height;
        newModule->fixed = false;
        Modules.push_back(newModule);
    }
    fin >> FIXEDMODULE >> num_fixedmodule;
    num_fixedmodules = num_fixedmodule;
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
    for (size_t i = Modules.size() - 1; i >= 0 && Modules[i]->fixed; --i)
        fixedModules.push_back(Modules[i]);
    // sort(fixedModules.begin(), fixedModules.end(), [](Module *a, Module *b)
    //  { return a->manhattanDistance_fixed_orig() < b->manhattanDistance_fixed_orig(); });
    sort(fixedModules.begin(), fixedModules.end(), [](Module *a, Module *b)
         { return a->fix_location.y < b->fix_location.y; });
    vector<int> breakpoints({0});
    for (size_t i = 1, n = fixedModules.size(); i < n; ++i)
    {
        if (fixedModules[i - 1]->fix_location.y != fixedModules[i]->fix_location.y)
            breakpoints.push_back(i);
    }
    // for (auto i : fixedModules)
    //     cout << i->name << ' ' << endl;
    // cout << endl;
    // for (auto i : breakpoints)
    //     cout << i << ' ';
    // cout << endl;
    for (size_t i = 0, n = breakpoints.size() - 1; i < n; ++i)
    {
        sort(fixedModules.begin() + breakpoints[i], (i == n - 1 ? fixedModules.end() : fixedModules.begin() + breakpoints[i + 1]), [](Module *a, Module *b)
             { return a->fix_location.x < b->fix_location.x; });
    }
    for (auto i : fixedModules)
        cout << i->name << ' ' << endl;
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

// Width_Height Solver::calculate_w_l(int area)
// {
//     float ratio[11] = {1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0};
//     float testarea = 1.0;
//     for(int i = 0; i < 100; i++){
//         for(int j=0; j<11; j++){
//             float w = sqrt(testarea/ratio[j]);
//             if(float(testarea / ((int(w)+1)*(int(testarea/w)+1))) < 0.8){
//                 cout << setprecision(5) << "testarea: " << testarea << "\tratio: " << ratio[j] << "\tw: " << int(w)+1 << "\th: " << int(testarea/w)+1;
//                 cout << "\tarea: " << (int(w)+1)*(int(testarea/w)+1) << "\trratio: " << float(float(int(testarea/w)+1)/float(int(w)+1));
//                 cout << "\taratio: " << float(testarea / ((int(w)+1)*(int(testarea/w)+1))) << endl;
//             }
//         }
//         testarea = testarea + 1.0;
//         // cout << endl;
//     }
// }

void Solver::floorplan(B_Tree &t, bool &_enable, bool isFixedMode) // _enable = 1 when we find the target node(...in dfs order we only have to ploorplan those who appears later )
{
    // cout << endl;
    Contour_H.clear();
    placeBlock(t.root, 0, isFixedMode);
    IsOutofChip();
}

void Solver::IsOutofChip()
{
    OutofChip_y = false;
    OutofChip_x = false;
    int max_y = 0;
    int l = Contour_H.size();
    for (int i = 0; i < l; i++)
    {
        // cout<<Contour_H[i].til_x<<" "<<Contour_H[i].height<<endl;
        if (Contour_H[i].height > max_y)
            max_y = Contour_H[i].height;
    }
    if (max_y > chip_height)
        OutofChip_y = true;
    floorplan_y = max_y;

    if (Contour_H[l - 1].til_x > chip_width)
        OutofChip_x = true;
    floorplan_x = Contour_H[l - 1].til_x;
}

void Solver::placeBlock(Node *node, int type, bool isFixedMode) // isFixedMode = 1 when we really want to treat fixed block as pre-placed module
{
    // if (node == _target)
    //     _enable = 1;
    if (node == NULL)
        return;
    Modules[node->index]->changeWH(node->WHtype);
    // cout << "index:" << node->index  <<"\tWHtype: " << node->WHtype;
    // cout << "\tw: " << Modules[node->index]->width << "\th: " << Modules[node->index]->height << endl;
    // cout << '\n'
    //      << node->index << ' ';
    if (node->isRotated() && !Modules[node->index]->fixed) // do not rotate the fixed block
        Modules[node->index]->rotate();
    int from_x = 0, to_x = 0, Yloc = 0;
    Node *_parent = node->parent;
    // if (isFixedMode && node->parent != 0) // skip those fixed and placed module for those non-root node
    // {
    //     bool _isretParentfixednPlaced = 0;
    //     bool isLeftChild = node->getParent(Modules, _parent, _isretParentfixednPlaced);
    //     if (_parent->parent == 0 && Modules[_parent->index]->fixed_status == 3) // if _parent is root and _parent is fixed & placed
    //     {
    //         // cout << "change to type 0 !!!!\n";
    //         type = 0;
    //     }
    //     else if (_parent != node->parent)
    //     {
    //         if (isLeftChild)
    //             type = 1;
    //         else
    //             type = 2;
    //     }
    // }
    // for (auto i : Contour_H)
    //     cout << "( " << i.height << " , " << i.til_x << " )"
    //          << " ; ";
    // cout << endl;
    if (Modules[node->index]->fixed_status != 3)
    {
        if (type == 0) // if fixed block is root???
        {
            if (Modules[node->index]->fixed && isFixedMode && Modules[node->index]->fixed_status == 2)
            {
                // from_x = Modules[node->index]->fix_location.x;
                // // cout << "from_x in type 0 = " << from_x << endl;
                // to_x = from_x + Modules[node->index]->width;
                // // Yloc = findYandUpdateContour_H_fixed(node->index, from_x, to_x);
                // Yloc = Modules[node->index]->fix_location.y;
                // Modules[node->index]->fixed_status = 3;
                // Coord loc(from_x, Yloc);
                // Modules[node->index]->location = loc;
            }
            else if (isFixedMode)
            {
                Coord root_loc(0, 0);
                Modules[node->index]->location = root_loc;
                from_x = root_loc.x;
                to_x = from_x + Modules[node->index]->width;
                Yloc = findY(node->index, from_x, to_x);
                Coord *_assume = new Coord(root_loc.x, Yloc), *_current = new Coord(from_x, Yloc), *_prevCoord = 0, *_coordIfBothFail = 0;
                int _prevIndex = -1;
                bool checking = 1, _prevOP = 0, _isPrevOverlap = 0, _isFailTwice = 0; // _prevOP = 0 -> previously put on top, state = 1 -> previously put at right
                Module *beyondBlock = 0;
                // for (auto i : fixedModules)
                int collision = isOverlap_specificCoord(Modules[node->index], _assume);
                if (collision != -1)
                {
                    randomPlacement(from_x, to_x, Yloc, node->index, beyondBlock, fixedModules[collision], _assume, _current);
                    // for (auto fixedModule : fixedModules) // try to place root at every fixed module's top and right
                    // {
                    //     if (fixedModules[collisionFixedModule]->isSlim())
                    //         placeAtRight(from_x, to_x, Yloc, node->index, beyondBlock, fixedModules[collisionFixedModule], _assume);
                    //     else
                    //         placeAtTop(from_x, to_x, Yloc, node->index, beyondBlock, fixedModules[collisionFixedModule], _assume);
                    //     if (isOverlap_specificCoord(Modules[node->index], _assume) == -1) // if no overlap break
                    //         break;
                    //     if (!fixedModules[collisionFixedModule]->isSlim())
                    //         placeAtRight(from_x, to_x, Yloc, node->index, beyondBlock, fixedModules[collisionFixedModule], _assume);
                    //     else
                    //         placeAtTop(from_x, to_x, Yloc, node->index, beyondBlock, fixedModules[collisionFixedModule], _assume);
                    //     if (isOverlap_specificCoord(Modules[node->index], _assume) == -1) // if no overlap break
                    //         break;
                    // }
                }
                Yloc = UpdateContour_H(node->index, from_x, to_x, beyondBlock);
                // while (checking)
                //     for (size_t i = 0, n = fixedModules.size(); i < n; ++i)
                //     {
                //         if (Modules[node->index]->isOverlap(fixedModules[i], _assume))
                //         {
                //             if (_isFailTwice == 1)
                //             {
                //                 _isFailTwice = 0;
                //                 delete _assume;
                //                 _assume = _coordIfBothFail;
                //             }
                //             if (_isPrevOverlap == 0)
                //             {
                //                 if (fixedModules[i]->isSlim()) // if the fixed block is slim -> put at its right
                //                 {
                //                     _prevCoord = new Coord(from_x, Yloc);
                //                     from_x = fixedModules[i]->fix_location.x + fixedModules[i]->width;
                //                     to_x = from_x + Modules[node->index]->width;
                //                     Yloc = findY(node->index, from_x, to_x);
                //                     _prevOP = 1;
                //                 }
                //                 else // if the fixed block is fat -> put on its top
                //                 {
                //                     _prevCoord = new Coord(from_x, Yloc);
                //                     Yloc = fixedModules[i]->fix_location.y + fixedModules[i]->height;
                //                     beyondBlock = fixedModules[i];
                //                     _prevOP = 0;
                //                 }
                //                 _coordIfBothFail = new Coord(from_x, Yloc);
                //                 _prevIndex = i;
                //                 _isPrevOverlap = 1;
                //             }
                //             else
                //             {
                //                 if (_prevOP == 0)
                //                 {
                //                     from_x = fixedModules[_prevIndex]->fix_location.x + fixedModules[_prevIndex]->width;
                //                     to_x = from_x + Modules[node->index]->width;
                //                     Yloc = findY(node->index, from_x, to_x);
                //                 }
                //                 else
                //                 {
                //                     from_x = _prevCoord->x;
                //                     Yloc = fixedModules[_prevIndex]->fix_location.y + fixedModules[_prevIndex]->height;
                //                     beyondBlock = fixedModules[_prevIndex];
                //                 }
                //                 delete _prevCoord;
                //                 _prevCoord = 0;
                //                 _isPrevOverlap = 0;
                //                 _prevIndex = -1;
                //                 _isFailTwice = 1;
                //             }

                //             delete _assume;
                //             _assume = new Coord(from_x, Yloc);
                //             break;
                //         }
                //         else if (i == n - 1)
                //         {
                //             Yloc = UpdateContour_H(node->index, from_x, to_x, beyondBlock);
                //             checking = 0;
                //             break;
                //         }
                //         // if (Modules[node->index]->isOverlap(fixedModules[i], _assume))
                //         // {
                //         //     Yloc = UpdateContour_H(node->index, from_x, to_x, fixedModules[i]);
                //         //     break;
                //         // }
                //         // else if (i == n - 1)
                //         // {
                //         //     Yloc = UpdateContour_H(node->index, from_x, to_x);
                //         //     break;
                //         // }
                //         _isFailTwice = 0;
                //     }
            }
            else
            {
                Coord root_loc(0, 0);
                Modules[node->index]->location = root_loc;
                Yloc = findYandUpdateContour_H(node->index, 0, Modules[node->index]->width);
            }
            Coord loc(from_x, Yloc);
            Modules[node->index]->location = loc;
        }
        else if (type == 1)
        {
            // int from_x = 0, to_x = 0, Yloc = 0;
            if (Modules[node->index]->fixed && isFixedMode && Modules[node->index]->fixed_status == 2)
            {
                from_x = Modules[node->index]->fix_location.x;
                // cout << "from_x in type 1 = " << from_x << endl;
                to_x = from_x + Modules[node->index]->width;
                // Yloc = findYandUpdateContour_H_fixed(node->index, from_x, to_x);
                Yloc = Modules[node->index]->fix_location.y;
                Modules[node->index]->fixed_status = 3;
            }
            else if (isFixedMode)
            {
                from_x = Modules[_parent->index]->location.x + Modules[_parent->index]->width;
                to_x = from_x + Modules[node->index]->width;
                // Yloc = findYandUpdateContour_H(node->index, from_x, to_x);
                Yloc = findY(node->index, from_x, to_x);
                Coord *_assume = new Coord(from_x, Yloc), *_current = new Coord(from_x, Yloc);
                Module *beyondBlock = 0;
                // for (auto i : fixedModules)
                bool checking = 1;
                int collision = isOverlap_specificCoord(Modules[node->index], _assume);
                if (collision != -1)
                    randomPlacement(from_x, to_x, Yloc, node->index, beyondBlock, fixedModules[collision], _assume, _current);
                // if (collision != -1)
                // {
                //     // for (auto fixedModule : fixedModules) // try to place root at every fixed module's top and right
                //     // {
                //     if (fixedModule->isSlim())
                //         placeAtRight(from_x, to_x, Yloc, node->index, beyondBlock, fixedModule, _assume);
                //     else
                //         placeAtTop(from_x, to_x, Yloc, node->index, beyondBlock, fixedModule, _assume);
                //     if (isOverlap_specificCoord(Modules[node->index], _assume) != -1) // if overlap
                //     {
                //         from_x = _current->x;
                //         Yloc = _current->y;
                //         if (!fixedModule->isSlim())
                //             placeAtRight(from_x, to_x, Yloc, node->index, beyondBlock, fixedModule, _assume);
                //         else
                //             placeAtTop(from_x, to_x, Yloc, node->index, beyondBlock, fixedModule, _assume);
                //         if (isOverlap_specificCoord(Modules[node->index], _assume) != -1) // if no overlap break
                //         {
                //             bool r = rand() % 2; // r == 0 -> use the bottom result
                //             if (r)
                //         }

                //     } // break;
                // }
                Yloc = UpdateContour_H(node->index, from_x, to_x, beyondBlock);
                // }

                // while (checking)
                //     for (size_t i = 0, n = fixedModules.size(); i < n; ++i)
                //     {
                //         if (Modules[node->index]->isOverlap(fixedModules[i], _assume))
                //         {
                //             from_x = fixedModules[i]->fix_location.x + fixedModules[i]->width;
                //             to_x = from_x + Modules[node->index]->width;
                //             Yloc = findY(node->index, from_x, to_x);
                //             delete _assume;
                //             _assume = new Coord(from_x, Yloc);
                //             break;
                //         }
                //         else if (i == n - 1)
                //         {
                //             Yloc = UpdateContour_H(node->index, from_x, to_x);
                //             checking = 0;
                //             break;
                //         }
                //     }
            }
            else
            {
                from_x = Modules[node->parent->index]->location.x + Modules[node->parent->index]->width;
                to_x = from_x + Modules[node->index]->width;
                Yloc = findYandUpdateContour_H(node->index, from_x, to_x);
            }
            Coord loc(from_x, Yloc);
            Modules[node->index]->location = loc;
        }
        else if (type == 2)
        {
            // int from_x = 0, to_x = 0, Yloc = 0;
            if (Modules[node->index]->fixed && isFixedMode && Modules[node->index]->fixed_status == 2)
            {
                from_x = Modules[node->index]->fix_location.x;
                // cout << "from_x in type 2 = " << from_x << endl;
                to_x = from_x + Modules[node->index]->width;
                // Yloc = findYandUpdateContour_H_fixed(node->index, from_x, to_x);
                Yloc = Modules[node->index]->fix_location.y;
                Modules[node->index]->fixed_status = 3;
            }
            else if (isFixedMode)
            {
                from_x = Modules[_parent->index]->location.x;
                to_x = from_x + Modules[node->index]->width;
                Yloc = findY(node->index, from_x, to_x);
                Coord *_assume = new Coord(from_x, Yloc), *_current = new Coord(from_x, Yloc);
                bool checking = 1;
                Module *beyondBlock = 0;
                // for (auto i : fixedModules)
                int collision = isOverlap_specificCoord(Modules[node->index], _assume);
                if (collision != -1)
                    randomPlacement(from_x, to_x, Yloc, node->index, beyondBlock, fixedModules[collision], _assume, _current);
                // while (checking)
                //     for (size_t i = 0, n = fixedModules.size(); i < n; ++i)
                //     {
                //         if (Modules[node->index]->isOverlap(fixedModules[i], _assume))
                //         {
                //             Yloc = fixedModules[i]->fix_location.y + fixedModules[i]->height;
                //             delete _assume;
                //             _assume = new Coord(from_x, Yloc);
                //             beyondBlock = fixedModules[i];
                //             // Yloc = UpdateContour_H(node->index, from_x, to_x, fixedModules[i]);
                //             break;
                //         }
                //         else if (i == n - 1)
                //         {
                Yloc = UpdateContour_H(node->index, from_x, to_x, beyondBlock);
                //             checking = 0;
                //             break;
                //         }
                //     }
            }
            else
            {
                from_x = Modules[node->parent->index]->location.x;
                to_x = from_x + Modules[node->index]->width;
                Yloc = findYandUpdateContour_H(node->index, from_x, to_x);
            }
            Coord loc(from_x, Yloc);
            Modules[node->index]->location = loc;
        }
    }
    // for (auto i : Contour_H)
    //     cout << "( " << i.height << " , " << i.til_x << " ) ; ";
    // cout << endl;
    // if (Modules[node->index]->fixed)
    //     cout << "fixed Module's location = " << Modules[node->index]->location.x << ' ' << Modules[node->index]->location.y << endl;
    placeBlock(node->left, 1, isFixedMode);
    placeBlock(node->right, 2, isFixedMode);
}

int Solver::findYandUpdateContour_H_fixed(int index, int from_x, int to_x)
{
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
    // cout << "pre_max_height: " << max_height << endl;
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
    // cout << "insert_place: " << insert_place << " max_height: " << max_height << endl;
    // Contour_horizontal a(to_x, Modules[index]->height + max_height);
    Contour_horizontal a(to_x, Modules[index]->fix_location.y + Modules[index]->height);
    Contour_H.insert(Contour_H.begin() + insert_place, a);

    // for (int i = 0; i < Contour_H.size(); i++)
    // {
    //     cout << " til_x: " << Contour_H[i].til_x << " height: " << Contour_H[i].height << endl;
    // }

    return Modules[index]->fix_location.y;
}
int Solver::findY(int index, int from_x, int to_x)
{
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
    for (int i = Contour_H.size() - 1; i >= 0; i--)
    {
        if (Contour_H[i].til_x > from_x && Contour_H[i].til_x <= to_x)
        {
            if (Contour_H[i].height > max_height)
                max_height = Contour_H[i].height;
        }
    }

    return max_height;
}
// if fixedBlock != 0 -> put the Modules[index] on top of fixedBlock
int Solver::UpdateContour_H(int index, int from_x, int to_x, Module *fixedBlock)
{
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
    // cout << "pre_max_height: " << max_height << endl;
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
    // cout << "insert_place: " << insert_place << " max_height: " << max_height << endl;
    Contour_horizontal a(to_x, (fixedBlock != 0) ? (fixedBlock->fix_location.y + fixedBlock->height + Modules[index]->height) : (Modules[index]->height + max_height));
    Contour_H.insert(Contour_H.begin() + insert_place, a);

    // for (int i = 0; i < Contour_H.size(); i++)
    // {
    //     cout << " til_x: " << Contour_H[i].til_x << " height: " << Contour_H[i].height << endl;
    // }

    return fixedBlock != 0 ? fixedBlock->fix_location.y + fixedBlock->height : max_height;
}
int Solver::findYandUpdateContour_H(int index, int from_x, int to_x)
{ // not sure all correct??
    // cout << "\nindex: " << index << " from_x: " << from_x << " to_x: " << to_x << endl;
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
    // cout << "pre_max_height: " << max_height << endl;
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
    // cout << "insert_place: " << insert_place << " max_height: " << max_height << endl;
    Contour_horizontal a(to_x, Modules[index]->height + max_height);
    Contour_H.insert(Contour_H.begin() + insert_place, a);

    // for (int i = 0; i < Contour_H.size(); i++)
    // {
    //     cout << " til_x: " << Contour_H[i].til_x << " height: " << Contour_H[i].height << endl;
    // }

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

bool Solver::checkOverlap()
{
    for (auto fixModule : fixedModules)
    {
        for (size_t i = 0, n = Modules.size(); i < n; ++i)
        {
            if (Modules[i]->fixed)
                continue;
            if (Modules[i]->isOverlap(fixModule))
            {
                cout << i << " & " << fixModule->index << " overlap!!!" << endl;
                return 0;
            }
        }
    }
    return 1;
}

void Solver::calculate_area_wirelength()
{
    A = 0;
    HPWL = 0;
    int prev_til_x = 0;

    int l = Contour_H.size();
    for (int i = 0; i < l; i++)
    {
        // cout<<Contour_H[i].til_x<<" "<<Contour_H[i].height<<endl;
        A += (Contour_H[i].til_x - prev_til_x) * Contour_H[i].height;
        prev_til_x = Contour_H[i].til_x;
    }
    // cout<<"A: "<<A<<endl;

    l = Connections.size();
    for (int i = 0; i < l; i++)
    {
        HPWL += (abs((Modules[Connections[i].index_name1]->location.x + double(Modules[Connections[i].index_name1]->width) / 2) - (Modules[Connections[i].index_name2]->location.x + double(Modules[Connections[i].index_name2]->width) / 2)) + abs((Modules[Connections[i].index_name1]->location.y + double(Modules[Connections[i].index_name1]->height) / 2) - (Modules[Connections[i].index_name2]->location.y + double(Modules[Connections[i].index_name2]->height) / 2))) * Connections[i].pin_Number;
    }
    // cout<<"HPWL: "<<HPWL<<endl;

    area_penalty = 0.0;
    if (floorplan_x > chip_width || floorplan_y > chip_height)
    {
        if (floorplan_x > chip_width && floorplan_y > chip_height)
        {
            area_penalty += (floorplan_x * floorplan_y - chip_width * chip_height);
        }
        else if (floorplan_x > chip_width)
        {
            area_penalty += ((floorplan_x - chip_width) * floorplan_y);
        }
        else if (floorplan_y > chip_height)
        {
            area_penalty += (floorplan_x * (floorplan_y - chip_height));
        }
        // area_penalty += ((floorplan_x - chip_width) * (floorplan_x - chip_width) \
        //             + (floorplan_y - chip_height) * (floorplan_y - chip_height));
        // area_penalty =  (area_penalty-A_min)/(A_max - A_min);
    }

    l = Modules.size();
    for (int i = 0; i < l; ++i)
    {
        int right_x = Modules[i]->location.x + Modules[i]->width;
        int top_y = Modules[i]->location.y + Modules[i]->height;
        if (right_x > chip_width && top_y <= chip_height)
            area_penalty += ((right_x - chip_width) * (right_x - chip_width));
        else if (right_x <= chip_width && top_y > chip_height)
            area_penalty += ((top_y - chip_height) * (top_y - chip_height));
        else if (right_x > chip_width && top_y > chip_height)
            area_penalty += ((right_x - chip_width) * (right_x - chip_width) + (top_y - chip_height) * (top_y - chip_height));
    }
}

float Solver::calculate_totalcost(float alpha, float beta)
{
    A = 0;
    HPWL = 0;
    int prev_til_x = 0;

    int l = Contour_H.size();
    for (int i = 0; i < l; i++)
    {
        // cout<<Contour_H[i].til_x<<" "<<Contour_H[i].height<<endl;
        A += (Contour_H[i].til_x - prev_til_x) * Contour_H[i].height;
        prev_til_x = Contour_H[i].til_x;
    }
    // A_norm = (A - A_min) / (A_max - A_min);
    // cout<<"A: "<<A<<endl;

    l = Connections.size();
    for (int i = 0; i < l; i++)
    {
        HPWL += (abs((Modules[Connections[i].index_name1]->location.x + double(Modules[Connections[i].index_name1]->width) / 2) - (Modules[Connections[i].index_name2]->location.x + double(Modules[Connections[i].index_name2]->width) / 2)) + abs((Modules[Connections[i].index_name1]->location.y + double(Modules[Connections[i].index_name1]->height) / 2) - (Modules[Connections[i].index_name2]->location.y + double(Modules[Connections[i].index_name2]->height) / 2))) * Connections[i].pin_Number;
    }
    // float HPWL_norm=(HPWL-HPWL_min)/(HPWL_max-HPWL_min);
    //  cout<<"HPWL: "<<HPWL<<endl;

    area_penalty = 0.0;
    if (floorplan_x > chip_width || floorplan_y > chip_height)
    {
        if (floorplan_x > chip_width && floorplan_y > chip_height)
        {
            area_penalty += (floorplan_x * floorplan_y - chip_width * chip_height);
        }
        else if (floorplan_x > chip_width)
        {
            area_penalty += ((floorplan_x - chip_width) * floorplan_y);
        }
        else if (floorplan_y > chip_height)
        {
            area_penalty += (floorplan_x * (floorplan_y - chip_height));
        }
        // area_penalty += ((floorplan_x - chip_width) * (floorplan_x - chip_width) \
        //             + (floorplan_y - chip_height) * (floorplan_y - chip_height));
        // area_penalty =  (area_penalty-A_min)/(A_max - A_min);
    }

    l = Modules.size();
    for (int i = 0; i < l; ++i)
    {
        int right_x = Modules[i]->location.x + Modules[i]->width;
        int top_y = Modules[i]->location.y + Modules[i]->height;
        if (right_x > chip_width && top_y <= chip_height)
            area_penalty += ((right_x - chip_width) * (right_x - chip_width));
        else if (right_x <= chip_width && top_y > chip_height)
            area_penalty += ((top_y - chip_height) * (top_y - chip_height));
        else if (right_x > chip_width && top_y > chip_height)
            area_penalty += ((right_x - chip_width) * (right_x - chip_width) + (top_y - chip_height) * (top_y - chip_height));
    }

    // cout<<0.3 * A<<" "<<0.7 * HPWL<<" "<<2*(Contour_H[Contour_H.size()-1].til_x-chip_width)<<" "<<2*(highest-chip_height)<<endl;
    // return alpha * A_norm + beta * HPWL_norm + (1-alpha-beta) * area_penalty; //////////////////////////////////////////////
    // return 0.2 * A_norm + 0.5 * HPWL_norm + 0.3 * area_penalty;
    // return 0.2 * A + 0.5 * HPWL + 0.3 * area_penalty;
    // return 0.5 * HPWL + 0.5 * area_penalty;
    // return beta * HPWL + (1-beta) * area_penalty;

    // cout<<"HPWL: "<<HPWL<<" HPWL_norm: "<<HPWL_norm<<endl;
    // cout<<"area: "<<area_penalty<<" areaL_norm: "<<area_penalty_norm<<endl;
    // cout<<endl;

    float ans = alpha * A / A_norm + beta * HPWL / HPWL_norm + (1 - alpha - beta) * area_penalty / area_penalty_norm;
    // cout << "A: " << A << " An: " << A_norm << endl;
    // cout << "A/An: " << A/A_norm << " H/Hn: " << HPWL/HPWL_norm << " a/an: " << area_penalty/area_penalty_norm << endl;
    // cout << "Cost: " << ans << endl;
    return ans;
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
void Solver::outputFloorPlan(int isPrePlaced)
{
    ofstream fout;
    // switch (isPrePlaced)
    // {
    // case 0:
    //     fout.open("floorplan_before.txt");
    // case 1:
    //     fout.open("floorplan.txt");
    // case 2:
    //     fout.open("floorplan_1.txt");
    // case 3:
    //     fout.open("floorplan_2.txt");
    // case 4:
    //     fout.open("floorplan_3.txt");
    // case 5:
    //     fout.open("floorplan_4.txt");
    // }
    if (isPrePlaced)
        fout.open("floorplan.txt");
    else
        fout.open("floorplan_before.txt");
    // cout << "outputFloorPlanning..." << endl;
    fout << chip_width << " " << chip_height << " " << Modules.size() << endl;
    for (int i = 0; i < Modules.size(); i++)
    {
        fout << Modules[i]->name << " " << Modules[i]->location.x << " " << Modules[i]->location.y << " " << Modules[i]->width << " " << Modules[i]->height << endl;
    }
    fout.close();
}
void Solver::outputFloorPlanRect(bool kk)
{
    ofstream fout;
    if (kk)
        fout.open("floorplan.txt");
    else
        fout.open("floorplan_before.txt");
    fout << chip_width << " " << chip_height << endl;
    fout << "HPWL " << HPWL << endl;
    fout << "SOFTMODULE " << num_softmodules << endl;
    for (int i = 0; i < num_softmodules; i++)
    {
        if (Modules[i]->area == Modules[i]->width * Modules[i]->height)
        {
            fout << Modules[i]->name << " " << 4 << endl;
            fout << Modules[i]->location.x << " " << Modules[i]->location.y << endl;
            fout << Modules[i]->location.x << " " << Modules[i]->location.y + Modules[i]->height << endl;
            fout << Modules[i]->location.x + Modules[i]->width << " " << Modules[i]->location.y + Modules[i]->height << endl;
            fout << Modules[i]->location.x + Modules[i]->width << " " << Modules[i]->location.y << endl;
        }
        else
        {
            fout << Modules[i]->name << " " << 6 << endl;
            fout << Modules[i]->location.x << " " << Modules[i]->location.y << endl;
            fout << Modules[i]->location.x << " " << Modules[i]->location.y + Modules[i]->height << endl;
            fout << Modules[i]->location.x + Modules[i]->width << " " << Modules[i]->location.y + Modules[i]->height << endl;
            fout << Modules[i]->location.x + Modules[i]->width << " " << Modules[i]->location.y + (Modules[i]->height * Modules[i]->width - Modules[i]->area) << endl;
            fout << Modules[i]->location.x + Modules[i]->width - 1 << " " << Modules[i]->location.y + (Modules[i]->height * Modules[i]->width - Modules[i]->area) << endl;
            fout << Modules[i]->location.x + Modules[i]->width - 1 << " " << Modules[i]->location.y << endl;
        }
    }
    fout << "FIXEDMODULE " << num_fixedmodules << endl;
    for (int i = num_softmodules; i < Modules.size(); i++)
    {
        fout << Modules[i]->name << " " << Modules[i]->location.x << " " << Modules[i]->location.y << " " << Modules[i]->width << " " << Modules[i]->height << endl;
    }
    fout.close();
}