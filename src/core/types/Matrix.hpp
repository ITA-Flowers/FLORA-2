// Matrix.hpp
#pragma once

#include <vector>
#include <Eigen/Dense>

class Matrix {
public:
    // ctors
    Matrix();
    Matrix(int rows, int cols);
    Matrix(int rows, int cols, double initialValue);
    Matrix(const std::vector<std::vector<double>>& data);
    
    // copy ctor & assignment operator
    Matrix(const Matrix& other);
    Matrix& operator=(const Matrix& other);
    
    // dtor
    ~Matrix() = default;
    
    // Getters & Setters
    int getRows() const;
    int getCols() const;
    double get(int row, int col) const;
    void set(int row, int col, double value);
    
    // Operations
    Matrix transpose() const;
    Matrix inverse() const;
    Matrix multiply(const Matrix& other) const;
    
    // Operators
    Matrix operator+(const Matrix& other) const;
    Matrix operator-(const Matrix& other) const;
    Matrix operator*(const Matrix& other) const;
    Matrix operator*(double scalar) const;
    
    // Methods
    bool isSquare() const;
    double determinant() const;
    std::vector<std::vector<double>> toVector() const;
    
    // Static methods
    static Matrix identity(int size);
    static Matrix zeros(int rows, int cols);
    static Matrix ones(int rows, int cols);

private:
    Eigen::MatrixXd data;
};