/* 

 Requires the Adafruit GFX and ST7735 Libraries. If using NeoPixels then the Adafruit_NeoPixel library is required as well
 
*/
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <SD.h>

#define TFT_CS  10          // Chip select line for TFT display
#define TFT_DC   8          // Data/command line for TFT
#define TFT_RST  9          // Reset line for TFT (or connect to +5V)
#define SD_CS    4          // Chip select line for SD card

/* For those of you interested in creating a Menu system, we have defined
   two arrays here. One holds the Menu Title and menu headings, and the other
   holds the associated function to be called. This is a great way to simplify
   the configuration of a menu especially when multiple menu's are rquired
*/   
char* menu[] = {"614Con 2018",
                "Readme     ",
                "Bling       ",
                "Viz      "
                }; 

char* bling[] = {"614Con 2018 ",
                 "Retro ",
                 "Spectrum ",
                 "Police",
                 "Random",
                 "Staff",
                 "<---- "};  

//Calculate the number of items in main menu and bling menu
#define numMenu (sizeof(menu)/sizeof(char *))-1        
#define numBling (sizeof(bling)/sizeof(char *))-1      

#define menu_top 14   // Postition of first menu item from top of screen
char menu_select;     // Currently selected menu item
char keydown=0;       //Marker for pressed key
char sd_card=0;       // SD Card inserted?


//Main handle for tft screen object
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

//Main handle for NeoPixel object
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(2, A5, NEO_GRB + NEO_KHZ800);


/*
 * 
 * MENU INITIALIZATIONS
 * 
 */

//Initialize and draw the main menu
void tftMenuInit()
{
  char i;
  
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(1);
  
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_GREEN, ST7735_BLACK); 
  tft.println(menu[0]);

  tft.drawLine(0, 9, tft.width()-1, 9, ST7735_GREEN);
  
  tft.setTextColor(ST7735_CYAN, ST7735_BLACK);  
  for(i=1;i<=numMenu;i++)
  {
     tft.setCursor(0, ((i-1)*10)+menu_top);    
     tft.println(menu[i]);
  }  
}

//Initialize and draw the bling menu 
void tftBlingInit()
{
  char i;
  
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(1);
  
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_GREEN, ST7735_BLACK); 
  tft.println(bling[0]);

  tft.drawLine(0, 9, tft.width()-1, 9, ST7735_GREEN);
  
  tft.setTextColor(ST7735_CYAN, ST7735_BLACK);  
  for(i=1;i<=numBling;i++)
  {
     tft.setCursor(0, ((i-1)*10)+menu_top);    
     tft.println(bling[i]);
  }
}  


/*
 * 
 * MENU NAVIGATION
 * 
 */

void tftMenuSelect(char menuitem) 
{
  // Highlight a selected menu item
  char i;
  // Remove highlight of current item
  tft.setCursor(0, ((menu_select-1)*10)+menu_top);
  tft.setTextColor(ST7735_CYAN, ST7735_BLACK); 
  tft.println(menu[menu_select]); 
  // Highlight new menu item
  tft.setCursor(0, ((menuitem-1)*10)+menu_top);
  tft.setTextColor(ST7735_CYAN, ST7735_MAGENTA); 
  tft.println(menu[menuitem]);
  // change menu_select to new item  
  menu_select=menuitem;

}

void tftBlingMenuSelect(char menuitem) 
{
  // Highlight a selected menu item
  char i;
  // Remove highlight of current item
  tft.setCursor(0, ((menu_select-1)*10)+menu_top);
  tft.setTextColor(ST7735_CYAN, ST7735_BLACK); 
  tft.println(bling[menu_select]); 
  // Highlight new menu item
  tft.setCursor(0, ((menuitem-1)*10)+menu_top);
  tft.setTextColor(ST7735_CYAN, ST7735_MAGENTA); 
  tft.println(bling[menuitem]);
  // change menu_select to new item  
  menu_select=menuitem;

}


/*
 * 
 * MENU AND UTILIITY FUNCTIONS
 * 
 */
void tftReadme()
{
  int button_select = 1;
  // Print some small text
  char * message=" Welcome to 614Con\n and congratulations\n on assembling your\n badge.\n\n Consider this an\n open platform to \n experiment and \n build with.\n\n If you are competing\n in the ctf, have a \n flag for assembling \n me.\n\n flag=b4dg3_71f3"; 
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_CYAN);
  tft.setTextWrap(true);
  tft.println(message); 
  // Wait for action button press before return
  while (button_select != 0){
      button_select = digitalRead(7);
      Serial.println(button_select);
  }
  return;
}  

void tftBarGraphTest() 
{
  // Print Bar Graph
  int16_t x;
  unsigned char origin_x=10;  
  unsigned char origin_y=115;
  unsigned char width=15;
  unsigned char spacing=20;
  unsigned char height=10;
  tft.fillScreen(ST7735_BLACK);
  
  tft.drawLine(origin_x, origin_y, origin_x, 1, ST7735_BLUE);
  tft.drawLine(origin_x, origin_y, tft.width(), origin_y, ST7735_BLUE);  

  for (int16_t x=origin_x+1; x <tft.width()-spacing; x+=spacing) 
  {
    tft.fillRect(x, origin_y-height , width, height, ST7735_GREEN);
    height+=10;
  }


  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.setCursor(0, origin_y);
  tft.print("0");
  tft.setCursor(0, 1);
  tft.print("10");  
  for (char i=0; i <7; i++) 
  {
    x=origin_x+8+(spacing*i);
    tft.setCursor(x, origin_y+3);
    tft.print(i+1);     
  } 
  tft.setCursor(40, 20);
  tft.print("Bar Graph");   
  
  delay(2000);
}
//
//void tftBitmapTest()
//{
//  File dir=SD.open("/");
//  dir.rewindDirectory();
//  
//  while(dir) {
//     File entry =  dir.openNextFile();
//     if (! entry) {
//       // no more files
//       Serial.println("**nomorefiles**");
//       break;
//     }
//     // Print the 8.3 name
//     Serial.print(entry.name());
//     // Recurse for directories, otherwise print the file size
//     if (entry.isDirectory()) {
//       Serial.println("/");
//     } 
//   else{
//       // files have sizes, directories do not
//       Serial.print("\t\t");
//       Serial.println(entry.size(), DEC);
//     if(bmpDraw(entry.name(),0,0))delay(5000);
//     }
//     entry.close();
//   }
//}

// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.


#define BUFFPIXEL 20

boolean bmpDraw(char *filename, uint8_t x, uint8_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if((x >= tft.width()) || (y >= tft.height())) return false;

  Serial.println();
  Serial.print("Loading image '");
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print("File not found");
    return false;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print("File size: "); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print("Image Offset: "); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print("Header size: "); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print("Bit Depth: "); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print("Image size: ");
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.Color565(r,g,b));
          } // end pixel
        } // end scanline
        Serial.print("Loaded in ");
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  
  if(!goodBmp)Serial.println("BMP format not recognized.");
  
  return goodBmp;
}


// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}


/*
 * 
 * BLING MODE FUNCTIONS
 * 
 */

void blingRetro(){
 bmpDraw("retro.bmp",0,0);
 while(true){
  int button_select = digitalRead(7);
  Serial.println(button_select);
  
  if ((button_select == 0)) {
      return;
  }
  else{
     pixels.setPixelColor(0, pixels.Color(250,0,255));
     pixels.setPixelColor(1, pixels.Color(250,0,255));
     pixels.show();
     
     delay(300);
     
     pixels.setPixelColor(0, pixels.Color(0,255,255));
     pixels.setPixelColor(1, pixels.Color(0,255,255));
     pixels.show();

     delay(50);

     Serial.print("2nd Color");
  }
}
}

void blingPolice(){
bmpDraw("614Con2.bmp",0,0);
while(true){
  int button_select = digitalRead(7);
  Serial.println(button_select);
  
  if ((button_select == 0)) {
      return;
  }
  else{
     pixels.setPixelColor(0, pixels.Color(0,0,255));
     pixels.setPixelColor(1, pixels.Color(255,0,0));
     pixels.show();
     
     delay(100);
     
     pixels.setPixelColor(0, pixels.Color(255,0,0));
     pixels.setPixelColor(1, pixels.Color(0,0,255));
     pixels.show();

     delay(100);

     Serial.print("2nd Color");
  }
}
}

void blingSpectrum(){
bmpDraw("614Con1.bmp",0,0);
while(true){
  int button_select = digitalRead(7);
  Serial.println(button_select);
  
  if ((button_select == 0)) {
      return;
  }
  else{
    uint16_t i, j;

    for(j=0; j<256; j++) {
     for(i=0; i<pixels.numPixels(); i++) {
       pixels.setPixelColor(i, Wheel((i*1+j) & 255));
     }
    pixels.show();
    delay(100);
  }
}
}
}

void blingRandom(){
bmpDraw("614Con2.bmp",0,0);
while(true){
  int button_select = digitalRead(7);
  Serial.println(button_select);
  
  if ((button_select == 0)) {
      return;
  }
  else{
    long i = random(0,2);
    long r = random(0,255);
    long g = random(0,255);
    long b = random(0,255);
    
    pixels.setPixelColor(i,pixels.Color(r,g,b));
  }
    pixels.show();
    delay(100);
  }
}

void blingStaff(){
bmpDraw("staff.bmp",0,0);
while(true){
  int button_select = digitalRead(7);
  Serial.println(button_select);
  
  if ((button_select == 0)) {
      return;
  }
  else{
    for (int i; i < 225 ; i++) {
      for(int j=0; j<pixels.numPixels(); j++) {
       pixels.setPixelColor(j, pixels.Color(i+15,0,0)); // yellow with a little extra red to make it warmer 
      }
      pixels.show();
      delay(10);
    }
    for (int i = 225; i > 0; i--) {
      for(int j=0; j<pixels.numPixels(); j++) {
        pixels.setPixelColor(j, pixels.Color(i+15,0,0));
      }
      pixels.show();
      delay(10);
    }
  }
    pixels.show();
    delay(100);
  }
}



/*
 * 
 *  BLING MODE ENTRY LOGIC 
 * 
 */
 
typedef void (* MenuFuncPtr) (); // this is a typedef to the menu functions
MenuFuncPtr bling_func[] = {0,
                blingRetro,
                blingSpectrum,
                blingPolice,
                blingRandom,
                blingStaff
};  


void tftBlingMode(){


  tftBlingInit();                    // Draw menu
  menu_select=1;                    // Select 1st menu item 
  tftBlingMenuSelect(menu_select);  // Highlight selected menu item

while(true){
  int button_left = digitalRead(2);
  int button_down = digitalRead(3);
  int button_right = digitalRead(5);
  int button_up = digitalRead(6);
  int button_select = digitalRead(7);

  pixels.setPixelColor(0, pixels.Color(255,0,255)); 
  pixels.setPixelColor(1, pixels.Color(255,0,255));
  pixels.show();
  
  if ((button_select == 0) & (keydown==0)) {
    // Select
    keydown=1;
    if (menu_select == numBling){
      menu_select = 1;
      tftMenuSelect(menu_select);
      return;
    }
    else{
      bling_func[menu_select]();       // Call the appropriate menu function from array
    }
    // Note the syntax for doing this
    //delay(2000);
    tftBlingInit();                  // Redraw the Menu
    tftBlingMenuSelect(menu_select);     // Highlight the current menu item
  }
  else if ((button_up == 0) & (keydown==0)) {
    // Up
    // move up one menu item, if at top wrap to bottom
    keydown=1;    
    if (menu_select>1) tftBlingMenuSelect(menu_select-1);
    else tftBlingMenuSelect(numBling);
  }
  else if ((button_left == 0) & (keydown==0)){
    // Left
    keydown=1;    
  }
  else if ((button_down == 0) & (keydown==0)){
    // Down
    // move down one menu item, if at bottom wrap to top
    keydown=1;    
    if (menu_select<numBling) tftBlingMenuSelect(menu_select+1);
    else tftBlingMenuSelect(1);   
  }
  else if ((button_right == 0) & (keydown==0)){
    // Right
    keydown=1;    
  }  
  else if ((button_left == 1) & (button_down == 1) & (button_right == 1) & (button_up == 1))
  {
    keydown=0; // key released
  }

  pixels.setPixelColor(0, pixels.Color(0,255,255));
  pixels.setPixelColor(1, pixels.Color(0,255,255));
  pixels.show(); 

}

}



/*               
 *                
 * ARDUINO ENTRY POINT LOGIC
 *                
 */
 
typedef void (* MenuFuncPtr) (); // this is a typedef to the menu functions
MenuFuncPtr menu_func[] = {0,
                tftReadme,
                tftBlingMode,
                vizSetup,
                };
 
void setup(void) {

  // Set TFT and SD Chip Select pins as output
  pinMode(10, OUTPUT);
  pinMode(SD_CS, OUTPUT);
  // Define pin #12 as input and activate the internal pull-up resistor
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(A5, OUTPUT);  
  
  Serial.begin(9600);
 
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735R chip, black tab    
  
  tft.setRotation(2);       

  //NeoPixel Setup
  pixels.begin();
  pixels.setBrightness(100);

  //analogWrite(lcdBacklight, 255);   // Turn Backlight on full
  
  // Check for SD Card
  if (!SD.begin(SD_CS)) 
  {
    // No SD card, or failed to initialise card
    sd_card=0;
    // Arduino SD library does something to SPI speed when it fails
    // So need to reset otherwise screen is slow.
    SPI.setClockDivider(SPI_CLOCK_DIV4); // 16/4 MHz
  }  
  else sd_card=1;
  
  tft.fillScreen(ST7735_BLACK);     // Fill screen with black

  tftMenuInit();                    // Draw menu
  menu_select=1;                    // Select 1st menu item 
  tftMenuSelect(menu_select);       // Highlight selected menu item

}

//This is the main arduino loop
void loop() {
  int x;

  int button_left = digitalRead(2);
  int button_down = digitalRead(3);
  int button_right = digitalRead(5);
  int button_up = digitalRead(6);
  int button_select = digitalRead(7);
  
  Serial.println(button_left);
  Serial.println(button_down);
  Serial.println(button_right);
  Serial.println(button_up);
  Serial.println(button_select);

  pixels.setPixelColor(0, pixels.Color(255,0,255)); 
  pixels.setPixelColor(1, pixels.Color(255,0,255)); 
  pixels.show(); 
  
  if ((button_select == 0) & (keydown==0)) {
    // Select
    keydown=1;
    menu_func[menu_select]();       // Call the appropriate menu function from array
    tftMenuInit();                  // Redraw the Menu
    tftMenuSelect(menu_select);     // Highlight the current menu item
    Serial.println("Main Function Return");
  }
  else if ((button_up == 0) & (keydown==0)) {
    // Up
    // move up one menu item, if at top wrap to bottom
    keydown=1;    
    if (menu_select>1) tftMenuSelect(menu_select-1);
    else tftMenuSelect(numMenu);
  }
  else if ((button_left == 0) & (keydown==0)){
    // Left
    keydown=1;    
  }
  else if ((button_down == 0) & (keydown==0)){
    // Down
    // move down one menu item, if at bottom wrap to top
    keydown=1;    
    if (menu_select<numMenu) tftMenuSelect(menu_select+1);
    else tftMenuSelect(1);   
  }
  else if ((button_right == 0) & (keydown==0)){
    // Right
    keydown=1;    
  }  
  else if ((button_left == 1) & (button_down == 1) & (button_right == 1) & (button_up == 1))
  {
    keydown=0; // key released
  }

  pixels.setPixelColor(0, pixels.Color(0,255,255));
  pixels.setPixelColor(1, pixels.Color(0,255,255));
  pixels.show();

}


/*
 * 
 * TFT GRAPHICS VIZ
 * 
 */

 float p = 3.1415926;

void vizSetup() {
    
  tft.fillScreen(ST7735_BLACK);

  // a single pixel
  tft.drawPixel(tft.width()/2, tft.height()/2, ST7735_GREEN);
  delay(500);

  // line draw test
  testlines(ST7735_YELLOW);
  delay(500);

  // optimized lines
  testfastlines(ST7735_RED, ST7735_BLUE);
  delay(500);

  testdrawrects(ST7735_GREEN);
  delay(500);

  testfillrects(ST7735_YELLOW, ST7735_MAGENTA);
  delay(500);

  tft.fillScreen(ST7735_BLACK);
  testfillcircles(10, ST7735_BLUE);
  testdrawcircles(10, ST7735_WHITE);
  delay(500);

  testroundrects();
  delay(500);

  testtriangles();

  return;
}

void testlines(uint16_t color) {
  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, 0, x, tft.height()-1, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, 0, tft.width()-1, y, color);
  }

  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, 0, x, tft.height()-1, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, 0, 0, y, color);
  }

  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, tft.height()-1, x, 0, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, tft.height()-1, tft.width()-1, y, color);
  }

  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, x, 0, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, 0, y, color);
  }
}

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST7735_BLACK);
  for (int16_t y=0; y < tft.height(); y+=5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x=0; x < tft.width(); x+=5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void testdrawrects(uint16_t color) {
  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=tft.width()-1; x > 6; x-=6) {
    tft.fillRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color1);
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color2);
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=radius; x < tft.width(); x+=radius*2) {
    for (int16_t y=radius; y < tft.height(); y+=radius*2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=0; x < tft.width()+radius; x+=radius*2) {
    for (int16_t y=0; y < tft.height()+radius; y+=radius*2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
}

void testtriangles() {
  tft.fillScreen(ST7735_BLACK);
  int color = 0xF800;
  int t;
  int w = tft.width()/2;
  int x = tft.height()-1;
  int y = 0;
  int z = tft.width();
  for(t = 0 ; t <= 15; t++) {
    tft.drawTriangle(w, y, y, x, z, x, color);
    x-=4;
    y+=4;
    z-=4;
    color+=100;
  }
}

void testroundrects() {
  tft.fillScreen(ST7735_BLACK);
  int color = 100;
  int i;
  int t;
  for(t = 0 ; t <= 4; t+=1) {
    int x = 0;
    int y = 0;
    int w = tft.width()-2;
    int h = tft.height()-2;
    for(i = 0 ; i <= 16; i+=1) {
      tft.drawRoundRect(x, y, w, h, 5, color);
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      color+=1100;
    }
    color+=100;
  }
}



/*
 * 
 * PONG IMPLEMENTATION
 * 
 */

/*
 TFT Pong
 This example is for any Arduino with a ST7735 compatible TFT screen.
 It reads the value of analog inputs to move a rectangular platform
 on the x and y axes. The platform can intersect with a ball
 causing it to bounce. This code is adapted from Esplora TFT Pong.
 
 This example code is in the public domain.
 
 Created by Tom Igoe December 2012
 Modified 15 April 2013 by Scott Fitzgerald
 http://arduino.cc/en/Tutorial/EsploraTFTPong
 
 Updated for ST7735 compatible TFT's by Jens Chr Brynildsen, October 2013
 https://github.com/jenschr/Arduino-libraries
 
 Requires Adafruit's ST7735 library + GFX library
 https://github.com/adafruit/Adafruit-ST7735-Library
 https://github.com/adafruit/Adafruit-GFX-Library
 */
//int xAxisPin = 5;
//int yAxisPin = 6;
//const int listSize = 10;
//int avgArray [listSize] = {0,0,0,0,0,0,0,0,0,0}; // smoothing of x value
//
//// variables for the position of the ball and paddle
//int paddleX = 0;
//int paddleY = 0;
//int oldPaddleX, oldPaddleY;
//int ballDirectionX = 1;
//int ballDirectionY = 1;
//
//int ballX, ballY, oldBallX, oldBallY;
//
//long lastFrameTime;
//long frameTimeAsMilliseconds;
//long count;
//
//void pongLoop() {
//  // save the width and height of the screen
//  int myWidth = tft.width();
//  int myHeight = tft.height();
//  
//  Serial.println( digitalRead( xAxisPin ) );
//  Serial.println("test");
//  
//  // map the paddle's location to the joystick's position 
//  paddleX = map( digitalRead( xAxisPin ), 0, 1024, 0, myWidth) - 20/2; 
//  paddleY = map( digitalRead( yAxisPin ), 1024, 0, 0, myHeight) - 5/2; 
//
//  // erase the previous position of the paddle if different from present
//  if (oldPaddleX != paddleX || oldPaddleY != paddleY) {
//    tft.fillRect(oldPaddleX, oldPaddleY, 20, 5, 0x000000);
//  }
//
//  // draw the paddle on screen, save the current position
//  // as the previous.
//  tft.fillRect(paddleX, paddleY, 20, 5, 0xffffff);
//  oldPaddleX = paddleX;
//  oldPaddleY = paddleY;
//
//  moveBall();
//  
//  delayMicroseconds( count % 16383  );
//  count++;
//}
//
//
//// this function determines the ball's position on screen
//void moveBall() {
//  // if the ball goes offscreen, reverse the direction:
//  if (ballX > tft.width() || ballX < 0) {
//    ballDirectionX = -ballDirectionX;
//  }
//
//  if (ballY > tft.height() || ballY < 0) {
//    ballDirectionY = -ballDirectionY;
//  }  
//
//  // check if the ball and the paddle occupy the same space on screen
//  if (inPaddle(ballX, ballY, paddleX, paddleY, 20, 5)) {
//    ballDirectionY = -ballDirectionY;
//  }
//
//  // update the ball's position
//  ballX += ballDirectionX;
//  ballY += ballDirectionY;
//
//  // erase the ball's previous position
//  if (oldBallX != ballX || oldBallY != ballY) {
//    tft.fillRect(oldBallX, oldBallY, 5, 5, 0x000000);
//  }
//
//  // draw the ball's current position
//  tft.fillRect(ballX, ballY, 5, 5, 0xffff00);
//
//  oldBallX = ballX;
//  oldBallY = ballY;
//
//}
//
//// this function checks the position of the ball
//// to see if it intersects with the paddle
//boolean inPaddle(int x, int y, int rectX, int rectY, int rectWidth, int rectHeight) {
//  boolean result = false;
//
//  if ((x >= rectX && x <= (rectX + rectWidth)) && 
//    (y >= rectY && y <= (rectY + rectHeight))) {
//    result = true; 
//  }
//
//  return result;  
//}
//
//void pongSetup() {
//
//  Serial.begin(9600);
//
//  // initialize the display
//  tft.initR(INITR_BLACKTAB);
//  // set the background the black
//  tft.fillScreen(ST7735_BLACK);
//
//  pongLoop();
//}
