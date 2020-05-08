#ifndef __EXTI_H__
#define __EXTI_H__

void EXTI_init(void);
void IO_init(void);
void WWDG_Init(uint8_t tr,uint8_t wr,uint8_t fprer);
unsigned short cal_serv_crc_N(unsigned char *message, unsigned int len,unsigned short crc_reg);
uint16_t Send_msg_Translate (uint8_t *tmsg,uint16_t len);	 
uint16_t Recv_msg_Translate (uint8_t *atmsg,uint16_t len);
void TIM3_Int_Init(u16 arr,u16 psc);
#endif
