#include "opora.h"
#include "itmr.h"
#include "button.h"

/*
		������ ������ � ������� �����/������.
		��������� ������� �� ������, ��������� ������� �� ������.
*/

void CoreClkConfig(void);
void PerClkConfig(void);
void InPortConfig(void);
void OutPortConfig(void);

int main(void)
{
	unsigned int stime;
	CoreClkConfig(); 	// ��������� ������������ ����
	PerClkConfig(); 	// ��������� ������������ ������������ �������
	itmr_Init();			// ��������� ������������ ������������� �������
	InPortConfig(); 	// ��������� ������ �� ����
	OutPortConfig(); 	// ��������� ������ �� �����
	
	stime = itmr_GetTick(); // ��������� ������������� ������������� �������
	while(1)
	{											
		// ����������� ������� �������
		switch(button_Press()){
				case UP:
					PORTA->RXTX  |= (1 << 9); // �������� ���� VD13
					break;					
				case DOWN: 
					PORTA->RXTX  &= ~(1 << 9);  // ��������� ���� VD13
					break;
				case RIGHT: 
					PORTA->RXTX  |= (1 << 13);  // �������� ���� VD12
					break;					
				case LEFT: 
					PORTA->RXTX  &= ~(1 << 13);  // ��������� ���� VD12
					break;					 
				case SELECT:
					PORTC->RXTX ^= (1 << 8) | (1 << 9); // ����������� ��������� ������ VD10 � VD11
					break;					
				case NOPRESS:					
					break;
				default:
					break;
			}			

		// ������������ ������ ��������� ������ VD6, VD7, VD8, VD9 � �������������� ������������ �������
		// ������ �������� ������������ ������ - ����� ������ � ������� �������
		if(itmr_IsTime(stime, 1000)){			
			PORTD->RXTX ^= (1 << 0) | (1 << 7) | (1 << 8) | (1 << 11);
			stime = itmr_GetTick();		
		}															
	}
}

void CoreClkConfig(void)
{
	uint32_t temp;
	RST_CLK->PER_CLOCK |= (1 << 27);				
	temp = BKP->REG_0E;
	temp &= 0xFFFFFFC0;
	BKP->REG_0E = temp | (5 << 3) | 5;				

	RST_CLK->HS_CONTROL = 0x00000001;						// HSE - ��������; ����� ����������
	while((RST_CLK->CLOCK_STATUS&0x04)!=0x04);	// �������� ���������� HSE
	RST_CLK->CPU_CLOCK=0x00000002;							// CPU_C1 = 16MHz
	RST_CLK->PLL_CONTROL=(1<<2)|(2<<8);					// CPU PLL �������, PLL_MULL=2
	while((RST_CLK->CLOCK_STATUS&0x02)!=0x02);
	RST_CLK->PER_CLOCK|=0x08;										// ��������� ������������ EEPROM_CTRL 
	EEPROM->CMD=1;
	RST_CLK->PER_CLOCK&=(~0x08);								// ��������� ������������ EEPROM_CTRL
	RST_CLK->CPU_CLOCK = 0x00000106;						// CPU Clock = 3*16MHz
}

void PerClkConfig(void)
{
	// ��������� ������������ ������ ��� ������������ �������
	RST_CLK->PER_CLOCK 	|= 	(1<<21) /*PORTA*/ 
												| (1<<23) /*PORTC*/ 
												| (1<<24) /*PORTD*/ 
												| (1 << 14) /*TIMER1*/;
	// ��������� ������ TIMER'� 1
	RST_CLK->TIM_CLOCK = 0x01000000; /*TIMER1*/
}

void InPortConfig(void)
{
	// ����������� ���� D �� ���� - � ���� ���������� ������	
	PORTD->FUNC &= ~((0x03 << (2 * 6)) |
					 (0x03 << (2 * 5)) |
					 (0x03 << (2 * 4)) |
					 (0x03 << (2 * 3)) |
					 (0x03 << (2 * 2)));
	PORTD->OE &= ~((1 << 6) | (1 << 5) | (1 << 4) | (1 << 3) | (1 << 2));
	PORTD->ANALOG |= (1 << 6) | (1 << 5) | (1 << 4) | (1 << 3) | (1 << 2);
	PORTD->PWR |= 	(0x03 << (2 * 6)) |
					(0x03 << (2 * 5)) |
					(0x03 << (2 * 4)) |
					(0x03 << (2 * 3)) |
					(0x03 << (2 * 2));				
}

void OutPortConfig(void)
{
	// ���� A - ����� VD13, VD12
	PORTA->FUNC |= 0x00000000;	
	PORTA->OE |= (1 << 9) | (1 << 13);
	PORTA->ANALOG |= (1 << 9) | (1 << 13);
	PORTA->PWR |= (0x3 << (9 * 2)) | (0x03 << (13 * 2));
	// ���� � - ����� VD10, VD11
	PORTC->FUNC |= 0x00000000;	
	PORTC->OE |= (1 << 8) | (1 << 9);
	PORTC->ANALOG |= (1 << 8) | (1 << 9);
	PORTC->PWR |= (0x3 << (8 * 2)) | (0x03 << (9 * 2));	
	// ���� D - ����� VD9, VD6, VD7, VD8
	PORTD->FUNC |= 0x00000000;
	PORTD->RXTX |= (1 << 0) | (1 << 8);
	PORTD->OE |= (1 << 0) | (1 << 7) | (1 << 8) | (1 << 11);
	PORTD->ANALOG |= (1 << 0) | (1 << 7) | (1 << 8) | (1 << 11);
	PORTD->PWR |= (0x3 << (0 * 2)) | (0x03 << (7 * 2)) | (0x03 << (8 * 2)) | (0x03 << (11 * 2));			
}
