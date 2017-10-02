PImage[] images;
color[] colours;
void setup()
{
  size(512,512);
  images = new PImage[140];
  colours = new color[140];
  int ind = 0;
  for(int k=0;k<140;k++)
  {
    images[ind] = loadImage("re"+(k+1)+".jpg");
    ind++;
  }
  noStroke();
  
  for(int i = 0;i<512;i+=1)
  {
    for(int j = 0;j<512;j+=1)
    {
      for(int l = 0;l<140;l++)
      {
        colours[l] = images[l].get(i,j);
      }
      fill(getAverage());
      rect(i,j,1,1);
      
    }
  }
}

color getAverage()
{
  float r=0,g=0,b=0;
  for(int i =0;i<140;i++)
  {
    r+= pow(red(colours[i]),2);
    g+= pow(green(colours[i]),2);
    b+= pow(blue(colours[i]),2);
  }
  color temp = color(sqrt(r/140),sqrt(g/140),sqrt(b/140));
  return temp;
  
}

void draw()
{
 
}