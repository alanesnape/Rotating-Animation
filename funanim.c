// funanim_fixed.c - 自动动画版本
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>    // 添加 Windows 头文件
#include <math.h>
#include <time.h>
#include "gfx.h"

#define PI 3.14159265358979323846
#define NUM_SHAPES 8
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

typedef struct {
    double center_x, center_y;
    double radius;
    double angle;
    double angular_velocity;
    int size;
    int color_r, color_g, color_b;
    int shape_type;
} RotatingShape;

void initialize_shapes(RotatingShape shapes[]);
void update_animation(RotatingShape shapes[]);
void draw_rotating_shape(RotatingShape *shape);
void draw_spiral(int center_x, int center_y, double time);
void change_colors(RotatingShape shapes[]);

int main() {
    RotatingShape shapes[NUM_SHAPES];

    gfx_open(WINDOW_WIDTH, WINDOW_HEIGHT, "Rotating Animation Fun");
    initialize_shapes(shapes);

    printf("Rotating Animation Controls:\n");
    printf("SPACE: Change colors randomly\n");
    printf("Mouse Click: Add temporary spiral effect\n");
    printf("'+': Increase rotation speed\n");
    printf("'-': Decrease rotation speed\n");
    printf("ESC or 'q': Quit\n");

    double spiral_time = 0;
    int spiral_active = 0;
    int spiral_x = 0, spiral_y = 0;

    while(1) {
        // === 关键：非阻塞事件处理 ===
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_LBUTTONDOWN) {
                spiral_active = 1;
                spiral_x = LOWORD(msg.lParam);
                spiral_y = HIWORD(msg.lParam);
                spiral_time = 0;
                printf("Spiral at: (%d, %d)\n", spiral_x, spiral_y);
            }
            else if (msg.message == WM_KEYDOWN) {
                char key = (char)msg.wParam;

                if (key == ' ') {
                    change_colors(shapes);
                    printf("Colors changed\n");
                }
                else if (key == '+') {
                    for (int i = 0; i < NUM_SHAPES; i++) {
                        shapes[i].angular_velocity *= 1.5;
                    }
                    printf("Speed increased\n");
                }
                else if (key == '-') {
                    for (int i = 0; i < NUM_SHAPES; i++) {
                        shapes[i].angular_velocity *= 0.7;
                    }
                    printf("Speed decreased\n");
                }
                else if (key == 'q' || key == 'Q' || key == 27) {
                    printf("Exiting...\n");
                    return 0;
                }
            }
        }

        // === 更新动画（总是执行）===
        update_animation(shapes);

        // 清屏
        gfx_clear();

        // 绘制所有旋转图形
        for (int i = 0; i < NUM_SHAPES; i++) {
            gfx_color(shapes[i].color_r, shapes[i].color_g, shapes[i].color_b);
            draw_rotating_shape(&shapes[i]);
        }

        // 绘制螺旋效果
        if (spiral_active) {
            gfx_color(255, 0, 0);  // 红色螺旋
            draw_spiral(spiral_x, spiral_y, spiral_time);
            spiral_time += 0.3;
            if (spiral_time > 4 * PI) {
                spiral_active = 0;
            }
        }

        // 显示控制提示
        gfx_color(0, 255, 0);  // 绿色文字
        gfx_text(20, 30, "Space: Colors  +/-: Speed  Click: Spiral  q: Quit");

        // 刷新显示
        gfx_flush();

        // 控制动画速度（Windows 用 Sleep）
        Sleep(30);
    }

    return 0;
}

// 初始化函数保持不变...
void initialize_shapes(RotatingShape shapes[]) {
    srand(time(NULL));

    for (int i = 0; i < NUM_SHAPES; i++) {
        shapes[i].center_x = WINDOW_WIDTH / 2;
        shapes[i].center_y = WINDOW_HEIGHT / 2;
        shapes[i].radius = 80 + i * 30;
        shapes[i].angle = 2 * PI * i / NUM_SHAPES;
        shapes[i].angular_velocity = 0.02 + (i * 0.005);
        shapes[i].size = 15 + i * 3;
        shapes[i].shape_type = i % 4;

        shapes[i].color_r = (i * 50) % 256;
        shapes[i].color_g = (i * 80) % 256;
        shapes[i].color_b = (i * 120) % 256;
    }
}

void update_animation(RotatingShape shapes[]) {
    for (int i = 0; i < NUM_SHAPES; i++) {
        shapes[i].angle += shapes[i].angular_velocity;
        if (shapes[i].angle > 2 * PI) {
            shapes[i].angle -= 2 * PI;
        }
    }
}

void draw_rotating_shape(RotatingShape *shape) {
    double x = shape->center_x + shape->radius * cos(shape->angle);
    double y = shape->center_y + shape->radius * sin(shape->angle);

    switch (shape->shape_type) {
        case 0:
            gfx_circle((int)x, (int)y, shape->size);
            break;
        case 1:
            gfx_line((int)x - shape->size, (int)y - shape->size,
                    (int)x + shape->size, (int)y - shape->size);
            gfx_line((int)x + shape->size, (int)y - shape->size,
                    (int)x + shape->size, (int)y + shape->size);
            gfx_line((int)x + shape->size, (int)y + shape->size,
                    (int)x - shape->size, (int)y + shape->size);
            gfx_line((int)x - shape->size, (int)y + shape->size,
                    (int)x - shape->size, (int)y - shape->size);
            break;
        case 2:
            gfx_line((int)x, (int)y - shape->size,
                    (int)x + shape->size, (int)y + shape->size);
            gfx_line((int)x + shape->size, (int)y + shape->size,
                    (int)x - shape->size, (int)y + shape->size);
            gfx_line((int)x - shape->size, (int)y + shape->size,
                    (int)x, (int)y - shape->size);
            break;
        case 3:
            for (int i = 0; i < 5; i++) {
                double angle1 = 2 * PI * i / 5;
                double angle2 = 2 * PI * (i + 1) / 5;
                int x1 = (int)x + shape->size * cos(angle1);
                int y1 = (int)y + shape->size * sin(angle1);
                int x2 = (int)x + shape->size * cos(angle2);
                int y2 = (int)y + shape->size * sin(angle2);
                gfx_line(x1, y1, x2, y2);
            }
            break;
    }
}

// 修改 draw_spiral 使用小圆代替点（如果需要）
void draw_spiral(int center_x, int center_y, double time) {
    double max_radius = 100;
    int points = 50;

    for (int i = 0; i < points; i++) {
        double t = time + i * 0.1;
        double radius = max_radius * (i / (double)points);
        double x = center_x + radius * cos(t);
        double y = center_y + radius * sin(t);

        // 使用小圆（更明显）
        gfx_circle((int)x, (int)y, 2);
    }
}

void change_colors(RotatingShape shapes[]) {
    for (int i = 0; i < NUM_SHAPES; i++) {
        shapes[i].color_r = rand() % 256;
        shapes[i].color_g = rand() % 256;
        shapes[i].color_b = rand() % 256;
    }
}