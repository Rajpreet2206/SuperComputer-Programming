#include <mpi.h>
#include <iostream>
#include <vector>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = 100; 
    if (argc > 1) {
        n = std::stoi(argv[1]);
    }
    int local_n = n / size;
    int remainder = n % size;
    int start_idx = rank * local_n + std::min(rank, remainder);
    int end_idx = start_idx + local_n;
    if (rank < remainder) {
        end_idx++;
    }


    std::vector<int> local_A(end_idx - start_idx);
    for (int i = start_idx; i < end_idx; ++i) {
        local_A[i - start_idx] = i;
    }
    int local_sum = 0;
    for (int i = 0; i < local_A.size(); ++i) {
        local_sum += local_A[i];
    }
    int step = 1;
    while (step < size) {
        if (rank % (2 * step) == 0) {
            if (rank + step < size) {
                int received_sum;
                MPI_Recv(&received_sum, 1, MPI_INT, rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                local_sum += received_sum;
            }
        } else {
            int target = rank - step;
            MPI_Send(&local_sum, 1, MPI_INT, target, 0, MPI_COMM_WORLD);
            break;
        }
        step *= 2;
    }
    if (rank == 0) {
        std::cout << "Global sum: " << local_sum << std::endl;
    }

    MPI_Finalize();
    return 0;
}
