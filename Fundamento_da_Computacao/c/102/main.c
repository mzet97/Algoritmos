#include "../bib1.h"

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int h = atoi(buf);
        if (!entrada(buf, sizeof(buf))) break;
        int m = atoi(buf);

        int minutes = m * 5;
        if (minutes == 60) minutes = 0;
        int hour = h;
        if (hour == 12) hour = 0;

        printf("%02d:%02d / %02d:%02d\n", hour, minutes, hour + 12, minutes);
    }
    return 0;
}
