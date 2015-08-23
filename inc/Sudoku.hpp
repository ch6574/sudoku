#ifndef SUDOKU_HPP_
#define SUDOKU_HPP_
//============================================================================
// Name        : Sudoku Class
// Author      : Christopher Hill
// Version     : v1.0
// Copyright   : May 2015 GPL v3
// Description : Represents a Sudoku grid, with ability to self solve
//============================================================================

#if __cplusplus <= 199711L
#error This needs at least a C++11 compliant compiler
#endif

#include <iostream>
#include <set>
#include <vector>

/* Sudoku grid is fixed at 9x9, with 9 boxes of 3x3.
 *
 * Internally this is a vector of 9 vectors of 9 integers, one for each cell
 * Values range from 1-9, with 0 to represent 'blank' or 'unknown'.
 *
 * e.g.
 * Sudoku puzzle({
 *		{0,6,0,0,0,0,4,0,0},
 *		{3,4,1,2,0,0,0,0,8},
 *		{5,0,9,0,6,8,0,0,0},
 *		{0,0,0,8,0,0,0,2,9},
 *		{2,1,7,0,5,0,8,4,6},
 *		{9,5,0,0,0,2,0,0,0},
 *		{0,0,0,1,9,0,2,0,4},
 *		{6,0,0,0,0,4,3,9,1},
 *		{0,0,4,0,0,0,0,8,0}
 *	});
 *  std::cout << puzzle << std::endl;
 *  if (puzzle.solve()) {
 *      std::cout << puzzle << std::endl;
 *  } else {
 *      std::cout << "Cannot solve." << std::endl;
 *  }
 */
class Sudoku
{
	static constexpr int size = 9;		// Size of puzzle
	static constexpr int boxsize = 3;	// Size of 'box'
	static constexpr int blank = 0;		// Blank / unknown cells

	std::vector<std::vector<int>> grid;		// The puzzle data grid

	std::vector<std::set<int>> rowCandidates;	// A vector per row containing the set of remaining candidates (i.e. unused numbers)
	std::vector<std::set<int>> colCandidates;	// Again, only per column
	std::vector<std::set<int>> boxCandidates;	// Again, only per box

	// Represents a cell on the grid, simple row/column offsets (could also use std::tuple<int, int> instead)
	struct Cell
	{
		int row, col;
	};

	// For a given cell, return what box it is in
	int getBox(const Cell& cell)
	{
		int boxRow = cell.row / Sudoku::boxsize; // n.b. this truncates fractions
		int boxCol = cell.col / Sudoku::boxsize;
		return (boxRow * Sudoku::boxsize) + boxCol;
	};

	// Try and set the cell to the supplied value, returns true if move was valid and thus succeeded
	bool
	set(const Cell& cell, const int num);

	// Unset the cell value
	void
	unset(const Cell& cell);

	// Given a cell, return the allowed candidates
	std::set<int>
	getCandidates(const Cell& cell);

	// Get the cell with the smallest candidate list, plus the list itself
	void
	getSmallestCandidate(Cell& cell, std::set<int>& candidate);

	// Prevent creating empty objects
	Sudoku() = delete;

public:
	// Creates the puzzle grid in the initial state (i.e supplied clues)
	Sudoku(std::vector<std::vector<int>> grid);

	// Attempt to solve the supplied grid, returns true if successful, else false
	bool
	solve();

	// Comparison operators
	bool
	operator==(const Sudoku&) const;

	bool
	operator!=(const Sudoku&) const;

	// PrettyPrint the puzzle
	friend std::ostream&
	operator<<(std::ostream&, const Sudoku&);
};

#endif /* SUDOKU_HPP_ */
