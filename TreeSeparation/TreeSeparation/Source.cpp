/*
*	Copyright (C) 2016 by
*       Jinhu Wang (jinhu.wang@tudelft.nl)
*       Roderik Lindenbergh (r.c.lindenbergh@tudelft.nl) [http://doris.tudelft.nl/~rlindenbergh/]
*       Laser and Optical Remote Sensing
*	Dept. of Geoscience and Remote Sensing
*	TU Delft, https://tudelft.nl
*
*	This is free software; you can redistribute it and/or modify
*	it under the terms of the GNU General Public License Version 3
*	as published by the Free Software Foundation.
*
*	TreeSeparation is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/


#include<iostream>
#include<sstream>

#include"FoxTree.h"


int main(int argc, char** argv)
{
	if (argc < 7)
	{
		std::cerr << "usage: " << argv[0] << " inputFile outputFile radius verticalResolution miniPtsPerCluster direction" << std::endl;
		return 1;
	}

	//Parameters
	double radius, verticalResolution;
	int miniPtsPerCluster, direction;
	std::istringstream(argv[3]) >> radius;
	std::istringstream(argv[4]) >> verticalResolution;
	std::istringstream(argv[5]) >> miniPtsPerCluster;
	std::istringstream(argv[6]) >> direction; //Top-down: 1; Bottom-up: -1

	std::vector<Point3D> points;
	Point3D tempPt;

	//Reading points from ASCII formated file;
	FILE* inFile = fopen(argv[1], "r");
	if (inFile)
	{
		while (!feof(inFile))
		{
			fscanf(inFile, "%lf %lf %lf\n",
				&tempPt.x, &tempPt.y, &tempPt.z);
			points.push_back(tempPt);
		}
		fclose(inFile);
	}
	else
	{
		std::cout << "There was an error in data loading..." << std::endl;
		return 1;
	}

	//Initialization
	FoxTree* foxTree = new FoxTree(points, radius, verticalResolution, miniPtsPerCluster);

	foxTree->separateTrees(1, direction);
	//Output separation results
	foxTree->outputTreesMy(argv[2], foxTree->m_nTrees);
	std::cout << "Finished" << std::endl;

	if (foxTree) delete foxTree; foxTree = nullptr;
	return 0;
}
