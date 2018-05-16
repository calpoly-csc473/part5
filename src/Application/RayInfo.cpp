
// Copyright (C) 2018 Ian Dunn
// For conditions of distribution and use, see the LICENSE file


#include "RayInfo.hpp"

#include <iostream>
#include <vector>
#include <map>

#include <RayTracer/PixelContext.hpp>


struct IterationTreeNode
{
	PixelContext::Iteration * iteration = nullptr;
	std::vector<IterationTreeNode *> children;
};

void PrintIterationInfo(std::string const & ParentPrefix, IterationTreeNode * Node, const bool decoration, const bool verbose)
{
	PixelContext::Iteration * Iteration = Node->iteration;

	std::string FirstLinePrefix, OtherLinePrefix;
	if (decoration)
	{
		FirstLinePrefix = "o - ";
		OtherLinePrefix = "|   ";
	}

	std::string p = ParentPrefix + FirstLinePrefix;

	if (! decoration)
	{
		std::cout << ParentPrefix << "----" << std::endl;
	}

	std::cout << p << "  Iteration type: ";

	p = ParentPrefix + OtherLinePrefix;

	if (Iteration->type == PixelContext::IterationType::Primary)
		std::cout << "Primary";
	if (Iteration->type == PixelContext::IterationType::Reflection)
		std::cout << "Reflection";
	if (Iteration->type == PixelContext::IterationType::Refraction)
		std::cout << "Refraction";
	std::cout << std::endl;

	std::cout << p << "             Ray: " << Iteration->ray << std::endl;

	if (Iteration->hitObject)
	{
		std::cout << p << "      Hit Object: (ID #" << Iteration->hitObject->GetID() << " - " << Iteration->hitObject->GetObjectType() << ")" << std::endl;
		std::cout << p << "    Intersection: {" << Iteration->results.intersectionPoint << "} at T = " << Iteration->hitTime << std::endl;
		std::cout << p << "          Normal: {" << Iteration->hitNormal << "}" << std::endl;

		std::cout << p << "     Final Color: {" << Iteration->results.ToColor() << "}" << std::endl;
		std::cout << p << "         Ambient: {" << Iteration->results.ambient << "}" << std::endl;
		std::cout << p << "         Diffuse: {" << Iteration->results.diffuse << "}" << std::endl;
		std::cout << p << "        Specular: {" << Iteration->results.specular << "}" <<std:: endl;
		std::cout << p << "      Reflection: {" << Iteration->results.reflection << "}" << std::endl;
		std::cout << p << "      Refraction: {" << Iteration->results.refraction << "}" << std::endl;
		std::cout << p << "   Contributions: " << Iteration->contributions.x << " Local, " << Iteration->contributions.y << " Reflection, " << Iteration->contributions.z << " Transmission" << std::endl;

		if (verbose)
		{
			int i = 0;
			for (auto const & ShadowInfo : Iteration->shadowRays)
			{
				std::cout << p << "   ShadowRay [" << (i++) << "]: " << ShadowInfo.ray << (ShadowInfo.Hit ? " (in shadow)" : "") << std::endl;
			}
		}
	}
	else
	{
		std::cout << p << "  No intersection." << std::endl;
	}

	if (Iteration->extraInfo.length())
	{
		std::cout << p << "      Extra Info:" << Iteration->extraInfo << std::endl;
	}

	if (Node->children.size())
	{
		if (decoration)
		{
			std::cout << ParentPrefix << "|" << std::endl;
		}

		std::string ChildPrefix, OtherPrefix;
		if (decoration)
		{
			ChildPrefix = "| ";
			OtherPrefix = "  ";
		}

		for (IterationTreeNode * Child : Node->children)
		{
			if (Child != Node->children.back())
			{
				if (decoration)
				{
					std::cout << ParentPrefix << "|\\" << std::endl;
				}
				PrintIterationInfo(ParentPrefix + ChildPrefix, Child, decoration, verbose);
			}
			else
			{
				if (decoration)
				{
					std::cout << ParentPrefix << " \\" << std::endl;
				}
				PrintIterationInfo(ParentPrefix + OtherPrefix, Child, decoration, verbose);
			}
		}
	}
	else
	{
		if (decoration)
		{
			std::cout << ParentPrefix << "|----" << std::endl;
		}
		std::cout << ParentPrefix << std::endl;
	}
}

void RayInfo::PrintRayInfo(RayTracer * rayTracer, Scene * scene, const int x, const int y, const bool decoration)
{
	// Cast rays with context to gather info
	PixelContext context;
	rayTracer->SetDebugContext(& context);
	Pixel const color = rayTracer->CastRaysForPixel(glm::ivec2(x, y));

	std::cout << "Pixel: [" << x << ", " << y << "] Color: " << color << std::endl;

	// Extract tree from context iterations
	std::map<PixelContext::Iteration *, IterationTreeNode *> nodeMap;

	IterationTreeNode * rootNode = new IterationTreeNode();
	nodeMap[nullptr] = rootNode;

	for (PixelContext::Iteration * Iteration : context.iterations)
	{
		IterationTreeNode * node = new IterationTreeNode();
		node->iteration = Iteration;

		nodeMap[Iteration] = node;
		nodeMap[Iteration->parent]->children.push_back(node);
	}

	// Print rays
	for (IterationTreeNode * node : rootNode->children)
	{
		PrintIterationInfo("", node, decoration, decoration);
	}

	std::cout << std::endl;
	std::cout << "--------------------------------------------------------------------------------" << std::endl;
	std::cout << std::endl;
}
