#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <thread>
#include <mutex>
#include <cmath>

std::mutex mtx;

std::istream& operator>>(std::istream& in, __int128& value) {
    std::string s;
    in >> s;
    value = 0;
    bool negative = false;
    size_t i = 0;
    if (s[0] == '-') {
        negative = true;
        i = 1;
    }
    for (; i < s.size(); ++i) {
        value = value * 10 + (s[i] - '0');
    }
    if (negative) value = -value;
    return in;
}

std::ostream& operator<<(std::ostream& out, __int128 value) {
    if (value == 0) {
        out << '0';
        return out;
    }
    std::string s;
    bool negative = false;
    if (value < 0) {
        negative = true;
        value = -value;
    }
    while (value > 0) {
        s += '0' + static_cast<int>(value % 10);
        value /= 10;
    }
    if (negative) s += '-';
    std::reverse(s.begin(), s.end());
    out << s;
    return out;
}

void factorize(__int128& n, __int128 start, __int128 end, std::vector<__int128>& factors) {
    for (__int128 p = start; p <= end; ++p) {
        if (n % p == 0) {
            std::lock_guard<std::mutex> lock(mtx);
            while (n % p == 0) {
                factors.push_back(p);
                n /= p;
            }
        }
    }
}

int main() {
    __int128 n;
    std::cin >> n;
    if (n <= 1) {
        return 0;
    }

    __int128 n1 = n;

    std::vector<__int128> factors;
    const int num_threads = std::thread::hardware_concurrency();;
    std::vector<std::thread> threads;

    __int128 end = static_cast<__int128>(std::sqrt(static_cast<double>(n))) + 1;
    __int128 chunk_size = end / num_threads + 1;

    for (int i = 0; i < num_threads; ++i) {
        __int128 cur_start = 2 + i * chunk_size;
        __int128 cur_end = std::min(cur_start + chunk_size - 1, end);
        threads.emplace_back(factorize, std::ref(n), cur_start, cur_end, std::ref(factors));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    if (n > 1) {
        factors.push_back(n);
    }

    sort(factors.begin(), factors.end());

    std::vector<__int128> new_factors;

    for (auto& factor : factors) {
        __int128 end = static_cast<__int128>(std::sqrt(static_cast<double>(factor))) + 1;
        factorize(factor, 2, end, new_factors);
        if (factor > 1) {
            new_factors.push_back(factor);
        }
    }

    for (const auto& factor : new_factors) {
        std::cout << factor << ' ';
    }
    std::cout << '\n';

    return 0;
}
