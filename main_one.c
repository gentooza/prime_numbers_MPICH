/*

This file is part of Prime numbers calculation using MPICH

Copyright 2024 Joaquín Cuéllar <joaquin.cuellar(at)uco(dot)es>

Prime numbers calculation using MPICH is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Prime numbers calculation using MPICH is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>


int is_NotPrime(long number, long *primesVector, int primesVectorSize)
{
    int isNotPrime = 0;
    for (int i = 0; i < primesVectorSize && !isNotPrime; i++)
    {
        if (number % primesVector[i] == 0)
            isNotPrime = 1;
    }
    return isNotPrime;
}

int main(int argc, char ** argv)
{
    double start, end;    
    if (argc != 2) 
    {
        fprintf(stderr, "Debes indicar el número de números primos a calcular: prime_numbers <cantidad de números primos a calcular>\n");
        exit(1);
    }

    int size, myRank;
    int numPrimeNumbers = atoi(argv[1]);
    if (numPrimeNumbers <= 5) 
    {
        fprintf(stderr, "Debes indicar una cantidad de números primos a calcular MAYOR de 5!\n");
        exit(1);
    }
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank( MPI_COMM_WORLD, &myRank);
    long *calculatedPrimeNumbers;
    calculatedPrimeNumbers = malloc(sizeof(long)*numPrimeNumbers);
    calculatedPrimeNumbers[0] = 2;
    start = MPI_Wtime();

    long calcNumPrimeNumbers = 1;
    long counter = 3;
    while (calcNumPrimeNumbers < numPrimeNumbers)
    {
        int isNotPrime = is_NotPrime(counter, &calculatedPrimeNumbers[0], calcNumPrimeNumbers);

        if (isNotPrime == 0)
        {
            calculatedPrimeNumbers[calcNumPrimeNumbers] = counter;
            calcNumPrimeNumbers++;
        }
        counter++;
    }

    end = MPI_Wtime();
    printf("the prime numbers calculated are: ");
    for (long j = 0; j < calcNumPrimeNumbers; j++)
        printf("%ld, ", calculatedPrimeNumbers[j]);
    printf("\nConsumed time: %f\n", end - start);
    free(calculatedPrimeNumbers);

    MPI_Finalize();
  
    return 0;
}