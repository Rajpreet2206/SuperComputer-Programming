#include <mpi.h>
#include <iostream>
#include <vector>
#include <chrono>



void performance_measure_ping_pong(int message_size, int num_iterations, int rank, int other_rank) {
    std::vector<char> message(message_size);
    MPI_Status status;
    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_iterations; ++i) {
        if (rank == 0) {
            MPI_Send(message.data(), message_size, MPI_CHAR, other_rank, 0, MPI_COMM_WORLD);
            MPI_Recv(message.data(), message_size, MPI_CHAR, other_rank, 0, MPI_COMM_WORLD, &status);
        } else if (rank == other_rank) {
            MPI_Recv(message.data(), message_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
            MPI_Send(message.data(), message_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;

    double total_time = duration.count();
    double round_trip_time = total_time / num_iterations;
    double bandwidth = message_size / (round_trip_time / 2) / (1024 * 1024); // MB/s

    if (rank == 0) {
        std::cout << "Message size: " << message_size << " bytes\n";
        std::cout << "Round-trip time: " << round_trip_time << " seconds\n";
        std::cout << "Bandwidth: " << bandwidth << " MB/s\n";
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        std::cerr << "At least two processes.\n";
        MPI_Finalize();
        return 1;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    int num_iterations = 1000; 
    for (int message_size = 20; message_size <= 224; ++message_size) {
        performance_measure_ping_pong(message_size, num_iterations, rank, 1);
    }

    MPI_Finalize();
    return 0;
}
