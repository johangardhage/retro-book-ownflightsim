#ifndef SPRITE_H

#define SPRITE_H
// Declaration of sprite class:

class Sprite
{
	private:
    char far **image;       // Pointer to sprite bitmaps
	int nsprit;                // number of sprite pointers
    int width,height;       // Width and height of sprite
                            //  bitmap
    int xsprite,ysprite;    // Current x,y coordinates of
                            //  sprite
    char far *savebuffer;   // Pointer to array for saving
                            //  sprite background
  public:
    //Constructor for sprite class:
    Sprite(int num_sprites,int width,int height);
	// Destructor:
	~Sprite();
    // Sprite grabber function:
    void grab(unsigned char far *buffer,int sprite_num,
               int x1,int y1);
    // Function to put sprite on display:
    void put(int sprite_num,int x,int y,
               unsigned char far *screen);
    // Function to erase sprite image:
    void erase(unsigned char far *screen);
};


#endif