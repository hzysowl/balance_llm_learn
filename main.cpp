#include <stdio.h>
#include <unistd.h>
#include <math.h>

typedef enum{
    STATE_INIT = 0,
    STATE_IDLE ,
    STATE_RUN ,
    STATE_ERROR
}SystemState;

float angle = 0.f;
float angle_t = 0.f;

#define WIDTH 60 //终端宽度

char wave_buf[WIDTH + 1]; 

SystemState current_state = STATE_INIT;

void init_wave_buf(void)
{
    for (int i = 0; i < WIDTH; i++) {
        wave_buf[i] = ' ';
    }
    wave_buf[WIDTH] = '\0';
}

void update_wave(double value)
{
    // value ∈ [-1, 1]
    int pos = (int)((value + 1.0) * 0.5 * (WIDTH - 1));

    // 整体左移
    for (int i = 0; i < WIDTH - 1; i++) {
        wave_buf[i] = wave_buf[i + 1];
    }

    // 默认空格
    wave_buf[WIDTH - 1] = ' ';

    // 在最右侧打点
    wave_buf[pos] = '*';
}

void print_wave_line(void)
{
    printf("\rangle |%s", wave_buf);
    fflush(stdout);
}


void screen_show(float value )
{
    int pos = (int)((value + 1.0 ) * 0.5 * WIDTH);
    printf("angle |");
    for(int i = 0;i<WIDTH;i++){
        if(i == pos){
            printf("*");
        }else{
            printf(" ");
        }
    }
    printf("\n");
}

void state_machine_step(void)
{
    static float angle_cnt = 0.f;
    angle_cnt += 0.1f ;
    angle = sinf(angle_cnt);
    if(angle_cnt > 360){
        angle_cnt = 0.f;
    }
    switch (current_state){
        case STATE_INIT:
        //init state confirmed running 
            printf("[state] INIT\n");
            init_wave_buf();
            current_state = STATE_IDLE;
            break;
        case STATE_IDLE:
           
            angle = sin(angle_t);
            angle_t += 0.1f;
            
            update_wave(angle);
            print_wave_line();
            break;
        case STATE_RUN:
            printf("[state] RUN\n");
            break;
        case STATE_ERROR:
            printf("[state] ERROR\n");
            break;

        default :
            break;
    }

}

int main ()
{
    while(1){
        state_machine_step();
        usleep(50 * 1000);
    }

}