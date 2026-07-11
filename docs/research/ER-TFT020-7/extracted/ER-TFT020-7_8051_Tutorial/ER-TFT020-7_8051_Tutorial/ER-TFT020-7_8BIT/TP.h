//cst816
/* touch panel interface define */
sbit SDA	   =    P3^0;
sbit SCL       =    P3^1;
sbit PEN       =    P3^2;	
sbit RESET     =    P3^3;		//reset


#define uchar      unsigned char
#define uint       unsigned int
#define ulong      unsigned long

#define WHITE          0xFFFF
#define BLACK          0x0000
#define GRAY           0xF7DE
#define BLUE           0x001F
#define BLUE2          0x051F
#define RED            0xF800
#define PURPLE         0xF81F
#define GREEN          0x07E0
#define CYAN           0x7FFF
#define YELLOW         0xFFE0
#define DGREEN         0x07E0


uchar    Gestures;

//Touch Status	 
#define Key_Down 0x01
#define Key_Up   0x00 

struct _ts_event
{
    uint    x1;
    uint    y1;

    uchar     touch_point;
	uchar     Key_Sta;	

};


struct _ts_event ts_event; 

 uchar addr;

#define WRITE_ADD	0x2a 
#define READ_ADD	0x2b  

#define ACK		0
#define NACK 	1

void TOUCH_Init(void);
void TOUCH_Start(void);
void TOUCH_Stop(void);
uchar TOUCH_Wait_Ack(void);
void TOUCH_Ack(void);
void TOUCH_NAck(void);




void delay_us(unsigned int i)
{

    while(i--);

}



//IIC start
void TOUCH_Start(void)
{ 
	SDA=1;  
	delay_us(5);	  
	SCL=1;
	delay_us(5);
	SDA=0;  
	delay_us(5);
	SCL=0;
	delay_us(5);
}	  


//IIC stop
void TOUCH_Stop(void)
{
	SDA=0;
	delay_us(5);
	SCL=1;
	delay_us(5);
	SDA=1;
	delay_us(5);
	SCL=0;
	delay_us(5);							   	
}



//Wait for an answer signal
uchar TOUCH_Wait_Ack(void)
{     bit Ack=1;uchar errtime=0;


	SDA=1;
	delay_us(5);
	SCL=1;
	_nop_();
	{
	    errtime++;
	    if(errtime>250)
		    {
		      TOUCH_Stop();
		      return 1;
		    }
	}
	SCL=0;
	delay_us(5);
	 return 0;
}




//Acknowledge
void TOUCH_Ack(void)
{	SCL=0;
	SDA=0;
	delay_us(5);
	SCL=1;
	delay_us(5);
	SCL=0;
	delay_us(5);
}



//NO Acknowledge		    
void TOUCH_NAck(void)
{	SCL=0;
	SDA=1;
	delay_us(5);
	SCL=1;
	delay_us(5);
	SCL=0;
	delay_us(5);
}	
	

//IIC send one byte		  
void TOUCH_Send_Byte(uchar Byte)
{	uchar t;  		

    for(t=0;t<8;t++)
    { 	SCL=0;            
	   	SDA=(bit)(Byte & 0x80) ;
	   	Byte <<=1;
	delay_us(5);
	   	SCL=1;
	delay_us(5);
	   	SCL=0;
	delay_us(5);
    }	
 

} 



//Read one byte，ack=0，Send Acknowledge，ack=1，NO Acknowledge   
uchar TOUCH_Read_Byte(uchar ack)
{	uchar t,receive=0;

	SCL=0;
	SDA=1;
	for(t = 0; t < 8; t++)
	{	_nop_();
	 	SCL = 1;
			delay_us(5);
	 	receive<<=1;
	 	if(SDA == 1)
	 	receive=receive|0x01;
	delay_us(5);
	 	SCL=0;
		delay_us(5);
	}

					 
   	if (ack)  TOUCH_NAck();//NO Acknowledge 
   	else       TOUCH_Ack(); //Send Acknowledge   
    
	 return receive;
}




void TOUCH_Wr_Reg(uchar RegIndex,uchar RegValue1)
{
	TOUCH_Start();
	TOUCH_Send_Byte(WRITE_ADD);
	TOUCH_Wait_Ack();	
	TOUCH_Send_Byte(RegIndex);
	TOUCH_Wait_Ack();
	
	TOUCH_Send_Byte(RegValue1);
	TOUCH_Wait_Ack();

	TOUCH_Stop();
	delay_us(10);
}

uchar TOUCH_Read_Reg(uchar RegIndex)
{	uchar receive=0;

	TOUCH_Start();
	TOUCH_Send_Byte(WRITE_ADD);
	TOUCH_Wait_Ack();
	TOUCH_Send_Byte(RegIndex);
	TOUCH_Wait_Ack();
	
	TOUCH_Start();
	TOUCH_Send_Byte(READ_ADD);
	TOUCH_Wait_Ack();	
	receive=TOUCH_Read_Byte(1);
	TOUCH_Stop();
 	 
	return receive;
}



void TOUCH_RdParFrPCTPFun(uchar *PCTP_Par,uchar ValFlag)
{	uchar k;

	TOUCH_Start();
	TOUCH_Send_Byte(READ_ADD);
	TOUCH_Wait_Ack();	
	for(k=0;k<ValFlag;k++)
	{
		if(k==(ValFlag-1))  *(PCTP_Par+k)=TOUCH_Read_Byte(1);
		else                *(PCTP_Par+k)=TOUCH_Read_Byte(0);
	}		
	TOUCH_Stop();
}







/******************************************************************************************
*Function name：Draw_Big_Point(u16 x,u16 y)
* Parameter：uint x,uint y xy
* Return Value：void
* Function：Draw touch pen nib point
*********************************************************************************************/		   
void Draw_Big_Point(uint x,uint y,uint colour)
{uint num;	    
	
	LCD_SetPos(x-2,x+2,y-2,y+2);
	    for(num=0;num<25;num++)
		{    
          	 Write_Data(colour>>8,colour);
		}
}


void inttostr(uint value,uchar *str)
{
	str[0]=value/1000+48;
	str[1]=value%1000/100+48;
	str[2]=value%100/10+48;
	str[3]=value%100%10+48;

}



////////////////////////////////////
void  counter0(void) interrupt 0
{
 	if(PEN==0)										//Detect the occurrence of an interrupt
 	{
		ts_event.Key_Sta=Key_Down;                              

 	}
}




uchar  read_pointdata() 
{ 
	uchar pointdata[8];	 uchar ret = 0;	  

	TOUCH_Start();
	TOUCH_Send_Byte(WRITE_ADD);
	TOUCH_Wait_Ack();
	TOUCH_Send_Byte(0x00);
	TOUCH_Wait_Ack();
	TOUCH_Stop();

  	TOUCH_RdParFrPCTPFun(pointdata, 8);
	Gestures = pointdata[1];
  	ts_event.touch_point = pointdata[2];

  	

	 
	if (ts_event.touch_point == 0) 	
		{ts_event.x1 = 0;
	     ts_event.y1 = 0;
			return 0;
  		}


		else	{	   //point	 */
       ts_event.x1 = (uint)(pointdata[3]&0x0f )<<8 | (uint)pointdata[4];
	   ts_event.y1 = (uint)(pointdata[5]&0x0f  )<<8 | (uint)pointdata[6];
	 
		ret = 1;														        
		}
				
	

	return ret;				
}








void CST816_init(void)
{  	ts_event.x1=0;
	ts_event.y1=0;
	RESET=0;
 	delayms(50);
	RESET=1;
  	delayms(100);
/*
	TOUCH_Read_Reg(0xA7);  //read ID  0XB4
	TOUCH_Read_Reg(0xA9);  //read Firmware version
  

    TOUCH_Wr_Reg(0xFD,0x00);	 //	  
    TOUCH_Wr_Reg(0xFE,0x01);	 //	Disable automatic low-power mode  
	TOUCH_Wr_Reg(0xFA,0x71);	 //	报点+手势模式
    TOUCH_Wr_Reg(0xEC,0x07);	 //	EnConLR EnConUD EnDClick  	
	
 */
	TOUCH_Wr_Reg(0xFA,0x71);	 //	报点+手势模式		
}


void TPTEST(void)
{  	 uchar chipID; uchar FWVer;		uint xa,ya,colour;	
	uchar ss[4]=0;	
	 IT0=1;        //Falling edge trigger  
	 EX0=1;
	 EA=1;

	  xa=30;ya =120;	colour=GREEN;
	 Gestures=0XFF;
     CST816_init();

 	ClearScreen(0x0000);	
 //   showzifustr(110,100,"TOUCH",RED,BLACK);	

	chipID=TOUCH_Read_Reg(0xA7);  //read ID  0XB4
	FWVer=TOUCH_Read_Reg(0xA9);  //read Firmware version
				   	inttostr(chipID,ss);
 					showzifustr(10,40,"chipID A7=",RED,BLACK);
					showzifustr(80,40,ss,RED,BLACK);

				   	inttostr(FWVer,ss);
 					showzifustr(10,55,"FWVer A9=",RED,BLACK);
					showzifustr(80,55,ss,RED,BLACK);
 


/*	  //test i2c addr
	  addr=44;
	   CST816_init();
	   ClearScreen(0x0000);	
 	while(1){
   		inttostr(addr,ss);
	showzifustr(10,140,ss,RED,WHITE);	     
	TOUCH_Wr_Reg(0xFE,0x01);
	  addr+=1;	
		}		
		 */




	while(KEY)
	{
		if(ts_event.Key_Sta==Key_Down)        //The touch screen is pressed
			{
				EX0=0;//Close interrupt
			
			do
		{	
				
					read_pointdata();
					ts_event.Key_Sta=Key_Up;
		/*	
					inttostr(ts_event.x1,ss);
					showzifustr(10,70,"X1:",RED,BLACK);
					showzifustr(40,70,ss,RED,BLACK);	
					inttostr(ts_event.y1,ss);	 
					showzifustr(10,85,"Y1:",RED,BLACK);
					showzifustr(40,85,ss,RED,BLACK);	
			  
	   		*/	 

					Draw_Big_Point(ts_event.x1,ts_event.y1,RED);	
				 
		
	


		 	  /*
					Gestures=TOUCH_Read_Reg(0x01);
					inttostr(Gestures,ss);
					showzifustr(5,120,"01=",BLUE,BLACK);
					showzifustr(xa,ya,ss,colour,BLACK);xa+=21;if(xa+21>=170)xa=30,ya+=10;	
					 colour=~colour;
			   	*/	
								
                     
				}while(PEN==0);
				EX0=1;
			}


    }



}























