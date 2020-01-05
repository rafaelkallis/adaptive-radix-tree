/**
 * @file bench entrypoint
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#define PICOBENCH_IMPLEMENT
#include "picobench/picobench.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  picobench::runner runner;
  runner.parse_cmd_line(argc, argv);
  if (runner.should_run()) {
    runner.set_default_state_iterations({1600000});
    runner.run_benchmarks();
    auto report = runner.generate_report();
    report.to_text(std::cout);
  }
  return 0;
}
