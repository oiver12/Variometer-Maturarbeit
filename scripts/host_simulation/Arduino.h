// Host-only Serial/String shim; filter code stays in SimulationArduino/.
#pragma once
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
using std::isnan;
using std::isinf;
using std::isfinite;
#define F(message) message
inline std::string String(double value, int digits) {
    std::ostringstream output;
    output << std::fixed << std::setprecision(digits) << value;
    return output.str();
}
class Print {
public:
    std::ostringstream buffer;
    void begin(unsigned long) {}
    template<class T> void print(const T &value) { buffer << value; }
    template<class T> void println(const T &value) { buffer << value << '\n'; }
    void clear() { buffer.str(""); buffer.clear(); }
};
extern Print Serial;
