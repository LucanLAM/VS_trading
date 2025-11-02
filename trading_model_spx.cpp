#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <ctime>

struct Record {
    std::string date;
    double price;
    std::string open, high, low, volume, change;
    double ma200;
};

struct Stock_holding {
    std::string stock_symbol;
    int stock_quantity;
    float cost;
};

class Trader {
    std::string trade_model_name = "nan";
    float cash = 10000;
    std::vector<Stock_holding> holding;

    public:
        Trader(std::string trade_model_name, float cash) { // Constructor with parameters
            trade_model_name = trade_model_name;
            cash = cash;
        }

        void set_cash(float s) {
            cash = s;
        }

        float get_cash() {
            return cash;
        }

        void set_stock_holding(std::vector<Stock_holding> holding_input) {
            holding = holding_input;
        }
};

int main() {
    std::cout << "Trading Model: S&P 500 MA 200" << std::endl;

    // Start and end test date
    struct tm datetime;
    time_t enddate;
    datetime.tm_year = 2025 - 1900; // Number of years since 1900
    datetime.tm_mon = 10 - 1; // Number of months since January
    datetime.tm_mday = 28;
    datetime.tm_hour = 0; datetime.tm_min = 0; datetime.tm_sec = 0;
    datetime.tm_isdst = -1;
    enddate = mktime(&datetime);

    time_t startdate;
    datetime.tm_year = 2006 - 1900; // Number of years since 1900
    datetime.tm_mon = 6 - 1; // Number of months since January
    datetime.tm_mday = 23;
    datetime.tm_hour = 0; datetime.tm_min = 0; datetime.tm_sec = 0;
    datetime.tm_isdst = -1;
    startdate = mktime(&datetime);

    time_t current_date;
    datetime.tm_year = 2006 - 1900; // Number of years since 1900
    datetime.tm_mon = 6 - 1; // Number of months since January
    datetime.tm_mday = 23;
    datetime.tm_hour = 0; datetime.tm_min = 0; datetime.tm_sec = 0;
    datetime.tm_isdst = -1;
    current_date = mktime(&datetime);

    char output[50];
    datetime = *localtime(&startdate);
    strftime(output, 50, "%e %B, %Y", &datetime);
    std::cout << "Simulation Start Date: " << output << std::endl;
    datetime = *localtime(&enddate);
    strftime(output, 50, "%e %B, %Y", &datetime);
    std::cout << "Simulation End Date: " << output << std::endl;
    datetime = *localtime(&current_date);
    strftime(output, 50, "%e %B, %Y", &datetime);
    std::cout << "Simulation current Date: " << output << std::endl;

    return 0;
}