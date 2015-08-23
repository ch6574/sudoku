#include <Sudoku.hpp>
#include <cassert>
#include <chrono>
#include <iostream>
#include <vector>

int main()
{
	std::cout <<
		"+-------------+\n" <<
		"| Solver v1.0 |\n" <<
		"+-------------+\n" << std::endl;

	// Create puzzles that we want to solve
	Sudoku puzzle1(	{
	{ 0, 6, 0, 0, 0, 0, 4, 0, 0 },
	{ 3, 4, 1, 2, 0, 0, 0, 0, 8 },
	{ 5, 0, 9, 0, 6, 8, 0, 0, 0 },
	{ 0, 0, 0, 8, 0, 0, 0, 2, 9 },
	{ 2, 1, 7, 0, 5, 0, 8, 4, 6 },
	{ 9, 5, 0, 0, 0, 2, 0, 0, 0 },
	{ 0, 0, 0, 1, 9, 0, 2, 0, 4 },
	{ 6, 0, 0, 0, 0, 4, 3, 9, 1 },
	{ 0, 0, 4, 0, 0, 0, 0, 8, 0 } });

	// Solution to compare against
	const Sudoku puzzle1Solution ( {
	{ 8, 6, 2, 9, 3, 1, 4, 7, 5 },
	{ 3, 4, 1, 2, 7, 5, 9, 6, 8 },
	{ 5, 7, 9, 4, 6, 8, 1, 3, 2 },
	{ 4, 3, 6, 8, 1, 7, 5, 2, 9 },
	{ 2, 1, 7, 3, 5, 9, 8, 4, 6 },
	{ 9, 5, 8, 6, 4, 2, 7, 1, 3 },
	{ 7, 8, 3, 1, 9, 6, 2, 5, 4 },
	{ 6, 2, 5, 7, 8, 4, 3, 9, 1 },
	{ 1, 9, 4, 5, 2, 3, 6, 8, 7 } });

	// Display, solve, and display
	std::cout << "Starting:\n" << puzzle1 << std::endl;

	assert(puzzle1 != puzzle1Solution);					// Verify not equal (i.e. work to do)
	auto start = std::chrono::system_clock::now();
	puzzle1.solve();
	auto end = std::chrono::system_clock::now();
	assert(puzzle1 == puzzle1Solution);					// Verify solved

	std::cout << "Solved:\n" << puzzle1 << std::endl;

	// Show how long it took
	std::chrono::duration<double> elapsed_seconds = end-start;
	std::cout << "Time spent: " << elapsed_seconds.count() << "s" << std::endl;

	return 0;
}
