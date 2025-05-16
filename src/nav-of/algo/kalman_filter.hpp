#pragma once

class Kalman1D {
public:
    Kalman1D() : x(0.0f), p(1.0f), q(0.01f), r(0.1f) {}

    float update(float measurement) {
        p += q;
        float k = p / (p + r);
        x += k * (measurement - x);
        p *= (1 - k);
        return x;
    }

private:
    float x, p, q, r;
};
