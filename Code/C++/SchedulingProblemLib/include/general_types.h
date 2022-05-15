#pragma once

#include <string>
#include <vector>
#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>

namespace scheduling_problem 
{
	struct vertex_weight_t { typedef boost::vertex_property_tag kind; };

	typedef std::size_t weight_t;

	typedef boost::property<vertex_weight_t, weight_t,
		boost::property<boost::vertex_index_t, std::size_t>> VertexProperty;

	typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, VertexProperty> Graph;

	typedef boost::property_map<Graph, boost::vertex_index_t>::type VertexID_Map;

	typedef std::unordered_map<std::string, std::vector<std::size_t>> VertexTiesMap;

	typedef std::unordered_map<std::string, std::unordered_set<std::size_t>> IdsSetsMap;
};
