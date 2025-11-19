#include <iostream>
#include <cmath> // for exp() and log()

int main() {
    double P, A;
    int num_days;

    std::cout << "Enter principal (P): ";
    std::cin >> P;
    std::cout << "Enter final amount (A): ";
    std::cin >> A;
    std::cout << "Enter number of days: ";
    std::cin >> num_days;

    if (P <= 0 || A <= 0 || num_days <= 0) {
        std::cerr << "Values must be positive!" << std::endl;
        return 1;
    }

    double r = 365 * std::exp(std::log(A / P) / num_days) - 365;

    std::cout << "Calculated r: " << r << std::endl;

    return 0;
}
