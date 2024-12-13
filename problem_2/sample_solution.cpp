#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <iomanip>

std::vector<size_t> read_array() {
    size_t length, a, b, p;
    std::cin >> length >> a >> b >> p;
    std::vector<size_t> result(length);
    result[0] = a % p;
    for (size_t i = 1; i < result.size(); ++i) {
        result[i] = (result[i - 1] * a + b) % p;
    }
    return result;
}

void sort_section(std::vector<size_t>& array, size_t start, size_t end) {
    std::sort(array.begin() + start, array.begin() + end);
}

void merge_sections(std::vector<size_t>& array, size_t start1, size_t end1, size_t start2, size_t end2) {
    std::vector<size_t> merged;
    size_t i = start1, j = start2;

    while (i < end1 && j < end2) {
        if (array[i] <= array[j]) {
            merged.push_back(array[i++]);
        } else {
            merged.push_back(array[j++]);
        }
    }
    while (i < end1) merged.push_back(array[i++]);
    while (j < end2) merged.push_back(array[j++]);

    std::copy(merged.begin(), merged.end(), array.begin() + start1);
}

void parallel_sort(std::vector<size_t>& array, int num_threads) {
    size_t chunk_size = (array.size() + num_threads - 1) / num_threads;
    std::vector<std::thread> threads;

    for (int t = 0; t < num_threads; ++t) {
        size_t start = t * chunk_size;
        size_t end = std::min(start + chunk_size, array.size());
        if (start < end) {
            threads.emplace_back(sort_section, std::ref(array), start, end);
        }
    }

    for (auto& thread : threads) {
        thread.join();
    }

    size_t step = chunk_size;
    while (step < array.size()) {
        for (size_t i = 0; i < array.size(); i += 2 * step) {
            size_t start1 = i;
            size_t end1 = std::min(start1 + step, array.size());
            size_t start2 = end1;
            size_t end2 = std::min(start2 + step, array.size());

            if (start2 < end2) {
                merge_sections(array, start1, end1, start2, end2);
            }
        }
        step *= 2;
    }
}

int main() {
    auto array = read_array();

    int num_threads = std::thread::hardware_concurrency();
    parallel_sort(array, num_threads);

    size_t k;
    std::cin >> k;
    for (size_t i = k - 1; i < array.size(); i += k) {
        std::cout << array[i] << ' ';
    }
    std::cout << "\n";

    return 0;
}
