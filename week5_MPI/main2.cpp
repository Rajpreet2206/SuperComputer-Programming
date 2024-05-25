//Vector based implementation of Reduction Operation

#include <iostream>
#include <vector>
#include <cmath>

void compute_local_sum(const std::vector<int>& array, int start_idx, int end_idx, int& local_sum) {
    local_sum = 0;
    for (int i = start_idx; i < end_idx; ++i) {
        local_sum += array[i];
    }
}

void tree_based_reduction(std::vector<int>& local_sums) {
    int size = local_sums.size();
    int step = 1;
    while (step < size) {
        for (int i = 0; i < size; i += 2 * step) {
            if (i + step < size) {
                local_sums[i] += local_sums[i + step];
            }
        }
        step *= 2;
    }
}

int main(int argc, char** argv) {
    int n = 100; 
    if (argc > 1) {
        n = std::stoi(argv[1]);
    }

    int num_processes = 4; 
    std::vector<int> A(n);
    for (int i = 0; i < n; ++i) {
        A[i] = i;
    }
    std::vector<int> local_sums(num_processes, 0);
    int local_n = n / num_processes;
    int remainder = n % num_processes;
    int start_idx = 0;

    for (int rank = 0; rank < num_processes; ++rank) {
        int end_idx = start_idx + local_n;
        if (rank < remainder) {
            end_idx++;
        }
        compute_local_sum(A, start_idx, end_idx, local_sums[rank]);
        start_idx = end_idx;
    }
    tree_based_reduction(local_sums);
    std::cout << "Global sum: " << local_sums[0] << std::endl;

    return 0;
}
