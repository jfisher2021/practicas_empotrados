PREGUNTAS:
1. Ejecutar los siguientes casos y justificar su comportamiento:
a. ./practica1 (multicore)
b. ./practica1 (monocore)
c. ./practica1 (monocore) + stress
d. ./practica1 (multicore) + stress
2. ¿En qué casos de ejecución (nombrados anteriormente) el sistema es capaz de
cumplir las restricciones temporales (tanto tiempo de cómputo como
periodicidad)?

Unicamente en el caso de ejecutar el programa en multicore sin stress, ya que en los demas casos el sistema no es capaz de cumplir las restricciones. En el resto de casos, el sistema no es capaz de cumplir las restricciones temporales, ya que el tiempo de ejecucion es mayor que el tiempo de computo. En el caso de stress con multicore y el monocore sin stress, el tiempo de ejecucion es mayor que el tiempo de computo pero en ese caso  la periodicidad si se cumple. Al ejecutar tasket -c 0 ./practica1 (monocore) con stress, no es capaz de cumplir las restricciones temporales y la periodicidad tampoco se cumple. Tardaba mucho en ejecutarse y tuve que terminar el proceso con ctrl+c.

![Alt text](<Captura desde 2023-10-05 12-37-04.png>)

3. ¿Qué número mínimo de cpus se necesitan para que tu programa ejecute
correctamente sin fallos de restricciones temporales? Usa el comando taskset
para comprobarlo.

Para que el programa ejecute correctamente sin fallos de restricciones temporales, se necesitan 4 cpus (sin Stress). Para comprobarlo, se ejecuta el comando taskset -c 0,1,2,4 ./practica1 (multicore) y se comprueba que el programa se ejecuta correctamente sin fallos de restricciones temporales. Si se ejecuta con Stress, aunque uses todas las cpus, el programa no cumple las restricciones temporales.

4. ¿Qué solución se podría proponer para cumplir plazos estrictos temporales de
periodicidad en la ejecución de los threads SIN cambiar la configuración actual
que tienen los ordenadores del laboratorio?

Dando prioridad a los procesos que se ejecutan en el sistema, para que el proceso que se ejecuta en el sistema tenga prioridad sobre los demas procesos que se ejecutan en el sistema. Para ello, se ejecuta el comando nice -n -20 ./practica1 (multicore) y se comprueba que el programa se ejecuta correctamente sin fallos de restricciones temporales.