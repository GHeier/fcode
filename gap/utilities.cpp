#include <math.h>
#include <iostream>
#include <iomanip>

void progress_bar(double progress) {
    int barWidth = 70;

    std::cout << "[";
        int pos = barWidth * progress;
            for (int i = 0; i < barWidth; ++i) {
                        if (i < pos) std::cout << "=";
                                else if (i == pos) std::cout << ">";
                                        else std::cout << " ";
                                            }
    std::cout << "] " << std::fixed << std::setprecision(2) << progress * 100.0 << " %  \r";
        std::cout.flush();
}

void progress_percent(double progress) {
    std::cout << std::setprecision(8) << progress << "    \r";
    std::cout.flush();
}

double round(double number, int decimal_places) {
    return round(number * pow(10, decimal_places)) / pow(10, decimal_places);
}

