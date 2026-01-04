#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

typedef enum {
    STATE_INIT = 0,
    STATE_IDLE,
    STATE_RUN,
    STATE_ERROR
} SystemState;

static SystemState current_state = STATE_INIT;

static double angle = 0.0;
static double t = 0.0;

double target_angle = 0.0;
double measure_angle = 0.0;

#define WIDTH  70
#define HEIGHT 15

static char canvas[HEIGHT][WIDTH + 1]; // 每行多1个 '\0'

// 把屏幕清空并把光标移到左上角
static void term_clear(void) {
    printf("\033[2J\033[H");
}

// 把光标移到左上角（重绘用，不清屏）
static void term_home(void) {
    printf("\033[H");
}

// 初始化画布
static void canvas_init(void) {
    for (int r = 0; r < HEIGHT; r++) {
        memset(canvas[r], ' ', WIDTH);
        canvas[r][WIDTH] = '\0';
    }
}

// 每个采样点：画布左移一列，并在最右列画一个点
static void canvas_update(double measure, double target)
{
    int row_m = (int)((1.0 - (measure + 1.0) * 0.5) * (HEIGHT - 1));
    int row_t = (int)((1.0 - (target  + 1.0) * 0.5) * (HEIGHT - 1));

    if (row_m < 0) row_m = 0;
    if (row_m >= HEIGHT) row_m = HEIGHT - 1;
    if (row_t < 0) row_t = 0;
    if (row_t >= HEIGHT) row_t = HEIGHT - 1;

    // 左移画布
    for (int r = 0; r < HEIGHT; r++) {
        memmove(&canvas[r][0], &canvas[r][1], WIDTH - 1);
        canvas[r][WIDTH - 1] = ' ';
    }

    // 先画 target（+）
    canvas[row_t][WIDTH - 1] = '+';

    // 再画 measure（*），如果重叠就显示 X
    if (canvas[row_m][WIDTH - 1] == '+')
        canvas[row_m][WIDTH - 1] = 'X';
    else
        canvas[row_m][WIDTH - 1] = '*';
}


// 重绘画布
static void canvas_render(void) {
    term_home();
    printf("[STATE] %s  angle=%.2f\n",
           (current_state == STATE_INIT) ? "INIT" :
           (current_state == STATE_IDLE) ? "IDLE" :
           (current_state == STATE_RUN)  ? "RUN"  : "ERROR",
           angle);

    // 顶部标尺可选
    // printf("+" ); for (int i=0;i<WIDTH;i++) putchar('-'); printf("+\n");

    for (int r = 0; r < HEIGHT; r++) {
        printf("|%s|\n", canvas[r]);
    }
    fflush(stdout);
}

static void state_machine_step(void)
{
    switch (current_state) {
    case STATE_INIT:
        term_clear();
        canvas_init();
        current_state = STATE_IDLE;
        break;

    case STATE_IDLE:
        measure_angle = sin(t);
        target_angle = 0.5 * sin(t * 0.5);
        t += 0.1;
        canvas_update(measure_angle, target_angle);
        canvas_render();
        break;

    default:
        break;
    }
}

int main(void)
{
    while (1) {
        state_machine_step();
        usleep(50 * 1000); // 50ms
    }
    return 0;
}
