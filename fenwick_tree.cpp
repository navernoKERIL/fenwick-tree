#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>

using namespace std;
using namespace std::chrono;

// Вспомогательная функция для LSB
int lsb(int x) 
{
    return x & -x;
}

// ==================== ДЕРЕВО ФЕНВИКА ====================
class Fenwick1D 
{
    vector<int> fenw;
    int n;
public:
    Fenwick1D(int size) : fenw(size + 1, 0), n(size) {} 
    
    void update(int i, int delta) {
        for (i = i + 1; i <= n; i += lsb(i)) {
            fenw[i] += delta;
        }
    }
    
    int prefix_sum(int i) {
        int sum = 0;
        for (i = i + 1; i > 0; i -= lsb(i)) {
            sum += fenw[i];
        }
        return sum;
    }
    
    int range_sum(int l, int r) {
        if (l > r) return 0;
        return prefix_sum(r) - prefix_sum(l - 1);
    }
};

// ==================== НАИВНЫЕ РЕАЛИЗАЦИИ ====================
class Naive1D_SumOnQuery 
{
    vector<int> arr;
public:
    Naive1D_SumOnQuery(int size) : arr(size, 0) {}
    
    void update(int i, int delta) {
        arr[i] += delta;
    }
    
    int prefix_sum(int i) {
        int sum = 0;
        for (int j = 0; j <= i; j++) {
            sum += arr[j];
        }
        return sum;
    }
};

class Naive1D_PrefixArray 
{
    vector<int> arr;
    vector<int> prefix;
public:
    Naive1D_PrefixArray(int size) : arr(size, 0), prefix(size + 1, 0) {}
    
    void update(int i, int delta) {
        arr[i] += delta;

        for (size_t j = i + 1; j < prefix.size(); j++) {  
            prefix[j] = prefix[j - 1] + arr[j - 1];
        }
    }
    
    int prefix_sum(int i) {
        return prefix[i + 1];
    }
};

// ==================== БЕНЧМАРК ====================
void run_benchmark() 
{
    const int n = 100000;        // Размер массива
    const int operations = 50000; // Общее количество операций
    
    cout << "Размер массива: " << n << endl;
    cout << "Количество операций: " << operations << endl << endl;
    
    // Инициализация структур
    Fenwick1D fenw(n);
    Naive1D_SumOnQuery naive1(n);
    Naive1D_PrefixArray naive2(n);
    
    // Генератор случайных чисел
    mt19937 rng(42);
    uniform_int_distribution<int> idx_dist(0, n - 1);
    uniform_int_distribution<int> op_dist(0, 1);
    uniform_int_distribution<int> delta_dist(1, 10);
    
    // ========== ТЕСТ 1: ТОЛЬКО ЗАПРОСЫ ==========
    cout << "=== ТЕСТ 1: 100% запросов ===" << endl;
    
    // Фенвик
    auto start = high_resolution_clock::now();
    for (int i = 0; i < operations; i++) {
        int idx = idx_dist(rng);
        fenw.prefix_sum(idx);
    }
    auto end = high_resolution_clock::now();
    auto fenw_time = duration_cast<milliseconds>(end - start);
    
    // Наивный 1 (сумма на запросе)
    start = high_resolution_clock::now();
    for (int i = 0; i < operations; i++) {
        int idx = idx_dist(rng);
        naive1.prefix_sum(idx);
    }
    end = high_resolution_clock::now();
    auto naive1_time = duration_cast<milliseconds>(end - start);
    
    // Наивный 2 (предподсчитанные префиксы)
    start = high_resolution_clock::now();
    for (int i = 0; i < operations; i++) {
        int idx = idx_dist(rng);
        naive2.prefix_sum(idx);
    }
    end = high_resolution_clock::now();
    auto naive2_time = duration_cast<milliseconds>(end - start);
    
    cout << "Фенвик:          " << fenw_time.count() << " ms" << endl;
    cout << "Наивный (запрос): " << naive1_time.count() << " ms" << endl;
    cout << "Наивный (префикс): " << naive2_time.count() << " ms" << endl;
    cout << endl;
    
    // ========== ТЕСТ 2: ТОЛЬКО ОБНОВЛЕНИЯ ==========
    cout << "=== ТЕСТ 2: 100% обновлений ===" << endl;
    
    // Фенвик
    start = high_resolution_clock::now();
    for (int i = 0; i < operations; i++) {
        int idx = idx_dist(rng);
        int delta = delta_dist(rng);
        fenw.update(idx, delta);
    }
    end = high_resolution_clock::now();
    fenw_time = duration_cast<milliseconds>(end - start);
    
    // Наивный 1
    start = high_resolution_clock::now();
    for (int i = 0; i < operations; i++) {
        int idx = idx_dist(rng);
        int delta = delta_dist(rng);
        naive1.update(idx, delta);
    }
    end = high_resolution_clock::now();
    naive1_time = duration_cast<milliseconds>(end - start);
    
    // Наивный 2
    start = high_resolution_clock::now();
    for (int i = 0; i < operations; i++) {
        int idx = idx_dist(rng);
        int delta = delta_dist(rng);
        naive2.update(idx, delta);
    }
    end = high_resolution_clock::now();
    naive2_time = duration_cast<milliseconds>(end - start);
    
    cout << "Фенвик:          " << fenw_time.count() << " ms" << endl;
    cout << "Наивный (запрос): " << naive1_time.count() << " ms" << endl;
    cout << "Наивный (префикс): " << naive2_time.count() << " ms" << endl;
    cout << endl;
    
    // ========== ТЕСТ 3: СМЕШАННАЯ НАГРУЗКА ==========
    cout << "=== ТЕСТ 3: 50% запросов + 50% обновлений ===" << endl;
    
    // Фенвик
    start = high_resolution_clock::now();
    for (int i = 0; i < operations; i++) {
        int idx = idx_dist(rng);
        if (op_dist(rng) == 0) {
            fenw.prefix_sum(idx);
        } else {
            int delta = delta_dist(rng);
            fenw.update(idx, delta);
        }
    }
    end = high_resolution_clock::now();
    fenw_time = duration_cast<milliseconds>(end - start);
    
    // Наивный 1
    start = high_resolution_clock::now();
    for (int i = 0; i < operations; i++) {
        int idx = idx_dist(rng);
        if (op_dist(rng) == 0) {
            naive1.prefix_sum(idx);
        } else {
            int delta = delta_dist(rng);
            naive1.update(idx, delta);
        }
    }
    end = high_resolution_clock::now();
    naive1_time = duration_cast<milliseconds>(end - start);
    
    // Наивный 2
    start = high_resolution_clock::now();
    for (int i = 0; i < operations; i++) {
        int idx = idx_dist(rng);
        if (op_dist(rng) == 0) {
            naive2.prefix_sum(idx);
        } else {
            int delta = delta_dist(rng);
            naive2.update(idx, delta);
        }
    }
    end = high_resolution_clock::now();
    naive2_time = duration_cast<milliseconds>(end - start);
    
    cout << "Фенвик:          " << fenw_time.count() << " ms" << endl;
    cout << "Наивный (запрос): " << naive1_time.count() << " ms" << endl;
    cout << "Наивный (префикс): " << naive2_time.count() << " ms" << endl;
    cout << endl;
    
    // ========== ВЫВОДЫ ==========
    cout << "=== ВЫВОДЫ ===" << endl;
    cout << "• Наивный (сумма на запросе): быстрые обновления, медленные запросы" << endl;
    cout << "• Наивный (префикс-массив): быстрые запросы, медленные обновления" << endl;
    cout << "• Дерево Фенвика: сбалансированная производительность" << endl;
    cout << "• Фенвик выигрывает при смешанной нагрузке (реальный сценарий)" << endl;
}

int main() 
{
    run_benchmark();
    return 0;
}