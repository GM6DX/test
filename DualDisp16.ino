// Using a MEGA and displaying the BMP gauge from an SD card 
//Uses the SD library in the .zip file. Note the sd.begin(10,11,12,13) It's needed for a MEGA


#include <SD.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;

// most mcufriend shields use these pins and Portrait mode:
uint8_t YP = A1;  // must be an analog pin, use "An" notation!
uint8_t XM = A2;  // must be an analog pin, use "An" notation!
uint8_t YM = 7;   // can be a digital pin
uint8_t XP = 6;   // can be a digital pin

#include <TouchScreen.h>
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint tp;

#define MINPRESSURE 20
#define MAXPRESSURE 1000

#define M_SIZE 1.3333
#define Y_OFFSET 0

float ltx = 0;    // Saved x coord of bottom of needle
uint16_t osx = M_SIZE*120, osy = M_SIZE*120; // Saved x & y coords
//uint32_t updateTime = 0;       // time for next update
int old_analog =  -999; // Value last displayed

byte bankmem[64] = {
         0x00, 0xBA, 0x02, 0x7A,
         0x00, 0xD6, 0x02, 0x96,
         0x00, 0xF2, 0x02, 0xB2,
         0x01, 0x0E, 0x02, 0xCE,
         0x01, 0x2A, 0x02, 0xEA, 
         0x01, 0x46, 0x03, 0x06,
         0x01, 0x62, 0x03, 0x22,
         0x01, 0x7E, 0x03, 0x3E,
         0x01, 0x9A, 0x03, 0x5A, 
         0x01, 0xB6, 0x03, 0x76,
         0x01, 0xD2, 0x03, 0x92,
         0x01, 0xEE, 0x03, 0xAE,
         0x02, 0x0A, 0x03, 0xCA,
         0x02, 0x26, 0x03, 0xE6,
         0x02, 0x42, 0x04, 0x02,
         0x02, 0xFE, 0x04, 0x1E};
    
String bankstr[16] = {
         "160M", "80M ", "60M ", "40M ", "30M ", "20M ", "17M ", "15M ",
         "12M ", "10M ", "6M  ", "RAD ", "AIR ", "2M  ", "70CM", "HFX "};
        
float ctcss[50] = {
         67, 69.3, 71.9, 74.4, 77, 79.7, 82.5, 85.4, 88.5, 91.5,
        94.8, 97.4, 100, 103.5, 107.2, 110.9, 114.8, 118.8, 123, 127.3, 
        131.8, 136.5, 141.3, 146.2, 151.4, 156.7, 159.8, 162.2, 165.5, 167.9, 
        171.3, 173.8, 177.3, 179.9, 183.5, 186.2, 189.9, 192.8, 196.6, 199.5, 
         203.5, 206.5, 210.7, 218.1, 225.7, 229.1, 233.6, 241.8, 250.3, 254.1};
        
byte dcs[104] = { 
         023, 025, 026, 031, 032, 036, 043, 047, 051, 053, 
         054, 065, 071, 072, 073, 074, 114, 115, 116, 122, 
         125, 131, 132, 134, 143, 145, 152, 155, 156, 162, 
         165, 172, 174, 205, 212, 223, 225, 226, 243, 244, 
         245, 246, 251, 252, 255, 261, 263, 265, 266, 271, 
         274, 306, 311, 315, 325, 331, 332, 343, 346, 351, 
         356, 364, 365, 371, 411, 412, 413, 423, 431, 432, 
         445, 446, 452, 454, 455, 462, 464, 465, 466, 503, 
         506, 516, 523, 526, 532, 546, 565, 606, 612, 624, 
         627, 631, 632, 654, 662, 664, 703, 712, 723, 731, 
         732, 734, 743, 754};


// 0x9488
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define ORANGE 0xFC00
#define GREEN 0x45C9
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF


byte Byte1[5];
byte Freqs[5];
byte TXData[5] = {0, 0, 0, 0, 0xF7};
byte TXSend[5] = {0, 0, 0, 0, 0};
byte RXData[5] = {0, 0, 0, 0, 0xE7};
byte FMData[5] = {0, 0, 0, 0, 3};
byte VData[5] = {0, 0x68, 0, 0, 0xBB};
byte GrxData[5] = {0, 0xA5, 0, 0, 0xBB};
byte GtxData[5] = {0, 0x95, 0, 0, 0xBB};
byte FMenu[5] = {0, 0x89, 0, 0, 0xBB};
byte M2[5] = {0, 0xAB, 0, 0, 0xBB};
byte M6[5] = {0, 0xAA, 0, 0, 0xBB};
byte M7[5] = {0, 0xAC, 0, 0, 0xBB};
byte MH[5] = {0, 0x9B, 0, 0, 0xBB};
byte EEv[5] = {0, 0xA8, 0, 0, 0xBB};
byte GSq[5] = {0, 0x72, 0, 0, 0xBB}; 
byte DWPRIdata[5] = {0, 0x8c, 0, 0, 0xBB};
byte ATCdata[5] = {0, 0xa9, 0, 0, 0xBB};
byte QSPLdata[5] = {0, 0x8d, 0, 0, 0xBB};
byte VOXKYRBKdata[5] = {0, 0x6b, 0, 0, 0xBB};
byte DISPNBdata[5] = {0, 0x6a, 0, 0, 0xBB};
int slev = 5;
int powr = 0;
uint8_t band = 0;
uint8_t cllr = 0;
String vfo ="VFx";
int MHz=0;
uint8_t agc = 0;
uint8_t dw = 0;
uint8_t pri = 0;
uint8_t atc = 0;
uint8_t qspl = 0;
uint8_t vox = 0;
uint8_t kyr = 0;
uint8_t bk = 0;
uint8_t disp = 0;
uint8_t nb = 0;
uint8_t lock = 0;
uint8_t lock2 = 0;
uint8_t man = 0;
uint8_t agcspeed = 0;
uint8_t gnsqnum = 0;
uint8_t swr = 0;
uint8_t ptt = 0;
String gaugerx = "";
String gaugetx = "";
int EEval = 0;
String fmen = "";
char fnum = 0;
String GnSq = "";
int sql=5;
int a1;
int a2;
int tmp;
int tmp2;
String str1;
String str2;
String str3;
String str4;
int v1;
int v2;
int v3;
int v4;
char packf[11] = "---.---.--";
char packf2[11] = "---.---.--";
String mode = "";
int modenum = 0;
uint8_t done = 0;
uint8_t nump = 0;
int analogPin = A12; 
int inval = 0; 
uint16_t myline[3][140];
float steps[11] = {0, 1, 2.5, 5, 6.25, 10, 12.5, 25, 50, 100, 1000};
String stepss[11] = {"", "1.00", "2.50", "5.00" ,"6.25", "10.0", "12.5", "25.0", "50.0", "100 ", "1000" };
uint8_t steps1 = 6;
uint8_t steps2 = 6;
uint32_t freq;
String freq2;
byte TXvals[5];
char namebuf[8] = "";
uint8_t TXint[9];
float volts = 0;
uint8_t info = 0;
uint8_t bnd = 1;
uint8_t hme = 0;
uint16_t addr1;
uint16_t addr2;
uint8_t banda;
uint8_t bandb;
uint8_t msb;
uint8_t lsb; 
uint8_t fdis = 0; 
uint8_t ipo = 0;
uint8_t att = 0;
uint8_t ipo2 = 0;
uint8_t att2 = 0;
uint8_t xfval = 185;
uint8_t vfot=0;
uint8_t was3 = 0;
uint8_t mmm = 0;
uint8_t mms = 0;
uint8_t swp = 0;
char tag[8] = "";
uint8_t rmenu = 0;
uint8_t need = 1;
uint8_t mValue = 0;
String wow = " VFA VFB HME MEM ";
uint8_t dovfo = 0;
int xtch = 0;
int ytch = 0;
uint8_t ty = 0;
uint8_t prc = 0;
uint8_t roc = 0;

struct datastore {
    uint8_t varA;
    uint8_t varB;
    uint8_t varC;
    uint8_t varD;
};

struct datastore myData;
File myFile;


void test(int num) 
{
    tft.setCursor(10,50);
    tft.setTextSize(2);   
    tft.print(num); 
    tft.print(" ");
}


void setup()
{
    //uint16_t ID;
    //ID = tft.readID();
    //if (ID == 0x0D3D3) ID = 0x9481;
    Serial3.begin(9600);
    Serial3.setTimeout(40);
    //tft.begin(0x9486);  //
    tft.begin(0x9488);  //
    //tft.begin(50437);  // 320 x 240 screen
    tft.fillScreen(WHITE);
    tft.setRotation(1);
    SD.begin(10,11,12,13);
    // showBMP("R_Meter3.bmp", 0, 0); // 0x9486
    showBMP("S_Meter2.bmp", 0, 0); //0x9488

    mylogo();


    tft.drawLine(320,0,320,319,BLACK);
    tft.drawLine(0,180,320,180,BLACK);
    tft.drawLine(0,261,320,261,BLACK);
    tft.drawLine(0,291,320,291,BLACK);

     // This uses 0x17 which is read from the eeprom calibtration settings. Not the actual voltage reading.
     // I'm not sure where the real value is stored.
     TXvals[0] = 0;
     TXvals[1] = 0x17;
     TXvals[2] = 0;
     TXvals[3] = 0 ;     
     TXvals[4] = 0xBB;
     Serial3.write(TXvals,5);
     Serial3.readBytes(Byte1,3);
     volts = Byte1[0];

      if (SD.exists("myFile"))
      { 
            myFile = SD.open("myFile", FILE_READ);
            myFile.read((const uint8_t *)&myData, sizeof(myData));
            myFile.close();
            need = myData.varA;
            mValue = myData.varB;
      }
}


void loop()
{   
        if(mValue)
        {
        inval = analogRead(analogPin);
        tmp = map(inval,0,1023,0,100);
        plotNeedle(tmp,0);
        }


        Serial3.write(FMData,5);
        Serial3.readBytes(Freqs,5);
        Convert(Freqs[0],16);
        Freqs[0]=a1;
        Convert(Freqs[1],16);
        Freqs[1]=a1;
        Convert(Freqs[2],16);
        Freqs[2]=a1;
        Convert(Freqs[3],16);
        Freqs[3]=a1;
        Convert(Freqs[4],16);
        modenum = a1;
                                                
        vfoab();
                  
        DispL();
        if(rmenu == 0) DispR0();
        if(rmenu == 1) DispR1();
        if(rmenu == 2) DispR2(); 
        touchareas();
        GetPW(); 

        Serial3.write(GrxData,5);  //guage rx data
        Serial3.readBytes(Byte1,2);
        tmp = Byte1[0];
        RXGauge(tmp);
        
        Serial3.write(GtxData,5);  //gauge tx data
        Serial3.readBytes(Byte1,2);
        tmp = Byte1[0];
        TXGauge(tmp);                                                                           

        Serial3.write(FMenu,5);  // f menu
        Serial3.readBytes(Byte1,2);
        tmp = Byte1[0];                          
        MFa(tmp);
        tmp=0;
                                                       
        if((rmenu == 1) || (rmenu == 2))
        {

                  Serial3.write(EEv,5);  // DSP
                  Serial3.readBytes(Byte1,2);
                  EEval = Byte1[0];    

                  Serial3.write(DWPRIdata,5); 
                  Serial3.readBytes(Byte1,2);
                  dw = Byte1[0];  
                  pri = Byte1[0];
                                            
                  Serial3.write(ATCdata,5);  
                  Serial3.readBytes(Byte1,2);
                  atc = Byte1[0];
                  prc = Byte1[0];  
                 
                  Serial3.write(QSPLdata,5);  
                  Serial3.readBytes(Byte1,2);
                  qspl = Byte1[0];
                  
                  Serial3.write(VOXKYRBKdata,5); 
                  Serial3.readBytes(Byte1,2);
                  vox = Byte1[0];
                  kyr = Byte1[0];
                  bk = Byte1[0];                          
                  
                  Serial3.write(DISPNBdata,5);  
                  Serial3.readBytes(Byte1,2);
                  disp = Byte1[0];
                  nb = Byte1[0]; 
                  lock = Byte1[0];
                  man = Byte1[0];
                  agcspeed = Byte1[0]; 

                  Serial3.write(TXData,5);
                  Serial3.readBytes(Byte1,2);
                  swr = 64 & Byte1[0];
                  ptt = 128 & Byte1[0];
                  
                  Serial3.write(RXData,5); //RX meter level
                  Serial3.readBytes(Byte1,2);
                  //tmp = 15 & Byte1[0];
                  tmp = 128 & Byte1[0];
                        if(tmp==0)
                        {
                             sql=0;
                        }
                        if(tmp==128)
                        {
                             sql=1;
                        }



                  Serial3.write(GSq,5);  // sq or gain
                  Serial3.readBytes(Byte1,2);
                  tmp = 128 & Byte1[0];
                  gnsqnum = tmp; 
                     if(tmp == 0)
                     {
                            GnSq = F("GN");
                     }
                     if(tmp == 128)
                     {
                            GnSq = F("SQ");
                     }                                                
                     tmp=0;


               TXvals[0] = 0;
               TXvals[1] = 0x71;
               TXvals[2] = 0;
               TXvals[3] = 0 ;     
               TXvals[4] = 0xBB;
               Serial3.write(TXvals,5);
               Serial3.readBytes(Byte1,2);
               if( (bitRead(Byte1[0],5) == 0) &&  (bitRead(Byte1[0],4) == 0)) lock2 = 0;
               if( (bitRead(Byte1[0],5) == 0) &&  (bitRead(Byte1[0],4) == 1)) lock2 = 1;
               if( (bitRead(Byte1[0],5) == 1) &&  (bitRead(Byte1[0],4) == 0)) lock2 = 2;
               if( (bitRead(Byte1[0],4) == 1) &&  (bitRead(Byte1[0],5) == 1)) lock2 = 3;
        }
      
}

void(* resetFunc) (void) = 0;

void drawMeter()
{
          tft.setTextColor(BLACK);
          tft.setCursor(35,95);
          tft.setTextSize(4);   
          tft.print("PLEASE WAIT"); 
          showBMP("S_Meter2.bmp", 0, 0);
          return;   
}

void setHome(uint8_t num)
{
 tft.setTextColor(BLUE,CYAN);   
 if(num == 10)
  {
    addr2 = 0x0456;  //6m
    tft.setCursor(330,64);
    tft.print("HOME 6M");
  }
  else
  {
    if(num == 14)
    {
      addr2 = 0x048d;  //uhf
      tft.setCursor(330,64);
      tft.print("HOME UHF");
    }
    else
    {
        if( (num == 11) || (num == 12) || (num == 13) )
        {
          addr2 = 0x0472; //vhf
          tft.setCursor(330,64);
          tft.print("HOME VHF");
        }
        else
        {
            addr2 = 0x043A;  //hf
            tft.setCursor(330,64);
            tft.print("HOME HF");
        }
    }
  }
 tft.setTextColor(BLACK,CYAN);              
}

void readBank()
{
   //vfo band info
   TXvals[0] = 0;
   TXvals[1] = 0x6C;
   TXvals[2] = 0;
   TXvals[3] = 0 ;     
   TXvals[4] = 0xBB;
   Serial3.write(TXvals,5);
   Serial3.readBytes(Byte1,2); 
   a1=int(Byte1[0]/16);
   bandb = a1;
   a2=a1*16;
   banda = Byte1[0] - a2;  
}

void writeBank()
{
   //current band info
   TXvals[0] = 0;
   TXvals[1] = 0x6C;
   TXvals[2] = 0;
   TXvals[3] = 0 ;     
   TXvals[4] = 0xBB;
   Serial3.write(TXvals,5);
   Serial3.readBytes(Byte1,2);
   Byte1[0] = (bandb * 16);
   Byte1[0] += banda;
   //write new band        
   TXvals[0] = 0;
   TXvals[1] = 0x6C;
   TXvals[2] = Byte1[0];
   TXvals[3] = Byte1[1];     
   TXvals[4] = 0xBC;
   Serial3.write(TXvals,5); 
}


void DispInfo()
{
         uint32_t split1;
         float split2;         
         uint8_t tmp;
         uint8_t cmode;
         uint8_t dmode;
         uint8_t cte;
         uint8_t ctd;
         uint8_t dce;
         uint8_t dcd;
         uint8_t tmp2 = 255;  

         readBank();
         tft.setTextSize(2);
         tft.setTextColor(BLACK,CYAN); 
         
         tft.setCursor(330,10);
         tft.print("VFOA");
         tft.print(" ");
         tft.print(bankstr[banda]);           
                              
         tft.setCursor(330,28);
         tft.print("VFOB");
         tft.print(" ");
         tft.print(bankstr[bandb]);                       
                      
         // memory channel number 
         TXvals[0] = 0x05;
         TXvals[1] = 0x0E;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2); 
         tmp = Byte1[0];         
         tft.setCursor(330,46);
         tft.print("MEM  ");
         tft.print("CH");
         tft.print(tmp+1);
         tft.print("  ");       
         tft.setCursor(330,64);
         tft.print("HOME");

         vfoab();
  
         if(bnd == 1)
         {
            addr2 = (banda * 28) + 0x00BA; 
            if(!hme == 1)
            {            
               tft.setTextColor(BLUE,CYAN);                       
               tft.setCursor(330,10);
               tft.print("VFOA");
               tft.print(" ");
               tft.print(bankstr[banda]); 
               tft.setTextColor(BLACK,CYAN); 
            }        
            if(hme == 1) setHome(banda);               
         }
         
         if(bnd == 2)
         {
            addr2 = (bandb * 28) + 0x027A;
            if(!hme == 1)
            {
              tft.setTextColor(BLUE,CYAN);            
              tft.setCursor(330,28);
              tft.print("VFOB");
              tft.print(" ");
              tft.print(bankstr[bandb]);
              tft.setTextColor(BLACK,CYAN);
            }
            if(hme == 1) setHome(bandb);                 
         }
         
         if(bnd == 3)
         {
            addr2 = (tmp * 28) + 0x0545;
            if(hme == 1)  // read band fromx channel to get band  
            {
                addr1 = addr2;           
                addr1 += 1; 
                msb = addr1 >> 8;
                lsb = addr1 & 0x00FF;
                TXvals[0] = msb;
                TXvals[1] = lsb;
                TXvals[2] = 0;
                TXvals[3] = 0 ;     
                TXvals[4] = 0xBB;
                Serial3.write(TXvals,5);
                Serial3.readBytes(Byte1,2);
                tmp = 7 & Byte1[0];
                if(tmp == 0) setHome(0); //hf
                if(tmp == 1) setHome(10); //6m
                if(tmp == 5) setHome(14); //uhf
                if((tmp > 1) && (tmp < 5)) setHome(12); //vhf
             }
             else
             {
         tft.setTextColor(BLUE,CYAN);
         tft.setCursor(330,46);
         tft.print("MEM  ");
         tft.print("CH");
         tft.print(tmp+1);
         tft.print("  ");
         tft.setTextColor(BLACK,CYAN);
             }
         }
                         

         //repeater split etc 
         addr1 = addr2;           
         addr1 += 1; 
         msb = addr1 >> 8;
         lsb = addr1 & 0x00FF;
         TXvals[0] = msb;
         TXvals[1] = lsb;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);
         tmp = Byte1[0]; 

         tft.setCursor(330,118);
         if((tmp & 192) == 192) tft.print("SPLIT  ");
         if((tmp & 128) == 128) tft.print("PLUS  +");
         if((tmp & 64) == 64)  tft.print("MINUS -");
         if((tmp & 192) == 0)  tft.print("SIMPLEX");

          split1 = 0;
          for(int a=12; a<16; a++)
          {
            //repeater rx split value 
            addr1 = addr2;           
            addr1 += a; 
            msb = addr1 >> 8;
            lsb = addr1 & 0x00FF;
            TXvals[0] = msb;
            TXvals[1] = lsb;
            TXvals[2] = 0;
            TXvals[3] = 0 ;     
            TXvals[4] = 0xBB;
            Serial3.write(TXvals,5);
            Serial3.readBytes(Byte1,2);
            split1 = split1 << 8;
            split1 = split1 + Byte1[0];
          } 
        split2 = split1;
        split2 /= 100000;
        tft.setCursor(330,208);
        tft.print("RX SPLIT ");
        if((tmp & 192) == 0) split2 = 0;
        tft.setCursor(330,226);        
        tft.print(dtostrf(split2,8,3,namebuf));   
        tft.print("MHz"); 

          split1 = 0;
          for(int a=16; a<20; a++)
          {
            //repeater tx split value 
            addr1 = addr2;           
            addr1 += a; 
            msb = addr1 >> 8;
            lsb = addr1 & 0x00FF;
            TXvals[0] = msb;
            TXvals[1] = lsb;
            TXvals[2] = 0;
            TXvals[3] = 0 ;     
            TXvals[4] = 0xBB;
            Serial3.write(TXvals,5);
            Serial3.readBytes(Byte1,2);
            split1 = split1 << 8;
            split1 = split1 + Byte1[0];
          } 
        split2 = split1;
        split2 /= 100000;
        tft.setCursor(330,244);
        tft.print("TX SPLIT ");
        if((tmp & 192) == 0) split2 = 0;
        tft.setCursor(330,262);        
        tft.print(dtostrf(split2,8,3,namebuf));   
        tft.print("MHz"); 


            //IPO ATT
            addr1 = addr2;           
            addr1 += 2; 
            msb = addr1 >> 8;
            lsb = addr1 & 0x00FF;
            TXvals[0] = msb;
            TXvals[1] = lsb;
            TXvals[2] = 0;
            TXvals[3] = 0 ;     
            TXvals[4] = 0xBB;
            Serial3.write(TXvals,5);
            Serial3.readBytes(Byte1,2);
            tmp = Byte1[0];
        tft.setCursor(330,280);
        tft.print("IPO  ");
        if(!bitRead(tmp, 5)) tft.print("OFF");
        if(bitRead(tmp, 5)) tft.print("ON ");
        
        tft.setCursor(330,298);
        tft.print("ATT  ");
        if(!bitRead(tmp, 4)) tft.print("OFF");
        if(bitRead(tmp, 4)) tft.print("ON ");        
          

         //ctcss or dcs 
         addr1 = addr2;           
         addr1 += 4; 
         msb = addr1 >> 8;
         lsb = addr1 & 0x00FF;
         TXvals[0] = msb;
         TXvals[1] = lsb;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2); 
         tmp = Byte1[0] & 0x0F;

         tft.setCursor(330,136);
         tft.print("CTSE       ");
         tft.setCursor(330,154);
         tft.print("DCSE        "); 
         tft.setCursor(330,172);
         tft.print("CTSD       "); 
         tft.setCursor(330,190);
         tft.print("DCSD       ");
 
                
         if((tmp & 1) || (tmp & 2))
         {
            if (tmp & 1)
            {
                //ctcss encode
                addr1 = addr2;            
                addr1 += 5; 
                msb = addr1 >> 8;
                lsb = addr1 & 0x00FF;
                TXvals[0] = msb;
                TXvals[1] = lsb;
                TXvals[2] = 0;
                TXvals[3] = 0 ;     
                TXvals[4] = 0xBB;
                Serial3.write(TXvals,5);
                Serial3.readBytes(Byte1,2); 
                cte = Byte1[0];
                tft.setCursor(330,136);
                tft.print("CTSE ");
                tft.print(ctcss[cte]);       
            }            
            if (tmp & 2)
            {
                //dcs encode
                addr1 = addr2;
                addr1 += 7; 
                msb = addr1 >> 8;
                lsb = addr1 & 0x00FF;
                TXvals[0] = msb;
                TXvals[1] = lsb;
                TXvals[2] = 0;
                TXvals[3] = 0 ;     
                TXvals[4] = 0xBB;
                Serial3.write(TXvals,5);
                Serial3.readBytes(Byte1,2); 
                dce = Byte1[0];
                tft.setCursor(330,154);
                tft.print("DCSE ");
                tft.print(dcs[dce]);
              }

         }
         if((tmp & 4) || (tmp & 8))
         {
            if (tmp & 4)
            {                  
                //ctcss decode
                addr1 = addr2;
                addr1 += 6; 
                msb = addr1 >> 8;
                lsb = addr1 & 0x00FF;
                TXvals[0] = msb;
                TXvals[1] = lsb;
                TXvals[2] = 0;
                TXvals[3] = 0 ;     
                TXvals[4] = 0xBB;
                Serial3.write(TXvals,5);
                Serial3.readBytes(Byte1,2); 
                ctd = Byte1[0];                 
                tft.setCursor(330,172);
                tft.print("CTSD ");
                tft.print(ctcss[ctd]);
 
            }             
            if (tmp & 8)
            {            
                //dcs decode
                addr1 = addr2;
                addr1 += 8; 
                msb = addr1 >> 8;
                lsb = addr1 & 0x00FF;
                TXvals[0] = msb;
                TXvals[1] = lsb;
                TXvals[2] = 0;
                TXvals[3] = 0 ;     
                TXvals[4] = 0xBB;
                Serial3.write(TXvals,5);
                Serial3.readBytes(Byte1,2); 
                dcd = Byte1[0];                 
                tft.setCursor(330,190);
                tft.print("DCSD ");
                tft.print(dcs[dcd]);
            }
        } 
        
                //read tag
                tft.setCursor(330,82);
                tft.print("TAG-");
                //tft.setCursor(330,100);
                for(int a = 0; a < 8; a++)
                {
                  addr1 = addr2;
                  addr1 = addr1 + (20 + a); 
                  msb = addr1 >> 8;
                  lsb = addr1 & 0x00FF;
                  TXvals[0] = msb;
                  TXvals[1] = lsb;
                  TXvals[2] = 0;
                  TXvals[3] = 0 ;     
                  TXvals[4] = 0xBB;
                  Serial3.write(TXvals,5);
                  Serial3.readBytes(Byte1,2);
                  tft.setTextColor(BLUE,CYAN);                 
                  tft.print(char(Byte1[0]));
                }
                tft.setTextColor(BLACK,WHITE);

               
}

void getChanMem()
{
         //vfo band info
         TXvals[0] = 0;
         TXvals[1] = 0x6C;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2); 
         a1=int(Byte1[0]/16);
         bandb = a1;
         a2=a1*16;
         banda = Byte1[0] - a2;    

         // memory channel number 
         TXvals[0] = 0x05;
         TXvals[1] = 0x0E;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2); 
         tmp = Byte1[0];  

         vfoab();

         if(bnd == 1)
         {
            addr2 = (banda * 28) + 0x00BA;         
            if(hme == 1) setHome2(banda);               
         }
         
         if(bnd == 2)
         {
            addr2 = (bandb * 28) + 0x027A;
            if(hme == 1) setHome2(bandb);                 
         }
         
         if(bnd == 3)
         {
            addr2 = (tmp * 28) + 0x0545;
            if(hme == 1)  // read band fromx channel to get band  
            {
                addr1 = addr2;           
                addr1 += 1; 
                msb = addr1 >> 8;
                lsb = addr1 & 0x00FF;
                TXvals[0] = msb;
                TXvals[1] = lsb;
                TXvals[2] = 0;
                TXvals[3] = 0 ;     
                TXvals[4] = 0xBB;
                Serial3.write(TXvals,5);
                Serial3.readBytes(Byte1,2);
                tmp = 7 & Byte1[0];
                if(tmp == 0) addr2 = 0x043A; //hf
                if(tmp == 1) addr2 = 0x0456; //6m
                if(tmp == 5) addr2 = 0x048d; //uhf
                if((tmp > 1) && (tmp < 5)) addr2 = 0x0472; //vhf
             }
         }
}
void setHome2(uint8_t num)
{ 
 if(num == 10)
    {
      addr2 = 0x0456;  //6m
    }
    else
    {
      if(num == 14)
      {
        addr2 = 0x048d;  //uhf
      }
      else
      {
          if( (num == 11) || (num == 12) || (num == 13) )
          {
            addr2 = 0x0472; //vhf
          }
          else
          {
              addr2 = 0x043A;  //hf
          }
      }
    }             
}

void mapTouch()
{
        xtch = map(tp.y, 966,175,1,480);
        ytch = map(tp.x, 200, 920, 1, 320);  
}

void third()
{
         TXvals[0] = 0;
         TXvals[1] = 0x68;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);
        
         Byte1[0] = Byte1[0] ^= 1 << 4;
         TXvals[0] = 0;
         TXvals[1] = 0x68;
         TXvals[2] = Byte1[0];
         TXvals[3] = Byte1[1];     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5); 
}
void fourth()
{
         TXvals[0] = 0;
         TXvals[1] = 0x68;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);
        
         Byte1[0] = Byte1[0] ^= 1 << 6;
         TXvals[0] = 0;
         TXvals[1] = 0x68;
         TXvals[2] = Byte1[0];
         TXvals[3] = Byte1[1];     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5); 
}

void touchareas()
{
      byte ModeS[5] = {0, 0, 0, 0, 0x07};
      uint8_t agctmp;
      uint8_t tmp = 1;
      uint8_t tick = 0;
        tp = ts.getPoint();
        pinMode(XM, OUTPUT);
        pinMode(YP, OUTPUT);
        pinMode(XP, OUTPUT);
        pinMode(YM, OUTPUT);
        mapTouch();

/*         
        // show touch x,y points
        tft.setCursor(0,10);
        tft.setTextSize(3);
        tft.setTextColor(RED,WHITE);
        //tft.print(tft.readPixel(xtch,ytch));
        tft.print("tp.x=" + String(tp.x) + " tp.y=" + String(tp.y) + "   ");
*/        
                       
    if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE)
    {
        tft.fillCircle(290, 110, 15, ORANGE);
        delay(300); 
        tft.fillCircle(290, 110, 15, WHITE);

        //display right hand side blue info panel
       if(((xtch > 265) & (xtch <305)) & ((ytch > 130) & (ytch < 170)))
       {
        tft.fillRect(321,0, 159,320,CYAN);  
        DispInfo();      
        do
        {
          tick += 1;
          tp = ts.getPoint();
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          pinMode(XP, OUTPUT);
          pinMode(YM, OUTPUT);
          if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) tmp = 0;
          delay(250);
        } while(tmp && (tick < 40) );      //stay here till screen is touched
        tft.fillRect(321,0,159,320,WHITE);
        if(rmenu == 0) mylogo(); 
        tmp = 1; 
        return;                   
       }

       // swap VFo
       if(((xtch > 5) & (xtch < 45)) & ((ytch > 130) & (ytch < 170)))
       {
          if((dovfo == 3) || (dovfo == 4) || (hme == 1) || (bnd == 3))
          {
          }
          else
          {
            TXSend[4] = 0x81;
            Serial3.write(TXSend,5);
            return;
          }         
       }

       // VFO HME MEM select
       if(((xtch > 50) & (xtch <110)) & ((ytch > 266) & (ytch < 296)))
       {
         tft.fillCircle(290, 110, 15, ORANGE);
         tft.setTextSize(3);
         tft.setCursor(13,265);     
         tft.print(wow);
         tmp = 1;
         delay(1000);
        do
        {
          tp = ts.getPoint();
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          pinMode(XP, OUTPUT);
          pinMode(YM, OUTPUT);
         
          if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) tmp = 0;
          
        } while(tmp);      //stay here till screen is touched
          mapTouch();

          //vfo-A area
          if(((xtch > 0) && (xtch < 89)) && ((ytch > 266) && (ytch < 296)))
          {
            if(dovfo == 4) fourth();
            delay(250);            
            if(dovfo == 3) third();
            delay(250);                        
              if(ty == 2) 
              {
                TXSend[4] = 0x81;
                Serial3.write(TXSend,5);
              }
              ty = 1;
              clearreadout();
          }
          //vfo-B area
          if(((xtch > 90) && (xtch < 159)) && ((ytch > 266) && (ytch < 296)))
          {
            if(dovfo == 4) fourth();
            delay(250);
            if(dovfo == 3) third();
            delay(250);                        
              if(ty == 1) 
              {
                TXSend[4] = 0x81;
                Serial3.write(TXSend,5);
              } 
              ty = 2;
              clearreadout();
          }
          was3 = ty;
          //3rd area
          if(((xtch > 160) && (xtch < 230)) && ((ytch > 266) && (ytch < 296)))
          {
            if(dovfo == 4) fourth();
            delay(250);            
            third();
            delay(250);            
            dovfo = 3;
          }
          
          //4th area
          if( ((xtch > 231) && (xtch < 319)) && ((ytch > 266) && (ytch < 296)) )
          {
            if(dovfo == 3) third();
            delay(250);            
            fourth();  
            delay(250);                      
            dovfo = 4;
          }

        tmp = 1;
        tft.fillCircle(290, 110, 15, WHITE);
        tft.setTextSize(3);
        tft.setCursor(13,265);     
        tft.print("                 "); 
        return;    

        /*
        TXSend[4] = 0x81;
        Serial3.write(TXSend,5);
        //vfoab();
        //swapvfo();
        return; 
        */        
       } 
/*          
       if(((xtch > 50) & (xtch <110)) & ((ytch > 266) & (ytch < 296)))
       {
         TXvals[0] = 0;
         TXvals[1] = 0x68;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);
        
         Byte1[0] = Byte1[0] ^= 1 << 0;
         TXvals[0] = 0;
         TXvals[1] = 0x68;
         TXvals[2] = Byte1[0];
         TXvals[3] = Byte1[1];     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5);
         //vfoab();
         return;         
       }
*/
       

   

       if(((xtch > 270) & (xtch < 305)) & ((ytch > 80) & (ytch < 125)))
       {         
          drawMeter();      
       }
      

       // toggle frequency area
       if(((xtch > 106) & (xtch < 212)) & ((ytch > 181) & (ytch < 264)))
       {       
          if(cllr == 0)
          {
              cllr = 3; 
          }
          else
          {
            if(cllr == 3) cllr = 0;          
            tft.setTextColor(WHITE, WHITE);
            tft.setTextSize(3);
            tft.setCursor(10,xfval + 5);   
            tft.print("<");       
            tft.setCursor(80,xfval + 5);
            tft.print(F("STEP "));    
            if(bnd == 1) tft.print(stepss[steps1]);
            if(bnd == 2) tft.print(stepss[steps2]);
            tft.setCursor(295,xfval + 5);          
            tft.print(">");
            tft.setTextColor(WHITE, WHITE);                   
          }
        return;           
       }
       
       if( ((xtch > 0) & (xtch <105)) & ((ytch > 181) & (ytch < 264)) & cllr == 3) 
       {
        if(bnd == 1)
        {
          steps1 --;
          if(steps1 == 0) steps1 = 10;
          return;
        }
        if(bnd == 2)
        {
          steps2 --;
          if(steps2 == 0) steps2 = 10;
          return;
        }         
       } 
             
       if( ((xtch > 213) & (xtch <318)) & ((ytch > 181) & (ytch < 264)) & cllr == 3) 
       {
        if(bnd == 1)
        {        
          steps1 ++;
          if(steps1 == 11) steps1 = 1;
          return; 
        }
        if(bnd == 2)
        {        
          steps2 ++;
          if(steps2 == 11) steps2 = 1;
          return; 
        }                
       }

                         
       //mode
       if(((xtch > 135) & (xtch <195)) & ((ytch > 266) & (ytch < 296)))
       {
          if(cllr == 0)
          {
              cllr = 2; 
          }
          else
          {
            if(cllr == 2) cllr = 0;            
          }
        return;           
       }  


         //S-Meter TX and RX scale modes
         if(((xtch > 0) & (xtch <50)) & ((ytch > 0) & (ytch < 35)))
         {
           TXvals[0] = 0;
           TXvals[1] = 0xA5;
           TXvals[2] = 0;
           TXvals[3] = 0 ;     
           TXvals[4] = 0xBB;
           Serial3.write(TXvals,5);
           Serial3.readBytes(Byte1,2); 
           tmp = Byte1[0];       
         
          switch (tmp) 
          {
            case 96: tmp = 97; break;
            case 97: tmp = 98; break;
            case 98: tmp = 99; break;
            case 99: tmp = 100; break;
            case 100: tmp = 101; break;
            case 101: tmp = 96; break;
            tmp = 96;
          }
      
         TXvals[0] = 0;
         TXvals[1] = 0xA5;
         TXvals[2] = tmp;
         TXvals[3] = Byte1[1];     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5); 
        return;            
        }


       if(((xtch > 270) & (xtch < 320)) & ((ytch > 0) & (ytch < 35)))
       {                                
         TXvals[0] = 0;
         TXvals[1] = 0x95;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2); 
         tmp = Byte1[0];       

         
         switch (tmp) 
         {

            case 4: tmp = 36; break;
            case 36: tmp = 68; break;
            case 68: tmp = 100; break;
            case 100: tmp = 132; break;
            case 132: tmp = 164; break;
            case 164: tmp = 196; break;
            case 196: tmp = 4; break;
            tmp = 4;       
          } 
       
         TXvals[0] = 0;
         TXvals[1] = 0x95;
         TXvals[2] = tmp;
         TXvals[3] = Byte1[1];     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5);
        return;                     
        }
 


       // Power change
       if(((xtch > 220) & (xtch <279)) & ((ytch > 266) & (ytch < 296)))
       {       
          if(cllr == 0)
          {
              cllr = 1; 
          }
          else
          {
              cllr = 0;
          }
        return;           
       } 

       // freq down
       if( ((xtch > 0) & (xtch <105)) & ((ytch > 181) & (ytch < 264)) & cllr == 0) 
       {
         if(bnd == 1) freq = freq - (steps[steps1] * 100);
         if(bnd == 2) freq = freq - (steps[steps2] * 100);
         putfreqinTX();        
         Serial3.write(TXvals,5);         
        return;
       } 

        //down
       if(((xtch > 0) & (xtch <49)) & ((ytch > 266) & (ytch < 296)))
       {

        
        if(cllr == 0)
        {
          //band down
          if(bnd == 1)
          {
            banda --;
            if(banda < 0) banda = 15;
            writeBank();
            return;
          }
          if(bnd == 2)
          {
            bandb --;
            if(bandb < 0) bandb = 15;
            writeBank();
            return;            
          }         
        }
        
        if(cllr == 1)
        {
         TXvals[0] = 0;
         TXvals[1] = band;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);

         agctmp = Byte1[0];
         if(agctmp > 19) agctmp = agctmp - 128;
         if(agctmp < 11)
         {
          agctmp -= 1;
         }
         else
         {
          agctmp -= 5;          
         }         
         if((band == 0x9B) && (agctmp < 5)) agctmp++;
         if((band == 0xAA) && (agctmp < 5)) agctmp++;
         if((band == 0xAB) && (agctmp < 5)) agctmp++;
         if((band == 0xAC) && (agctmp < 2)) agctmp++;                  
         if(agctmp > 19) agctmp = agctmp + 128;

         TXvals[0] = 0;
         TXvals[1] = band;
         TXvals[2] = agctmp;
         TXvals[3] = Byte1[1] ;     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5);          
        }
        if(cllr == 2)
        {
        switch (modenum) 
        {
          case 0: modenum = 0x0C; break;
          case 1: modenum = 0; break;
          case 2: modenum = 1; break;
          case 3: modenum = 2; break;
          case 4: modenum = 3; break;       
          case 8: modenum = 4; break;
          case 0x0A: modenum = 8; break;
          case 0x0C: modenum = 0x0A; break; 
          default: modenum = 8; break;
        }
       
         ModeS[0] = modenum;
         Serial3.write(ModeS,5);
        }
        return; 
       }

       //freq up      
       if( ((xtch > 213) & (xtch <318)) & ((ytch > 181) & (ytch < 264)) & cllr == 0) 
       {
         if(bnd == 1) freq = freq + (steps[steps1] * 100);
         if(bnd == 2) freq = freq + (steps[steps2] * 100);
         putfreqinTX();        
         Serial3.write(TXvals,5); 
       }


       // up
       if(((xtch > 280) & (xtch <329)) & ((ytch > 266) & (ytch < 296)))
       {
        if(cllr == 0)
        {
          //band up
          if(bnd == 1)
          {
            banda ++;
            if(banda > 15) banda = 0;
            writeBank(); 
            return;                      
          }
          if(bnd == 2)
          {
            bandb ++;
            if(bandb > 15) bandb = 0;
            writeBank();
            return;                        
          }         
        }
        if(cllr == 1)
        {
         TXvals[0] = 0;
         TXvals[1] = band;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);

         agctmp = Byte1[0];
         if(agctmp > 19) agctmp = agctmp - 128;
         if(agctmp < 10)
         {
          agctmp += 1;
         }
         else
         {
          agctmp += 5;          
         }
         if((band == 0x9B) && (agctmp > 100)) agctmp--;
         if((band == 0xAA) && (agctmp > 100)) agctmp--;
         if((band == 0xAB) && (agctmp > 50)) agctmp--;
         if((band == 0xAC) && (agctmp > 20)) agctmp--;          
         if(agctmp > 19) agctmp = agctmp + 128;

         TXvals[0] = 0;
         TXvals[1] = band;
         TXvals[2] = agctmp;
         TXvals[3] = Byte1[1] ;     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5);          
        }
                                
        if(cllr == 2)
        {
        switch (modenum) 
        {

          case 0: modenum = 1; break;
          case 1: modenum = 2; break;
          case 2: modenum = 3; break;
          case 3: modenum = 4; break;
          case 4: modenum = 8; break;       
          case 8: modenum = 0x0A; break;
          case 0x0A: modenum = 0x0C; break;
          case 0x0C: modenum = 0; break; 
          default: modenum = 8; break;
        }
   
         ModeS[0] = modenum;
         Serial3.write(ModeS,5);          
        }
        return;         
       }


       // mfx menu ++ 
       if(((xtch > 165) & (xtch < 330)) & ((ytch > 296) & (ytch < 320))) 
       {
         TXvals[0] = 0;
         TXvals[1] = 0x89;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);
        
       
         fnum ++;
         if(fnum == 17) fnum = 0;
         TXvals[0] = 0;
         TXvals[1] = 0x89;
         TXvals[2] = fnum;
         TXvals[3] = Byte1[1];     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5);
         return;        
       } 

        // mfx menu --
       if(((xtch > 0) & (xtch < 165)) & ((ytch > 296) & (ytch < 320))) 
       {
         TXvals[0] = 0;
         TXvals[1] = 0x89;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2); 
       
         fnum -- ;
         if(fnum == -1) fnum = 16;       
         TXvals[0] = 0;
         TXvals[1] = 0x89;
         TXvals[2] = fnum;
         TXvals[3] = Byte1[1];     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5); 
         return;       
       }        
            

//start of right side menu


    if(rmenu == 1)
    {

       // Lock on/off
       if(((xtch > 330) & (xtch <410)) & ((ytch > 0) & (ytch < 32))) 
       {
        if((lock & 64) == 64) TXSend[4] = 0x00;
        if((lock & 64) == 0) TXSend[4] = 0x80;                       
        Serial3.write(TXSend,5);
        return;                
       }
      

        //lock selection
       if(((xtch > 411) & (xtch <480)) & ((ytch > 0) & (ytch < 32))) 
       {
        if((lock & 64) == 0)
        {
         TXvals[0] = 0;
         TXvals[1] = 0x71;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);

         Byte1[0] = Byte1[0] + 16;
         if(Byte1[0] == 0x43) Byte1[0] = 0x03;
         TXvals[0] = 0;
         TXvals[1] = 0x71;
         TXvals[2] = Byte1[0];
         TXvals[3] = Byte1[1];     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5);
         if( (bitRead(Byte1[0],5) == 0) &&  (bitRead(Byte1[0],4) == 0)) lock2 = 0;
         if( (bitRead(Byte1[0],5) == 0) &&  (bitRead(Byte1[0],4) == 1)) lock2 = 1;
         if( (bitRead(Byte1[0],5) == 1) &&  (bitRead(Byte1[0],4) == 0)) lock2 = 2;
         if( (bitRead(Byte1[0],4) == 1) &&  (bitRead(Byte1[0],5) == 1)) lock2 = 3;      
        }
        return;
       }

       //fast slow tune
       if(((xtch > 330) & (xtch <410)) & ((ytch > 32) & (ytch < 64))) 
       {
        
         TXvals[0] = 0;
         TXvals[1] = 0x6a;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);
        
         man = man ^= 1 << 7;
         TXvals[0] = 0;
         TXvals[1] = 0x6a;
         TXvals[2] = man;
         TXvals[3] = Byte1[1];     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5);
         return; 
       } 




       // GN SQ
        if(((xtch > 411) & (xtch <480)) & ((ytch > 65) & (ytch < 96)))
        {
         TXvals[0] = 0;
         TXvals[1] = 0x72;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);
        
         gnsqnum = gnsqnum ^= 1 << 7;
         TXvals[0] = 0;
         TXvals[1] = 0x72;
         TXvals[2] = gnsqnum;
         TXvals[3] = Byte1[1];     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5);
         return; 
        }
               
       //pri 
       if(((xtch > 330) & (xtch <410)) & ((ytch > 97) & (ytch < 128))) 
       {
         TXvals[0] = 0;
         TXvals[1] = 0x8c;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);

        dw = Byte1[0];
        if(bitRead(dw,3) && bitRead(dw,4))
        {
         bitClear(dw,4);
         bitClear(dw,3);          
        }
        else
        {
          if(((bitRead(dw,3) == 0 ) && (bitRead(dw,4) == 0) || (bitRead(dw,3) == 1 ) && (bitRead(dw,4) == 0)))
          {
          bitSet(dw,4);
          bitClear(dw,3);                    
          }
          else
          {
          bitClear(dw,4);            
          }
        }            
    
         TXvals[0] = 0;
         TXvals[1] = 0x8C;
         TXvals[2] = dw;
         TXvals[3] = Byte1[1] ;     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5);
         return;        
       }

        
        // dw  works?
        if(((xtch > 411) & (xtch <480)) & ((ytch > 97) & (ytch < 128))) 
        {
         TXvals[0] = 0;
         TXvals[1] = 0x8c;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);
         
        dw = Byte1[0];
        if(bitRead(dw,3) && bitRead(dw,4))
        {
         bitClear(dw,4);          
        }
        else
        {
          if(((bitRead(dw,3) == 0 ) && (bitRead(dw,4) == 0) || (bitRead(dw,3) == 1 ) && (bitRead(dw,4) == 0)))
          {
            bitSet(dw,4);
            bitSet(dw,3);                    
          }
          else
          {
          bitClear(dw,4);            
          }
        }            
         TXvals[0] = 0;
         TXvals[1] = 0x8C;
         TXvals[2] = dw;
         TXvals[3] = Byte1[1] ;     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5);
         return; 
        }


       // agc
       if(((xtch > 330) & (xtch <410)) & ((ytch > 127) & (ytch < 160)))
       {
         TXvals[0] = 0;
         TXvals[1] = 0xA8;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);
        
                  EEval = EEval ^= 1 << 5;     
                  TXvals[0] = 0;
                  TXvals[1] = 0xA8;
                  TXvals[2] = EEval;
                  TXvals[3] = Byte1[1];     
                  TXvals[4] = 0xBC;
                  Serial3.write(TXvals,5);
                  return;
       }

       if(((xtch > 411) & (xtch <480)) & ((ytch > 127) & (ytch < 160)))
       {   
        if((EEval & 0x20) == 0x20)
       
          {

            agctmp=(agcspeed & 3);
            switch (agctmp)
              {
                //auto
                case 0:
         TXvals[0] = 0;
         TXvals[1] = 0x6a;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);
         
                  agcspeed = agcspeed ^= 1 << 0;     
                  TXvals[0] = 0;
                  TXvals[1] = 0x6a;
                  TXvals[2] = agcspeed;
                  TXvals[3] = Byte1[1];     
                  TXvals[4] = 0xBC;
                  Serial3.write(TXvals,5);                
                  break;
                  
                //fast  
                case 1:
         TXvals[0] = 0;
         TXvals[1] = 0x6a;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);
         
                  agcspeed = agcspeed ^= 1 << 0;
                  agcspeed = agcspeed ^= 1 << 1;     
                  TXvals[0] = 0;
                  TXvals[1] = 0x6a;
                  TXvals[2] = agcspeed;
                  TXvals[3] = Byte1[1];     
                  TXvals[4] = 0xBC;
                  Serial3.write(TXvals,5);                  
                  break;
                  
                //slow                   
                case 2:
         TXvals[0] = 0;
         TXvals[1] = 0x6a;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);
         
                  agcspeed = agcspeed ^= 1 << 1;    
                  TXvals[0] = 0;
                  TXvals[1] = 0x6a;
                  TXvals[2] = agcspeed;
                  TXvals[3] = Byte1[1];     
                  TXvals[4] = 0xBC;
                  Serial3.write(TXvals,5);
                  break;  
              }          
          }
          return;
       }
        

      // atc
       if(((xtch > 330) & (xtch <410)) & ((ytch > 161) & (ytch < 192))) 
       {
         TXvals[0] = 0;
         TXvals[1] = 0xA9;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2); 
       
         atc = atc ^= 1 << 0;            
         TXvals[0] = 0;
         TXvals[1] = 0xA9;
         TXvals[2] = atc;
         TXvals[3] = Byte1[1];     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5);
         return;
       }


       //ipo                       
       if(((xtch > 411) & (xtch <480)) & ((ytch > 161) & (ytch < 192)))
       {
         getChanMem();
         addr1 = addr2;           
         addr1 += 2; 
         msb = addr1 >> 8;
         lsb = addr1 & 0x00FF;
         TXvals[0] = msb;
         TXvals[1] = lsb;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2); 
         ipo = Byte1[0];

         //ipo is bit 5
         if(bitRead(ipo, 4)) 
         {
          ipo = ipo ^= 1 << 4;
          att2 = 0;
         }
         
         ipo = ipo ^= 1 << 5; 
         if(bitRead(ipo, 5)) 
         {
          ipo2 = 1;          
         }
         else
         {
          ipo2 = 0;
         }
         TXvals[0] = msb;
         TXvals[1] = lsb;
         TXvals[2] = ipo;
         TXvals[3] = Byte1[1];     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5);
         return;          
       }

       //att
       if(((xtch > 411) & (xtch <480)) & ((ytch > 193) & (ytch < 224)))
       {
         getChanMem();
         addr1 = addr2;           
         addr1 += 2; 
         msb = addr1 >> 8;
         lsb = addr1 & 0x00FF;
         TXvals[0] = msb;
         TXvals[1] = lsb;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2); 
         att = Byte1[0];

         //att is bit 4
         if(bitRead(att, 5))
         {
          att = att ^= 1 << 5;
          ipo2 = 0;
         }
         att = att ^= 1 << 4;
         if(bitRead(att, 4))
         {
          att2 = 1;             
         }
         else
         {
          att2 = 0;
         }         
         TXvals[0] = msb;
         TXvals[1] = lsb;
         TXvals[2] = att;
         TXvals[3] = Byte1[1];     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5);
         return;            
       }      


       //spl
       if(((xtch > 330) & (xtch <410)) & ((ytch > 193) & (ytch < 224))) 
       {
         TXvals[0] = 0;
         TXvals[1] = 0x8d;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);
         
         qspl = qspl ^= 1 << 7;            
         TXvals[0] = 0;
         TXvals[1] = 0x8d;
         TXvals[2] = qspl;
         TXvals[3] = Byte1[1];     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5);
         return;
       }




       // dbf
       if(((xtch > 330) & (xtch <410)) & ((ytch > 225) & (ytch < 256))) 
       {
         TXvals[0] = 0;
         TXvals[1] = 0xA8;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);
        
        EEval = EEval ^= 1 << 2;
        EEval = EEval ^= 1 << 3;     
         TXvals[0] = 0;
         TXvals[1] = 0xA8;
         TXvals[2] = EEval;
         TXvals[3] = Byte1[1];     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5);
         return; 
       }

       // nb
       if(((xtch > 411) & (xtch <480)) & ((ytch > 225) & (ytch < 256)))
        {
         TXvals[0] = 0;
         TXvals[1] = 0x6a;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);
         
         nb = nb ^= 1 << 5;            
         TXvals[0] = 0;
         TXvals[1] = 0x6a;
         TXvals[2] = nb;
         TXvals[3] = Byte1[1];     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5);
         return; 
        }

        //  DNR  2
       if(((xtch > 330) & (xtch <410)) & ((ytch > 257) & (ytch < 288))) 
       {
         TXvals[0] = 0;
         TXvals[1] = 0xA8;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);
        
         EEval = EEval ^= 1 << 1;     
         TXvals[0] = 0;
         TXvals[1] = 0xA8;
         TXvals[2] = EEval;
         TXvals[3] = Byte1[1];     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5);
         return;       
       }
       
       // DNF  1
       if(((xtch > 411) & (xtch < 480)) & ((ytch > 257) & (ytch < 288))) 
       {
         TXvals[0] = 0;
         TXvals[1] = 0xA8;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);
        
         EEval = EEval ^= 1 << 0;     
         TXvals[0] = 0;
         TXvals[1] = 0xA8;
         TXvals[2] = EEval;
         TXvals[3] = Byte1[1];     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5);
         return;        
       }

        // to right
       if(((xtch > 411) & (xtch < 480)) & ((ytch > 289) & (ytch < 320)))
       {       
               rmenu = 2; 
               tft.fillRect(321,0,159,320,WHITE);
               return;            
       }  
       // to left
       if(((xtch > 330) & (xtch < 410)) & ((ytch > 289) & (ytch < 320)))
       {       
               rmenu = 0;
               tft.fillRect(321,0,159,320,WHITE);
               mylogo();
               return;             
       }      
     }
     
     if(rmenu == 2)
     {
      // vox
       if(((xtch > 330) & (xtch < 410)) & ((ytch > 0) & (ytch < 32))) 
       {
         TXvals[0] = 0;
         TXvals[1] = 0x6b;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);
         
         vox = vox ^= 1 << 7;            
         TXvals[0] = 0;
         TXvals[1] = 0x6b;
         TXvals[2] = vox;
         TXvals[3] = Byte1[1];     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5);
         return;
       }
       // disp
       if(((xtch > 411) & (xtch < 480)) & ((ytch > 0) & (ytch < 32)))
       {       
         TXvals[0] = 0;
         TXvals[1] = 0x6a;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);
          
         disp = Byte1[0] ^= 1 << 2;            
         TXvals[0] = 0;
         TXvals[1] = 0x6a;
         TXvals[2] = disp;
         TXvals[3] = Byte1[1];     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5); 
         return;                  
       }

       
       // kyr
       if(((xtch > 330) & (xtch <410)) & ((ytch > 33) & (ytch < 64))) 
       {
         TXvals[0] = 0;
         TXvals[1] = 0x6b;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);

         kyr = kyr ^= 1 << 4;                    
         TXvals[0] = 0;
         TXvals[1] = 0x6b;
         TXvals[2] = kyr;
         TXvals[3] = Byte1[1];     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5);
         return;
       }

       // bk
       if(((xtch > 411) & (xtch < 480)) & ((ytch > 33) & (ytch < 64))) 
       {
         TXvals[0] = 0;
         TXvals[1] = 0x6b;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2);
         
         bk = bk ^= 1 << 5;            
         TXvals[0] = 0;
         TXvals[1] = 0x6b;
         TXvals[2] = bk;
         TXvals[3] = Byte1[1] ;     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5);
         return;  
       }


       //needle width
       if(((xtch > 330) & (xtch < 410)) & ((ytch > 65) & (ytch < 96))) 
       {
        if(need == 3)
        {
          need = 1;
          return;
        }
        need = 3;
       }

       //mtr needle on/off
       if(((xtch > 411) & (xtch < 480)) & ((ytch > 65) & (ytch < 96))) 
       {
                if(mValue == 0) 
        {
          mValue = 1;
        }
        else 
        {
          mValue = 0;
        }
        return;
       }

     
       //redraw screen
       if(((xtch > 330) & (xtch < 410)) & ((ytch > 96) & (ytch < 128))) 
       {
          drawMeter();
          return;
       }


       // save settings
       if(((xtch > 411) & (xtch < 480)) & ((ytch > 96) & (ytch < 128))) 
       {
          if (SD.exists("myFile")) SD.remove("myFile");
          myData.varA = need;
          myData.varB = mValue;
          myFile = SD.open("myFile",FILE_WRITE);
          myFile.write((const uint8_t *)&myData, sizeof(myData));
          myFile.close();          
          return;
       }

       // M-
       if(((xtch > 330) & (xtch < 410)) & ((ytch > 129) & (ytch < 160))) 
       {
         // memory channel number 
         TXvals[0] = 0x05;
         TXvals[1] = 0x0E;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2); 
         tmp = Byte1[0];
         tmp = tmp - 1;
         TXvals[0] = 0x05;
         TXvals[1] = 0x0E;
         TXvals[2] = tmp;
         TXvals[3] = Byte1[1] ;     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5);
         return;             
       }

       // M+
       if(((xtch > 411) & (xtch < 480)) & ((ytch > 129) & (ytch < 160))) 
       {
         // memory channel number 
         TXvals[0] = 0x05;
         TXvals[1] = 0x0E;
         TXvals[2] = 0;
         TXvals[3] = 0 ;     
         TXvals[4] = 0xBB;
         Serial3.write(TXvals,5);
         Serial3.readBytes(Byte1,2); 
         tmp = Byte1[0];
         tmp = tmp + 1;
         TXvals[0] = 0x05;
         TXvals[1] = 0x0E;
         TXvals[2] = tmp;
         TXvals[3] = Byte1[1] ;     
         TXvals[4] = 0xBC;
         Serial3.write(TXvals,5);
         return;                 
       }


       // PSE
       if(((xtch > 330) & (xtch < 410)) & ((ytch > 161) & (ytch < 192))) 
       {
       tft.fillCircle(290, 110, 15, ORANGE);
        do
        {
          Serial3.end();
          tmp = 1;
          tp = ts.getPoint();
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          pinMode(XP, OUTPUT);
          pinMode(YM, OUTPUT);
          if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) tmp = 0;
          delay(500);
        } while(tmp);      //stay here till screen is touched
        tmp = 1;
        Serial3.begin(9600);
        tft.fillCircle(290, 110, 15, WHITE); 
        return;                   
       }
       
       // PRC
       if(((xtch > 411) & (xtch < 480)) & ((ytch > 161) & (ytch < 192)))
       {
          if(dovfo == 1) roc = banda;
          if(dovfo == 2) roc = bandb;
          if(roc < 9)
          {        
               TXvals[0] = 0;
               TXvals[1] = 0xa9;
               TXvals[2] = 0;
               TXvals[3] = 0 ;     
               TXvals[4] = 0xBB;
               Serial3.write(TXvals,5);
               Serial3.readBytes(Byte1,2);
               
               prc = prc ^= 1 << 1;            
               TXvals[0] = 0;
               TXvals[1] = 0xa9;
               TXvals[2] = prc;
               TXvals[3] = Byte1[1] ;     
               TXvals[4] = 0xBC;
               Serial3.write(TXvals,5);
               return;
          }   
       }

       
       //  reset arduino
       if(((xtch > 411) & (xtch < 480)) & ((ytch > 289) & (ytch < 320))) 
       {
          resetFunc();
          return;
       }             


       //to left
       if(((xtch > 330) & (xtch < 410)) & ((ytch > 289) & (ytch < 320)))
       {       
               rmenu = 1;
               tft.fillRect(321,0,159,320,WHITE);
               return;            
       }
     }
     
     if(rmenu == 0)
     {
       // to right
       if(((xtch > 411) & (xtch < 480)) & ((ytch > 289) & (ytch < 320)))
       {       
               rmenu = 1;
               tft.fillRect(321,0,159,320,WHITE);
               return;             
       }      
     }
   }         
}            

               
void DispL()
{  
     //freq
    if(cllr ==3)
    {
      if(bnd == 1) xfval = 225;
      if(bnd == 2) xfval = 185;
      if((fdis == 0))
      {
        tft.setTextColor(BLACK, WHITE);
        tft.setTextSize(4);
        tft.setCursor(40,xfval);   
        tft.print("          ");       
        fdis = 1;
      }
        tft.setTextColor(BLACK, WHITE);
        tft.setTextSize(3);
        tft.setCursor(10,xfval + 5);   
        tft.print("<");       
        tft.setCursor(80,xfval + 5);
        tft.print(F("STEP "));    
        if(bnd ==1) tft.print(stepss[steps1]);
        if(bnd ==2) tft.print(stepss[steps2]);
        tft.setCursor(295,xfval + 5);          
        tft.print(">");
        tft.setTextColor(BLACK, WHITE);             
    }
    else
    {
         
    putinstring();
          
    if(was3 == 1)
    {
    tft.setTextColor(GREEN, WHITE);
    tft.setCursor(10,185);
    tft.setTextSize(5);
    tft.print(packf);
    fdis = 0;
    tft.setTextColor(BLACK, WHITE);    
    tft.setTextSize(4);
    tft.setCursor(40,225);
    tft.print(packf2);
    was3 = 0;
    }
   
    if(was3 == 2)
    { 
      tft.setTextColor(BLACK, WHITE);
      tft.setTextSize(4);
      tft.setCursor(40,185);
      tft.print(packf2);
      fdis = 0;
      tft.setTextColor(GREEN, WHITE);
      tft.setCursor(10,220);
      tft.setTextSize(5);
      tft.print(packf);
      was3 = 0;           
    }
  }     
    tft.setTextColor(BLACK, WHITE);     

    //RX-TX
    tft.setTextSize(2);
    tft.setCursor(15,5);
    tft.print(F("RX"));
    tft.setCursor(10,25);      
    tft.print(gaugerx);

    tft.setTextColor(BLACK, WHITE);    
    tft.setCursor(285,5);
    tft.print(F("TX"));
    tft.setCursor(280,25);         
    tft.print(gaugetx);

    tft.setTextColor(BLUE, WHITE);
    tft.setCursor(5,135);
    tft.print(F("A/B"));
    tft.setCursor(260,135);
    tft.print(F("INFO"));
    tft.setTextColor(BLACK, WHITE);


    
    //vfo band
    readBank();
    tft.setCursor(10,160);
    if(hme == 1) 
    {
        tft.print("Home");
        dovfo = 3;
    }
    else
    {
        if(bnd == 1) 
        {
          tft.print(bankstr[banda]);
        dovfo = 1;          
        }
        if(bnd == 2) 
        {
          tft.print(bankstr[bandb]);
        dovfo = 2;          
        }
        if(bnd == 3) 
        {
          tft.print("Mem ");
        dovfo = 4;                
        }
    }

    
    //voltage
    tft.setCursor(255,160);
    if(volts/10 >= 12) tft.setTextColor(GREEN,WHITE);
    if(volts/10 < 12) tft.setTextColor(RED,WHITE);     
    tft.print(dtostrf((volts/10),4,1,namebuf));
    tft.print("v");   
    tft.setTextColor(BLACK,WHITE);
    
    tft.setTextSize(3);
    // Frequency up down arrows
    tft.setCursor(10,265);
    if((cllr == 1) || (cllr == 2)) tft.setTextColor(ORANGE,WHITE);     
    tft.print("<");
    tft.setCursor(295,265);      
    tft.print(">");
    tft.setTextColor(BLACK,WHITE);   

    //vfo mode power qspl												  
    tft.setCursor(60,265);  
    tft.print(vfo);
    tft.setTextColor(BLACK,WHITE);
    
    modeset(modenum);
    if(cllr == 2) tft.setTextColor(ORANGE, WHITE);
    tft.setCursor(135,265);   
    tft.print(mode);
    tft.setTextColor(BLACK,WHITE);
    
    if(cllr == 1) tft.setTextColor(ORANGE, WHITE);
    tft.setCursor(210,265);  
    tft.print(powr);  
    if(powr < 100) tft.print(F("W  "));
    if(powr > 99) tft.print(F("W "));    
    tft.setTextColor(BLACK,WHITE); 
       
    //fmenu  
    tft.setTextSize(3);   
    tft.setTextColor(BLACK,WHITE); 
    tft.setCursor(10,295);           
    tft.print("<");   
    tft.setTextSize(2);   
    tft.setTextColor(BLUE,WHITE); 
    tft.setCursor(55,298);           
    tft.print(fmen);   
    tft.setTextSize(3);   
    tft.setTextColor(BLACK,WHITE); 
    tft.setCursor(295,295);           
    tft.print(">");


}

void mylogo()
{
    showBMP("logo.bmp", 340,75);
}

void DispR0()
{   
    tft.setCursor(410,294);
    tft.print(F("-->")); 
    tft.setTextColor(BLACK,WHITE);  
}
     
void DispR1()
{
    //Lock
    tft.setTextSize(3);
    tft.setCursor(330,5); 
    if((lock & 64) == 0)
    {
      tft.setTextColor(RED,WHITE);
      tft.print(F("LCK>"));
      tft.setTextColor(BLACK,WHITE);
      tft.setCursor(410,5);
      if(lock2 == 0) 
      {
        tft.setTextColor(ORANGE,WHITE);
        tft.print(F("DIL"));
      }
      if(lock2 == 1) 
      {
        tft.setTextColor(MAGENTA,WHITE);
        tft.print(F("FRQ"));        
      }
      if(lock2 == 2) 
      {
        tft.setTextColor(BLUE,WHITE);
        tft.print(F("PNL"));
      }
      if(lock2 == 3) 
      {
        tft.setTextColor(GREEN,WHITE);
        tft.print(F("ALL"));
      }
    }
    if((lock & 64) == 64) 
    {
      tft.setTextColor(BLACK,WHITE);
      tft.print(F("LCK>"));
      tft.setCursor(410,5);
      tft.print(F("OFF"));;      
    }
    tft.setTextColor(BLACK,WHITE);
    
    //dial speed
    tft.setCursor(330,38); 
    if((man & 128) == 128)
    {
      tft.setTextColor(BLUE,WHITE);     
      tft.print(F("SLW"));       
    }
    else
    {
      tft.setTextColor(GREEN,WHITE);     
      tft.print(F("FST"));      
    }
    tft.setTextColor(BLACK,WHITE);    

    //ptt swr
    tft.setCursor(410,38);                                  
    tft.print(F("PTT"));
    //tft.setCursor(410,38);
    //tft.print(F("SWR"));

    if(ptt < 128)   
    {
          tft.setCursor(410,38);                               
          tft.setTextColor(RED,WHITE);    
          tft.print(F("PTT"));
      
      if(swr == 64)   
      {
           tft.setTextColor(RED,WHITE);
           tft.setCursor(410,38);
           tft.print(F("SWR"));
      }
    }
    tft.setTextColor(BLACK,WHITE);     

    tft.setCursor(330,70);
    if(sql == 0) tft.setTextColor(BLACK,WHITE);
    if(sql == 1) tft.setTextColor(RED,WHITE);
    tft.print(F("SQL")); 
    tft.setTextColor(BLACK,WHITE);
    
    tft.setCursor(410,70);
    if(GnSq == F("SQ")) tft.setTextColor(BLUE,WHITE);
    if(GnSq == F("GN")) tft.setTextColor(ORANGE,WHITE);    
    tft.print(GnSq);
    tft.setTextColor(BLACK,WHITE); 


    tft.setCursor(410,102);
    tft.print(F("DW")); 
    tft.setCursor(330,102); 
    tft.print(F("PRI")); 
    if((dw & 0x10) == 0x10)
    {
      
      tft.setTextColor(GREEN,WHITE);
      if((dw & 0x18) == 0x18) 
      {
            tft.setCursor(410,102);
            tft.print(F("DW"));       
      }
      else
      {
            tft.setCursor(330,102); 
            tft.print(F("PRI"));        
      }
      
    }

    
    //tft.setTextSize(2);
    
    tft.setCursor(330,134);
    if((EEval & 0x20) == 0x20)
    {
      tft.setTextColor(GREEN,WHITE);
      tft.print(F("AGC>"));
      tft.setCursor(410,134);
      if((agcspeed & 3) == 0)
      {
      tft.setTextColor(MAGENTA,WHITE);        
      tft.print(F("AUT"));        
      }
       if((agcspeed & 3) == 1)
      {
      tft.setTextColor(ORANGE,WHITE);        
      tft.print(F("FST"));        
      }
            if((agcspeed & 3) == 2)
      {
      tft.setTextColor(BLUE,WHITE);        
      tft.print(F("SLW"));        
      }           
    }
    else
    {
      tft.setTextColor(BLACK,WHITE);
      tft.print(F("AGC>"));
      tft.setCursor(410,134);
      tft.print(F("OFF"));       
    }

    tft.setTextColor(BLACK,WHITE);


    tft.setTextSize(3);    

    tft.setCursor(330,166);
    if((atc & 0x01) == 0x01) tft.setTextColor(GREEN,WHITE);    
    tft.print(F("ATC"));
    tft.setTextColor(BLACK,WHITE); 

    tft.setCursor(330,198);
    if((qspl & 0x80) == 0x80) tft.setTextColor(GREEN,WHITE); 
    tft.print(F("SPL"));
    tft.setTextColor(BLACK,WHITE); 

    tft.setCursor(410,166);
    if(ipo2 == 1) tft.setTextColor(GREEN,WHITE); 
    tft.print(F("IPO"));
    tft.setTextColor(BLACK,WHITE); 

    tft.setCursor(410,198);
    if(att2 == 1)   tft.setTextColor(GREEN,WHITE);         
    tft.print(F("ATT"));
    tft.setTextColor(BLACK,WHITE);     

    tft.setCursor(330,230);
    if((EEval & 12) == 12)  tft.setTextColor(GREEN,WHITE);
    tft.print(F("DBF"));
    tft.setTextColor(BLACK,WHITE); 

    tft.setCursor(410,230);
    if((nb & 32) == 32)   tft.setTextColor(GREEN,WHITE);         
    tft.print(F("NB"));
    tft.setTextColor(BLACK,WHITE);

    //DSP Stuff 
    tft.setCursor(330,262);
    if((EEval & 2) == 2) tft.setTextColor(GREEN,WHITE); 
    tft.print(F("DNR"));
    tft.setTextColor(BLACK,WHITE); 
    
    tft.setCursor(410,262);
    if((EEval & 1) == 1)  tft.setTextColor(GREEN,WHITE);
    tft.print(F("DNF")); 
    tft.setTextColor(BLACK,WHITE); 

    tft.setCursor(330,294);
    tft.print(F("<--")); 
    tft.setTextColor(BLACK,WHITE); 

    tft.setCursor(410,294);
    tft.print(F("-->")); 
    tft.setTextColor(BLACK,WHITE); 
}

void DispR2()
{
    tft.setCursor(330,5);
    if((vox & 128) == 128) tft.setTextColor(RED,WHITE); 
    tft.print(F("VOX"));
    tft.setTextColor(BLACK,WHITE);
    
    tft.setCursor(410,5);
    if((disp & 4) == 4)   tft.setTextColor(BLUE,WHITE);
    tft.print(F("DIS")); 
    tft.setTextColor(BLACK,WHITE); 

    tft.setCursor(330,38);
    if((kyr & 16) == 16)  tft.setTextColor(GREEN,WHITE);
    tft.print(F("KYR"));
    tft.setTextColor(BLACK,WHITE); 

    tft.setCursor(410,38);
    if((bk & 32) == 32)  tft.setTextColor(GREEN,WHITE);
    tft.print(F("BK")); 
    tft.setTextColor(BLACK,WHITE); 

    tft.setCursor(330,70);
    tft.print("ND");
    tft.print(need);

    tft.setCursor(410,70);
    tft.print("MTR");

    tft.setCursor(330,102);
    tft.print("SCR");

    tft.setCursor(410,102);
    tft.print("SDT");

    tft.setCursor(330,134);
    tft.print("M-");

    tft.setCursor(410,134);
    tft.print("M+");

    tft.setCursor(330,166);
    tft.print("PSE"); 

    tft.setCursor(410,166);
    if((atc & 0x02) == 0x02) tft.setTextColor(GREEN,WHITE); 
    tft.print("PRC"); 
    tft.setTextColor(BLACK,WHITE);      

    tft.setCursor(410,294);
    tft.print("RST"); 

           
    

    tft.setCursor(330,294);
    if((disp & 4) == 4)   tft.setTextColor(BLUE,WHITE);
    tft.print(F("<--")); 
    tft.setTextColor(BLACK,WHITE);
}


void putinstring()
{
    freq = 0;
    MHz = 0;
    str1 = Freqs[0];
    v1 = Freqs[0];
    str2 = Freqs[1]; 
    v2 = Freqs[1];      
    str3 = Freqs[2];
    v3 = Freqs[2];    
    str4 = Freqs[3]; 
    v4 = Freqs[3]; 
    freq = freq + v4;
    freq = freq + (v3 * 100);
    freq = freq + (v2 * 10000);
    //freq = freq * v1 * 1000000;

    if(v4<10)
    {
      bum(9,v4);
      bum(8,0);
    }
    else
    {
      bum(8,int(v4/10));
      tmp=int(v4/10);
      a1=tmp*10;
      tmp=v4-a1;
      bum(9,tmp);
    }
    if(v3<10)
    {
      bum(6,v3);
      bum(5,0);
    }
    else
    {
      bum(5,int(v3/10));
      tmp=int(v3/10);
      a1=tmp*10;
      tmp=v3-a1;
      bum(6,tmp);
    }
    
    if(v2<10)
    {
      bum(4,v2);
      bum(2,0);
      MHz=0;
    }
    else
    {
      bum(2,int(v2/10));
      MHz = MHz + int(v2/10);
      tmp=int(v2/10);
      a1=tmp*10;
      tmp=v2-a1;
      bum(4,tmp);

    }
    if(v1<10)
    {
      bum(1,v1);
      MHz = MHz + v1 * 10;
      if(v1==0)
      {
        packf[1]= ' ';     
      }
        packf[0]= ' ';
    }
    else
    {
      bum(0,int(v1/10));
      MHz = MHz + int(v1/10) * 100;
      tmp=int(v1/10);
      a1=tmp*10;
      tmp=v1-a1;
      bum(1,tmp);
      MHz = MHz + tmp * 10;
    } 
    freq2 = (char*)packf;
    tmp = freq2.length();   
    freq2.remove(tmp - 3, 1);
    freq2.remove(tmp - 7, 1); 
    freq =  freq2.toFloat();         
}


void putfreqinTX()
{ 
      TXint[8] = freq % 10;
      TXint[7] = (freq / 10) % 10;
      TXint[6] = (freq / 100) % 10;
      TXint[5] = (freq / 1000) % 10;
      TXint[4] = (freq / 10000) % 10;
      TXint[3] = (freq / 100000) % 10;
      TXint[2] = (freq / 1000000) % 10;
      TXint[1] = (freq / 10000000) % 10;

      TXvals[4] = 1;
      TXvals[3] = dec2bcd(TXint[8] + (TXint[7] * 10));
      TXvals[2] = dec2bcd(TXint[6] + (TXint[5] * 10));
      TXvals[1] = dec2bcd(TXint[4] + (TXint[3] * 10));
      TXvals[0] = dec2bcd(TXint[2] + (TXint[1] * 10));  
}

uint8_t dec2bcd(uint8_t n)
{
  uint16_t a = n;
  byte b = (a * 103) >> 10;
  return n + b * 6;
}


void MFa(int num)
{
          fnum = num;

          TXvals[0] = 0;
          TXvals[1] = 0x8E;
          TXvals[2] = 0;
          TXvals[3] = 0 ;     
          TXvals[4] = 0xBB;
          Serial3.write(TXvals,5);
          Serial3.readBytes(Byte1,2);
          
        switch (num) 
        {
          case 0: fmen = F("MFa A/B  A=B  SPL "); break;
          case 1: fmen = F("MFb MW   SKIP TAG "); break;          
          case 2: fmen = F("MFc STO  RCL  PROC"); break;
          case 3: fmen = F("MFd RPT  REV  VOX "); break;
          case 4:
          
          if((Byte1[0] & 2) == 2)
          {
           fmen = F("MFe ENC  DEC  TDCH");
          }
          else
          {
           fmen = F("MFe TON       TDCH");                      
        }
          break;
          case 5: fmen = F("MFf ARTS SRCH PMS "); break;
          case 6: fmen = F("MFg SCN  PRI  DW  "); break;
          case 7: fmen = F("MFh SCOP WID  STEP"); break;          
          case 8: fmen = F("MFi MTR  PWR  DISP"); break;
          case 9: fmen = F("MFj SPOT BK   KVR "); break;
          case 10: fmen = F("MFk TUNE DOWN UP  "); break;          
          case 11: fmen = F("MFl NB   AGC  AUTO"); break;
          case 12: fmen = F("MFm IPO  ATT  NAR "); break;
          case 13: fmen = F("MFn CFIL N/A  N/A "); break;          
          case 14: fmen = F("MFo PLY1 PLY2 PLY3"); break;
          case 15: fmen = F("MFp DNR  DNF  DBF "); break;          
          case 16: fmen = F("MFq MONI QSPL ATC "); break;
        }
}


void RXGauge( int num)
{
      switch (num) 
      {
          case 96: gaugerx = F("SIG"); break;
          case 97: gaugerx = F("CTR"); break;
          case 98: gaugerx = F("VLT"); break;
          case 99: gaugerx = F("N/A"); break;
          case 100: gaugerx = F("FS "); break;
          case 101: gaugerx = F("OFF"); break;
          gaugerx = F("SIG");
      } 
}


void TXGauge( int num)
{
       switch (num) 
       {
          case 4: gaugetx = F("PWR"); break;
          case 36: gaugetx = F("ALC"); break;
          case 68: gaugetx = F("MOD"); break;
          case 100: gaugetx = F("SWR"); break;
          case 132: gaugetx = F("VLT"); break;
          case 164: gaugetx = F("N/A"); break;
          case 196: gaugetx = F("OFF"); break;
          gaugetx = "PWR";         
      } 
}


void vfoab()
{
   vfot = bnd;
   Serial3.write(VData,5);  //vfo
   Serial3.readBytes(Byte1,2);
   tmp = Byte1[0];
 
  if((tmp & 16) == 16) hme = 1;
  if((tmp & 16) == 0) hme = 0;
 
  if((tmp & 1) == 0)
  {
        vfo=F("VFA");
        bnd = 1;
        was3 = 1;
  }
     
  if((tmp & 1) == 1)
  { 
        vfo=F("VFB");
        bnd = 2;
        was3 = 2;
  }

  ty = was3;

  if((tmp & 64) == 64)
  {
        vfo=F("MEM");
        bnd = 3;          
  }
      
  if(hme == 1) vfo=F("HME");

  if((vfot != bnd) && ((bnd != 3) && (vfot != 3)))
  {     
            for (int i = 0; i < 10; i++) 
            {
                packf2[i] = packf[i];
            }                   
            clearreadout();       
  }
}

void clearreadout()
{
                tft.setTextColor(BLACK, WHITE);
                tft.setCursor(10,185);
                tft.setTextSize(5);
                tft.print("          ");
                tft.setCursor(10,220);
                tft.setTextSize(5);
                tft.print("          ");   
}


void GetPW()
{  
                if(MHz > 0 && MHz <33)
                {                      
                Serial3.write(MH,5);  //hf power level
                          Serial3.readBytes(Byte1,6);
                          tmp = Byte1[0];
                          twomp(tmp); 
                          tmp=0;
                          band = 0x9B;
                }    
                
                 if(MHz >= 33 && MHz <56)
                {                   
                Serial3.write(M6,5);  //6M power level
                          Serial3.readBytes(Byte1,6);
                          tmp = Byte1[0];
                          twomp(tmp); 
                          tmp=0;
                          band = 0xAA;
                }
                
                if(MHz >= 56 && MHz <164)
                {                         
                Serial3.write(M2,5);  //2M power level
                          Serial3.readBytes(Byte1,6);
                          tmp = Byte1[0];
                          twomp(tmp); 
                          tmp=0;
                          band = 0xAB;
                }
                
                if(MHz >= 164 && MHz <470)
                {                                                   
                Serial3.write(M7,5);  //70cm power level
                          Serial3.readBytes(Byte1,6);
                          tmp = Byte1[0];
                          twomp(tmp); 
                          tmp=0;
                          band = 0xAC;
                }          
}

void twomp(int num)
{
  if(num > 19)
  {
    num = num - 128;
  }
  if((num > 0) && (num < 101)) powr=num;
}


void bum(int pos,int num)
{
      switch (num) 
      {
          case 0: packf[pos]= '0'; break;
          case 1: packf[pos]= '1'; break;
          case 2: packf[pos]= '2'; break;
          case 3: packf[pos]= '3'; break;
          case 4: packf[pos]= '4'; break;
          case 5: packf[pos]= '5'; break;
          case 6: packf[pos]= '6'; break;         
          case 7: packf[pos]= '7'; break;
          case 8: packf[pos]= '8'; break;         
          case 9: packf[pos]= '9'; break;
      }
}


void Convert(int num, int by)
{
    a1=int(num/by);
    a2=a1*by;
    tmp=num-a2;
    a2=a1*10;
    a1=a2+tmp;
}


void modeset(int num)
{
      switch (num) 
      {
          case 0: mode=F("LSB"); break;
          case 1: mode=F("USB"); break;
          case 2: mode=F("CW "); break;
          case 3: mode=F("CWR"); break;
          case 4: mode=F("AM "); break;
          case 6: mode=F("WFM "); break;        
          case 8: mode=F("FM "); break;
          case 0x0A: mode=F("DIG"); break;
          case 0x0C: mode=F("PKT"); break; 
          case 0x82: mode=F("CWN"); break;        
          case 0x88: mode=F("FMN"); break;
          
          //mode="---";
      }
}


// #########################################################################
// Update needle position
// This function is blocking while needle moves, time depends on ms_delay
// 10ms minimises needle flicker if text is drawn within needle sweep area
// Smaller values OK if text not in sweep area, zero for instant movement but
// does not look realistic... (note: 100 increments for full scale deflection)
// #########################################################################
void plotNeedle(int value, byte ms_delay)
{
  //int num = 0;
  //num = map(value,0,14,0,100);
  //value = num;
  tft.setTextColor(BLACK, WHITE);
  char buf[8]; dtostrf(value/3.333, 4, 0, buf);
  //tft.drawRightString(buf, M_SIZE*40, M_SIZE*(119 - 20)+Y_OFFSET, 2);

  if (value < -10) value = -10; // Limit value to emulate needle end stops
  if (value > 110) value = 110;

  // Move the needle until new value reached
  while (!(value == old_analog)) {
    if (old_analog < value) old_analog++;
    else old_analog--;

    if (ms_delay == 0) old_analog = value; // Update immediately if delay is 0

    float sdeg = map(old_analog, -10, 110, -150, -30); // Map value to angle
    // Calculate tip of needle coords
    float sx = cos(sdeg * 0.0174532925);
    float sy = sin(sdeg * 0.0174532925);
    // Calculate x delta of needle start (does not start at pivot point)
    float tx = tan((sdeg + 90) * 0.0174532925);


    if(nump == 0)
    {
    // Draw the needle in the new postion and save pixel colours
      lineup(M_SIZE*(120 + 20 * ltx), M_SIZE*(150 - 20)+Y_OFFSET, osx, osy+Y_OFFSET);
      nump = 1;
    }
    else
    {
        // Erase old needle image and replace pixels
        line(M_SIZE*(120 + 20 * ltx), M_SIZE*(150 - 20)+Y_OFFSET, osx, osy+Y_OFFSET);
        // Store new needle end coords for next erase
        ltx = tx;
        osx = M_SIZE*(sx * 108 + 120); 
        osy = M_SIZE*(sy * 108 + 150);
        // Draw the needle in the new postion and save pixel colours
        lineup(M_SIZE*(120 + 20 * ltx), M_SIZE*(150 - 20)+Y_OFFSET, osx, osy+Y_OFFSET);
    }

    // Slow needle down slightly as it approaches new postion
    //if (abs(old_analog - value) < 10) ms_delay += ms_delay / 5;
    //delay(ms_delay);
  }
}

//Redraw background pixels from saved colour array that were under the needle
void line(int x0, int y0, int x1, int y1) 
{
  int count = 0;
  int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
  int err = (dx>dy ? dx : -dy)/2, e2;
 
  for(;;){
   if(need == 3)
   {
        tft.drawPixel(x0-1,y0, myline[0][count]);    
        tft.drawPixel(x0,y0, myline[1][count]);    
        tft.drawPixel(x0+1,y0, myline[2][count]);
   }
   else
   {
        tft.drawPixel(x0,y0, myline[1][count]);      
   }
       
    if (x0==x1 && y0==y1) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0 += sx; }
    if (e2 < dy) { err += dx; y0 += sy; }
    count ++;
  }
}

//Read pixel colours along new needle position, and then draw needle.
void lineup(int x0, int y0, int x1, int y1)
{
  int count = 0;
  int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
  int err = (dx>dy ? dx : -dy)/2, e2;
 
  for(;;){
    if(need == 3)
    {
        myline[0][count]=tft.readPixel(x0-1,y0);     
        tft.drawPixel(x0-1,y0, BLACK);
           
        myline[1][count]=tft.readPixel(x0,y0);     
        tft.drawPixel(x0,y0, BLACK);
        
        myline[2][count]=tft.readPixel(x0+1,y0);     
        tft.drawPixel(x0+1,y0, BLACK);
    }
    else
    {
        myline[1][count]=tft.readPixel(x0,y0);     
        tft.drawPixel(x0,y0, BLACK);      
    }
       
    if (x0==x1 && y0==y1) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0 += sx; }
    if (e2 < dy) { err += dx; y0 += sy; }
    count ++;
  }
}


#define BMPIMAGEOFFSET 54
#define PALETTEDEPTH   4
#define BUFFPIXEL 20

uint16_t read16(File& f) {
    uint16_t result;         // read little-endian
    result = f.read();       // LSB
    result |= f.read() << 8; // MSB
    return result;
}

uint32_t read32(File& f) {
    uint32_t result;
    result = f.read(); // LSB
    result |= f.read() << 8;
    result |= f.read() << 16;
    result |= f.read() << 24; // MSB
    return result;
}

uint8_t showBMP(char *nm, int x, int y)
{
    File bmpFile;
    int bmpWidth, bmpHeight;    
    uint8_t bmpDepth;           
    uint32_t bmpImageoffset;   
    uint32_t rowSize;           
    uint8_t sdbuffer[3 * BUFFPIXEL];    
    uint16_t lcdbuffer[(1 << PALETTEDEPTH) + BUFFPIXEL], *palette = NULL;
    uint8_t bitmask, bitshift;
    boolean flip = true;        
    int w, h, row, col, lcdbufsiz = (1 << PALETTEDEPTH) + BUFFPIXEL, buffidx;
    uint32_t pos;               
    boolean is565 = false;     
    uint16_t bmpID;
    uint16_t n;                 
    uint8_t ret;

    if ((x >= tft.width()) || (y >= tft.height()))
        return 1;               

    bmpFile = SD.open(nm);
    bmpID = read16(bmpFile);   
    (void) read32(bmpFile);    
    (void) read32(bmpFile);    
    bmpImageoffset = read32(bmpFile);      
    (void) read32(bmpFile);     
    bmpWidth = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    n = read16(bmpFile);        
    bmpDepth = read16(bmpFile); 
    pos = read32(bmpFile);      
    if (bmpID != 0x4D42) ret = 2; 
    else if (n != 1) ret = 3;   
    else if (pos != 0 && pos != 3) ret = 4; 
    else {
        bool first = true;
        is565 = (pos == 3);             
        rowSize = (bmpWidth * bmpDepth / 8 + 3) & ~3;
        if (bmpHeight < 0) {             
            bmpHeight = -bmpHeight;
            flip = false;
        }

        w = bmpWidth;
        h = bmpHeight;
        if ((x + w) >= tft.width())      
            w = tft.width() - x;
        if ((y + h) >= tft.height())     
            h = tft.height() - y;

        if (bmpDepth <= PALETTEDEPTH) {   
            bmpFile.seek(BMPIMAGEOFFSET); 
            bitmask = 0xFF;
            if (bmpDepth < 8)
                bitmask >>= bmpDepth;
            bitshift = 8 - bmpDepth;
            n = 1 << bmpDepth;
            lcdbufsiz -= n;
            palette = lcdbuffer + lcdbufsiz;
            for (col = 0; col < n; col++) {
                pos = read32(bmpFile);    
                palette[col] = ((pos & 0x0000F8) >> 3) | ((pos & 0x00FC00) >> 5) | ((pos & 0xF80000) >> 8);
            }
        }

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x + w - 1, y + h - 1);
        for (row = 0; row < h; row++) { 
            uint8_t r, g, b, *sdptr;
            int lcdidx, lcdleft;
            if (flip)   
                pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
            else       
                pos = bmpImageoffset + row * rowSize;
            if (bmpFile.position() != pos) { 
                bmpFile.seek(pos);
                buffidx = sizeof(sdbuffer); 
            }

            for (col = 0; col < w; ) {  
                lcdleft = w - col;
                if (lcdleft > lcdbufsiz) lcdleft = lcdbufsiz;
                for (lcdidx = 0; lcdidx < lcdleft; lcdidx++) { // buffer at a time
                    uint16_t color;
                    if (buffidx >= sizeof(sdbuffer)) { // Indeed
                        bmpFile.read(sdbuffer, sizeof(sdbuffer));
                        buffidx = 0; // Set index to beginning
                        r = 0;
                    }
                    switch (bmpDepth) {        
                        case 24:
                            b = sdbuffer[buffidx++];
                            g = sdbuffer[buffidx++];
                            r = sdbuffer[buffidx++];
                            color = tft.color565(r, g, b);
                            break;
                        case 16:
                            b = sdbuffer[buffidx++];
                            r = sdbuffer[buffidx++];
                            if (is565)
                                color = (r << 8) | (b);
                            else
                                color = (r << 9) | ((b & 0xE0) << 1) | (b & 0x1F);
                            break;
                        case 1:
                        case 4:
                        case 8:
                            if (r == 0)
                                b = sdbuffer[buffidx++], r = 8;
                            color = palette[(b >> bitshift) & bitmask];
                            r -= bmpDepth;
                            b <<= bmpDepth;
                            break;
                    }
                    lcdbuffer[lcdidx] = color;
                }
                tft.pushColors(lcdbuffer, lcdidx, first);
                first = false;
                col += lcdidx;
            }           // end cols
        }               // end rows
        tft.setAddrWindow(0, 0, tft.width() - 1, tft.height() - 1); //restore full screen
        ret = 0;        // good render
    }
    bmpFile.close();
    return (ret);
}
