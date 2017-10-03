

PImage always,never; 
int b = 0;
color c = color(255, 204, 0);

void setup()
{
 smooth();
  size(530,530);
  frameRate(999);
  always = loadImage("always.png");
  never = loadImage("never.png");
  background(0);  
 
}
void draw()
{
  for(int i =0;i<100;i++)
  {
    b++;
    float x = random(height);
    float y = random(width);
    if(b < 93583)
    {
      c = always.get((int)x,(int)y);
    }
    else
    {
      c = never.get((int)x,(int)y);
    }
    fill(c);
    noStroke();
    ellipse(x,y,1.75,1.75);  
    if(b > 400000)
    {
      exit();
    }
  }
}