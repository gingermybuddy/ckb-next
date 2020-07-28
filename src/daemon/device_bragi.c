#include "command.h"
#include "device.h"
#include "devnode.h"
#include "firmware.h"
#include "input.h"
#include "profile.h"
#include "usb.h"
#include "bragi_proto.h"

int start_mouse_bragi(usbdevice* kb, int makeactive){
    (void)makeactive;

#warning "FIXME"
    uchar pollrateLUT[5] = {-1};
    pollrateLUT[BRAGI_POLLRATE_1MS] = 1;
    pollrateLUT[BRAGI_POLLRATE_2MS] = 2;
    pollrateLUT[BRAGI_POLLRATE_4MS] = 4;
    pollrateLUT[BRAGI_POLLRATE_8MS] = 8;
    // Get pollrate
    uchar pkt[64] = {BRAGI_MAGIC, BRAGI_GET, BRAGI_POLLRATE, 0};
    uchar response[64] = {0};
    if(!usbrecv(kb, pkt, response))
        return 1;
    
    uchar pollrate = response[3];
    if(pollrate > 4)
        return 1;
    
    kb->pollrate = pollrateLUT[pollrate];
    
    kb->features |= FEAT_ADJRATE;
    kb->features &= ~FEAT_HWLOAD;

    return 0;
}

int cmd_pollrate_bragi(usbdevice* kb, usbmode* dummy1, int dummy2, int rate, const char* dummy3){
    (void)dummy1;
    (void)dummy2;
    (void)dummy3;
    
    if(rate > 8 || rate < 0)
        return 0;
    
    uchar pollrateLUT[9] = {
        1,
        BRAGI_POLLRATE_1MS,
        BRAGI_POLLRATE_2MS,
        1,
        BRAGI_POLLRATE_4MS,
        1,
        1,
        1,
        BRAGI_POLLRATE_8MS,
    };

    uchar pkt[64] = {BRAGI_MAGIC, BRAGI_SET, BRAGI_POLLRATE, 0, pollrateLUT[rate]};
    uchar response[64] = {0};
    if(!usbrecv(kb, pkt, response))
        return 1;
    
    kb->pollrate = rate;
    return 0;
}

int setactive_bragi(usbdevice* kb, int active){
    uchar pkt[64] = {BRAGI_MAGIC, BRAGI_SET, BRAGI_MODE, 0, active};
    uchar response[64] = {0};
    if(!usbrecv(kb, pkt, response))
        return 1;
    kb->active = active - 1;
    return 0;
}

int cmd_active_bragi(usbdevice* kb, usbmode* dummy1, int dummy2, int dummy3, const char* dummy4){
    (void)dummy1;
    (void)dummy2;
    (void)dummy3;
    (void)dummy4;

    return setactive_bragi(kb, BRAGI_MODE_SOFTWARE);
}

int cmd_idle_bragi(usbdevice* kb, usbmode* dummy1, int dummy2, int dummy3, const char* dummy4){
    (void)dummy1;
    (void)dummy2;
    (void)dummy3;
    (void)dummy4;

    return setactive_bragi(kb, BRAGI_MODE_HARDWARE);
}
