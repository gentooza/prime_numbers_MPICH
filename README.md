Cálculo de números primos con MPICH // Prime numbers calculation using MPICH
===

## Purpose and Algorithm

<img src="algorithm.png" alt="Schema"
    title="Prime numbers algorithm used" width="300"/>

La idea es implementar este algoritmo de cálculo de números primos en programación distribuida empleando la librería MPICH.

Partimos de almacenar los números primos obtenidos de un vector de números, el nodo maestro repartirá estos números (gather) entre los distintos nodos del clúster.

Irá preguntando por los siguientes números que se calculen a cada nodo (con un reduce), de forma que si un número nuevo resulta primo se añade al vector, y se vuelve a repartir y a seguir con los cálculos.

## Copyright

2024 Joaquín Cuéllar \<joaquin.cuellar(at)uco(dot)es\>

## License

```
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
```

## images license

./algorithm.png

is licensed under CC-BY-SA
copyright 2024 Joaquín Cuéllar \<joaquin.cuellar(at)uco(dot)es\>