#include "heat.h"
#include <mpi.h>


double residual_jacobi(double *u, unsigned sizex, unsigned sizey, MPI_Comm cart_comm) {
    unsigned i, j;
    double unew, diff, sum = 0.0;
    double global_sum = 0.0;

    for (j = 1; j < sizex - 1; j++) {
        for (i = 1; i < sizey - 1; i++) {
            unew = 0.25 * (u[i * sizex + (j - 1)] +  // left
                           u[i * sizex + (j + 1)] +  // right
                           u[(i - 1) * sizex + j] +  // top
                           u[(i + 1) * sizex + j]);  // bottom

            diff = unew - u[i * sizex + j];
            sum += diff * diff;
        }
    }

    // Reduce the residual sum across all processes
    MPI_Allreduce(&sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, cart_comm);

    return global_sum;
}

/*
 * One Jacobi iteration step
 */
void relax_jacobi(double *u, double *utmp, unsigned sizex, unsigned sizey, MPI_Comm cart_comm) {
    int i, j;

    // Exchange boundaries with neighbors
    exchange_boundaries(u, sizex, sizey, cart_comm);

    for (j = 1; j < sizex - 1; j++) {
        for (i = 1; i < sizey - 1; i++) {
            utmp[i * sizex + j] = 0.25 * (u[i * sizex + (j - 1)] +  // left
                                          u[i * sizex + (j + 1)] +  // right
                                          u[(i - 1) * sizex + j] +  // top
                                          u[(i + 1) * sizex + j]);  // bottom
        }
    }

    // Copy from utmp to u
    for (j = 1; j < sizex - 1; j++) {
        for (i = 1; i < sizey - 1; i++) {
            u[i * sizex + j] = utmp[i * sizex + j];
        }
    }
}

/*
 * Function to exchange boundaries with neighboring processes
 */
void exchange_boundaries(double *u, unsigned sizex, unsigned sizey, MPI_Comm cart_comm) {
    MPI_Request reqs[8];
    int north, south, east, west;

    // Get the ranks of the neighboring processes
    MPI_Cart_shift(cart_comm, 0, 1, &north, &south);
    MPI_Cart_shift(cart_comm, 1, 1, &west, &east);

    // North-South communication
    MPI_Isend(&u[1 * sizey + 1], sizey - 2, MPI_DOUBLE, north, 0, cart_comm, &reqs[0]);
    MPI_Irecv(&u[0 * sizey + 1], sizey - 2, MPI_DOUBLE, north, 0, cart_comm, &reqs[1]);
    MPI_Isend(&u[(sizex - 2) * sizey + 1], sizey - 2, MPI_DOUBLE, south, 0, cart_comm, &reqs[2]);
    MPI_Irecv(&u[(sizex - 1) * sizey + 1], sizey - 2, MPI_DOUBLE, south, 0, cart_comm, &reqs[3]);

    // East-West communication
    MPI_Isend(&u[1 * sizey + (sizey - 2)], 1, MPI_DOUBLE, east, 0, cart_comm, &reqs[4]);
    MPI_Irecv(&u[1 * sizey + (sizey - 1)], 1, MPI_DOUBLE, east, 0, cart_comm, &reqs[5]);
    MPI_Isend(&u[1 * sizey + 1], 1, MPI_DOUBLE, west, 0, cart_comm, &reqs[6]);
    MPI_Irecv(&u[1 * sizey + 0], 1, MPI_DOUBLE, west, 0, cart_comm, &reqs[7]);

    MPI_Waitall(8, reqs, MPI_STATUSES_IGNORE);
}
