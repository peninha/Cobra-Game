from PIL import Image

def rgb2hex(r, g, b):
    return '#{:02x}{:02x}{:02x}'.format(r, g, b)

filename = "cobra_01"


im = Image.open("images/"+filename+".png")
file = open("txts/"+filename+".txt", 'w')
pixel = im.load()
N, M = im.size
baseline = 20 #how many display line are below the bottom of the image
topline = 0 #how many display lines are above the top of the image
renderBlack = True #output black pixels?

for j in range(M):
    for i in range(N):
        r, g, b = pixel[i, j]
        string = rgb2hex(r, g, b)
        x = i
        y = M-j-1
        if renderBlack or sum((r, g, b))!=0:
            led = x*M + y + (x%2)*(M-1-2*y) + baseline*(2*int(i/2)+1) + topline*(2*int((i+1)/2))
            file.write(string.replace("#", "leds["+str(led)+"] = 0x")+";")
    file.write("\n")
file.close()