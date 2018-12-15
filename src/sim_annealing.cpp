#include "../include/sim_annealing.hpp"
#include "../include/greedy_search.hpp"
#include "utils.cpp"

#include <math.h>
#include <iostream>
#include <random>

namespace tsp_approx {

sim_annealing::sim_annealing(const annealing_data& data,
                             Adjacency_Matrix& matrix)
    : temperature_limit_{data.temperature_},
      temp_factor_{data.temp_factor_},
      time_limit_{data.time_limit_},
      matrix_{matrix}
{
}

Path sim_annealing::run()
{
    auto gs{greedy_search(matrix_)};
    Path current_path = gs.run();
    best_path = Path(std::vector<int>(), std::numeric_limits<int>::max(), "");
    annealing(current_path);
    return best_path;
}

void sim_annealing::annealing(Path& current_path)
{
    double temperature = temperature_limit_;
    start_time         = utils::time_now();
    auto temp_path     = current_path;

    while (!check_time_bound(utils::time_now())) {
        Path new_path = neighbour(temp_path);
        update_path(new_path, temp_path, temperature);
        temperature *= temp_factor_;
    }
}

bool sim_annealing::check_time_bound(double current_time)
{
    return current_time - start_time >= time_limit_ ? true : false;
}

Path sim_annealing::neighbour(Path& current_path)
{
    Path cpy_path = current_path;

    int index1{utils::random_int(1, current_path.path_.size() - 2)};
    int index2{utils::random_int(1, current_path.path_.size() - 2)};

    auto it1{std::begin(cpy_path) + index1};
    auto it2{std::begin(cpy_path) + index2};

    std::iter_swap(it1, it2);
    cpy_path.recalc_cost(matrix_);
    return cpy_path;
}

void sim_annealing::update_path(Path& new_path, Path& current_path,
                                double temperature)
{
    if (new_path < current_path) {
        current_path = new_path;
        best_path    = current_path < best_path ? current_path : best_path;
    }
    else if (utils::random_double(0.0, 1.0) <
             calc_probability(new_path, current_path, temperature)) {
        current_path = new_path;
    }
}

double sim_annealing::calc_probability(Path& new_path, Path& current_path,
                                       double temperature)
{
    int cost_diff = (current_path.cost_ - new_path.cost_);
    return temperature <= 0 ? 0 : exp(cost_diff / temperature);
}
}  // namespace tsp_approx