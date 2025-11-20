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

time_t parse_date_to_time_t(const std::string& date_str);
std::vector<StockData> load_stock_data(const std::string& filename);
void time_t_output(const std::time_t& time);

class Trader {
    std::string trade_model_name = "Francis Old Trader";
    float cash = 1300;
    float borrow_cash = 0;
    std::unordered_map<std::string, Position> holdings;
    std::vector<trade_history> history;
    int last_month = -1;
    float monthly_deposit = 1300;
    float QQQ_ATH = 120.5;

    // current test dates
    struct tm datetime{};
    time_t current_date_month{};

    public:
        void execute_trade(const StockData& QQQ_day) {
            struct tm* d = localtime(&QQQ_day.date);
            int month = d->tm_mon + 1;
            if (month != last_month) {
                cash += monthly_deposit;
                last_month = month;
            }
            // Buy Condition:
            // If QQQ drop below ATH 10%, buy QLD (2XQQQ)
            // If QQQ drop below ATH 50%, buy TQQQ (3XQQQ)
            // If QQQ rise ATH 90%, sell all TQQQ (3XQQQ) and buy QLD(2XQQQ)
            // Otherwise, buy QQQ
            // note: since TQQQ appear on 2/11/2010, use 2nd line

            // This programme will use the following to replace the above algo
            // trade have two account cash and borrowed cash
            // when buy QLD (2XQQQ), buy 2X QQQ (e.g. cash = 500, QQQ=100, buy 10 shares => cash = 0, borrow_cash = -500)
            // when buy TQQQ (3XQQQ), buy 3X QQQ (e.g. cash = 500, QQQ=100, buy 15 shares => cash = 0, borrow_cash = -1000)
            // QLD and TQQQ is marked as D_QQQ and T_QQQ in histroy and holdings
            if (QQQ_day.high > QQQ_ATH) {
                std::cout << "Trigger ATH change, old ATH: " << QQQ_ATH << " New ATH: " << QQQ_day.high << "\n";
                QQQ_ATH = QQQ_day.high;
            }
            std::cout << "QQQ / QQQ_ATH ratio: " << QQQ_day.price / QQQ_ATH << "\n";

            if ((QQQ_day.price > 0.9 * QQQ_ATH) && (holdings["T_QQQ"].shares > 0)) {
                // std::cout << "QQQ above ATH 90%, sell\n";
                //sell T_QQQ
                int stock_sell = holdings["T_QQQ"].shares;
                double cost = holdings["T_QQQ"].total_cost;
                double exit_price = 0.25 * (QQQ_day.high - QQQ_day.low) + QQQ_day.low;
                double gain = exit_price * holdings["T_QQQ"].shares;
                borrow_cash += holdings["T_QQQ"].total_cost * (2 / 3);
                cash += gain - holdings["T_QQQ"].total_cost * (2 / 3);
                // Save to holding
                holdings["T_QQQ"].shares = 0;
                holdings["T_QQQ"].total_cost = 0;

                // Save to history
                history.push_back({ QQQ_day.date, "T_QQQ", -stock_sell, -cost });

                double entry_price = 0.75 * (QQQ_day.high - QQQ_day.low) + QQQ_day.low;
                int stock_buy = ((gain - holdings["T_QQQ"].total_cost * (2 / 3)) / entry_price);
                if (stock_buy <= 0) return; // not enough cash

                cost = stock_buy * entry_price;

                // Deduct Cash
                cash -= cost;
                borrow_cash -= cost;

                // Add to holdings
                holdings["D_QQQ"].shares += 2 * stock_buy;
                holdings["D_QQQ"].total_cost += 2 * cost;

                // Save to history
                history.push_back({ QQQ_day.date, "D_QQQ", 2 * stock_buy, 2 * cost });

            }

            if (QQQ_day.price < 0.5 * QQQ_ATH) {
                // std::cout << "QQQ below ATH 50%\n";
                double entry_price = 0.75 * (QQQ_day.high - QQQ_day.low) + QQQ_day.low;
                int stock_buy = (cash / entry_price);
                if (stock_buy <= 0) return; // not enough cash

                double cost = stock_buy * entry_price;

                // Deduct Cash
                cash -= cost;
                borrow_cash -= 2 * cost;

                // Add to holdings
                holdings["T_QQQ"].shares += 3 * stock_buy;
                holdings["T_QQQ"].total_cost += 3 * cost;

                // Save to history
                history.push_back({ QQQ_day.date, "T_QQQ", 3 * stock_buy, 3 * cost });

            } else if (QQQ_day.price < 0.95 * QQQ_ATH) {
                // std::cout << "QQQ below ATH 90%\n";
                double entry_price = 0.75 * (QQQ_day.high - QQQ_day.low) + QQQ_day.low;
                int stock_buy = (cash / entry_price);
                if (stock_buy <= 0) return; // not enough cash

                double cost = stock_buy * entry_price;

                // Deduct Cash
                cash -= cost;
                borrow_cash -= cost;

                // Add to holdings
                holdings["D_QQQ"].shares += 2 * stock_buy;
                holdings["D_QQQ"].total_cost += 2 * cost;

                // Save to history
                history.push_back({ QQQ_day.date, "D_QQQ", 2 * stock_buy, 2 * cost });
            } else {
                // std::cout << "QQQ above 90%\n";
                double entry_price = 0.75 * (QQQ_day.high - QQQ_day.low) + QQQ_day.low;
                int stock_buy = (cash / entry_price);
                if (stock_buy <= 0) return; // not enough cash

                double cost = stock_buy * entry_price;

                // Deduct Cash
                cash -= cost;

                // Add to holdings
                holdings["QQQ"].shares += stock_buy;
                holdings["QQQ"].total_cost += cost;

                // Save to history
                history.push_back({ QQQ_day.date, "QQQ", stock_buy, cost });
            }

        }

        void output_states() {
            std::cout << "=== Trader State ===\n";
            std::cout << "Algo Name: " << trade_model_name << std::endl;
            std::cout << "Cash: " << cash << std::endl;
            std::cout << "Borrow Cash: " << borrow_cash << std::endl;
            
            std::cout << "Holdings:" << std::endl; 
            for (const auto& [symbol, pos] : holdings) {
                std::cout << symbol << ": " << pos.shares << " shares, total cost: " << pos.total_cost << std::endl;
            }

            std::cout << "Histroy:" << std::endl;
            for (const auto& h : history) {
                std::cout << "  Date: ";
                time_t_output(h.date);
                std::cout << "\n";
                std::cout << "   Symbol: " << h.stock_symbol
                        << "   Qty: " << h.stock_quantity
                        << "   Cost: " << h.cost
                        << "\n";
            }
        }
};

int main() {
    std::cout << "Trading Model: Francis Old" << std::endl;

    // Start and end test date
    time_t enddate = parse_date_to_time_t("10/28/2025");

    time_t startdate = parse_date_to_time_t("06/23/2006");

    std::cout << "Start Date: ";
    time_t_output(startdate);
    std::cout << "\n";

    std::cout << "End Date: ";
    time_t_output(enddate);
    std::cout << "\n";

    int time_diff_sec = difftime(enddate, startdate);
    int time_diff_days = time_diff_sec / (24 * 60 * 60);
    std::cout << "Time diff in days: " << time_diff_days << std::endl;

    auto stock_data_QQQ = load_stock_data("QQQ_Historical_Data.csv");
    Trader trader; // create an object
    for (auto& QQQ_day : stock_data_QQQ) {
        if (QQQ_day.date < startdate) continue;

        if (QQQ_day.date > enddate) break;

        trader.execute_trade(QQQ_day);
        // // Pause for debugging
        // std::cout << "Debug Start:--------------------------------------------------" << std::endl;
        // time_t_output(QQQ_day.date);
        // trader.output_states();
        // std::cout << "Press Enter to continue to next day..." << std::endl;
        // std::cin.get(); // waits for Enter key

    }
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

        data.push_back(record);

    }
    std::reverse(data.begin(), data.end());
    return data;
}

time_t parse_date_to_time_t(const std::string& date_str) {
    int month, day, year;
    if (std::sscanf(date_str.c_str(), "%d/%d/%d", &month, &day, &year) != 3) {
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

void time_t_output(const std::time_t& time) {
    struct tm datetime;
    char output[50];
    datetime = *localtime(&time);
    strftime(output, 50, "%e %B, %Y", &datetime);
    std::cout << output;
}