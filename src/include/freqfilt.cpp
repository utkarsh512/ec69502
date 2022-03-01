#include "freqfilt.h"

const double EPS = 1e-8;

// FFT and IFFT implementation

// performs FFT/IFFT over a sequence in-place 
// `invert` should be true for IFFT, otherwise false
void fft(std::vector<cd>& seq, bool invert) {
	int n = seq.size();

    // applying bit-reversal permutation to perform in-place FFT/IFFT
    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1)
            j ^= bit;
        j ^= bit;
        if (i < j) std::swap(seq[i], seq[j]);
    }

    for (int len = 2; len <= n; len <<= 1) {
        double ang = 2 * PI / len * (invert ? -1 : 1);
        cd wlen(cos(ang), sin(ang));
        for (int i = 0; i < n; i += len) {
            cd w(1);
            for (int j = 0; j < len / 2; j++) {
                cd u = seq[i + j], v = seq[i + j + len / 2] * w;
                seq[i + j] = u + v;
                seq[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }
    if (invert) {
        for (cd& x : seq)
            x /= n;
    }
}

// performs FFT/IFFT over a matrix
// `invert` should be true for IFFT, otherwise false
void fft(std::vector<std::vector<cd>>& mat, bool invert) {
    // applying operation over rows
    for (auto& row : mat) fft(row, invert);
    transpose(mat);

    // applying operation over rows of transposed matrix
    // which is equivalent to applying over columns of the original matrix
    for (auto& row : mat) fft(row, invert);

    // copying the matrix back
    transpose(mat);
}

// shifts the spectrum
void fftshift(std::vector<std::vector<cd>>& mat) {
    int n = mat.size();
    int offset = n / 2;
    for (int i = 0; i < offset; i++) {
        for (int j = 0; j < offset; j++) {
            std::swap(mat[i][j], mat[i + offset][j + offset]);
        }
    }
    for (int i = offset; i < n; i++) {
        for (int j = 0; j < offset; j++) {
            std::swap(mat[i][j], mat[i - offset][j + offset]);
        }
    }
}


// filter implementation

// ideal filter 
void ideal(std::vector<std::vector<cd>>& filter, double d, int order, bool high) {
    int n = filter.size();
    d *= d;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double dist = (i - n / 2) * (i - n / 2) + (j - n / 2) * (j - n / 2);
            double val = (dist < d) ? 1 : 0;
            if (high) val = 1.0 - val;
            filter[i][j] = cd(val, val);
        }
    }
}

// gaussian filter
void gaussian(std::vector<std::vector<cd>>& filter, double d, int order, bool high) {
    int n = filter.size();
    d *= d; d = std::max(d, EPS);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double dist = (i - n / 2) * (i - n / 2) + (j - n / 2) * (j - n / 2);
            double val = exp(-dist / d * 2);
            if (high) val = 1.0 - val;
            filter[i][j] = cd(val, val);
        }
    }
}

// butterworth filter
void butterworth(std::vector<std::vector<cd>>& filter, double d, int order, bool high) {
    int n = filter.size();
    d *= d; d = std::max(d, EPS);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double dist = (i - n / 2) * (i - n / 2) + (j - n / 2) * (j - n / 2);
            double val = 1.0 / (1.0 + pow(dist / d, order));
            if (high) val = 1.0 - val;
            filter[i][j] = cd(val, val);
        }
    }
}

// helper function implementation

// performs transposition of matrix
void transpose(std::vector<std::vector<cd>>& mat) {
    int n = mat.size();
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            std::swap(mat[i][j], mat[j][i]);
        }
    }
}

// replaces the atomic entries in container with its conjugate
void conjugate(std::vector<std::vector<cd>>& mat) {
    for (auto& row : mat) {
        for (auto& val : row) val = std::conj(val);
    }
}

// perform (-1)^(i + j) operation over a matrix
void cofactor(std::vector<std::vector<cd>>& mat) {
    int n = mat.size();
    int m = mat[0].size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if ((i + j) & 1) mat[i][j] *= -1;
        }
    }
}

// reads 2D std::vector object from cv::Mat object
std::vector<std::vector<cd>> readVector(const cv::Mat& image) {
    int n = image.rows;
    int m = image.cols;
    std::vector<std::vector<cd>> res = std::vector<std::vector<cd>>(n, std::vector<cd>(m));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            res[i][j] = cd(image.at<uchar>(i, j), 0);
            res[i][j] /= 255;
        }
    }
    return res;
}

// writes 2-D std::vector object to cv::Mat object
// `mag` should be true if we need to consider magnitude of complex numbers
// otherwise, only real part is considered
void writeVector(cv::Mat& image, const std::vector<std::vector<cd>>& mat, bool mag) {
    int n = mat.size();
    int m = mat[0].size();
    assert(n == image.rows && m == image.cols);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            double value = mag ? hypot(mat[i][j].real(), mat[i][j].imag()) : abs(mat[i][j].real());
            value *= 255;
            image.at<uchar>(i, j) = std::min(255, static_cast<int>(value));
        }
    }
}


// return dot product of two complex numbers
cd dot(const cd& a, const cd& b) {
    return cd(a.real() * b.real(), a.imag() * b.imag());
}
