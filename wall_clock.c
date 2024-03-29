#include "rtx.h"
#include "uart_polling.h"
#include "printf.h"
#include "wall_clock.h"
#include "sys_procs.h"

extern int current_time;

int state, time;
	
void wc_process() {
	MSGBUF* keyreg_env;
	MSGBUF* message_to_crt;
	keyreg_env = (MSGBUF*)request_memory_block();
	keyreg_env->mtype = KCD_REG;
	keyreg_env->mtext[0] = '%';
	keyreg_env->mtext[1] = 'W';
	keyreg_env->mtext[2] = 'S';
	keyreg_env->mtext[3] = '\0';
	send_message(KCD_PROC_ID, (void*)keyreg_env);

	keyreg_env = (MSGBUF*)request_memory_block();
	keyreg_env->mtype = KCD_REG;
	keyreg_env->mtext[0] = '%';
	keyreg_env->mtext[1] = 'W';
	keyreg_env->mtext[2] = 'T';
	keyreg_env->mtext[3] = '\0';
	send_message(KCD_PROC_ID, (void*)keyreg_env);

	keyreg_env = (MSGBUF*)request_memory_block();
	keyreg_env->mtype = KCD_REG;
	keyreg_env->mtext[0] = '%';
	keyreg_env->mtext[1] = 'W';
	keyreg_env->mtext[2] = 'R';
	keyreg_env->mtext[3] = '\0';
	send_message(KCD_PROC_ID, (void*)keyreg_env);	
	
	state = 0; // 0 means terminated; 1 means running;
	time = current_time;

	while (1) {
		MSGBUF* next_second_message;
		MSGBUF* message = receive_message(NULL);

		if (message->mtype == NOTIFY_WALL_CLOCK) {
			//uart0_put_char('0'+(current_time/1000)%10);
			if (state) {
				int crt_time,seconds,minutes,hours,s1f,s0f,m1f,m0f,h1f,h0f;
				//release_memory_block((void*)message);
				//message = NULL;
				message_to_crt = (MSGBUF*)request_memory_block();
				message_to_crt->mtype = CRT_REQ;
				crt_time = current_time - time;
				if(crt_time >= 1000){
					
				seconds = crt_time / 1000 % 60;
				minutes = crt_time / 1000 / 60 % 60;
				hours   = crt_time / 1000 / 60 / 60 % 24;
				s1f = seconds / 10;
				s0f = seconds % 10;
				m1f = minutes / 10;
				m0f = minutes % 10;
				h1f = hours   / 10;
				h0f = hours   % 10;
				message_to_crt->mtext[0] = h1f + '0';
				message_to_crt->mtext[1] = h0f + '0';
				message_to_crt->mtext[2] = ':';
				message_to_crt->mtext[3] = m1f + '0';
				message_to_crt->mtext[4] = m0f + '0';
				message_to_crt->mtext[5] = ':';
				message_to_crt->mtext[6] = s1f + '0';
				message_to_crt->mtext[7] = s0f + '0';
				message_to_crt->mtext[8] = '\n';
				message_to_crt->mtext[9] = '\r';
				message_to_crt->mtext[10] = '\0';
			
				send_message(CRT_PROC_ID, (void*)message_to_crt);
			}
			} else {
				//release_memory_block(message);
				//message = NULL;
			}
		}
		
		if (message->mtype == DEFAULT) {

			switch (message->mtext[2]){
			case 'R': {
				if(state){
					message->mtext[0] = '%';
					message->mtext[1] = 'W';
					message->mtext[2] = 'T';
					message->mtext[3] = '\r';
				
					message->mtype = DEFAULT;
					send_message(WALL_CLOCK_PROC_ID, message);
				}
				state = 1;
				time = current_time;
				break;
			}
			
			case 'S': {
				int time_set;
				int h1,h0,m1,m0,s1,s0,ter;
				h1 = message->mtext[4] - '0';
				h0 = message->mtext[5] - '0';
				m1 = message->mtext[7] - '0';
				m0 = message->mtext[8] - '0';
				s1 = message->mtext[10] - '0';
				s0 = message->mtext[11] - '0';
				ter = message->mtext[12];
				if(h1 > 2 || h1 < 0 || h0 > 9 || h0 < 0 || (h1 == 2 && h0 > 3) || m1 > 6 
					|| m1 < 0 || m0 > 9 || m0 < 0 || s1 > 6 || s1 < 0 || s0 > 9 || s0 < 0 ||
					ter != '\r' || message->mtext[6] != ':' || message->mtext[9] != ':'){
					time_set = -1;
				}
				else{
					time_set = 1000*((h1*10 + h0)*3600 + (m1*10 + m0)*60 + (s1*10 + s0));	
				}	
				if(state){
					message->mtext[0] = '%';
					message->mtext[1] = 'W';
					message->mtext[2] = 'T';
					message->mtext[3] = '\r';
					message->mtype = DEFAULT;
					send_message(WALL_CLOCK_PROC_ID, message);
				}
					
				if(time_set < 0){
					
					message->mtype = CRT_REQ;
					message->mtext[0] = 'E';
					message->mtext[1] = 'R';
					message->mtext[2] = 'R';
					message->mtext[3] = '\n';
					message->mtext[4] = '\r';
					message->mtext[5] = '\0';
					send_message(CRT_PROC_ID, (void*)message);
				}
				else{
					state = 1;
					time = -time_set + current_time;
				}
				
				break;
		  }
			
		  case 'T': {
				state = 0;
				break;			
			}

			default: {
					// should print some message here
					state = 0;
					//release_memory_block(message);
					//message = NULL;
					break;
			}				
			
			
		}
			
		}
		
		if(state){ 
			next_second_message = (MSGBUF*)request_memory_block();
			next_second_message->mtype = NOTIFY_WALL_CLOCK;
			delayed_send(WALL_CLOCK_PROC_ID, next_second_message, 1000);
		}
		release_memory_block(message);
		message = NULL;
		release_processor();
	}
}
