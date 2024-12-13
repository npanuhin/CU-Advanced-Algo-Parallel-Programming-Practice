#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <iomanip>
#include <thread>


std::vector<std::vector<double>> read_matrix() {
    size_t rows, cols;
    std::cin >> rows >> cols;

    size_t a, b, x, y, z, p;
    std::cin >> a >> b >> x >> y >> z >> p;
    std::vector<std::vector<size_t>> intermediate(rows, std::vector<size_t>(cols, b % p));
    intermediate[0][0] = a % p;
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            if (i > 0 && j > 0) {
                intermediate[i][j] = (intermediate[i][j] + intermediate[i - 1][j - 1] * x) % p;
            }
            if (i > 0) {
                intermediate[i][j] = (intermediate[i][j] + intermediate[i - 1][j] * y) % p;
            }
            if (j > 0) {
                intermediate[i][j] = (intermediate[i][j] + intermediate[i][j - 1] * z) % p;
            }
        }
    }

    size_t max_value = 0;
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            max_value = std::max(max_value, intermediate[i][j]);
        }
    }

    std::vector<std::vector<double>> result(rows, std::vector<double>(cols));
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            result[i][j] = static_cast<double>(intermediate[i][j]) / static_cast<double>(max_value);
        }
    }

    return result;
}

void multiply_section(
    const std::vector<std::vector<double>>& left,
    const std::vector<std::vector<double>>& right,
    std::vector<std::vector<double>>& result,
    int start_row, int end_row
) {
    const auto left_cols = left[0].size();
    const auto right_cols = right[0].size();

    for (int i = start_row; i < end_row; ++i) {
        for (int j = 0; j < right_cols; ++j) {
            for (int k = 0; k < left_cols; ++k) {
                result[i][j] += left[i][k] * right[k][j];
            }
        }
    }
}

int main() {
    auto left = read_matrix();
    auto right = read_matrix();
    auto left_rows = left.size();
    auto left_cols = left[0].size();
    auto right_cols = right[0].size();

    if (left.empty() || right.empty() || left[0].size() != right.size()) {
        std::cerr << "Wrong matrices";
        return 1;
    }

    std::vector<std::vector<double>> result(left_rows, std::vector<double>(right_cols));

    const int num_threads = std::thread::hardware_concurrency();
    const int chunk_size = (left_rows + num_threads - 1) / num_threads;
    std::vector<std::thread> threads;

    for (int t = 0; t < num_threads; ++t) {
        int start_row = t * chunk_size;
        int end_row = std::min(start_row + chunk_size, static_cast<int>(left_rows));
        if (start_row < end_row) {
            threads.emplace_back(multiply_section, std::cref(left), std::cref(right), std::ref(result), start_row, end_row);
        }
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << left_rows << ' ' << right_cols << "\n";
    for (int i = 0; i < left_rows; ++i) {
        for (int j = 0; j < right_cols; ++j) {
            std::cout << std::setprecision(12) << result[i][j] << ' ';
        }
        std::cout << "\n";
    }

    return 0;
}
