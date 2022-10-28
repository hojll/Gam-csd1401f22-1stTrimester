#ifndef UI_H
#define UI_H

void set_timer(float time_in_seconds);

void update_timer(void);

void bar_system(float x, float y, float width, float height, int value, int total_value);

void update_bullet_bar(int current_bullet_count, int total_bullet_count);
#endif // !UI_H

