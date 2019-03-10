#include "ev3api.h"
#include "app.h"

static int32_t fontw, fonth;

static uint_t cyc1_count = 0;
static uint_t cyc2_count = 0;

void cyc1_led_task(intptr_t unused) {
    if (((++cyc1_count) % 2) == 0) {
        ev3_led_set_color(LED_GREEN);
    } else {
        ev3_led_set_color(LED_OFF);
    }

    ev3_lcd_fill_rect(0, fonth * 1, EV3_LCD_WIDTH, fonth, EV3_LCD_WHITE);
    char buf[100];
    sprintf(buf, "CYC1 COUNT %d", cyc1_count);
    ev3_lcd_draw_string(buf, 0, fonth * 1);
}

void main_task(intptr_t unused) {
    ev3_lcd_set_font(EV3_FONT_MEDIUM);
    ev3_font_get_size(EV3_FONT_MEDIUM, &fontw, &fonth);

#if 0
    memfile_t memfile;
    ev3_memfile_load("/test.bmp", &memfile);
    image_t image;
    ev3_image_load(&memfile, &image);
    ev3_lcd_draw_image(&image, 0, fonth * 2);
#endif

    syslog(LOG_NOTICE, "TEST DTQ");
    intptr_t data = 0xdeadbeef;
    SVC_PERROR(snd_dtq(DTQ1, data));
    data = 0;
    SVC_PERROR(rcv_dtq(DTQ1, &data));
    assert(data == 0xdeadbeef);

    syslog(LOG_NOTICE, "TEST PDQ");
    data = 0xdeadbeef;
    SVC_PERROR(snd_pdq(PDQ1, data, 1));
    data = 0xdeadbee2;
    SVC_PERROR(snd_pdq(PDQ1, data, 2));
    PRI datapri;
    SVC_PERROR(rcv_pdq(PDQ1, &data, &datapri));
    assert(data == 0xdeadbeef && datapri == 1);
    SVC_PERROR(rcv_pdq(PDQ1, &data, &datapri));
    assert(data == 0xdeadbee2 && datapri == 2);

    syslog(LOG_NOTICE, "TEST MTX");
    SVC_PERROR(loc_mtx(MTX1));
    SVC_PERROR(unl_mtx(MTX1));

    syslog(LOG_NOTICE, "TEST DONE");

    SYSTIM tim1, tim2;
    while (1) {
        SVC_PERROR(get_tim(&tim1));
        SVC_PERROR(sta_cyc(CYC2));
        do {
            SVC_PERROR(wai_sem(CYC2_CNT_SEM));
            ev3_lcd_fill_rect(0, fonth * 2, EV3_LCD_WIDTH, fonth, EV3_LCD_WHITE);
            char buf[100];
            sprintf(buf, "CYC2 COUNT %d", ++cyc2_count);
            ev3_lcd_draw_string(buf, 0, fonth * 2);
        } while ((cyc2_count % 10) != 0);
        SVC_PERROR(stp_cyc(CYC2));
        SVC_PERROR(get_tim(&tim2));
        SVC_PERROR(dly_tsk(10000U * 1000U - (tim2 - tim1)));
    }
}
