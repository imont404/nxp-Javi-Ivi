//----------------------------------------------------------------------
//EASTRISING TECHNOLOGY CO,.LTD.//
// Module    : ER-TFT020-7  2.0 INCH  IPS TFT LCD  240RGB*320 DOTS
// Lanuage   : C51 Code
// Create    : JAVEN LIU
// Date      : 2025-08-13
// Drive IC  : LCD:ST7789P  CTP:CSD816D   
// INTERFACE : 8080-8BIT    SD CARD: SPI	CTP:I2C
// MCU 		 : STC12LE5C60S2
//----------------------------------------------------------------------

#include <STC12C5A.H>
#include <stdio.h>
#include <intrins.h>
#include<7789.h>
#include<tp.h>
//=============================================================
//define SD card IO
sbit SD_CLK = P1^7;
sbit SD_DI  = P1^5;
sbit SD_DO  = P1^6;
sbit SD_CS  = P1^4;
//===========================================================

//===========================================================
//Buffer
unsigned char xdata DATA[512];

//===========================================================
//write one byte to SD
void SdWrite(unsigned char n)
{

unsigned char i;

for(i=8;i;i--)
{
SD_CLK=0;
SD_DI=(n&0x80);
n<<=1;
SD_CLK=1;
}
SD_DI=1; 
} 
//===========================================================
//read one byte
unsigned char SdRead()
{
unsigned char n,i;
for(i=8;i;i--)
{
SD_CLK=0;
SD_CLK=1;
n<<=1;
if(SD_DO) n|=1;

}
return n;
}
//============================================================
//Detecting the response of the SD card
unsigned char SdResponse()
{
unsigned char i=0,response;

while(i<=8)
{
response = SdRead();
if(response==0x00)
break;
if(response==0x01)
break;
i++;
}
return response;
} 
//================================================================
//Send commands to the SD card
void SdCommand(unsigned char command, unsigned long argument, unsigned char CRC)
{

SdWrite(command|0x40);
SdWrite(((unsigned char *)&argument)[0]);
SdWrite(((unsigned char *)&argument)[1]);
SdWrite(((unsigned char *)&argument)[2]);
SdWrite(((unsigned char *)&argument)[3]);
SdWrite(CRC);
}
//================================================================
//Initialize the SD card
unsigned char SdInit(void)
{
int delay=0, trials=0;
unsigned char i;
unsigned char response=0x01;

SD_CS=1;
for(i=0;i<=9;i++)
SdWrite(0xff);
SD_CS=0;

//Send Command 0 to put MMC in SPI mode
SdCommand(0x00,0,0x95);


response=SdResponse();

if(response!=0x01)
{
return 0;
} 

while(response==0x01)
{
SD_CS=1;
SdWrite(0xff);
SD_CS=0;
SdCommand(0x01,0x00ffc000,0xff);
response=SdResponse();
} 

SD_CS=1;
SdWrite(0xff);
return 1; 
}
//================================================================
//Specify the address to write  the SD card data, a maximum of 512 bytes
unsigned char SdWriteBlock(unsigned char *Block, unsigned long address,int len)
{
unsigned int count;
unsigned char dataResp;
//Block size is 512 bytes exactly
//First Lower SS

SD_CS=0;
//Then send write command
SdCommand(0x18,address,0xff);

if(SdResponse()==00)
{
SdWrite(0xff);
SdWrite(0xff);
SdWrite(0xff);
//command was a success - now send data
//start with DATA TOKEN = 0xFE
SdWrite(0xfe);
//now send data
for(count=0;count<len;count++) SdWrite(*Block++);

for(;count<512;count++) SdWrite(0);
//data block sent - now send checksum
SdWrite(0xff); 
SdWrite(0xff);
//Now read in the DATA RESPONSE token
dataResp=SdRead();
//Following the DATA RESPONSE token
//are a number of BUSY bytes
//a zero byte indicates the MMC is busy

while(SdRead()==0);

dataResp=dataResp&0x0f; //mask the high byte of the DATA RESPONSE token
SD_CS=1;
SdWrite(0xff);
if(dataResp==0x0b)
{
//printf("DATA WAS NOT ACCEPTED BY CARD -- CRC ERROR\n");
return 0;
}
if(dataResp==0x05)
return 1;

//printf("Invalid data Response token.\n");
return 0;
}
//printf("Command 0x18 (Write) was not received by the MMC.\n");
return 0;
}

//=======================================================================
//Specified address from the SD card to read data, a maximum of 512 bytes
unsigned char SdReadBlock(unsigned char *Block, unsigned long address,int len)
{
unsigned int count;
//Block size is 512 bytes exactly
//First Lower SS

 //printf("MMC_read_block\n");

SD_CS=0;
//Then send write command
SdCommand(0x11,address,0xff);

if(SdResponse()==00)
{
//command was a success - now send data
//start with DATA TOKEN = 0xFE
while(SdRead()!=0xfe);

for(count=0;count<len;count++) *Block++=SdRead(); 

for(;count<512;count++) SdRead();

//data block sent - now send checksum
SdRead();
SdRead();
//Now read in the DATA RESPONSE token
SD_CS=1;
SdRead();
return 1;
}
 //printf("Command 0x11 (Read) was not received by the MMC.\n");
return 0;
}

//============================================================	
main()
{
 unsigned char  xa=0; 
 unsigned long j;  
  unsigned int f,i,ya=0;
  unsigned long AddTemp;////SD card physical address of the first data address initial value
 
  	P0M1=0x00; //设置P0口为推挽输出
	P0M0=0xff;

   	P1M1=0x00; //设置P1口为推挽输出
	P1M0=0xff;
 
 	P1=0xff;
 	P2=0xff;
 	P3=0xff;
 	P0=0xff;

	ST7789_Initial();//LCD initialization

while(1)
{ 
		 TPTEST();	

	ClearScreen(0x00);	//all off	  
    showzifustr(60,60,"HELLOW!EASTRISING",0x001f,0x00);	
    showzifustr(80,100,"ER-TFT020-7",0xf800,0x00);	
 	 while(KEY);

 
	LCD_FULL(0xf800);
	 while(KEY);
//	BL_ON=0;	    
	LCD_FULL(0x07e0);
	 while(KEY);
//	BL_ON=1;	 	
	LCD_FULL(0x001f);
	 while(KEY);
	LCD_FULL(0x07ff);
     while(KEY); 
	LCD_FULL(0xffe0);
     while(KEY); 
	LCD_FULL(0xf81f);
     while(KEY);

	ClearScreen(0xffff);	//all display
	 while(KEY);


 	DispRGBGray();
     while(KEY); 

	show_picture();        //pictuer in mcu
     while(KEY); 
	 

	SdInit();         //SD Card initialization
	f=10;			//Total number of picture
	AddTemp=3469824;
 while(f--)
 {
  	   LCD_SetPos(0,240,0,320); 
 for(j=0;j<300;j++)   ////A picture information  comprising 300x512 bytes
    {
    SdReadBlock(DATA,AddTemp+(j*512),512);
    for(i=0;i<256;i++)                    
	   {   

   	   Write_Data(DATA[2*i],DATA[2*i+1]);	 

       }
     } 
     AddTemp = AddTemp+((j+20)*512);         
     while(KEY);                        
	 
 } 

}
}

