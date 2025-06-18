// Matrix.cpp
#include "Matrix.hpp"
#include <stdexcept>

// ctors
Matrix::Matrix() : data(0, 0) {}

Matrix::Matrix(int rows, int cols) : data(rows, cols) {
    data.setZero();
}

Matrix::Matrix(int rows, int cols, double initialValue) : data(rows, cols) {
    data.setConstant(initialValue);
}

Matrix::Matrix(const std::vector<std::vector<double>>& input) {
    if (input.empty()) {
        data = Eigen::MatrixXd(0, 0);
        return;
    }
    
    int rows = input.size();
    int cols = input[0].size();
    
    // Check if all rows have the same length
    for (const auto& row : input) {
        if (row.size() != static_cast<size_t>(cols)) {
            throw std::invalid_argument("All rows must have the same length");
        }
    }
    
    data = Eigen::MatrixXd(rows, cols);
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            data(i, j) = input[i][j];
        }
    }
}

// copy ctor & assignment operator
Matrix::Matrix(const Matrix& other) : data(other.data) {}

Matrix& Matrix::operator=(const Matrix& other) {
    if (this != &other) {
        data = other.data;
    }
    return *this;
}

// Getters & Setters
int Matrix::getRows() const {
    return data.rows();
}

int Matrix::getCols() const {
    return data.cols();
}

double Matrix::get(int row, int col) const {
    if (row < 0 || row >= data.rows() || col < 0 || col >= data.cols()) {
        throw std::out_of_range("Matrix indices out of range");
    }
    return data(row, col);
}

void Matrix::set(int row, int col, double value) {
    if (row < 0 || row >= data.rows() || col < 0 || col >= data.cols()) {
        throw std::out_of_range("Matrix indices out of range");
    }
    data(row, col) = value;
}

// Operations
Matrix Matrix::transpose() const {
    Matrix result;
    result.data = data.transpose();
    return result;
}

Matrix Matrix::inverse() const {
    if (!isSquare()) {
        throw std::invalid_argument("Matrix must be square to compute inverse");
    }
    
    Matrix result;
    result.data = data.inverse();
    return result;
}

Matrix Matrix::multiply(const Matrix& other) const {
    if (data.cols() != other.data.rows()) {
        throw std::invalid_argument("Matrix dimensions do not match for multiplication");
    }
    
    Matrix result;
    result.data = data * other.data;
    return result;
}

// Operators
Matrix Matrix::operator+(const Matrix& other) const {
    if (data.rows() != other.data.rows() || data.cols() != other.data.cols()) {
        throw std::invalid_argument("Matrix dimensions do not match for addition");
    }

    Matrix result;
    result.data = data + other.data;
    return result;
}

Matrix Matrix::operator-(const Matrix& other) const {
    if (data.rows() != other.data.rows() || data.cols() != other.data.cols()) {
        throw std::invalid_argument("Matrix dimensions do not match for subtraction");
    }
    
    Matrix result;
    result.data = data - other.data;
    return result;
}

Matrix Matrix::operator*(const Matrix& other) const {
    return multiply(other);
}

Matrix Matrix::operator*(double scalar) const {
    Matrix result;
    result.data = data * scalar;
    return result;
}

// Methods
bool Matrix::isSquare() const {
    return data.rows() == data.cols();
}

double Matrix::determinant() const {
    if (!isSquare()) {
        throw std::invalid_argument("Determinant is defined only for square matrices");
    }
    return data.determinant();
}

std::vector<std::vector<double>> Matrix::toVector() const {
    std::vector<std::vector<double>> result(data.rows(), std::vector<double>(data.cols()));
    
    for (int i = 0; i < data.rows(); ++i) {
        for (int j = 0; j < data.cols(); ++j) {
            result[i][j] = data(i, j);
        }
    }
    
    return result;
}

// Static methods
Matrix Matrix::identity(int size) {
    Matrix result;
    result.data = Eigen::MatrixXd::Identity(size, size);
    return result;
}

Matrix Matrix::zeros(int rows, int cols) {
    Matrix result;
    result.data = Eigen::MatrixXd::Zero(rows, cols);
    return result;
}

Matrix Matrix::ones(int rows, int cols) {
    Matrix result;
    result.data = Eigen::MatrixXd::Ones(rows, cols);
    return result;
}