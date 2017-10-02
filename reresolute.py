from PIL import Image
size = 512,512
for i in range(1,141):
    im = Image.open(str(i)+".jpg")
    im_resized = im.resize(size, Image.ANTIALIAS)
    im_resized.save("re"+str(i)+".jpg", "JPEG")
