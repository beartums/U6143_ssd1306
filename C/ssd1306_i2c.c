#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "config.h"
#include "ssd1306_i2c.h"

int i2cd;

// Init SSD1306
void ssd1306_begin(unsigned int vccstate, unsigned int i2caddr)
{
  (void)vccstate;
  // I2C Init
  i2cd = open(g_config.i2c_bus, O_RDWR);
  if (i2cd < 0)
  {
    fprintf(stderr, "Failed to open I2C device %s\n", g_config.i2c_bus);
    return;
  }
  if (ioctl(i2cd, I2C_SLAVE_FORCE, i2caddr) < 0)
  {
    return;
  }
	OLED_WR_Byte(0xAE,OLED_CMD);//Disable display
	OLED_WR_Byte(0x40,OLED_CMD);//---set low column address
	OLED_WR_Byte(0xB0,OLED_CMD);//---set high column address
	OLED_WR_Byte(0xC8,OLED_CMD);//-not offset
	OLED_WR_Byte(0x81,OLED_CMD);//Set Contrast
	OLED_WR_Byte(0xff,OLED_CMD);
	OLED_WR_Byte(0xa1,OLED_CMD);
	OLED_WR_Byte(0xa6,OLED_CMD);
	OLED_WR_Byte(0xa8,OLED_CMD);
	OLED_WR_Byte(0x1f,OLED_CMD);
	OLED_WR_Byte(0xd3,OLED_CMD);
	OLED_WR_Byte(0x00,OLED_CMD);
	OLED_WR_Byte(0xd5,OLED_CMD);
	OLED_WR_Byte(0xf0,OLED_CMD);
	OLED_WR_Byte(0xd9,OLED_CMD);
	OLED_WR_Byte(0x22,OLED_CMD);
	OLED_WR_Byte(0xda,OLED_CMD);
	OLED_WR_Byte(0x02,OLED_CMD);
	OLED_WR_Byte(0xdb,OLED_CMD);
	OLED_WR_Byte(0x49,OLED_CMD);
	OLED_WR_Byte(0x8d,OLED_CMD);
	OLED_WR_Byte(0x14,OLED_CMD);
	OLED_WR_Byte(0xaf,OLED_CMD);
}

//Coordinate setting
void OLED_Set_Pos(unsigned char x, unsigned char y)
{
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD);
}

//Write a byte
void OLED_WR_Byte(unsigned dat, unsigned cmd)
{
	if (cmd)
	{
		Write_IIC_Data(dat);
	}
	else
	{
		Write_IIC_Command(dat);
	}
	usleep(500);
}

//To send data
void Write_IIC_Data(unsigned char IIC_Data)
{
  unsigned char msg[2]={0x40,0};
  msg[1]=IIC_Data;
  write(i2cd, msg, 2);
}

//Send the command
void Write_IIC_Command(unsigned char IIC_Command)
{
  unsigned char msg[2]={0x00,0};
  msg[1]=IIC_Command;
  write(i2cd, msg, 2);
}

/*
*	Clear specified row
*/
void OLED_ClearLint(unsigned char x1, unsigned char x2)
{
	unsigned char i,n;
	for(i=x1;i<x2;i++)
	{
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //Set page address
		OLED_WR_Byte (0x00,OLED_CMD);      //Sets the display location - column low address
		OLED_WR_Byte (0x10,OLED_CMD);      //Sets the display location - column high address
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA);
	}
}

void OLED_Clear(void)
{
	unsigned char i,n;
	for(i=0;i<4;i++)
	{
		OLED_WR_Byte (0xb0+i,OLED_CMD);
		OLED_WR_Byte (0x00,OLED_CMD);
		OLED_WR_Byte (0x10,OLED_CMD);
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA);
	}
}
