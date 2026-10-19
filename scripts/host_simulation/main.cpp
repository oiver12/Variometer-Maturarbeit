// Execute the unmodified sketch, then its commented-out regression experiment.
#include "Arduino.h"
#include <iostream>
#include <algorithm>
Print Serial;
#include "SimulationArduino.ino"
int main() {
    setup();
    if (K.status) return 1;
    std::cout << "KALMAN_BEGIN\n" << Serial.buffer.str() << "KALMAN_END\nREGRESSION_BEGIN\n";
    unsigned extra = 0;
    for (size_t i = 0; i < sizeof(times)/sizeof(times[0]); ++i) {
        Serial.clear();
        variometer.addSample(measHeights[i], times[i]);
        const auto output = Serial.buffer.str();
        auto count = std::count(output.begin(), output.end(), ',');
        if (count < 1 || count > 2) return 2;
        extra += count - 1;
        std::cout << std::setprecision(10) << times[i] << ',' << measHeights[i] << ',' << measAccelerations[i] << ',' << std::stod(output) << '\n';
    }
    std::cerr << "Ignored extra startup zero prints: " << extra << '\n';
}
