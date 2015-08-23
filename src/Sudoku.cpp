//============================================================================
// Name        : Sudoku Class
// Author      : Christopher Hill
// Version     : v1.0
// Copyright   : May 2015 GPL v3
// Description : Represents a Sudoku grid, with ability to self solve
//============================================================================

#include "Sudoku.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <set>
#include <stdexcept>
#include <vector>

// Creates the puzzle grid in the initial state (i.e supplied clues)
// N.B. See header for format and example.
Sudoku::Sudoku(std::vector<std::vector<int>> grid) :
		// NxN grid filled with zeros
		grid(Sudoku::size, std::vector<int>(Sudoku::size, 0)), // TODO use 'Sudoku::empty' instead of 0?

		// Every row, col, and box can take any value to begin with
		rowCandidates(Sudoku::size, std::set<int> { 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
		colCandidates(Sudoku::size, std::set<int> { 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
		boxCandidates(Sudoku::size, std::set<int> { 1, 2, 3, 4, 5, 6, 7, 8, 9 })
{
	// Validate the supplied grid is NxN shape, i.e. square.  Fail and throw runtime exception when not.
	if (grid.size() != Sudoku::size)
		throw std::runtime_error("Sudoku must contain " + std::to_string(Sudoku::size) + " rows, found " + std::to_string(grid.size()));
	for (const auto& row : grid)
	{
		if (row.size() != Sudoku::size)
			throw std::runtime_error("Sudoku must contain " + std::to_string(Sudoku::size) + " columns, found " + std::to_string(row.size()));
	}

	// Iterate over the supplied grid, and add to our model. Fail and throw runtime exception for invalid ones.
	for (int i = 0; i < Sudoku::size; ++i)
	{
		for (int j = 0; j < Sudoku::size; ++j)
		{
			int num = grid[i][j];
			if (num == Sudoku::blank)
			{	// Ignore blanks
				continue;
			}
			if (num < 1 || num > 9)
			{	// Only accept numbers 1 through 9
				throw std::runtime_error("Can only accept cell values 1 through 9!");
			}
			if (!set(Cell{ i, j }, num))
			{	// Set non-blanks, check they were accepted
				throw std::runtime_error("Invalid initial puzzle grid!");
			}
		}
	}
}

// Try and set the cell to the supplied value, returns true if move was valid and thus succeeded
bool Sudoku::set(const Cell& cell, const int num)
{
	assert(cell.row >= 0 && cell.row < Sudoku::size && cell.col >= 0 and cell.col < Sudoku::size);
	assert(num >= 1 && num <= 9);

	auto box = getBox(cell);

	// Check this cell can accept this value first (i.e. it exists in all 3 candidate lists)
	if (rowCandidates[cell.row].count(num) > 0 &&
		colCandidates[cell.col].count(num) > 0 &&
		boxCandidates[box].count(num) > 0)
	{	// Value allowed, so set it
		grid[cell.row][cell.col] = num;

		// Reduce the candidate sets accordingly
		rowCandidates[cell.row].erase(num);
		colCandidates[cell.col].erase(num);
		boxCandidates[box].erase(num);
		return true;
	}
	else
	{
		return false;
	}
}

// Unset the cell value
void Sudoku::unset(const Cell& cell)
{
	assert(cell.row >= 0 && cell.row < Sudoku::size && cell.col >= 0 and cell.col < Sudoku::size);

	// First check if it is set, otherwise nothing to do!
	if (grid[cell.row][cell.col] == Sudoku::blank)
	{
		return;
	}

	auto box = getBox(cell);
	auto num = grid[cell.row][cell.col];

	assert(num >= 1 && num <= 9);

	// Add back to candidate sets
	rowCandidates[cell.row].insert(num);
	colCandidates[cell.col].insert(num);
	boxCandidates[box].insert(num);

	// Erase the cell
	grid[cell.row][cell.col] = Sudoku::blank;
}

// Given a cell, will get the allowed candidates
std::set<int> Sudoku::getCandidates(const Cell& cell)
{
	assert(cell.row >= 0 && cell.row < Sudoku::size && cell.col >= 0 and cell.col < Sudoku::size);

	std::set<int> candidates;

	// Now fill with the intersection of the 3 candidate sets (row, column, and box)
	std::set<int> tmp;
	std::set_intersection(rowCandidates[cell.row].begin(), rowCandidates[cell.row].end(),
			colCandidates[cell.col].begin(), colCandidates[cell.col].end(),
			std::inserter(tmp, tmp.end()));

	auto box = getBox(cell);
	std::set_intersection(boxCandidates[box].begin(), boxCandidates[box].end(),
			tmp.begin(), tmp.end(),
			std::inserter(candidates, candidates.end()));

	assert(candidates.size() <= 9);
	return candidates;	// C++11, so will be moved, not copied
}

// Will find the cell with the smallest candidate list, plus get the list itself.
// This is the main advantage this algorithm has over simple brute force attempts
// as we pick cells with the smallest candidate list first, and try brute forcing
// them. This naturally reduces the options for remaining cells with larger candidates
// too, making the process more efficient by eliminating them sooner.
// N.B. Many people do this manually on pen and paper.
void Sudoku::getSmallestCandidate(Cell& cell, std::set<int>& candidates)
{
	unsigned int smallest = 10;	// Bigger than worst case with all 1-9 possible

	// Iterate over the grid and find blank cells
	for (int i = 0; i < Sudoku::size; ++i)
	{
		for (int j = 0; j < Sudoku::size; ++j)
		{
			if (grid[i][j] == Sudoku::blank)
			{	// For each blank cell, get the potential candidates it can take
				std::set<int> tmp = getCandidates(Cell { i, j });

				// Check if this is the shortest we have seen yet, if so remember it
				if (tmp.size() < smallest)
				{
					smallest = tmp.size();
					cell = { i, j };
					candidates = tmp;
				}
			}
		}
	}
	assert(candidates.size() <= 9);
}

// Attempt to solve the supplied grid.
// Runs recursively cell by blank cell until all have been solved.
// Returns true if successful, else false.
bool Sudoku::solve()
{
	Cell cell { 0, 0 };
	std::set<int> candidates;

	// Pick a cell with the smallest candidate list to solve, if none are left we can return
	getSmallestCandidate(cell, candidates);
	if (candidates.size() == 0)
	{
		return true;
	}

	// Try each candidate in turn
	for (const auto& val : candidates)
	{	// Set the candidate
		bool ret = set(cell, val);
		assert (ret);

		// Now recursively solve the puzzle for the next shortest cell candidate list
		if (solve())
		{
			return true;
		}
		else
		{	// This choice didn't work, so unset and try the next
			unset(cell);
		}
	}
	// None of them worked
	return false;
}

// Comparison operators
bool
Sudoku::operator==(const Sudoku& other) const
{
	// Only compare grid contents, the candidate lists must also be in sync
	return grid == other.grid;
}

bool
Sudoku::operator!=(const Sudoku& other) const
{
	return !(*this == other);
}

// PrettyPrint the puzzle
std::ostream&
operator<<(std::ostream& os, const Sudoku& obj)
{
	int boxRow = 0, boxCol = 0;

	for (const auto &row : obj.grid)
	{
		if (boxRow++ % Sudoku::boxsize == 0)
		{	// Each hyphen run is ((Sudoku::boxsize * 2) +1) long
			os << "+-------+-------+-------+\n";
		}
		os << "| ";
		for (const auto &cell : row)
		{
			os << cell << " ";
			if (++boxCol % Sudoku::boxsize == 0)
			{
				os << "| ";
			}
		}
		os << "\n";
	}
	os << "+-------+-------+-------+\n";
	return os;
}
