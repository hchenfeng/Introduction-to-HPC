//
// Created by kingofpirates on 10/16/21.
//

#include <iostream>
#include <chrono>

using namespace std;

#define ARRAY_SIZE 10

int main(int argc, char *argv[]){
    int my_arr[ARRAY_SIZE];
    srand(0);

//    for (int & i : my_arr) {
//        i = rand() % 10;
//    }

    for (int i = 0; i < ARRAY_SIZE; i++) {
        my_arr[i] = i;
    }


    for (int j : my_arr) {
        cout << j << " ";
    }

    cout << endl << endl;

    auto start_time = chrono::steady_clock::now();
    reverse(my_arr, my_arr + (sizeof(my_arr) / sizeof(my_arr[0])));
    auto end_time = chrono::steady_clock::now();

    for (int k : my_arr) {
        cout << k << " ";
    }

    cout << endl << endl;

    cout << chrono::duration_cast<chrono::microseconds>(end_time - start_time).count()
         << endl;

};