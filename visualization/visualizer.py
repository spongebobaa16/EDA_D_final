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
    for i, rectangle in enumerate(positions):
        color, fontcolor = get_color(i)
        r = patches.Rectangle(
            xy=(rectangle["x"], rectangle["y"]),
            width=rectangle["width"],
            height=rectangle["height"],
            edgecolor="#000000",
            facecolor=color,
            alpha=1.0,
            fill=True,
        )
        ax.add_patch(r)

        # Add text label
        centering_offset = 0.011
        center_x = rectangle["x"] + rectangle["width"] / \
            2 - bb_width * centering_offset
        center_y = rectangle["y"] + rectangle["height"] / \
            2 - bb_height * centering_offset
        ax.text(x=center_x, y=center_y,
                s=rectangle["name"], fontsize=18, color=fontcolor)

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

def get_upper_contour(corners):
    # skip first corner since it's the leftmost & lowest corner
    # tuple (left x coordinate, right y coordinate, absolute height)
    min = corners[1][2] # lowest of the top contour -> i
    for i in range(1, len(corners), 2):
        print(1)
        

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
        # discard the second line
        f.readline()
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
            module[1:] = sorted(module[1:], key = lambda i : i[0])
            prev_x = module[1][0]
            veriticalLine = [1] # record the break point of each veritcal line
            for i in range(2, len(module[1:]) + 1):
                if module[i][0] != prev_x :
                    prev_x = module[i][0]
                    veriticalLine.append(i)
                # for j in veriticalLine[1:]:
                    
            print(veriticalLine)
            print(module)
    #     for line in f:
    #         tokens = line.split()
    #         if isFirstLine:
    #             bounding_box = [float(i) for i in tokens]
    #         else:
    #             positions.append(
    #                 {"name": tokens[0], "x": float(tokens[1]), "y": float(tokens[2]), "width": float(tokens[3]), "height": float(tokens[4])})
    #         isFirstLine = False
    # visualize(positions, bounding_box)
    # print(bounding_box)
    # print(positions[0]["name"])


if __name__ == "__main__":
    main()
