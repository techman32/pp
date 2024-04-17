#include <iostream>
#include <omp.h>
#include <Windows.h>

/* Задание 1 */

#define N 10000000

int main() {
    setlocale(LC_ALL, "Russian");

    double pi = 0.0;
    double start_time, end_time;

    start_time = omp_get_wtime();
    for (int i = 0; i < N; i++) {
        pi += (i % 2 == 0 ? 1.0 : -1.0) / (2 * i + 1);
    }
    pi *= 4;
    end_time = omp_get_wtime();
    std::cout << "С использованием for (синхронно): " << pi << ", время выполнения: " << end_time - start_time << " секунд" << std::endl;

    pi = 0.0;
    start_time = omp_get_wtime();
#pragma omp parallel for
    for (int i = 0; i < N; i++) {
        pi += (i % 2 == 0 ? 1.0 : -1.0) / (2 * i + 1);
    }
    pi *= 4;
    end_time = omp_get_wtime();
    std::cout << "С использованием параллельного for (работает неверно): " << pi << ", время выполнения: " << end_time - start_time << " секунд" << std::endl;

    pi = 0.0;
    start_time = omp_get_wtime();
#pragma omp parallel for
    for (int i = 0; i < N; i++) {
#pragma omp atomic
        pi += (i % 2 == 0 ? 1.0 : -1.0) / (2 * i + 1);
    }
    pi *= 4;
    end_time = omp_get_wtime();
    std::cout << "Рабочее решение с использованием parallel for и atomic: " << pi << ", время выполнения: " << end_time - start_time << " секунд" << std::endl;

    pi = 0.0;
    start_time = omp_get_wtime();
#pragma omp parallel for reduction(+:pi)
    for (int i = 0; i < N; i++) {
        pi += (i % 2 == 0 ? 1.0 : -1.0) / (2 * i + 1);
    }
    pi *= 4;
    end_time = omp_get_wtime();
    std::cout << "С использованием reduction: " << pi << ", время выполнения: " << end_time - start_time << " секунд" << std::endl;

    return 0;
}

/* Задание 2 */

/*
* private(x)
* Каждый поток будет иметь свое собственное значение x, которое он изменяет в своем цикле. 
* Значение x за пределами цикла остается неизменным (44).
* 
* firstprivate(x)
* Каждый поток начинает с копии значения x (44). 
* Каждый поток изменяет свою копию переменной x внутри цикла. Значение x за пределами цикла остается неихменным (44).
* 
* lastprivate(x) 
* Значение x в основном потоке после выполнения цикла будет равно последнему значению, которое было установлено в переменной x внутри цикла.
*/