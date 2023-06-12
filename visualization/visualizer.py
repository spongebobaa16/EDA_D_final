# from typing import Tuple

import matplotlib.patches as patches
from matplotlib import pylab as plt


def visualize(positions, bounding_box, path="floorplan.png", title="Floorplan"):
    plt.rcParams["font.size"] = 14

    # Figure settings
    bb_width = bounding_box[0]
    bb_height = bounding_box[1]
    fig = plt.figure(figsize=(10, 10 * bb_height / bb_width + 0.5))
    ax = plt.axes()
    ax.set_aspect("equal")
    plt.xlim([0, bb_width])
    plt.ylim([0, bb_height])
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.title(title)

    # Plot every rectangle
    for i, rectlinear in enumerate(positions):
        color, fontcolor = get_color(i)
        if(not rectlinear["isFixed"]):
            name_pos = int(len(rectlinear["info"]) / 2)
            cnt = 1
            for rectangle in rectlinear["info"]:
                r = patches.Rectangle(
                    xy=(rectangle["x"], rectangle["y"]),
                    width=rectangle["width"],
                    height=rectangle["height"],
                    edgecolor="#000000",
                    facecolor=color,
                    alpha=1.0,
                    fill=True,
                    linewidth = 0,
                )
                ax.add_patch(r)

                # Add text label
                centering_offset = 0.011
                center_x = rectangle["x"] + rectangle["width"] / \
                    2 - bb_width * centering_offset
                center_y = rectangle["y"] + rectangle["height"] / \
                    2 - bb_height * centering_offset
                ax.text(x=center_x, y=center_y,
                        s=rectlinear["name"] if cnt == name_pos else "", fontsize=18, color=fontcolor)
                cnt += 1
        else:
            rectangle = rectlinear
            r = patches.Rectangle(
                    xy=(rectangle["x"], rectangle["y"]),
                    width=rectangle["width"],
                    height=rectangle["height"],
                    edgecolor="#000000",
                    facecolor=color,
                    alpha=1.0,
                    fill=True,
                    linewidth = 0,
                )
            ax.add_patch(r)

            # Add text label
            centering_offset = 0.011
            center_x = rectangle["x"] + rectangle["width"] / \
                2 - bb_width * centering_offset
            center_y = rectangle["y"] + rectangle["height"] / \
                2 - bb_height * centering_offset
            ax.text(x=center_x, y=center_y,
                    s= rectangle["name"], fontsize=18, color=fontcolor)
    # Output
    if path is None:
        plt.show()
    else:
        fig.savefig(path)

    plt.close()


def get_color(i=0):
    """
    Gets rectangle face color (and its font color) from matplotlib cmap.
    """
    cmap = plt.get_cmap("tab10")
    color = cmap(i % cmap.N)
    brightness = max(color[0], color[1], color[2])

    if 0.85 < brightness:
        fontcolor = "#000000"
    else:
        fontcolor = "#ffffff"

    return (color, fontcolor)

def addVertex(verticalLine, module):
    tmp = module[1]
    module = module[2:]
    module.append(tmp)
    # print((module))
    module_new = []
    for i in range(0 ,len(module), 2):
        module_new.append(module[i])
        for j in verticalLine:
            if(j > module[i][0] and j < module[i + 1][0]) or (j < module[i][0] and j > module[i + 1][0]):
                module_new.append([j, module[i][1]])    # seperate into two vertex so that we can pair it directly
                module_new.append([j, module[i][1]])
        module_new.append(module[i + 1])
    return module_new


def getPosition(module_new, verticalLine, name):
    rect_num = len(verticalLine) - 1
    current_rect = 1
    info = []
    position = []
    pairs = []  # pair(x coordinate, y coordinate, which rect)
    for i in range(0, int(len(module_new) / 2), 2):  # pairwise sort each horizontal line into different sliced rectangle(1,2,3,..., len(verticalines))
        pairs.append([ module_new[i][1], current_rect])
        current_rect += 1
    current_rect = rect_num
    for i in range(int(len(module_new) / 2), len(module_new), 2):  # pairwise sort each horizontal line into different sliced rectangle(1,2,3,..., len(verticalines))
        pairs.append([ module_new[i][1], current_rect])
        current_rect -= 1
    # print(verticalLine)
    # print(pairs)
    pairs = sorted(pairs, key = lambda i : i[1])
    # print(pairs)
    for i in range(rect_num):   # x coordinate : the one identical with each verticalLine
        info.append({"x":verticalLine[i], "y": min(pairs[i*2][0], pairs[i*2 + 1][0]), "width": verticalLine[i+1] - verticalLine[i], "height":abs(pairs[i*2][0] - pairs[i*2 + 1][0])})
    return {"isFixed" : False,"name": name,"info" : info}
        # {"name": name, "x": float(tokens[1]), "y": float(tokens[2]), "width": float(tokens[3]), "height": float(tokens[4])})
        
    # print(verticalLine)
    
    
def main(): 
    # read file
    #_src = input("Enter the file name you want to visualize : ")
    _src = "floorplan.txt"

    modules_corner = [] # first element of each module : name of the module; the rest of the elements : i-th corner
    positions = []
    bounding_box = []
    with open(_src, 'r') as f:
        # read chip width & height
        tokens = f.readline().split()
        bounding_box = [float(i) for i in tokens]
        positions = []
        # discard the second line
        f.readline()
        
        # read SOFTBLOCK
        tokens = f.readline().split()   # tokens[1] denotes the number of soft blocks
        for i in range(int(tokens[1])):
            tokens_i = f.readline().split() # tokens_i[1] denotes the number of corners in module i
            corners = [tokens_i[0]]
            for j in range(int(tokens_i[1])):
                tokens_i = f.readline().split()
                corners.append([float(tokens_i[0]), float(tokens_i[1])])
            modules_corner.append(corners)  
            
        # slice the module into a set of rectangle
        for module in modules_corner:
            module_sorted = [module[0]]
            module_sorted = sorted(module[1:], key = lambda i : i[0])
            prev_x = module_sorted[1][0]
            verticalLines = [1] # record the break point of each veritcal line
            for i in range(2, len(module_sorted[1:]) + 1):
                if module_sorted[i][0] != prev_x :
                    prev_x = module_sorted[i][0]
                    verticalLines.append(i)
            verticalLine = []   # x coordinate of slicing line
            for line in verticalLines:
                verticalLine.append(module_sorted[line][0])     # slice out len(verticalLine) - 1 rectangle(1, 2, 3, ..., len(verticalLine) - 1 )
            module_new = addVertex(verticalLine, module)
            # print(module)
            positions.append(getPosition(module_new, verticalLine, module[0]))
            
    # read FIXEDBLOCK
        tokens = f.readline().split()   # tokens[1] denotes the number of fixed blocks
        for i in range(int(tokens[1])):
            tokens_i = f.readline().split()
            positions.append(
                {"isFixed" : True,"name": tokens_i[0], "x": float(tokens_i[1]), "y": float(tokens_i[2]), "width": float(tokens_i[3]), "height": float(tokens_i[4])})

    
    visualize(positions, bounding_box)
    # print(bounding_box)
    # print(positions[0]["name"])


if __name__ == "__main__":
    main()
