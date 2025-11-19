#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <unordered_map>


struct StockData {
    time_t date;
    double price;
    double open;
    double high;
    double low;
    double ma200;
};

struct Position {
    int shares = 0;
    double total_cost = 0.0;
};

struct trade_history {
    time_t date;
    std::string stock_symbol;
    int stock_quantity;
    double cost;
};

class Trader_MA200 {
    std::string trade_model_name = "SPX DCA Trader";
    float cash = 1300;
    std::unordered_map<std::string, Position> holdings;
    std::vector<trade_history> history;
    int last_month = -1;
    float monthly_deposit = 1300;

    // current test dates
    struct tm datetime{};
    time_t current_date_month{};

    public:
        void execute_trade(const StockData& stock_data_day) {
            struct tm* d = localtime(&stock_data_day.date);
            int month = d->tm_mon + 1;
            if (month != last_month) {
                cash += monthly_deposit;
                last_month = month;
            }
            // Buy Condition: all in
            // Assume but at 75% of H to L. In other word entry price = 0.75 * (H - L) + L
            // Number of stock need to buy
            double entry_price = 0.75 * (stock_data_day.high - stock_data_day.low) + stock_data_day.low;
            int stock_buy = cash / entry_price;
            if (stock_buy <= 0) return; // not enough cash
            
            double cost = stock_buy * entry_price;
            
            // Deduct Cash
            cash -= cost;

            // Add to holdings
            holdings["SPY"].shares += stock_buy;
            holdings["SPY"].total_cost += cost;

            // Save to history
            history.push_back({ stock_data_day.date, "SPY", stock_buy, cost });
                
        }

        void output_states() {
            std::cout << "=== Trader State ===\n";
            std::cout << "Algo Name: " << trade_model_name << std::endl;
            std::cout << "Cash: " << cash << std::endl;
            
            std::cout << "Holdings:" << std::endl; 
            for (const auto& [symbol, pos] : holdings) {
                std::cout << symbol << ": " << pos.shares << " shares, total cost: " << pos.total_cost << std::endl;
            }

            std::cout << "Histroy:" << std::endl;
            for (const auto& h : history) {
                struct tm datetime;
                char output[50]{};
                datetime = *localtime(&h.date);
                strftime(output, 50, "%e %B, %Y", &datetime);
                std::cout << "  Date: " << output << std::endl;
                std::cout << "   Symbol: " << h.stock_symbol
                        << "   Qty: " << h.stock_quantity
                        << "   Cost: " << h.cost
                        << "\n";
            }
        }
};


time_t parse_date_to_time_t(const std::string& date_str);
std::vector<StockData> load_stock_data(const std::string& filename);
void compute_MA200(std::vector<StockData>& data);

int main() {
    std::cout << "Trading Model: S&P 500 DCA" << std::endl;

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

    char output[50];
    datetime = *localtime(&startdate);
    strftime(output, 50, "%e %B, %Y", &datetime);
    std::cout << "Simulation Start Date: " << output << std::endl;
    datetime = *localtime(&enddate);
    strftime(output, 50, "%e %B, %Y", &datetime);
    std::cout << "Simulation End Date: " << output << std::endl;

    int time_diff_sec = difftime(enddate, startdate);
    int time_diff_days = time_diff_sec / (24 * 60 * 60);
    std::cout << "Time diff in days: " << time_diff_days << std::endl;

    auto stock_data = load_stock_data("SPY_Historical_Data.csv");

    // for (size_t i = 0; i < stock_data.size(); ++i) {
    //
    //     time_t date = stock_data[i].date;
    //     double price = stock_data[i].price;
    //     double open = stock_data[i].open;
    //     double high = stock_data[i].high;
    //     double low = stock_data[i].low;
    //     double ma200 = stock_data[i].ma200;
    //
    //     datetime = *localtime(&date);
    //     strftime(output, 50, "%e %B, %Y", &datetime);
    //     std::cout << "Date: " << output << " ";
    //
    //     std::cout << price << " " << open << " " << high << " " << low << " " << ma200 << std::endl;
    // }

    Trader_MA200 trader; // create an object
    for (auto& stock_data_day : stock_data) {
        if (stock_data_day.date < startdate) continue;

        if (stock_data_day.date > enddate) break;

        trader.execute_trade(stock_data_day);
        // Pause for debugging
        // std::cout << "Debug Start:--------------------------------------------------" << std::endl;
        // datetime = *localtime(&stock_data_day.date);
        // strftime(output, 50, "%e %B, %Y", &datetime);
        // std::cout << "Simulation current Date: " << output << std::endl;
        // trader.output_states();
        // std::cout << "Press Enter to continue to next day..." << std::endl;
        // std::cin.get(); // waits for Enter key

    }
    datetime = *localtime(&enddate);
    strftime(output, 50, "%e %B, %Y", &datetime);
    trader.output_states();

    return 0;
}

std::vector<StockData> load_stock_data(const std::string& filename) {
    std::vector<StockData> data;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open " << filename << "\n";
        return data;
    }

    std::string line;
    std::getline(file, line); // skip header

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string date, price, open, high, low, vol, change;

        std::getline(ss, date, ',');
        std::getline(ss, price, ',');
        std::getline(ss, open, ',');
        std::getline(ss, high, ',');
        std::getline(ss, low, ',');
        std::getline(ss, vol, ',');
        std::getline(ss, change, ',');

        StockData record;
        record.date = parse_date_to_time_t(date);
        record.price = std::stod(price);
        record.open = std::stod(open);
        record.high = std::stod(high);
        record.low = std::stod(low);
        record.ma200 = 0.0;  // placeholder for now

        data.push_back(record);

    }
    
    compute_MA200(data);

    return data;
}

time_t parse_date_to_time_t(const std::string& date_str) {
    int month, day, year;
    if (std::sscanf(date_str.c_str(), "%d/%d/%d", &day, &month, &year) != 3) {
        return -1; // invalid format
    }

    struct tm tm_date{};
    tm_date.tm_year = year - 1900; // years since 1900
    tm_date.tm_mon  = month - 1;   // months since January
    tm_date.tm_mday = day;
    tm_date.tm_hour = 0;
    tm_date.tm_min  = 0;
    tm_date.tm_sec  = 0;
    tm_date.tm_isdst = -1;

    return mktime(&tm_date);
}

void compute_MA200(std::vector<StockData>& data) {
    std::reverse(data.begin(), data.end());
    const int window = 200;
    double sum = 0.0;
    for (size_t i = 0; i < data.size(); i++) {

        sum += data[i].price;

        if (i >= window)
            sum -= data[i - window].price;

        if (i >= window - 1)
            data[i].ma200 = sum / window;
        else
            data[i].ma200 = 0.0; // insufficient data
    }
}