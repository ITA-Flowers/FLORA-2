#pragma once

class Kalman1D {
public:
    Kalman1D(float processNoise = 1e-5, float measurementNoise = 1e-2, float estimateError = 1, float initialValue = 0);
    float update(float measurement);

private:
    float q; 
    float r;
    float p; 
    float x;
    float k;
};
