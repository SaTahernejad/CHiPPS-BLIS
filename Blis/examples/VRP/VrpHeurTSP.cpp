/*===========================================================================*
 * This file is part of a solver for the Vehicle Routing Problem             *
 * developed using the BiCePS Linear Integer Solver (BLIS).                  *
 *                                                                           *
 * This solver is distributed under the Common Public License as part of     * 
 * the COIN-OR repository (http://www.coin-or.org).                          *
 *                                                                           *
 * Authors: Yan Xu, Lehigh University                                        *
 *          Ted Ralphs, Lehigh University                                    *
 *                                                                           *
 * Copyright (C) 2007 Yan Xu and Ted Ralphs.                                 *
 * All Rights Reserved.                                                      *
 *===========================================================================*/

#include <vector>
#include "VrpHeurTSP.h"
#include "VrpModel.h"
#include "VrpVariable.h"

//#############################################################################

//------------------------------------------------------
// Given current LP solution. Rounding number closed to 1 to 1
// Let those edge be in the route, then use nearest neighbor idea
// to construct a tour.
//------------------------------------------------------
// 1. stand on an arbitrary vertex as current vertex.
// 2. find out the lightest edge connecting current vertex and 
//    an unvisited vertex V.
// 3. set current vertex be V.
// 4. mark V as visited.
// 5. if all the verteces in domain are visited then terminate.
// 6. Go to step 2.
//------------------------------------------------------
bool
VrpHeurTSP::searchSolution(double & objectiveValue, double * newSolution)
{
    bool foundSolution = true;

    int j, k, len, tourLen;
    int currV, nextV;
    int count = 0;

    CoinPackedVector * vList = NULL;
    VrpVariable * aVar = NULL;
    
    VrpModel *model = dynamic_cast<VrpModel *>(model_);

    std::vector<VrpVariable *> edges = model->getEdgeList();

    int numVertices = model->getNumVertices();
    int numEdges = edges.size();
    int msgLevel = model->AlpsPar()->entry(AlpsParams::msgLevel);
    int numNodes = model->getNumNodes();
    
    double upperBound = model->getKnowledgeBroker()->getIncumbentValue();

    objectiveValue  = 0.0;
    const double *objCoef = model->getObjCoef();

    //------------------------------------------------------
    // Determine if call heuristic
    //------------------------------------------------------

    if (numNodes > 1) {
	return false;
    }
    
    //------------------------------------------------------
    // Start heuristic
    //------------------------------------------------------

    preNode_ = numNodes;
    
    memset(newSolution, 0, numEdges * sizeof(double));
    //const double * solution = model_->getLpSolution();
    //memcpy(newSolution, solution, numEdges * sizeof(double));


    // Allocated memory and clear buffer.
    if (!visited_) {
	visited_ = new bool [numVertices];
    }
    for (k = 0; k < numVertices; ++k) {
	visited_[k] =  false;
    }
    tour_.clear();

    
    // 1. stand on an arbitrary vertex as current vertex
    currV = 0;
    visited_[currV] = true;

    tour_.push_back(currV);
    tourLen = tour_.size();
    
    while (tourLen < numVertices) {
	// 2. find out the lightest edge connecting current vertex and 
	//    an unvisited vertex V.
	vList = adjList_[currV];
	len = vList->getNumElements();
	for (k = 0; k < len; ++k) {
	    nextV = (vList->getIndices())[k];
	    if (!visited_[nextV]) {
		tour_.push_back(nextV);
		// 3. set current vertex be V.
		currV = nextV;
		// 4. mark V as visited.
		visited_[nextV] = true;
		break;
	    }
	}
	tourLen = tour_.size();
    }
    
    // Transfer the tour into a solution
    int beg, end;
    int tourLenAddOne = tourLen + 1;
    count = 0;
    currV = tour_[0];
    for (k = 1; k < tourLenAddOne; ++k) {
        if (k < tourLen) {
            nextV = tour_[k];
        }
        else {  // edge {currV, tour_[0]}
            nextV = tour_[0];
        }
        
	// Edge is stored in the way that beginning vertex index is 
	// greater than the end vertex index.
	if (currV < nextV) {
	    beg = currV;
	    end = nextV;
	}
	else {
	    beg = nextV;
	    end = currV;
	}
	// Find edge(variable) {beg, end} in the edge(variable) list and 
	// set it to 1.0. 
	for (j = 0; j < numEdges; ++j) {
	    aVar = edges[j];
	    if (aVar->getv0() == beg) {
		if (aVar->getv1() == end) {
                    ++count;
		    newSolution[j] = 1.0;
                    objectiveValue += objCoef[j];
                    if (objectiveValue >= upperBound) {
                        foundSolution = false;
                        break;
                    }
		}
	    }
	}
	currV = nextV;
    }
    
    if (msgLevel > 200 && foundSolution) {
        std::cout << "count = " << count << std::endl;
	std::cout << "Found a better tour : len = " << tourLen 
                  << " ; cost = " << objectiveValue << std::endl;
        assert(count == tourLen);

	count = 0;
	for (k = 0; k < tourLen; ++k) {
	    std::cout << tour_[k]+1 << " ";
	    ++count;
	    if (count % 15 == 0) {
		std::cout << std::endl;
	    }
	}
	std::cout << std::endl;
    }
    
    return foundSolution;
}

//#############################################################################

void
VrpHeurTSP::createAdjList(VrpModel *model) 
{
    int k, beg, end;
    double cost;

    VrpVariable *var = NULL;
    std::vector<VrpVariable *> edges = model->getEdgeList();
    int numVertices = model->getNumVertices();
    int numEdges = edges.size();

    assert(numEdges == model->getNumEdges());

    int msgLevel = model->AlpsPar()->entry(AlpsParams::msgLevel);
    AlpsTimer timer;

    if (msgLevel > 0) {
	timer.start();
    }

    // Allocate memory
    for (k = 0; k < numVertices; ++k) {
	CoinPackedVector * vec = new CoinPackedVector;
	adjList_.push_back(vec);
    }

    // Create adj list
    for (k = 0; k < numEdges; ++k) {
	var = edges[k];
	cost = var->getObjCoef();
        beg = var->getv0();
        end = var->getv1();
	
	// Symetric
	adjList_[beg]->insert(end, cost);
	adjList_[end]->insert(beg, cost);
    }    

    // Sort the adjacent list of each vertex incremently
    // based on weights.
    for (k = 0; k < numVertices; ++k) {
	adjList_[k]->sortIncrElement();
	if (msgLevel > 300) {
	    std::cout << "vertex " << k << " : size " 
		      << adjList_[k]->getNumElements() << std::endl;
	}
    }
    
    if (msgLevel > 200) {
	// Creating adjlist takes the half time of solving a LP.
	std::cout << "createAdjList took " << timer.getCpuTime() 
		  << " seconds." << std::endl;
    }
}

//#############################################################################
