#include <SFML/Graphics.hpp>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range2d.h>
#include <chrono>
#include <cassert>
#include <complex>
#include <iostream>
#include <string>
#include <thread>
#include <fstream>



using Complex = std::complex<double>;

int mandelbrot(Complex const &c)
{
  int i = 0;
  auto z = c;
  for (; i != 256 && norm(z) < 4.; ++i)
  {
    z = z * z + c;
  }
  return i;
}

auto to_color(int k)
{
  return k < 256 ? sf::Color{static_cast<sf::Uint8>(10 * k), 0, 0} : sf::Color::Black;
}

int main()
{


  // Canvas properties
  int const display_width{800};
  int const display_height = display_width;

  // Setting complex plane
  Complex const top_left{-2.2, 1.5};
  Complex const lower_right{0.8, -1.5};
  auto const diff = lower_right - top_left;

  auto const delta_x = diff.real() / display_width;
  auto const delta_y = diff.imag() / display_height;

  // Definition of two vectors containing the grain sizes and the corresponding times respectively
  std::vector<int> grains;
  std::vector<double> times;
  
  
 // Creation of the image used for the pixel art of the Mandelbrot set
  sf::Image image;
  image.create(display_width, display_height);

  // Creating a vector of pairs, in order to be able to write the last for loop in a very compact form
  std::vector<std::pair<int, double>> grain_time;

  // Choosing different grain sizes of the parallel_for loop in order to check the time performances
  for (int grain_size = 1; grain_size <= display_height; grain_size < 20 ? ++grain_size : grain_size += 20)
  {
    // Timing the imaging process
    auto start = std::chrono::steady_clock::now();

    tbb::simple_partitioner partitioner{};

    // Dividing the 2D canvas into smaller pieces and running each one on a different thread
    tbb::parallel_for(
        tbb::blocked_range2d<int>(0, display_height, grain_size, 0, display_width, grain_size),
        [&](const tbb::blocked_range2d<int> &minijob)
        {
          for (int row = minijob.rows().begin(); row != minijob.rows().end(); ++row)
          {
            for (int column = minijob.cols().begin(); column != minijob.cols().end(); ++column)
            {
              auto k = mandelbrot(top_left + Complex{delta_x * column, delta_y * row});
              image.setPixel(column, row, to_color(k));
            }
          }
        },
        partitioner);

    auto end = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    times.emplace_back(time / 1000.);
    grains.emplace_back(grain_size); 

    // Printing the different grain sizes and the corresponding elapsed times 
    std::cout << "Grain size: " << grain_size << ", elapsed time: " << time << " microseconds" << std::endl;

    grain_time.emplace_back(grain_size, time / 1000.);

  }
  // Saving the last-iteration image only (corresponding to a grain size of 'display_height')
  image.saveToFile(static_cast<std::string>("Mandelbrot.png"));

  std::ofstream out("Time_vs_grain_size.txt", std::ios::out);
  out << "Grain sizes and corresponding execution times\n\nGrain size\tExecution time [ms]\n\n";
  
  
  for (auto const &[grain_size, time] : grain_time)
  {
    // Writing the different grain sizes and the corresponding elapsed times in a .txt file
    out << grain_size << "\t\t" << time << '\n';
  }

  // Finding the minimum (in terms of both grain size and time), 
  // printing it and writing it in a .txt file
  auto min_time = std::min_element(times.begin(), times.end());
  int min_grain_size = grains[std::distance(times.begin(), min_time)];
  std::cout << "\nThe minimum execution time is " << *min_time << " ms and corresponds to a grain size of "
            << min_grain_size << ".\n\n";

  out << "\nThe minimum execution time is " << *min_time << " ms and corresponds to a grain size of "
      << min_grain_size << ".\n";
  out.close();

  
}
