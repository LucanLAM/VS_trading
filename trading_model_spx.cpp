#include <iostream>
#include <string>
#include <fstream>
#include <vector>

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
    

    return 0;
}