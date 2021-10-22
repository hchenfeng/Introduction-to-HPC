// CIS 677 High Performance Computing
// Chenfeng Hao
// Project 2
//

#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <chrono>
#include <omp.h>

using namespace std;

// Choose a number of particles
#define NUM_OF_PARTICLES 3000
#define NUM_OF_THREADS 12

int main() {

    // Use chrono to time execution
    auto start_time = chrono::steady_clock::now();

    // Set rand() seed
//    unsigned int seed = time(nullptr);
    srand(4);

    // Vectors for fixed particles
    vector<int> row_crystal(NUM_OF_PARTICLES);
    vector<int> col_crystal(NUM_OF_PARTICLES);

    // Vectors for free particles
    vector<int> row_particles(NUM_OF_PARTICLES);
    vector<int> col_particles(NUM_OF_PARTICLES);

    // Set max range and origin
    const int MAX_RANGE = 100;
    const int ORIGIN = 50;

    // Set origin at (100, 100)
    // Put origin to fixed particles vector
    // Assume origin is fixed
    row_crystal[0] = ORIGIN;
    col_crystal[0] = ORIGIN;

    // Initialize radius as 0, and other helper vars
    unsigned int radius = 0;
    unsigned int i, j, m, n, k, free_particles_size, crystal_size, temp_radius;
    free_particles_size = 0;
    crystal_size = 1;

    // Loop through all particles
//    omp_set_num_threads(NUM_OF_THREADS);
//#pragma omp parallel for private (i)
    for (i = 1; i <= NUM_OF_PARTICLES; i++) {

        // Get row and col from rand(), between 0 and MAX_RANGE
        int rand_row = rand() % MAX_RANGE;
        int rand_col = rand() % MAX_RANGE;

        // Distance to origin equals row difference plus col difference.
        unsigned int distance_to_origin = abs(rand_row - row_crystal[0]) + abs(rand_col - col_crystal[0]);

        // Make sure any new particle is at least (radius + 1) away from the origin
        while (distance_to_origin <= radius + 1) {
            rand_row = rand() % MAX_RANGE;
            rand_col = rand() % MAX_RANGE;
            distance_to_origin = abs(rand_row - row_crystal[0]) + abs(rand_col - col_crystal[0]);

        }

        // Add new particle to the free particle vector
        row_particles[i - 1] = rand_row;
        col_particles[i - 1] = rand_col;

        // Increment free particles size var
        free_particles_size += 1;

        // Set omp threads
        omp_set_num_threads(NUM_OF_THREADS);
#pragma omp parallel for private (j, temp_radius)
        // Loop through all free particles
        for (j = 0; j < free_particles_size; j++) {
            // Once a free particle becomes fixed,
            // its position would be changed to the same as origin.
            // This is to avoid changing vector size (vector.erase()),
            // as resizing incurs more performance cost.
            if (row_particles[j] == ORIGIN && col_particles[j] == ORIGIN) continue;

            // Break not allowed in omp
            // A flag makes loop task negligible if condition is met
            volatile bool flag = false;

#pragma omp parallel for private (m)
            // Check each free particle against all fixed particles
            for (m = 0; m < crystal_size; m++) {
                if (flag) continue;

                // Get row/col distances
                int row_dist = abs(row_crystal[m] - row_particles[j]);
                int col_dist = abs(col_crystal[m] - col_particles[j]);

                // Exclusive OR
                // If the row distance between two particles is 1 AND the col is the same for both,
                // OR (exclusively) the col distance between two particles is 1 AND the row is the same for both,
                // then they are adjacent and the new particle becomes fixed.
                if (!((row_dist == 1) && (col_crystal[m] == col_particles[j])) !=
                    !((col_dist == 1) && (row_crystal[m] == row_particles[j]))) {

                    // Put particle to fixed vector
                    row_crystal[crystal_size] = row_particles[j];
                    col_crystal[crystal_size] = col_particles[j];

                    crystal_size += 1;
                    free_particles_size -= 1;

                    temp_radius =
                            abs(row_crystal[0] - row_particles[j]) + abs(col_crystal[0] - col_particles[j]);

                    // Update radius if new radius is greater than current
                    if (temp_radius > radius) {
                        radius = temp_radius;
                    }

                    // Remove particle from free vector
                    row_particles[j] = ORIGIN;
                    col_particles[j] = ORIGIN;
                    flag = true;
                }
            }

#pragma omp critical
            {
                if (flag) {
                    if (temp_radius > radius) {
                        radius = temp_radius;
                    }
                }
            }
        }

        // Each free particle takes a random step
#pragma omp parallel for private (n)
        for (n = 0; n < free_particles_size; n++) {
            if (row_particles[n] == ORIGIN && col_particles[n] == ORIGIN) continue;

            // Use if...else instead of switch,
            // because "break" is not allowed in omp
            int direction = rand() % 4;
            if (direction == 0) {
                row_particles[n] -= 1;
            } else if (direction == 1) {
                row_particles[n] += 1;
            } else if (direction == 2) {
                col_particles[n] += 1;
            } else if (direction == 3) {
                col_particles[n] -= 1;
            }

            // Remove particle if stepping out of range
            if (row_particles[n] < 0 || row_particles[n] > MAX_RANGE ||
                col_particles[n] < 0 || col_particles[n] > MAX_RANGE) {
                row_particles[n] = ORIGIN;
                col_particles[n] = ORIGIN;
            }
        }
    }

    auto end_time = chrono::steady_clock::now();

// Output vector of fixed particles to a csv file
    fstream fout;
    fout.open("crystal.csv", ios::out | ios::trunc);
    for (
            k = 0;
            k < crystal_size;
            ++k) {
        fout << row_crystal[k] << ", "
             << col_crystal[k] << "\n";
    }
    fout.close();

// Output execution time to console
    cout << chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count()
         << endl;

    return 0;
}