//服务器辅助程序，主要是一些功能函数
//例如清数组，判断TCP数据等

#ifndef _SERVER_FUC_H
#define _SERVER_FUC_H

#include "common.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "servertask.h"

uint8_t Server_CRC(uint8_t *databuf,uint16_t datanum);
void server_send_string(char *send_string);
int32_t server_rec_string(uint8_t *rec_string);
uint8_t SERV_recv_data(void);
void Clear_arry(uint8_t *array, uint8_t num);
uint8_t *GPRS_frame(uint8_t arry[],uint8_t number);
void TCP_frame(uint8_t *arry);
void PTR_arry(uint8_t array[],uint8_t *pointer);

void GET_IPMessage(char *REC_string,uint8_t IP_address[]);
void GET_SOFT(char *REC_string,char soft_mess[]);
void GET_STATE(char *REC_string,char state_mess[]);
void GET_HARD(char *REC_string,char hard_mess[]);

void server_send_len(char *send_string,uint16_t number);
uint8_t MODESET_CRC(e_SERV_TCPmode *stru);
uint8_t return_CRC(e_SERV_RETURN *struc);
//uint8_t SET_servermode(e_SERV_TCPmode stu);
void server_send_string(char *send_string);
#endif


