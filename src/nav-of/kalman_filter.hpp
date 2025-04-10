#pragma once

class Kalman1D {
public:
    Kalman1D(float processNoise = 1e-5f, float measurementNoise = 1e-2f, float estimateError = 1.0f, float initialValue = 0.0f);
    float update(float measurement);

private:
    float q, r, p, x, k;
};
