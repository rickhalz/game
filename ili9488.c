#include "ili9488.h"

extern SPI_HandleTypeDef hspi1;

uint16_t width;
uint16_t height;

void ILI9488_SendCommand(uint8_t com)
{
	//*(__IO uint8_t *)(0x60000000) = com;
	//Set DC HIGH for COMMAND mode
	DC_COMMAND();    //Put CS LOW
	CS_LOW();	//Write byte using SPI
	HAL_SPI_Transmit(&hspi1, &com, 1, 1);
	//SendByte(tmpCmd);
	//WaitLastData();
    //Bring CS HIGH
	CS_HIGH();
}

void ILI9488_SendData(uint8_t data)
{

	//*(__IO uint8_t *)(0x60040000) = data;
	//Set DC LOW for DATA mode
	DC_DATA();
	//Put CS LOW
	CS_LOW();
	//Write byte using SPI
	HAL_SPI_Transmit(&hspi1, &data, 1, 1);
	//SendByte(tmpCmd);
	//WaitLastData();
	CS_HIGH();
}

void ILI9488_SendData_Multi(uint8_t *buff, size_t buff_size){
	DC_DATA();
	/*CS_A();
	for (uint32_t i = 0; i < buff_size; i++)
	  {
	    SendByte(*buff);
	    buff++;
	  }

	  WaitLastData();
	  CS_D();*/
	CS_LOW();
	while (buff_size > 0){
		uint16_t chunk_size = buff_size > 32768 ? 32768 : buff_size;
		HAL_SPI_Transmit(&hspi1, buff, chunk_size, HAL_MAX_DELAY);
		buff += chunk_size;
		buff_size -= chunk_size;
	}
	CS_HIGH();
}

void ILI9488_Init()
{

	 /*//Copy SPI settings
	 memcpy(&lcdSPIhandle, spiLcdHandle, sizeof(*spiLcdHandle));
	 //CS pin
	 tftCS_GPIO = csPORT;
	 tftCS_PIN = csPIN;
	 //DC pin
	 tftDC_GPIO = dcPORT;
	 tftDC_PIN = dcPIN;
	 //HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_SET);
	 CS_D();
	 //RESET pin
	 tftRESET_GPIO = resetPORT;
	 tftRESET_PIN = resetPIN;
	 //HAL_GPIO_WritePin(tftRESET_GPIO, tftRESET_PIN, GPIO_PIN_RESET);  //Turn LCD ON
	 RST_A();
	 HAL_Delay(10);
	 RST_D();
	 //HAL_GPIO_WritePin(tftRESET_GPIO, tftRESET_PIN, GPIO_PIN_SET);  //Turn LCD ON*/
	//SPI1->CR1 |= SPI_CR1_SPE;
	CS_HIGH();
	RST_LOW();
		 HAL_Delay(10);
		 RST_HIGH();
	 width=ILI9488_WIDTH;
	 height=ILI9488_HEIGHT;
	 //CS_A();
	 ILI9488_SendCommand(ILI9488_PGAMCTL);
	 ILI9488_SendData(0x00);
	 ILI9488_SendData(0x03);
	 ILI9488_SendData(0x09);
	 ILI9488_SendData(0x08);
	 ILI9488_SendData(0x16);
	 ILI9488_SendData(0x0A);
	 ILI9488_SendData(0x3F);
	 ILI9488_SendData(0x78);
	 ILI9488_SendData(0x4C);
	 ILI9488_SendData(0x09);
	 ILI9488_SendData(0x0A);
	 ILI9488_SendData(0x08);
	 ILI9488_SendData(0x16);
	 ILI9488_SendData(0x1A);
	 ILI9488_SendData(0x0F);

	 	ILI9488_SendCommand(ILI9488_NGAMCTL);
	 	ILI9488_SendData(0x00);
	 	ILI9488_SendData(0x16);
	 	ILI9488_SendData(0x19);
	 	ILI9488_SendData(0x03);
	 	ILI9488_SendData(0x0F);
	 	ILI9488_SendData(0x05);
	 	ILI9488_SendData(0x32);
	 	ILI9488_SendData(0x45);
	 	ILI9488_SendData(0x46);
	 	ILI9488_SendData(0x04);
	 	ILI9488_SendData(0x0E);
	 	ILI9488_SendData(0x0D);
	 	ILI9488_SendData(0x35);
	 	ILI9488_SendData(0x37);
	 	ILI9488_SendData(0x0F);

	 	ILI9488_SendCommand(ILI9488_PWCTR1);      //Power Control 1
	 	ILI9488_SendData(0x17);    //Vreg1out
	 	ILI9488_SendData(0x15);    //Verg2out

	 	ILI9488_SendCommand(ILI9488_PWCTR2);      //Power Control 2
	 	ILI9488_SendData(0x41);    //VGH,VGL

	 	ILI9488_SendCommand(ILI9488_VMCTR1);      //Power Control 3
	 	ILI9488_SendData(0x00);
	 	ILI9488_SendData(0x12);    //Vcom
	 	ILI9488_SendData(0x80);

	 	ILI9488_SendCommand(LI9488_MAC);      //Memory Access
	 	ILI9488_SendData(0x48);

	 	ILI9488_SendCommand(ILI9488_PXFMT);      // Interface Pixel Format
	 	ILI9488_SendData(0x66); 	  //18 bit

	 	ILI9488_SendCommand(ILI9488_RGBITF);      // Interface Mode Control
	 	ILI9488_SendData(0x80);     			 //SDO NOT USE

	 	ILI9488_SendCommand(ILI9488_FRC);      //Frame rate
	 	ILI9488_SendData(0xA0);    //60Hz

	 	ILI9488_SendCommand(0xB4);      //Display Inversion Control
	 	ILI9488_SendData(0x02);    //2-dot

	 	ILI9488_SendCommand(ILI9488_DFC); //Display Function Control  RGB/MCU Interface Control

	 	ILI9488_SendData(0x02);    //MCU
	 	ILI9488_SendData(0x02);    //Source,Gate scan direction

	 	ILI9488_SendCommand(0XE9);      // Set Image Function
	 	ILI9488_SendData(0x00);    // Disable 24 bit data

	 	ILI9488_SendCommand(0xF7);      // Adjust Control
	 	ILI9488_SendData(0xA9);
	 	ILI9488_SendData(0x51);
	 	ILI9488_SendData(0x2C);
	 	ILI9488_SendData(0x82);    // D7 stream, loose

	 	ILI9488_SendCommand(ILI9488_SLPOUT);    //Exit Sleep

	 	HAL_Delay(120);

	 	ILI9488_SendCommand(ILI9488_DISON);    //Display on

}

void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	ILI9488_SendCommand(ILI9488_CASET); // Column addr set
		{
		uint8_t data[] = {(x0 >> 8) & 0xFF, x0 & 0xFF, (x1 >> 8) & 0xFF, x1 & 0xFF};
		ILI9488_SendData_Multi(data, sizeof(data));
		}
		ILI9488_SendCommand(ILI9488_PASET);
		{
		uint8_t data[] = {(y0 >> 8) & 0xFF, y0 & 0xFF, (y1 >> 8) & 0xFF, y1 & 0xFF};
		ILI9488_SendData_Multi(data, sizeof(data));
		}
		ILI9488_SendCommand(ILI9488_WR); // write to RAM*/
}


void setRotation(uint8_t r)
{

	ILI9488_SendCommand(ILI9488_MAC);
	uint8_t rotation = r % 4; // can't be higher than 3
	switch (rotation) {
	case 0:
		ILI9488_SendData(MADCTL_MX | MADCTL_BGR);
		width = ILI9488_WIDTH;
		height = ILI9488_HEIGHT;
		break;
	case 1:
		ILI9488_SendData(MADCTL_MV | MADCTL_BGR);
		width = ILI9488_HEIGHT;
		height = ILI9488_WIDTH;
		break;
	case 2:
		ILI9488_SendData(MADCTL_MY | MADCTL_BGR);
		width = ILI9488_WIDTH;
		height = ILI9488_HEIGHT;
		break;
	case 3:
		ILI9488_SendData(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
		width = ILI9488_HEIGHT;
		height = ILI9488_WIDTH;
		break;
	}
}

void fillScreen(uint16_t color)
{
	fillRect(0, 0,  width, height, color);
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{

	uint32_t i, n, cnt, buf_size;
	if ((x >= width) || (y >= height))
		return;
	if ((x + w - 1) >= width)
		w = width - x;
	if ((y + h - 1) >= height)
		h = height - y;
	setAddrWindow(x, y, x + w - 1, y + h - 1);
	uint8_t r = (color & 0xF800) >> 11;
	uint8_t g = (color & 0x07E0) >> 5;
	uint8_t b = color & 0x001F;

	r = (r * 255) / 31;
	g = (g * 255) / 63;
	b = (b * 255) / 31;

	n = w*h*3;
	if (n <= 65535){
		cnt = 1;
		buf_size = n;
	}
	else {
		cnt = n/3;
		buf_size = 3;
		uint8_t min_cnt = n/65535+1;
		for (i=min_cnt; i < n/3; i++){
			if(n%i == 0){
				cnt = i;
				buf_size = n/i;
				break;
			}
		}
	}
	uint8_t frm_buf[buf_size];
	for (i=0; i < buf_size/3; i++)
	{
		frm_buf[i*3] = r;
		frm_buf[i*3+1] = g;
		frm_buf[i*3+2] = b;
	}
	DC_DATA();
	CS_LOW();
		while(cnt>0)
		{
			HAL_SPI_Transmit(&hspi1, frm_buf, buf_size, HAL_MAX_DELAY);

			cnt -= 1;
		}
		CS_HIGH();

}


