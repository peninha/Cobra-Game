from PIL import Image

def rgb2hex(r, g, b):
    return '#{:02x}{:02x}{:02x}'.format(r, g, b)

filename = "map_02"


im = Image.open("images/"+filename+".png")
file = open("maps/"+filename+".txt", 'w')
pixel = im.load()
N, M = im.size
baseline = 0 #how many display line are below the bottom of the image
topline = 10 #how many display lines are above the top of the image

for j in range(M):
    for i in range(N):
        r, g, b = pixel[i, j]
        if sum((r, g, b))!=0:
            file.write("map["+str(i)+"]["+str(M-j-1)+"] = true;")
    file.write("\n")
file.close()