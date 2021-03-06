#include "../include/Graph.h"
#include <cmath>
#include <iostream>
#include <algorithm>

Graph::Graph(std::ifstream& file)
	: f(file),
	matrix(0),
	xy(0,0)
{
	this->parseGraphFromFile();
}

Graph::~Graph()
{
}

inline int Graph::getValueOfNode(const int& x, const int& y) const
{
	if (x < 0 || x >= this->xy.first || y < 0 || y >= this->xy.second)
		return 0;
	return this->matrix[this->xy.first*y + x];
}

int Graph::getXsize() const
{
	return this->xy.first;
}

int Graph::getYsize() const
{
	return this->xy.second;
}

double Graph::approxDistance(const int& x1, const int& y1, const int& x2, const int& y2) const
{
	return this->getValueOfNode(x2,y2)*sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

std::vector<std::pair<int, int>> Graph::getPatch(const int &x1, const int &y1, const int &x2, const int &y2) const
{
	struct node
	{
		int x;
		int y;
		int G;
		int H;
		int F;
		std::pair<int,int> parent;
		node::node(int x, int y) : x(x), y(y), G(0), H(0), F(0), parent(0,0) {};
	};

	std::vector<node> open{};
	std::vector<node> close{};
	open.emplace_back(x1, y1);
	int smallestF, smallestFID, index;
	node tempQ(0,0);
	bool isInClose, isInOpen;
	node temp(0, 0),temp2(0,0);

	auto isInVector = [](const std::vector<node> &v, int x, int y)->bool
	{
		for (const auto &in: v)
		{
			if (in.x == x && in.y == y)
				return true;
		}
		return false;
	};

	std::vector<std::pair<int,int>> result{};

	while(!open.empty())
	{
		//look for node with smallest F
		smallestF = open[0].F;
		smallestFID = 0;
		index = 0;
		for(const auto &p : open)
		{
			if(p.F < smallestF)
			{
				smallestF = p.F;
				smallestFID = index;
			}
			index++;
		}
		//get node from open
		tempQ = open[smallestFID];
		open.erase(open.begin() + smallestFID);
		close.push_back(tempQ);
		//check if is destination
		if (tempQ.x == x2 && tempQ.y == y2)
		{
			result.emplace_back(tempQ.x,tempQ.y);
			while (tempQ.x != x1 || tempQ.y != y1)
			{
				for (const auto &p : close)
				{
					if (p.x == tempQ.parent.first && p.y == tempQ.parent.second)
					{
						result.emplace_back(p.x, p.y);
						tempQ = p;
						break;
					}
				}
			}
			std::reverse(result.begin(), result.end());
			return result;
		}
		//for all neihgbors
		for(const auto &p : this->getNeighbors(tempQ.x, tempQ.y))
		{
			//is on close list
			isInClose = isInVector(close, p.first, p.second);
			if(!isInClose)
			{
				isInOpen = isInVector(open, p.first, p.second);
				//calc new values
				temp.x = p.first;
				temp.y = p.second;
				temp.parent.first = tempQ.x;
				temp.parent.second = tempQ.y;
				temp.G = tempQ.G + this->approxDistance(temp.x, temp.y, tempQ.x, tempQ.y);
				temp.H = this->approxDistance(temp.x, temp.y, x2, y2);
				temp.F = temp.G + temp.H;

				if(!isInOpen)
				{
					open.push_back(temp);
				}
				else
				{
					index = 0;
					for (const auto &inOpen : open)
					{
						if (inOpen.x == p.first && inOpen.y == p.second)
						{
							smallestFID = index;
							break;
						}
						index++;
					}
					if(temp.G < open[smallestFID].G)
					{
						open[smallestFID] = temp;
					}
				}
			}
		}
	}

	return result;
}

std::vector<std::pair<int, int>> Graph::getNeighbors(const int& x, const int& y) const
{
	std::vector<std::pair<int, int>> nodes;
	if (this->getValueOfNode(x - 1, y))
		nodes.emplace_back(x - 1, y);
	if (this->getValueOfNode(x + 1, y))
		nodes.emplace_back(x + 1, y);
	if (this->getValueOfNode(x, y-1))
		nodes.emplace_back(x, y-1);
	if (this->getValueOfNode(x, y + 1))
		nodes.emplace_back(x, y + 1);
	if (this->getValueOfNode(x - 1, y - 1))
		nodes.emplace_back(x - 1, y - 1);
	if (this->getValueOfNode(x + 1 , y + 1))
		nodes.emplace_back(x + 1, y + 1);
	if (this->getValueOfNode(x - 1, y + 1))
		nodes.emplace_back(x - 1, y + 1);
	if (this->getValueOfNode(x + 1, y - 1))
		nodes.emplace_back(x + 1, y - 1);

	return nodes;
}

void Graph::parseGraphFromFile()
{
	int size;
	this->f >> size;
	this->xy.first = size;
	this->f >> size;
	this->xy.second = size;
	size = this->xy.first*this->xy.second;
	this->matrix.reserve(size);
	int readNumber;
	for(int i = 0; i < size; i++)
	{
		this->f >> readNumber;
		this->matrix.emplace_back(readNumber);
	}
}
