#pragma once
#include <iostream>
#include <random>
#include <Room.h>
#include <vector>
#include <character/Direction.h>


class MoveProbabilityMatrix
{
public:
	explicit MoveProbabilityMatrix(const std::vector<std::shared_ptr<mazer::Room>>& rooms)
	{
		moveProbabilityMatrix = new double[rooms.size()][4]; // 4 directions: up, down, left, right

		for (const auto& room : rooms)
		{
			auto countPossibleMoves = 0;
			const auto roomNumber = room->GetRoomNumber();
			constexpr auto lastDirectionIndex = static_cast<int>(gamelib::Direction::Right);
			auto countPossibleDirections = lastDirectionIndex + 1; // zero based index

			// Calculate move probabilities for each direction based on wall presence
			moveProbabilityMatrix[roomNumber][static_cast<int>(gamelib::Direction::Up)] = static_cast<double>(!room->HasTopWall()) / countPossibleDirections;  // Up	
			moveProbabilityMatrix[roomNumber][static_cast<int>(gamelib::Direction::Down)] = static_cast<double>(!room->HasBottomWall()) / countPossibleDirections; // Down
			moveProbabilityMatrix[roomNumber][static_cast<int>(gamelib::Direction::Left)] = static_cast<double>(!room->HasLeftWall()) / countPossibleDirections; // Left
			moveProbabilityMatrix[roomNumber][static_cast<int>(gamelib::Direction::Right)] = static_cast<double>(!room->HasRightWall()) / countPossibleDirections; // Right

			// Count possible moves valid, i.e., where probability > 0
			for (int i = 0; i < countPossibleDirections; i++)
			{
				if (moveProbabilityMatrix[roomNumber][i] > 0)
				{
					countPossibleMoves++;
				}
			}

			// Adjust probabilities to ensure they sum to 1.0
			if (countPossibleMoves < countPossibleDirections)
			{
				const auto possibleMoveShares = static_cast<double>(countPossibleMoves) * 1 / countPossibleDirections;
				const auto numImpossibleMoves = countPossibleDirections - countPossibleMoves;
				const auto shareOfMissingMoves = ((1.0 - possibleMoveShares) / (countPossibleMoves));

				// Distribute the missing probability share among possible moves
				for (int i = 0; i < countPossibleDirections; i++)
				{
					if (moveProbabilityMatrix[roomNumber][i] > 0)
					{
						moveProbabilityMatrix[roomNumber][i] += shareOfMissingMoves;
					}
				}
			}

			std::cout << "Room:" << room->GetRoomNumber()
				<< " Up:" << !room->HasTopWall() << "(" << moveProbabilityMatrix[roomNumber][0] << ")"
				<< " Down:" << !room->HasBottomWall() << "(" << moveProbabilityMatrix[roomNumber][1] << ")"
				<< " Left:" << !room->HasLeftWall() << "(" << moveProbabilityMatrix[roomNumber][2] << ")"
				<< " Right:" << !room->HasRightWall() << "(" << moveProbabilityMatrix[roomNumber][3] << ")"
				<< "\n";
		}
	}
	~MoveProbabilityMatrix()
	{

		delete[] moveProbabilityMatrix;
	}

	gamelib::Direction SelectAction(const std::shared_ptr<mazer::Room>& room)
	{
		const auto roomNumber = room->GetRoomNumber();
		return static_cast<gamelib::Direction>(stochastic_selection({
			moveProbabilityMatrix[roomNumber][static_cast<int>(gamelib::Direction::Up)],
			moveProbabilityMatrix[roomNumber][static_cast<int>(gamelib::Direction::Down)],
			moveProbabilityMatrix[roomNumber][static_cast<int>(gamelib::Direction::Left)],
			moveProbabilityMatrix[roomNumber][static_cast<int>(gamelib::Direction::Right)]
			}));
	}

private:
	double(*moveProbabilityMatrix)[4];
	// Selects one index out of a vector of probabilities, "probs"
// The sum of all elements in "probs" must be 1.
	static std::vector<double>::size_type stochastic_selection(const std::vector<double>& probs) {

		// The unit interval is divided into sub-intervals, one for each
		// entry in "probs".  Each sub-interval's size is proportional
		// to its corresponding probability.

		// You can imagine a roulette wheel divided into differently-sized
		// slots for each entry.  An entry's slot size is proportional to
		// its probability and all the entries' slots combine to fill
		// the entire roulette wheel.

		// The roulette "ball"'s final location on the wheel is determined
		// by generating a (pseudo)random value between 0 and 1.
		// Then a linear search finds the entry whose sub-interval contains
		// this value.  Finally, the selected entry's index is returned.

		static std::default_random_engine rng;

		std::uniform_real_distribution<double> dist(0.0, 1.0);
		const double point = dist(rng);
		double cur_cutoff = 0;

		for (std::vector<double>::size_type i = 0; i < probs.size() - 1; ++i) {
			cur_cutoff += probs[i];
			if (point < cur_cutoff) return i;
		}
		return probs.size() - 1;
	}
};

