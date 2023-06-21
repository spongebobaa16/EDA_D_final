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


def main():
    # read file
    _pre = input("1 for before, 0 for after : ")
    # _pre = '0'
    _src = "floorplan.txt" if _pre == '0' else "floorplan_before.txt"
    isFirstLine = True
    positions = []
    bounding_box = []
    with open(_src, 'r') as f:
        for line in f:
            tokens = line.split()
            if isFirstLine:
                bounding_box = [float(i) for i in tokens]
            else:
                # name = tokens[0][0] + str(int(tokens[0][1:]) - 1)
                name = tokens[0]
                positions.append(
                    {"name": name, "x": float(tokens[1]), "y": float(tokens[2]), "width": float(tokens[3]), "height": float(tokens[4])})
            isFirstLine = False
    visualize(positions, bounding_box, "floorplan.png" if _pre == '0' else "floorplan_before.png")
    # print(bounding_box)
    # print(positions[0]["name"])


if __name__ == "__main__":
    main()