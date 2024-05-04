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

int sharePrimeNumbers(int _lastNode, int _size)
{
    int newNode = 0;
    if (_size > 0)
    {
        newNode = _lastNode + 1;
        if (newNode >= _size)
            newNode = 0;
    }
    return newNode;
}

int isNotPrime(long _number, long *_primesvector, int _primesvectorsize)
{
    int notprime = 0;
    if(_primesvectorsize != 0 && _primesvector != NULL)
    {
        for (int i = 0; i < _primesvectorsize && !notprime; i++)
        {
            if (_number % _primesvector[i] == 0)
                notprime = 1;
        }
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
    long totalPrimeNumbers[numPrimeNumbers];
    int totalPrimeNumbersIndex = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank( MPI_COMM_WORLD, &myRank);
    long *nodePrimeNumbers = NULL;
    int nodePrimeNumberSize = 0;
    int toNode = 0;

    if (myRank == 0)
    {
        nodePrimeNumbers = malloc(sizeof(long));
        totalPrimeNumbers[0] = nodePrimeNumbers[0] = 2;
        totalPrimeNumbersIndex = nodePrimeNumberSize = 1;
        start = MPI_Wtime();
    }
    long counter = 3;

    while (totalPrimeNumbersIndex < numPrimeNumbers)
    {
        MPI_Bcast( &counter, 1, MPI_LONG, 0, MPI_COMM_WORLD);
        int noPrime = isNotPrime(counter, &nodePrimeNumbers[0], nodePrimeNumberSize);
        int redNoPrime;
        MPI_Allreduce(&noPrime, &redNoPrime, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        if ( redNoPrime == 0)
        {
            if (myRank == 0)
            {
                totalPrimeNumbers[totalPrimeNumbersIndex] = counter;
                totalPrimeNumbersIndex++;
            }
            int toNode = sharePrimeNumbers(toNode, size);
            MPI_Bcast(&totalPrimeNumbersIndex, 1, MPI_LONG, 0, MPI_COMM_WORLD);
            if(myRank == toNode)
            {
                nodePrimeNumbers = realloc(nodePrimeNumbers, sizeof(long) + sizeof(long) * nodePrimeNumberSize);
                nodePrimeNumbers[nodePrimeNumberSize] = counter;
                nodePrimeNumberSize++;
            }
        }    
        counter++;
    }
    
    if (nodePrimeNumbers != NULL)
        free(nodePrimeNumbers);
    if (myRank == 0)
    {
        end = MPI_Wtime();
        printf("the prime numbers calculated are: ");
        for (long j = 0; j < totalPrimeNumbersIndex; j++)
            printf("%ld, ", totalPrimeNumbers[j]);
        printf("\nConsumed time: %f\n", end - start);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    printf("I'm node %d and I collaborated in this work!\n", myRank);
    MPI_Finalize();
    return 0;
}