

PImage always,never; 
int b = 0;
color c = color(255, 204, 0);

void setup()
{
 smooth();
  size(500,500);
  frameRate(999);
  always = loadImage("always.png"); //hello darkness
  never = loadImage("never.png");
  background(0);  
 
}
void draw()
{
  for(int i =0;i<30;i++)
  {
    b++;
    float x = random(height);
    float y = random(width);
    if(b < 85000)
    {
      c = always.get((int)x,(int)y);
    }
    else
    {
      c = never.get((int)x,(int)y);
    }
    
    fill(c);
    noStroke();
    text("o",x,y);
    //ellipse(x,y,1.75,1.75);  
    if(b > 400000)
    {
      exit();
    }
  }
}