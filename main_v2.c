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

void sharePrimeNumbers(int * _scatteredsizes, int * _scatteredstrides, int _size, long _primesnumbers)
{
    if (_size > 0 && _primenumbers > 0)
    {
        int added = 0;
        for (int i = 0; i < _size; i++)
        {
            int addedtothis = 0;
            if ((_primesnumbers - added) % _size != 0)
                addedtothis = 1;
            _scatteredsizes[i] = _primesnumbers / _size + addedtothis;
            if (i == 0)
                _scatteredstrides[i] = 0;
            else
                _scatteredstrides[i] = _scatteredsizes[i - 1] + _scatteredstrides[i - 1];
            added += addedtothis;
        }
    }
    return;
}

int isNotPrime(long _number, long *_primesvector, int _primesvectorsize)
{
    int notprime = 0;
    for (int i = 0; i < _primesvectorsize && !notprime; i++)
    {
        if (_number % _primesvector[i] == 0)
            notprime = 1;
    }
    return notprime;
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
    int scatteredSizes[size];
    int scatteredStrides[size];

    if (myRank == 0)
    {
        calculatedPrimeNumbers = malloc(sizeof(long)*numPrimeNumbers);
        calculatedPrimeNumbers[0] = 2;
        start = MPI_Wtime();
    }
    long calcNumPrimeNumbers = 0;
    long new_calcNumPrimeNumbers = 1;
    long counter = 3;
    long * myPrimes = NULL;
    while (new_calcNumPrimeNumbers < numPrimeNumbers)
    {
        int localNumberOfPrimes = 0;
        if(new_calcNumPrimeNumbers != calcNumPrimeNumbers)
        {
            if (myPrimes != NULL)
                free(myPrimes);
            calcNumPrimeNumbers = new_calcNumPrimeNumbers;
            share_Prime_Numbers(&scatteredSizes[0], &scatteredStrides[0], size, calcNumPrimeNumbers);
            MPI_Scatter(scatteredSizes, 1, MPI_INT, &localNumberOfPrimes, 1, MPI_INT, 0, MPI_COMM_WORLD);
            myPrimes = malloc(sizeof(long)*localNumberOfPrimes);
            MPI_Scatterv(calculatedPrimeNumbers, scatteredSizes, scatteredStrides, MPI_LONG, myPrimes, localNumberOfPrimes, MPI_LONG, 
                                                              0, MPI_COMM_WORLD);
        }
        MPI_Bcast( &counter, 1, MPI_LONG, 0, MPI_COMM_WORLD);
        int isNotPrime = is_NotPrime(counter, &myPrimes[0], localNumberOfPrimes);
        int redIsNotPrime;
        MPI_Reduce(&isNotPrime, &redIsNotPrime, 1, MPI_INT, MPI_SUM, 0,
           MPI_COMM_WORLD);

        if (myRank == 0)
        {
            if (redIsNotPrime == 0)
            {
                calculatedPrimeNumbers[calcNumPrimeNumbers] = counter;
                new_calcNumPrimeNumbers++;
            }
        }
        MPI_Bcast( &new_calcNumPrimeNumbers, 1, MPI_LONG, 0, MPI_COMM_WORLD);
        counter++;
    }
    
    if (myPrimes != NULL)
        free(myPrimes);
    if (myRank == 0)
    {
        end = MPI_Wtime();
        printf("the prime numbers calculated are: ");
        for (long j = 0; j < new_calcNumPrimeNumbers; j++)
            printf("%ld, ", calculatedPrimeNumbers[j]);
        printf("\nConsumed time: %f\n", end - start);
        free(calculatedPrimeNumbers);
    }
    MPI_Finalize();
    return 0;
}