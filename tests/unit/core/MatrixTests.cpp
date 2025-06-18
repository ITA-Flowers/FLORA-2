#include <gtest/gtest.h>
#include "core/Matrix.hpp"
#include <cmath>

class MatrixTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Config before each test
    }

    void TearDown() override {
        // Clean up after each test
    }
};

// Default constructor
TEST_F(MatrixTest, DefaultConstructor) {
    Matrix m;
    EXPECT_EQ(m.getRows(), 0);
    EXPECT_EQ(m.getCols(), 0);
}

// Dimension constructor
TEST_F(MatrixTest, DimensionConstructor) {
    Matrix m(3, 4);
    EXPECT_EQ(m.getRows(), 3);
    EXPECT_EQ(m.getCols(), 4);
    
    // Check if all elements are zero
    for (int i = 0; i < m.getRows(); ++i) {
        for (int j = 0; j < m.getCols(); ++j) {
            EXPECT_DOUBLE_EQ(m.get(i, j), 0.0);
        }
    }
}

// Initial value constructor
TEST_F(MatrixTest, ValueConstructor) {
    double initialValue = 2.5;
    Matrix m(2, 3, initialValue);
    EXPECT_EQ(m.getRows(), 2);
    EXPECT_EQ(m.getCols(), 3);
    
    // Check if all elements are equal to the initial value
    for (int i = 0; i < m.getRows(); ++i) {
        for (int j = 0; j < m.getCols(); ++j) {
            EXPECT_DOUBLE_EQ(m.get(i, j), initialValue);
        }
    }
}

// Vector constructor
TEST_F(MatrixTest, VectorConstructor) {
    std::vector<std::vector<double>> data = {
        {1.0, 2.0, 3.0},
        {4.0, 5.0, 6.0}
    };
    
    Matrix m(data);
    EXPECT_EQ(m.getRows(), 2);
    EXPECT_EQ(m.getCols(), 3);
    
    for (int i = 0; i < m.getRows(); ++i) {
        for (int j = 0; j < m.getCols(); ++j) {
            EXPECT_DOUBLE_EQ(m.get(i, j), data[i][j]);
        }
    }
}

// Getters and setters
TEST_F(MatrixTest, SetAndGet) {
    Matrix m(2, 2);
    m.set(0, 0, 1.0);
    m.set(0, 1, 2.0);
    m.set(1, 0, 3.0);
    m.set(1, 1, 4.0);
    
    EXPECT_DOUBLE_EQ(m.get(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(m.get(0, 1), 2.0);
    EXPECT_DOUBLE_EQ(m.get(1, 0), 3.0);
    EXPECT_DOUBLE_EQ(m.get(1, 1), 4.0);
    
    // Out of range exceptions
    EXPECT_THROW(m.get(-1, 0), std::out_of_range);
    EXPECT_THROW(m.get(0, -1), std::out_of_range);
    EXPECT_THROW(m.get(2, 0), std::out_of_range);
    EXPECT_THROW(m.get(0, 2), std::out_of_range);
    
    EXPECT_THROW(m.set(-1, 0, 0.0), std::out_of_range);
    EXPECT_THROW(m.set(0, -1, 0.0), std::out_of_range);
    EXPECT_THROW(m.set(2, 0, 0.0), std::out_of_range);
    EXPECT_THROW(m.set(0, 2, 0.0), std::out_of_range);
}

// Transpose
TEST_F(MatrixTest, Transpose) {
    Matrix m({
        {1.0, 2.0, 3.0},
        {4.0, 5.0, 6.0}
    });
    
    Matrix mt = m.transpose();
    EXPECT_EQ(mt.getRows(), 3);
    EXPECT_EQ(mt.getCols(), 2);
    
    EXPECT_DOUBLE_EQ(mt.get(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(mt.get(0, 1), 4.0);
    EXPECT_DOUBLE_EQ(mt.get(1, 0), 2.0);
    EXPECT_DOUBLE_EQ(mt.get(1, 1), 5.0);
    EXPECT_DOUBLE_EQ(mt.get(2, 0), 3.0);
    EXPECT_DOUBLE_EQ(mt.get(2, 1), 6.0);
}

// Multiplication
TEST_F(MatrixTest, Multiplication) {
    Matrix a({
        {1.0, 2.0, 3.0},
        {4.0, 5.0, 6.0}
    });
    
    Matrix b({
        {7.0, 8.0},
        {9.0, 10.0},
        {11.0, 12.0}
    });
    
    Matrix c = a.multiply(b);
    EXPECT_EQ(c.getRows(), 2);
    EXPECT_EQ(c.getCols(), 2);
    
    EXPECT_DOUBLE_EQ(c.get(0, 0), 1.0*7.0 + 2.0*9.0 + 3.0*11.0);
    EXPECT_DOUBLE_EQ(c.get(0, 1), 1.0*8.0 + 2.0*10.0 + 3.0*12.0);
    EXPECT_DOUBLE_EQ(c.get(1, 0), 4.0*7.0 + 5.0*9.0 + 6.0*11.0);
    EXPECT_DOUBLE_EQ(c.get(1, 1), 4.0*8.0 + 5.0*10.0 + 6.0*12.0);
    
    // Operator *
    Matrix d = a * b;
    EXPECT_EQ(d.getRows(), c.getRows());
    EXPECT_EQ(d.getCols(), c.getCols());
    
    for (int i = 0; i < d.getRows(); ++i) {
        for (int j = 0; j < d.getCols(); ++j) {
            EXPECT_DOUBLE_EQ(d.get(i, j), c.get(i, j));
        }
    }
    
    // Non-matching dimensions
    Matrix e(2, 3);
    EXPECT_THROW(e.multiply(a), std::invalid_argument);
}

// Inverse
TEST_F(MatrixTest, Inverse) {
    Matrix m({
        {4.0, 7.0},
        {2.0, 6.0}
    });
    
    Matrix mi = m.inverse();
    
    // determinant
    double det = m.get(0, 0) * m.get(1, 1) - m.get(0, 1) * m.get(1, 0);
    
    // Check if m^(-1) = 1/det * adj(m)
    EXPECT_DOUBLE_EQ(mi.get(0, 0), m.get(1, 1) / det);
    EXPECT_DOUBLE_EQ(mi.get(0, 1), -m.get(0, 1) / det);
    EXPECT_DOUBLE_EQ(mi.get(1, 0), -m.get(1, 0) / det);
    EXPECT_DOUBLE_EQ(mi.get(1, 1), m.get(0, 0) / det);
    
    // Check if m * m^(-1) = I
    Matrix identity = m * mi;
    EXPECT_NEAR(identity.get(0, 0), 1.0, 1e-10);
    EXPECT_NEAR(identity.get(0, 1), 0.0, 1e-10);
    EXPECT_NEAR(identity.get(1, 0), 0.0, 1e-10);
    EXPECT_NEAR(identity.get(1, 1), 1.0, 1e-10);
    
    // Non-square matrix exception
    Matrix nonSquare(2, 3);
    EXPECT_THROW(nonSquare.inverse(), std::invalid_argument);
}

// Addition
TEST_F(MatrixTest, Addition) {
    Matrix a({
        {1.0, 2.0},
        {3.0, 4.0}
    });
    
    Matrix b({
        {5.0, 6.0},
        {7.0, 8.0}
    });
    
    Matrix c = a + b;
    EXPECT_EQ(c.getRows(), 2);
    EXPECT_EQ(c.getCols(), 2);
    
    EXPECT_DOUBLE_EQ(c.get(0, 0), 6.0);
    EXPECT_DOUBLE_EQ(c.get(0, 1), 8.0);
    EXPECT_DOUBLE_EQ(c.get(1, 0), 10.0);
    EXPECT_DOUBLE_EQ(c.get(1, 1), 12.0);
    
    // Non-matching dimensions
    Matrix d(3, 2);
    EXPECT_THROW(a + d, std::invalid_argument);
}

// Special matrices
TEST_F(MatrixTest, SpecialMatrices) {
    // Identity matrix
    Matrix identity = Matrix::identity(3);
    EXPECT_EQ(identity.getRows(), 3);
    EXPECT_EQ(identity.getCols(), 3);
    
    for (int i = 0; i < identity.getRows(); ++i) {
        for (int j = 0; j < identity.getCols(); ++j) {
            if (i == j) {
                EXPECT_DOUBLE_EQ(identity.get(i, j), 1.0);
            } else {
                EXPECT_DOUBLE_EQ(identity.get(i, j), 0.0);
            }
        }
    }
    
    // Zero matrix
    Matrix zeros = Matrix::zeros(2, 3);
    EXPECT_EQ(zeros.getRows(), 2);
    EXPECT_EQ(zeros.getCols(), 3);
    
    for (int i = 0; i < zeros.getRows(); ++i) {
        for (int j = 0; j < zeros.getCols(); ++j) {
            EXPECT_DOUBLE_EQ(zeros.get(i, j), 0.0);
        }
    }
    
    // Ones matrix
    Matrix ones = Matrix::ones(4, 2);
    EXPECT_EQ(ones.getRows(), 4);
    EXPECT_EQ(ones.getCols(), 2);
    
    for (int i = 0; i < ones.getRows(); ++i) {
        for (int j = 0; j < ones.getCols(); ++j) {
            EXPECT_DOUBLE_EQ(ones.get(i, j), 1.0);
        }
    }
}