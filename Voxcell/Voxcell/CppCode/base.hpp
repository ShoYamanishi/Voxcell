#ifndef _WAILEA_UNDIRECTED_BASE_HPP_
#define _WAILEA_UNDIRECTED_BASE_HPP_

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <exception>

#ifdef UNIT_TESTS
#include "gtest/gtest_prod.h"
#endif

/**
 * @file undirected/base.hpp
 *
 * @brief Base classes that implement the basic concept of undirected graph
 *        G(V,E) and manipulations.
 *     
 * @details
 *        G is represented by Graph, v in V is by Node, and e in E is by Edge.
 *        It consists of three main classes: Graph, Node, and Edge.
 *        A Graph contains Node and Edge objects that belong to the graph.
 *        A Graph owns the Node and Edge objects belonging to it.
 *        A Node represents an undirected  graph node. 
 *        A Node has an ordered incident edge list.
 *        At one time, a Node object can belong to at most one Graph.
 *        An Edge represents an undirected graph edge.
 *        An Edge holds two incident nodes information.
 *
 *        Supported manipulations are:
 *        - add a node to a graph.
 *        - add an edge between two existing nodes in a graph.
 *        - move an edge from the current incident nodes to a new pair in 
 *          a graph.
 *        - remove an edge from a graph.
 *        - remove a node together with the incident edges from a graph.
 *        - contract an edge.
 *        - split an edge into two with a new node in between.
 *        - find the edge-induced node set for a given set of edges.
 *        - find the node-induced edge set for a given set of nodes.
 *        - move a subgraph from a graph to another.
 *        - make a copy of subgraph.
 *        - find a cutset (edge set) between a given subgraph and its 
 *          complementary graph in a graph.
 *        - remove a cutset (edge set) between a given subgraph and its 
 *          complementary graph in a graph.
 *
 *         Node and Edge objects are intended to be managed by Graph objects.
 *         and hence copy construction, move construction, copy assignment, and
 *         move assignment are prohibited.
 *
 *         Only move construction and move assignment are permitted for Graph 
 *         objects. Hence, Graph objects are contained in the standard 
 *         containers in limited conditions just like std::unique_ptr objects.
 * 
 *         Graph object is copied via a specific method of Graph called
 *         copySubgraph().
 *
 *         Instantiation of Graph, Node, and Edge objects are allowed only by
 *         the factory classes inherited  from BaseGraphFactory class.
 *
 * @remarks design goals
 *          - C++ 14 compliant. The development started in 2016.
 *          - Portability utilizing standards.
 *          - Robustness against errors.  Consistency upon out-of-memory 
 *             situations.
 *          - Easy graph manipulations including splitting, contracting, 
 *            and moving & copy subgraphs.
 *          - Expansion of funcionalities via inheritance (mixins).
 *          - No PIMPL as there is no need to hide the details, and to maximize
 *            the chances for inlining by putting definitions of the small
 *            functions in the header.
 */

namespace Wailea {

namespace Directed {

    class DiGraph;
    class DiNode;
    class DiEdge;
    class DiNodeTests;
    class DiEdgeTests;
    class DiGraphTests;
}


namespace Undirected { 

using namespace std;


class Graph;
class Edge;
class Node;

using generation_t        = unsigned long long;
using utility_t           = unsigned long long;
using node_list_t         = list<unique_ptr<Node>>;
using node_list_it_t      = node_list_t::iterator;
using edge_list_t         = list<unique_ptr<Edge>>;
using edge_list_it_t      = edge_list_t::iterator;
using node_incidence_t    = list<edge_list_it_t>;
using node_incidence_it_t = node_incidence_t::iterator;
using node_ptr_t          = unique_ptr<Node>;
using edge_ptr_t          = unique_ptr<Edge>;

/** @class  Constants
 *  @brief  constants used in the argument of invalid_argument exception
 *          raised by the operations.
 */
class Constants {

  public:
    static constexpr const char* kExceptionEdgeAlreadyInGrpah
                                      = "edge already part of a graph";
    static constexpr const char* kExceptionEdgeNotInGraph
                                      = "edge not in this graph";
    static constexpr const char* kExceptionNodeAlreadyInGrpah
                                      = "node already part of a graph";
    static constexpr const char* kExceptionNodeNotInGraph
                                      = "node not in this graph";
    static constexpr const char* kExceptionWrongType
                                      = "wrong type for this operation";
    static constexpr const char* kExceptionStackIndex
                                      = "stack index is invalid";
    static constexpr const char* kExceptionNotEmpty
                                      = "Graph is not empty before this operation";
};


/** @class  Node
 *  @brief  This class represents the basic undirected node concept.
 */
class Node {

  public:


    /** @brief  default-constructs Node to this.
     */
    inline Node() noexcept;


    /** @brief copy construction is not allowed.
     */
    Node(const Node& rhs)            = delete;


    /** @brief move construction is not allowed.
     */
    Node(Node&& rhs)                 = delete;


    /** @brief copy assignment operator is not allowed.
     */
    Node& operator=(const Node& rhs) = delete;


    /** @brief move assignment operator is not allowed.
     */
    Node& operator=(Node&& rhs)      = delete;


    /** @brief  destructs this object
     */
    inline virtual ~Node() noexcept;


    /** @brief  returns pair of begin and end iterators of the incident edges
     *          in a list.
     *
     *  @throw  std::invalid_argument(Constants::kExceptionNodeNotInGraph)
     *              if the node does not belong to a graph.
     *
     *  @return pair of begin and end iterators
     */
    inline pair<node_incidence_it_t,node_incidence_it_t> incidentEdges();



    /** @brief  reorders the incident edges list
     *
     *  @throw  std::invalid_argument(Constants::kExceptionNodeNotInGraph)
     *              if the node does not belong to a graph.
     *
     *  @param  reorderedEdges (in):list incident edges in the new order.
     */
    inline void reorderIncidence(vector<edge_list_it_t>&& reorderedEdges);


    /** @brief  reorders the incident edges list
     *
     *  @throw  std::invalid_argument(Constants::kExceptionNodeNotInGraph)
     *              if the node does not belong to a graph.
     *
     *  @param  edges (in):list incident edges in the new order.
     */
    inline void reorderIncidence(list<edge_list_it_t>&& edges);


    /** @brief  returns the degree of this node
     *
     *  @throw  std::invalid_argument(Constants::kExceptionNodeNotInGraph)
     *              if the node does not belong to a graph.
     *
     *  @return degree
     */
    inline size_t degree() const;


    /** @brief  resets the general purpose counter to zero.
     */
    inline void resetGeneration() noexcept;

    /** @brief returns the iteratror in mNodes list of the containing Graph.
     */
    inline node_list_it_t backIt() const noexcept;

    /** @brief returns the containing graph.
     *
     *  @throw  std::invalid_argument(Constants::kExceptionNodeNotInGraph)
     *              if the node does not belong to a graph.
     *
     *  @return reference to the containing graph.
     */
    inline Graph& graph();


    /** @brief returns if the containing graph is set.
     *
     *  @return true if the containing graph is set.
     */
    inline bool isGraphValid() const noexcept;


    /** @brief pushes a forward link to the stack.
     *
     *  @param  it   (in): iterator to a Node.
     *
     *  @return index to the link pushed on the stack
     */
    inline size_t pushIGForwardLink(node_list_it_t it);


    /** @brief pushes a backward link to the stack.
     *
     *  @param  it   (in): iterator to a Node.
     *
     *  @return index to the link pushed on the stack
     */
    inline size_t pushIGBackwardLink(node_list_it_t it);


    /** @brief pops the top of the forward link stack.
     *
     *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
     *              if the stack is empty
     *
     *  @return the iterator on the top of the forward link stack
     */
    inline node_list_it_t popIGForwardLink();


    /** @brief pops the top of the backward link stack.
     *
     *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
     *              if the stack is empty
     *
     *  @return the iterator on the top of the backward link stack
     */
    inline node_list_it_t popIGBackwardLink();


    /** @brief returns the size of the forward link stack.
     */
    inline size_t IGForwardLinksSize() const;


    /** @brief returns the size of the backward link stack.
     */
    inline size_t IGBackwardLinksSize() const;


    /** @brief returns the iterator on the forward stack
     *         at the specified index.
     *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
     *              if the index is invalid.
     *
     *  @param  index (in): the index into the stack.
     *         The index is from the stack top down to the bottom.
     *
     *  @return the iterator on the stack
     */
    inline node_list_it_t IGForwardLink(size_t index=0) const;
    inline Node& IGForwardLinkRef(size_t index=0) const;

    /** @brief returns the iterator on the backward stack
     *         at the specified index.
     *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
     *              if the index is invalid.
     *
     *  @param  index (in): the index into the stack.
     *         The index is from the stack top down to the bottom.
     *
     *  @return the iterator on the stack
     */
    inline node_list_it_t IGBackwardLink(size_t index=0) const;
    inline Node& IGBackwardLinkRef(size_t index=0) const;


    /** @brief set an iterator on the forward link stack at the specified index
     *
     *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
     *              if the stack is empty
     *
     *  @param  it    (in): the iterator to be set.
     *
     *  @param  index (in): the index into the stack.
     *         The index is from the stack top down to the bottom.
     */
    inline void setIGForwardLink (node_list_it_t it, size_t index=0);


    /** @brief set an iterator on the backward link stack at the specified
     *         index
     *
     *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
     *              if the stack is empty
     *
     *  @param  it    (in): the iterator to be set.
     *
     *  @param  index (in): the index into the stack.
     *         The index is from the stack top down to the bottom.
     */
    inline void setIGBackwardLink (node_list_it_t it, size_t index=0);


    /** @brief pushes a utility value to the stack.
     *
     *  @param  val   (in): value to be pushed
     *
     *  @return index to the value pushed on the stack
     */
    inline size_t pushUtility(utility_t val);


    /** @brief pops the top of the utility stack.
     *
     *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
     *              if the stack is empty
     *
     *  @return the value on the top of the utility stack
     */
    inline utility_t popUtility();


    /** @brief returns the size of the utility stack
     */
    inline size_t utilitySize() const;


    /** @brief returns the value on the utility stack at the specified index
     *
     *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
     *              if the stack is empty
     *
     *  @param  index (in): the index into the stack.
     *         The index is from the stack top down to the bottom.
     *
     *  @return the value on the stack
     */
    inline utility_t utility(size_t index=0) const;


    /** @brief set a value to the utility stack at the specified index
     *
     *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
     *              if the stack is empty
     *
     *  @param  index (in): the index into the stack.
     *         The index is from the stack top down to the bottom.
     *
     *  @param  val (in): new value to be set
     */
    inline void setUtility(utility_t val, size_t index=0);
    
  private:

    /** @brief back pointer to the Graph object it belongs.
     *         nulptr if it is not managed by any Graph object.
     */
    Graph*           mGraph;      

    /** @brief internal general purpose counter.
     *         an example usage is to identify the edge induced nodes.
     */
    generation_t     mGeneration;

    /** @brief iterator in the node list of the graph it belongs to.
     *         it indicates the position of this node in the list
     *         for insersion, deletion, and reorderint operations.
     */
    node_list_it_t   mBackIt;

    /** @brief utility stack that holds the forward links to other nodes
     *         Available for the users (subclasses).
     */
    vector<node_list_it_t> mIGForwardLinkStack;

    /** @brief utility stack that holds the backward links to other nodes
     *         Available for the users (subclasses).
     */
    vector<node_list_it_t> mIGBackwardLinkStack;

    /** @brief utility stack that holds integer values.
     *         Available for the users (subclasses).
     */
    vector<utility_t> mUtilityStack;

  protected:
    /** @brief incicence edge list that holds the iterators of mEdges
     *         list of the graph it belongs to.
     */
    node_incidence_t mIncidence;

  friend class Graph;
  friend class Wailea::Directed::DiGraph;
  friend class Wailea::Directed::DiEdge;

#ifdef UNIT_TESTS
  friend class NodeTests;
  friend class GraphTests;
  friend class Wailea::Directed::DiNodeTests;
  friend class Wailea::Directed::DiGraphTests;
#endif

};

/** @class  Edge
 *  @brief  This class represents the basic undirected edge concept.
 */
class Edge {

  public:

    /** @brief  default-constructs Edge to this.
     */
    inline Edge() noexcept;


    /** @brief copy constructor is not allowed.
     */
    Edge(const Edge& rhs)            = delete;


    /** @brief move constructor is not allowed.
     */
    Edge(Edge&& rhs)                 = delete;


    /** @brief copy assignment operator is not allowed.
     */
    Edge& operator=(const Edge& rhs) = delete;


    /** @brief move assignment operator is not allowed.
     */
    Edge& operator=(Edge&& rhs)      = delete;


    /** @brief  destructs this object
     */
    inline virtual ~Edge() noexcept;


    /** @brief  returns the incident node 1
     *
     *  @throw  std::invalid_argument(Constants::kExceptionEdgeNotInGraph)
     *              if the edge does not belong to a graph.
     *
     *  @return reference to node 1
     */
    inline Node& incidentNode1() const;


    /** @brief  returns the incident node 2
     *
     *  @throw  std::invalid_argument(Constants::kExceptionEdgeNotInGraph)
     *              if the edge does not belong to a graph.
     *
     *  @return reference to node 2
     */
    inline Node& incidentNode2() const;


    /** @brief  returns the iterator in the incidence list of node 1
     *
     *  @throw  std::invalid_argument(Constants::kExceptionEdgeNotInGraph)
     *              if the edge does not belong to a graph.
     *
     *  @return iterator in the incidence list of node 1.
     */
    inline node_incidence_it_t incidentBackItNode1() const noexcept;


    /** @brief  returns the iterator in the incidence list of node 2
     *
     *  @throw  std::invalid_argument(Constants::kExceptionEdgeNotInGraph)
     *              if the edge does not belong to a graph.
     *
     *  @return iterator in the incidence list of node 2.
     */
    inline node_incidence_it_t incidentBackItNode2() const noexcept;


    /** @brief  returns the adjacent node of this edge opposed to the node 
     *          specified by the parameter.
     *          
     *  @param  v (in): opposing incident node of this edge.
     *
     *  @throw  std::invalid_argument(Constants::kExceptionEdgeNotInGraph)
     *              if the edge does not belong to a graph.
     *
     *  @return the other incident node of this edge.
     */
    inline Node& adjacentNode(Node& v) const;


    /** @brief  resets the general purpose counter to zero.
     */
    inline void resetGeneration() noexcept;


    /** @brief returns the iteratror in mNodes list of the containing Graph.
     */
    inline edge_list_it_t backIt() const noexcept;


    /** @brief returns the containing graph.
     *
     *  @throw  std::invalid_argument(Constants::kExceptionEdgeNotInGraph)
     *              if the edge does not belong to a graph.
     *
     *  @return reference to the containing graph.
     */
    inline Graph& graph();


    /** @brief returns if the containing graph is set.
     *
     *  @return true if the containing graph is set.
     */
    inline bool isGraphValid() const noexcept;


    /** @brief pushes a forward link to the stack.
     *
     *  @param  it   (in): iterator to an Edge.
     *
     *  @return index to the link pushed on the stack
     */
    inline size_t pushIGForwardLink(edge_list_it_t it);


    /** @brief pushes a backward link to the stack.
     *
     *  @param  it   (in): iterator to an Edge.
     *
     *  @return index to the link pushed on the stack
     */
    inline size_t pushIGBackwardLink(edge_list_it_t it);


    /** @brief pops the top of the forward link stack.
     *
     *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
     *              if the stack is empty
     *
     *  @return the iterator on the top of the forward link stack
     */
    inline edge_list_it_t popIGForwardLink();


    /** @brief pops the top of the backward link stack.
     *
     *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
     *              if the stack is empty
     *
     *  @return the iterator on the top of the backward link stack
     */
    inline edge_list_it_t popIGBackwardLink();


    /** @brief returns the size of the forward link stack.
     */
    inline size_t IGForwardLinksSize() const;


    /** @brief returns the size of the backward link stack.
     */
    inline size_t IGBackwardLinksSize() const;


    /** @brief returns the iterator on the forward stack
     *         at the specified index.
     *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
     *              if the index is invalid.
     *
     *  @param  index (in): the index into the stack.
     *         The index is from the stack top down to the bottom.
     *
     *  @return the iterator on the stack
     */
    inline edge_list_it_t IGForwardLink(size_t index=0) const;
    inline Edge& IGForwardLinkRef(size_t index=0) const;

    /** @brief returns the iterator on the backward stack
     *         at the specified index.
     *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
     *              if the index is invalid.
     *
     *  @param  index (in): the index into the stack.
     *         The index is from the stack top down to the bottom.
     *
     *  @return the iterator on the stack
     */
    inline edge_list_it_t IGBackwardLink(size_t index=0) const;
    inline Edge& IGBackwardLinkRef(size_t index=0) const;

    /** @brief set an iterator on the forward link stack at the specified index
     *
     *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
     *              if the stack is empty
     *
     *  @param  it    (in): the iterator to be set.
     *
     *  @param  index (in): the index into the stack.
     *         The index is from the stack top down to the bottom.
     */
    inline void setIGForwardLink (edge_list_it_t it, size_t index=0);


    /** @brief set an iterator on the backward link stack at the specified
     *         index
     *
     *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
     *              if the stack is empty
     *
     *  @param  it    (in): the iterator to be set.
     *
     *  @param  index (in): the index into the stack.
     *         The index is from the stack top down to the bottom.
     */
    inline void setIGBackwardLink (edge_list_it_t it, size_t index=0);


    /** @brief pushes a utility value to the stack.
     *
     *  @param  val   (in): value to be pushed
     *
     *  @return index to the value pushed on the stack
     */
    inline size_t pushUtility(utility_t val);


    /** @brief pops the top of the utility stack.
     *
     *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
     *              if the stack is empty
     *
     *  @return the value on the top of the utility stack
     */
    inline utility_t popUtility();


    /** @brief returns the size of the utility stack
     */
    inline size_t utilitySize() const;


    /** @brief returns the value on the utility stack at the specified index
     *
     *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
     *              if the stack is empty
     *
     *  @param  index (in): the index into the stack.
     *         The index is from the stack top down to the bottom.
     *
     *  @return the value on the stack
     */
    inline utility_t utility(size_t index=0) const;


    /** @brief set a value to the utility stack at the specified index
     *
     *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
     *              if the stack is empty
     *
     *  @param  index (in): the index into the stack.
     *         The index is from the stack top down to the bottom.
     *
     *  @param  val (in): new value to be set
     */
    inline void setUtility(utility_t val, size_t index=0);

  private:

    /** @brief back pointer to the Graph object it belongs.
     *         nulptr if it is not managed by any Graph object.
     */
    Graph*              mGraph;

    /** @brief iterator in the edge list of the graph it belongs to.
     *         it indicates the position of this node in the list
     *         for insersion, deletion, and reorderint operations.
     */
    edge_list_it_t      mBackIt;

    /** @brief incident node 1 as an iterator in mEdges edge list of the graph
     */
    node_list_it_t      mIncidentNode1;

    /** @brief incident node 2 as an iterator in mEdges edge list of the graph
     */
    node_list_it_t      mIncidentNode2;

    /** @brief iterator in the incidence edge list of the node 1
     *         it indicates the position of this edge in the list
     *         for insersion, deletion, and reorderint operations.
     */
    node_incidence_it_t mBackItNode1;

    /** @brief iterator in the incidence edge list of the node 2
     *         it indicates the position of this edge in the list
     *         for insersion, deletion, and reorderint operations.
     */
    node_incidence_it_t mBackItNode2;

    /** @brief internal general purpose counter.
     *         an example usage is to identify the edge induced nodes.
     */
    generation_t        mGeneration;

    /** @brief utility stack that holds the links to other edges
     *         Available for the users (subclasses).
     */
    vector<edge_list_it_t> mIGForwardLinkStack;
    vector<edge_list_it_t> mIGBackwardLinkStack;

    /** @brief utility stack that holds integer values.
     *         Available for the users (subclasses).
     */
    vector<utility_t> mUtilityStack;

  friend class Graph;
  friend class Node;

  friend class Wailea::Directed::DiGraph;
  friend class Wailea::Directed::DiNode;

#ifdef UNIT_TESTS
  friend class EdgeTests;
  friend class GraphTests;
  friend class Wailea::Directed::DiEdgeTests;
  friend class Wailea::Directed::DiGraphTests;
#endif

};


/** @class  Graph
 *  @brief  This class represents the basic undirected graph concept.
 *          It contains zero or more nodes and edges.
 */
class Graph {

  public:


    /** @brief  default-constructs Graph to this.
     */
    inline Graph() noexcept;


    /** @brief copy constructor is not allowed.
     */
    Graph(const Graph& rhs) = delete;


    /** @brief  move-constructs Graph to this.
     *
     *  @param  rhs  (in): Graph whose contents are swapped with this.
     */
    inline Graph(Graph&& rhs) noexcept;


    /** @brief copy assignment operator is not allowed.
     */
    Graph& operator=(const Graph& rhs) = delete;


    /** @brief  move-assigns Graph to this.
     *
     *  @param  rhs  (in): Graph whose contents are swapped with this.
     *
     *  @return reference to this.
     */
    inline Graph& operator=(Graph&& rhs) noexcept;


    /** @brief  destructs this object
     */
    inline virtual ~Graph() noexcept;


    /** @brief  adds the specified node to the graph. The ownership of the node
     *          is transferred from the caller to the graph.
     *  
     *  @param  n     (in): node to be added.
     *
     *  @return reference to the inserted node.
     * 
     *  @throws invalid_argument(Constants::kExceptionNodeAlreadyInGraph)
     *              if n is already part of a graph.
     *
     *  @throws bad_alloc()
     *              if default allocator is used for vector<>.
     *              if any insert of list<> has failed due to memory shortage.
     *
     *  @details n is inserted at the end of mNodes list in the graph.
     *
     *  @remark exception safety:
     *              If an exception is thrown, all the data structures in the
     *              graph and the parameters don't change.
     */
    Node& addNode(node_ptr_t&& n);


    /** @brief  adds the specified node to the graph. The ownership of the node
     *          is transferred from the caller to the graph.
     *  
     *  @param  n     (in): node to be added.
     *  @param  pos   (in): the position in mNodes before which this node is
     *                      inserted.
     *  @return reference to the inserted node.
     * 
     *  @throws invalid_argument(Constants::kExceptionNodeAlreadyInGraph)
     *              if n is already part of a graph.
     *
     *  @throws bad_alloc()
     *              if default allocator is used for vector<>.
     *              if any insert of list<> has failed due to memory shortage.
     *
     *  @remark exception safety:
     *              If an exception is thrown, all the data structures in the
     *              graph and the parameters don't change.
     *
     *  @remark undefined behavior:
     *              If an invalid value is given to pos,
     *              it causes undefined behavior because the
     *              underlying list<>::insert behaves that way.
     */
    Node& addNode(node_ptr_t&& n, const node_list_it_t& pos);


    /** @brief  removes the specified node from the graph together with all
     *          the incident edges. The ownerships of the node and the edges
     *          are transferred to the caller.
     *
     *  @param  n            (in):  node to be removed.
     *  @param  removedEdges (out): list where edges to be removed will be 
     *                              stored together with their ownerships.
     *
     *  @return the removed node wrapped in a unique pointer.
     *
     *  @throws invalid_argument(Constants::kExceptionNodeNotInGraph)
     *              if n is already part of a graph.
     *
     *  @throws bad_alloc()
     *              if default allocator is used for vector<>.
     *              if any insert of list<> has failed due to memory shortage.
     *
     *  @remark exception safety:
     *              If an exception is thrown, all the data structures in the
     *              graph and the parameters don't change.
     */
    virtual node_ptr_t removeNode(Node&n,vector<edge_ptr_t>&removedEdges);


    /** @brief  removes the specified node from the graph together with all
     *          the incident edges. The ownership of the node is transferred
     *          to the owner. The removed edges in Edge objects will be 
     *          deleted.
     *
     *  @param  n            (in):  node to be removed.
     *
     *  @return the removed node wrapped in a unique pointer.
     *
     *  @throws invalid_argument(Constants::kExceptionNodeNotInGraph)
     *              if n is already part of a graph.
     *
     *  @throws bad_alloc()
     *              if default allocator is used for vector<>.
     *              if any insert of list<> has failed due to memory shortage.
     *
     *  @remark exception safety:
     *              If an exception is thrown, all the data structures in the
     *              graph and the parameters don't change.
     */
    virtual node_ptr_t removeNode(Node&n);



    virtual Edge& addEdge(edge_ptr_t&& e, Node& n1, Node& n2);


    /** @brief  adds the specified edge to the graph between two specified 
     *          nodes, and transfers the ownership of the edge from the caller
     *          to the graph.
     *
     *  @param  e          (in): pointer to the edge to be added to the graph.
     *  @param  n1         (in): node to become incident node 1 of e.
     *  @param  n2         (in): node to become incident node 2 of e.
     *  @param  posInNode1 (in): the position in the incidence list of node 1
     *                           before which e is inserted.
     *  @param  posInNode2 (in): the position in the incidence list of node 2
     *                           before which e is inserted.
     *
     *  @return a reference to the edge added to the graph.
     *
     *  @throws invalid_argument(Constants::kExceptionEdgeAlreadyInGrpah)
     *              if e is already part of a graph.
     *
     *  @throws bad_alloc()
     *              if default allocator is used for list<>.
     *              if an insersion of list<> has failed due to memory
     *              shortage.
     *
     *  @remark exception safety:
     *              If an exception is thrown, all the data structures in the
     *              graph and the parameters don't change.
     *
     *  @remark undefined behavior:
     *              If an invalid value is given to posInGraph, posInNode1, or
     *              posInNode2y, it causes undefined behavior because the
     *              underlying list<>::insert behaves that way.
     */
    virtual Edge& addEdge(edge_ptr_t&& e, Node& n1, Node& n2,
                  const node_incidence_it_t& posInNode1,
                  const node_incidence_it_t& posInNode2 );


    /** @brief  adds the specified edge to the graph between two specified
     *          nodes, and transfers the ownership of the edge from the caller
     *          to the graph.
     *
     *  @param  e          (in): pointer to the edge to be added to the graph.
     *  @param  n1         (in): node to become incident node 1 of e.
     *  @param  n2         (in): node to become incident node 2 of e.
     *
     *  @return a reference to the edge added to the graph.
     *
     *  @throws invalid_argument(Constants::kExceptionEdgeAlreadyInGrpah)
     *              if e is already part of a graph.
     *
     *  @throws bad_alloc()
     *              if default allocator is used for list<>.
     *              if an insersion of list<> has failed due to memory
     *              shortage.
     *
     *  @details e is inserted at the end of mEdges edge list in the graph.
     *           e is inserted at the end of mIncidence list in each of the
     *           node.
     *
     *  @remark exception safety:
     *              If an exception is thrown, all the data structures in the
     *              graph and the parameters don't change.
     *
     *  @remark undefined behavior:
     *              If an invalid value is given to posInGraph, posInNode1, or
     *              posInNode2y, it causes undefined behavior because the
     *              underlying list<>::insert behaves that way.
     */
    virtual Edge& addEdge(edge_ptr_t&& e, Node& n1, Node& n2,
                  const node_incidence_it_t& posInNode1,
                  const node_incidence_it_t& posInNode2,
                  const edge_list_it_t&      posInGraph );


    /** @brief   removes the specified edge from the graph, and transfers the
     *           ownership to the caller.
     *
     *  @param   e (in): the edge to be removed.
     *
     *  @return  removed edge wrapped in a unique pointer.
     *
     *  @throws  invalid_argument(Constants::kExceptionEdgeNotInGraph)
     *               if e is not part of this graph.}
     *  @remark  if an exception is thrown, no data structure in this graph
     *           is changed.
     */
    virtual edge_ptr_t removeEdge(Edge& e);


    /** @brief  splits the specified edge into two. The specified edge e
     *          is removed, the new node n is inserted, and then two new
     *          edges e1 and e2 are inserted.
     *          Before: n1 - (e) - n2
     *          After:  n1 - (e1) - n - (e2) - n2
     * 
     *  @param  e           (in):  edge to be split (removed)
     *  @param  removedEdge (out): edge removed with ownership transferred to
     *                             the caller.
     *  @param  e1          (in):  new edge to be inserted between n1 and n.
     *  @param  e2          (in):  new edge to be inserted between n and n2.
     *  @param  n           (in):  new node inserted between e1 and e2.
     *  @param  pos         (in):  the position in mNodes before which n is
     *                             inserted.
     *  @return a tuple of 3 references:
     *              1. added node n 
     *              2. added edge e1
     *              3. added edge e2
     *
     *  @throws bad_alloc()
     *              if default allocator is used for list<>.
     *              if any insert of list<> has failed due to memory shortage.
     *
     *  @remark exception safety:
     *              If an exception is thrown, all the data structures in the
     *              graph and the parameters don't change.
     *
     *  @remark undefined behavior:
     *              If an invalid value is given to pos
     *              it causes undefined behavior because the
     *              underlying list<>::insert behaves that way.
     */
    virtual std::tuple<Node&, Edge&, Edge&> 
        splitEdge(
            Edge&                 e,
            edge_ptr_t&           removedEdge, 
            edge_ptr_t&&          e1, 
            edge_ptr_t&&          e2,
            node_ptr_t&&          n,
            const node_list_it_t& pos          );


    /** @brief moves the specified edge between new nodes specified
     *         by toN1 and toN2.
     *
     *  @param  e       (in): target edge to be moved
     *  @param  toN1    (in): the new incident node 1 of e.
     *  @param  toN2    (in): the new incident node 2 of e.
     *
     *  @throws bad_alloc() (if default allocator is used for list<>.)
     *              - if any insert of list<> has failed due to memory
     *                shortage.
     *
     *  @details the edge e is inserted at the end of the incidence lists of 
     *           toN1 and toN2.
     *
     *  @remark exception safety:
     *              If an exception is thrown, all the data structures in the
     *              graph and the parameters don't change.
     *
     *  @remark undefined behavior:
     *              If an invalid value is given to pos
     *              it causes undefined behavior because the
     *              underlying list<>::insert behaves that way.
     */
    virtual void moveEdge(Edge&e, Node& toN1, Node& toN2);


    /** @brief moves the specified edge between new nodes specified
     *         by toN1 and toN2.
     *
     *  @param  e       (in): target edge to be moved
     *  @param  toN1    (in): the new incident node 1 of e.
     *  @param  toN1Pos (in): the position in the incidence lis of node 1
     *                        before which e is inserted.
     *  @param  toN2    (in): the new incident node 2 of e.
     *  @param  toN2Pos (in): the position in the incidence lis of node 2
     *                        before which e is inserted.
     *  @throws bad_alloc() (if default allocator is used for list<>.)
     *              - if any insert of list<> has failed due to memory
     *                shortage.
     *
     *  @remark exception safety:
     *              If an exception is thrown, all the data structures in the
     *              graph and the parameters don't change.
     *
     *  @remark undefined behavior:
     *              If an invalid value is given to pos
     *              it causes undefined behavior because the
     *              underlying list<>::insert behaves that way.
     */
    virtual void moveEdge(Edge& e, Node& toN1, 
         node_incidence_it_t toN1Pos, Node& toN2, node_incidence_it_t toN2Pos);


    /** @brief  contracts the specified edge. Of the two adjacent nodes, node2
     *          is removed, and all the incident edges to node 2 except for the
     *          one conracted are relinked to node 1.
     *          The ownerships of the specified edge and node 1 are transferred
     *          to the caller.
     *
     *  @param  e            (in):  the edge to be contracted.
     *  @param  removedNode  (out): the node 2 removed with its ownership.
     *
     *  @return the edge contracted with its ownership.
     *
     *  @throws bad_alloc()
     *              if default allocator is used for list<>.
     *              if any insert of list<> has failed due to memory shortage.
     *
     *  @remark exception safety:
     *              If an exception is thrown, all the data structures in the
     *              graph and the parameters don't change.
     */
    virtual edge_ptr_t contractEdge(Edge&e, node_ptr_t&removedNode);


    /** @brief  contracts the specified edge. Of the two adjacent nodes, node2
     *          is removed, and all the incident edges to node 2 except for the
     *          one conracted are relinked to node 1.
     *          The ownership of the specified edge is transferred to the
     *          caller. The Node object of node 2 is deleted.
     *
     *  @param  e            (in):  the edge to be contracted.
     *
     *  @return the edge contracted with its ownership.
     *
     *  @throws bad_alloc()
     *               if default allocator is used for list<>.
     *               if any insert of list<> has failed due to memory shortage.
     *
     *  @remark exception safety:
     *              If an exception is thrown, all the data structures in the
     *              graph and the parameters don't change.
     */
    virtual edge_ptr_t contractEdge(Edge&e);


    /** @brief  identifies the induced nodes by the given edges in this graph.
     *
     *  @param  edges (in): edges for which induced nodes are to be  found.
     *
     *  @return induced nodes
     *
     *  @throws bad_alloc() 
     *              if default allocator is used for list<>.
     *              if any insert of list<> has failed due to memory shortage.
     *
     *  @remark exception safety:
     *              If an exception is thrown, all the data structures in the
     *              graph and the parameters don't change.
     */
    vector<node_list_it_t> edgeInducedNodes(vector<edge_list_it_t>&edges);


    /** @brief  identifies the induced edges by the given nodes in this graph.
     *
     *  @param  nodes  (in): nodes for which induced edges are to be  found.
     *
     *  @return induced edges.
     *
     *  @throws bad_alloc() 
     *              if default allocator is used for list<>.
     *              if any insert of list<> has failed due to memory shortage.
     *
     *  @remark exception safety:
     *              If an exception is thrown, all the data structures in the
     *              graph and the parameters don't change.
     */
    vector<edge_list_it_t> nodeInducedEdges(vector<node_list_it_t>&nodes);


    /** @brief  identifies the edges lying between the specified subgraph  and
     *          its complement, and removes them from the graph with their 
     *          ownerships transferred to the caller.
     *  @param  edges  (in): edges in the subgraph. 
     *  @param  nodes  (in): nodes in the subgraph.
     *
     *  @return removed edges with their ownerships.
     *
     *  @details if the edge set given by "edges" is not the node-induced 
     *           edges by the node set given by "nodes", then the edges
     *           not in edges will be considered part of the cutset, though
     *           their incident nodes are both in "nodes".
     *           To avoid this, you should make edges induced by nodes.
     *
     *  @throws bad_alloc() 
     *              if default allocator is used for list<>.
     *              if any insert of list<> has failed due to memory shortage.
     *
     *  @remark exception safety:
     *              If an exception is thrown, all the data structures in the
     *              graph and the parameters don't change.
     */
    virtual vector<edge_ptr_t> removeCutSet(
              vector<edge_list_it_t>&edges, vector<node_list_it_t>&nodes);


    /** @brief  identifies the edges lying between the specified subgraph  and
     *          its complement.
     *          ownerships transferred to the caller.
     *  @param  edges  (in): edges in the subgraph. 
     *  @param  nodes  (in): nodes in the subgraph.
     *
     *  @return cutset (edge set)
     *
     *  @details if the edge set given by "edges" is not the node-induced 
     *           edges by the node set given by "nodes", then the edges
     *           not in edges will be considered part of the cutset, though
     *           their incident nodes are both in "nodes".
     *           To avoid this, you should make edges induced by nodes.
     *
     *  @throws bad_alloc() 
     *              if default allocator is used for list<>.
     *              if any insert of list<> has failed due to memory shortage.
     *
     *  @remark exception safety:
     *              If an exception is thrown, all the data structures in the
     *              graph and the parameters don't change.
     */
    vector<edge_list_it_t> findCutSet(
              vector<edge_list_it_t>&edges, vector<node_list_it_t>&nodes);


    /** @brief  moves subgraph induced by the given edges to another graph
     *          specified by g. It also removes from this graph the edges
     *          that span the subgraph and its complement (cutset)
     *
     *  @param  edges  (in): edges to be moved together with their induced
     *                       nodes.
     *  @param  g      (in): the target graph to which the subgraph is moved.
     *
     *  @return removed cutset edges with their ownerships.
     *
     *  @throws bad_alloc() (if default allocator is used for list<>.)
     *              - if any insert of list<> has failed due to memory 
     *                shortage.
     *
     *  @remark exception safety:
     *              If an exception is thrown, all the data structures in the
     *              graph and the parameters don't change.
     */
    virtual vector<edge_ptr_t> moveEdgeInducedSubgraph(
                                  vector<edge_list_it_t>&edges, Graph& g);


    /** @brief  moves subgraph induced by the given nodes to another graph
     *          specified by g. It also removes from this graph the edges
     *          that span the subgraph and its complement (cutset)
     *
     *  @param  nodes  (in): nodes to be moved together with their induced
     *                       edges.
     *  @param  g      (in): the target graph to which the subgraph is moved.
     *
     *  @return removed cutset edges with their ownerships.
     *
     *  @throws bad_alloc() (if default allocator is used for list<>.)
     *              - if any insert of list<> has failed due to memory
     *                shortage.
     *
     *  @remark exception safety:
     *              If an exception is thrown, all the data structures in the
     *              graph and the parameters don't change.
     *             
     */
    virtual vector<edge_ptr_t> moveNodeInducedSubgraph(
                                  vector<node_list_it_t>&nodes, Graph& g);


    /** @brief  creates a subgraph in another Graph object specified by g,
     *          that are isomorphic to the subgraph in this Graph specified by 
     *          the nodes and edges given in the parameters using new Nodes and
     *          Edges objects given in the parameters.
     *
     *  @param    nodePairs  (in): list of pairs
     *                first:  node whose copy is created in g.
     *                second: Node object with which first's copy is created 
     *                in g. The ownership is transferred from this list to g.
     *                       
     *  @param    edgePairs  (in): list of pairs
     *                first:  edge whose copy is created in g.
     *                second: Edge Object with which first's copy is created
     *                        in g.
     *                        The ownership is transferred from this list to g.
     *
     *  @param    g          (in): Graph object to which an isomorphic subgraph
     *                             is created.
     *
     *  @throws bad_alloc() 
     *              if default allocator is used for list<>.
     *              if any insert of list<> has failed due to memory shortage.
     *
     *  @remark exception safety:
     *              If an exception is thrown, all the data structures in the
     *              graph and the parameters don't change.
     *
     *  @remark on ordering:
     *              The ordering of the copied nodes in g.nodes() is aligned
     *              with the ordering in nodePairs.
     *              The ordering of the copied nodes in g.edges() is aligned
     *              with the ordering in edgePairs.
     *              The ordering of incidentEdges() of each node in g is 
     *              aligned with the appropriate subsequence of incidentEdges()
     *              of the original node in this graph.
     */
    virtual void copySubgraph(
                      vector<pair<node_list_it_t,node_ptr_t>>& nodePairs,
                      vector<pair<edge_list_it_t,edge_ptr_t>>& edgePairs,
                      Graph& g);


    /** @brief  returns the number of nodes in the graph.
     *  @return number of nodes.
     */
    inline size_t numNodes() const noexcept;


    /** @brief  returns the number of edges in the graph.
     *  @return number of edges.
     */
    inline size_t numEdges() const noexcept;


    /** @brief  returns the list of nodes in the graph in a pair of begin and
     *          end iterators.
     *         
     *  @return a pair of begin and end iterators for the node list.
     */
    inline pair< node_list_it_t,node_list_it_t >nodes() noexcept;


    /** @brief  returns the list of edges in the graph in a pair of begin and
     *          end iterators.
     *         
     *  @return a pair of begin and end iterators for the edge list.
     */
    inline pair< edge_list_it_t,edge_list_it_t >edges() noexcept;


    /** @brief  resets the general purpose counters of this graph and
     *          its owning Nodes and Edges to zero.
     */
    void resetGeneration() noexcept;


    /** @brief finds (multi)edges between two given nodes.
     *
     *  @param   n1      (in):incident node 1.
     *  @param   n2      (in):incident node 2.
     *
     *  @return  edges found.
     */
    vector<edge_list_it_t>findMultiEdges(Node& n1, Node& n2);


    inline void pushDefaultIGForwardLinkToNodes();
    inline void pushDefaultIGBackwardLinkToNodes();
    inline void pushDefaultUtilityToNodes();


    inline void pushDefaultIGForwardLinkToEdges();
    inline void pushDefaultIGBackwardLinkToEdges();
    inline void pushDefaultUtilityToEdges();



    /** @brief pops a link from the link stack of all the nodes.
     *
     *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
     *              if the stack is empty
     */
    inline void popIGForwardLinkFromNodes();
    inline void popIGBackwardLinkFromNodes();

    /** @brief pops a utility from the utility stack of all the nodes.
     *
     *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
     *              if the stack is empty
     */
    inline void popUtilityFromNodes();


    /** @brief pops a link from the link stack of all the edges.
     *
     *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
     *              if the stack is empty
     */
    inline void popIGForwardLinkFromEdges();
    inline void popIGBackwardLinkFromEdges();

    /** @brief pops a utility from the utility stack of all the edges.
     *
     *  @throw  std::invalid_argument(Constants::kExceptionStackIndex
     *              if the stack is empty
     */
    inline void popUtilityFromEdges();


private:
    /** @brief  moves the specified nodes and the edges to another graph
     *          specified by g.
     *
     *  @param   edges (in): edges to be moved to g.
     *  @param   nodes (in): nodes to be moved to g.
     *  @param   g     (in): graph to which edges and nodes are moved.
     *
     *  @throws bad_alloc() 
     *              if default allocator is used for list<>.
     *              if any insert of list<> has failed due to memory shortage.
     *
     *  @remark on the incidence list of each nodes. The order of the edges
     *              in mIncidentEdges of each node is kept across the move.
     *             
     *  @ramark conditions on which this function can be called:
     *              This is a private function. It must be called with 
     *              the input "edges" and "nodes" that have the following 
     *              constrainsts. The subgraph specified "nodes" and "edges"
     *              must be a node induced subgraph. That means all the 
     *              edges between any two nodes in "nodes" must be in "edges".
     *              Also, there must be no edge (cut set) between the subgraph
     *              and its complement.
     *          
     *  @remark exception safety:
     *              If an exception is thrown, all the data structures in the
     *              graph and the parameters don't change.
     */
    virtual void moveEdgesAndNodes( vector<edge_list_it_t>&edges,
                                  vector<node_list_it_t>&nodes, Graph& g);


    /**  @brief the list of the nodes in the graph with their ownerships.
     */
    list<node_ptr_t> mNodes;

    /**  @brief the list of the edges in the graph with their ownerships.
     */
    list<edge_ptr_t> mEdges;

    /** @brief internal general purpose counter.
     *         an example usage is to identify the edge induced nodes.
     */
    generation_t     mGeneration;

  friend class Wailea::Directed::DiGraph;

#ifdef UNIT_TESTS
  friend class GraphTests;
  friend class Wailea::Directed::DiGraphTests;
#endif

};


/** 
 *  Singleton idiom
 *  Returns unique ptr
 */
class BaseFactory {
  protected:
    BaseFactory(){;};
  public:
    static BaseFactory& getInstance()
    {
        static BaseFactory instance;
        return instance;
    }
    unique_ptr<Graph> Graph();
    unique_ptr<Node> Node();
    unique_ptr<Edge> Edge();
    BaseFactory(const BaseFactory &rhs) = delete;
    BaseFactory& operator=(const BaseFactory &rhs) = delete;
    BaseFactory(BaseFactory &&rhs) = delete;
    BaseFactory& operator=(BaseFactory &&rhs) = delete;
};


class DerivedFactory : public BaseFactory {
  private:
    DerivedFactory(){;};
  public:
    static DerivedFactory& getInstance()
    {
        static DerivedFactory instance;
        return instance;
    }

    DerivedFactory(const DerivedFactory &rhs) = delete;
    DerivedFactory& operator=(const DerivedFactory &rhs) = delete;
    DerivedFactory(DerivedFactory &&rhs) = delete;
    DerivedFactory& operator=(DerivedFactory &&rhs) = delete;
};





/*
 * The following are implementation of inline functions.
 */


/** @brief  default-constructs Node to this.
 */
inline Node::Node() noexcept :
             mGraph(nullptr), mGeneration(0){};


/** @brief  destructs this object
 */
inline Node::~Node() noexcept {};


/** @brief returns the iteratror in mNodes list of the containing Graph.
 */
inline node_list_it_t Node::backIt() const noexcept {return mBackIt;}

/** @brief  returns pair of begin and end iterators of the incident edges
 *          in a list.
 *
 *  @throw  std::invalid_argument(Constants::kExceptionNodeNotInGraph)
 *              if the node does not belong to a graph.
 *
 *  @return pair of begin and end iterators
 */
inline std::pair<node_incidence_it_t,node_incidence_it_t> 
Node::incidentEdges()
{
    if (mGraph == nullptr) {
        throw std::invalid_argument(Constants::kExceptionNodeNotInGraph);
    }

    std::pair<node_incidence_it_t,
              node_incidence_it_t> p(mIncidence.begin(),mIncidence.end());
    return p;
}


/** @brief  returns the degree of this node
 *
 *  @throw  std::invalid_argument(Constants::kExceptionNodeNotInGraph)
 *              if the node does not belong to a graph.
 *
 *  @return degree
 */
inline size_t Node::degree() const
{
    if (mGraph == nullptr) {
        throw std::invalid_argument(Constants::kExceptionNodeNotInGraph);
    }
    return mIncidence.size();
}


/** @brief  reorders the incident edges list
 *
 *  @throw  std::invalid_argument(Constants::kExceptionNodeNotInGraph)
 *              if the node does not belong to a graph.
 *
 *  @param  reorderedEdges (in):list incident edges in the new order.
 */
inline void Node::reorderIncidence(
    std::vector<edge_list_it_t>&& reorderedEdges
)
{
    if (mGraph == nullptr) {
        throw std::invalid_argument(Constants::kExceptionNodeNotInGraph);
    }

    auto mit = mIncidence.begin();

    for (auto nit : reorderedEdges) {

        (*mit) = std::move(nit);

        Edge& e = *(*(*mit));
        if (&(e.incidentNode1())==this) {
            e.mBackItNode1 = mit;
        }
        else {
            e.mBackItNode2 = mit;
        }
        mit++;
    }

}


/** @brief  reorders the incident edges list
 *
 *  @throw  std::invalid_argument(Constants::kExceptionNodeNotInGraph)
 *              if the node does not belong to a graph.
 *
 *  @param  reorderedEdges (in):list incident edges in the new order.
 */
inline void Node::reorderIncidence(std::list<edge_list_it_t>&& reorderedEdges)
{
    if (mGraph == nullptr) {
        throw std::invalid_argument(Constants::kExceptionNodeNotInGraph);
    }

    mIncidence = std::move(reorderedEdges);

    for (auto mit = mIncidence.begin(); mit != mIncidence.end(); mit++) {

        Edge& e = *(*(*mit));
        if (&(e.incidentNode1())==this) {
            e.mBackItNode1 = mit;
        }
        else {
            e.mBackItNode2 = mit;
        }
    }
}

/** @brief  resets the general purpose counter to zero.
 */
inline void Node::resetGeneration() noexcept { mGeneration = 0; }


/** @brief returns the containing graph.
 *
 *  @throw  std::invalid_argument(Constants::kExceptionNodeNotInGraph)
 *              if the node does not belong to a graph.
 *
 *  @return reference to the containing graph.
 */
inline Graph& Node::graph(){

    if (mGraph == nullptr) {
        throw std::invalid_argument(Constants::kExceptionNodeNotInGraph);
    }
    return *mGraph;

}


/** @brief returns if the containing graph is set.
 *
 *  @return true if the containing graph is set.
 */
inline bool Node::isGraphValid() const noexcept {return mGraph != nullptr;};


/** @brief pushes a forward link to the stack.
 *
 *  @param  it   (in): iterator to a Node.
 *
 *  @return index to the link pushed on the stack
 */
inline size_t Node::pushIGForwardLink(node_list_it_t it)
{
    mIGForwardLinkStack.push_back(it);
    return mIGForwardLinkStack.size() - 1;
}


/** @brief pushes a backward link to the stack.
 *
 *  @param  it   (in): iterator to a Node.
 *
  *  @return index to the link pushed on the stack
  */
inline size_t Node::pushIGBackwardLink(node_list_it_t it)
{
    mIGBackwardLinkStack.push_back(it);
    return mIGBackwardLinkStack.size() - 1;
}


/** @brief pops the top of the forward link stack.
 *
 *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
 *              if the stack is empty
 *
 *  @return the iterator on the top of the forward link stack
 */
inline node_list_it_t Node::popIGForwardLink()
{
    if (mIGForwardLinkStack.size()==0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    auto it = *(mIGForwardLinkStack.rbegin());
    mIGForwardLinkStack.pop_back();
    return it;
}


/** @brief pops the top of the backward link stack.
 *
 *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
 *              if the stack is empty
 *
 *  @return the iterator on the top of the backward link stack
 */
inline node_list_it_t Node::popIGBackwardLink()
{
    if (mIGBackwardLinkStack.size()==0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    auto it = *(mIGBackwardLinkStack.rbegin());
    mIGBackwardLinkStack.pop_back();
    return it;
}


/** @brief returns the size of the forward link stack.
 */
inline size_t Node::IGForwardLinksSize() const
{
    return mIGForwardLinkStack.size();
}


/** @brief returns the size of the backward link stack.
 */
inline size_t Node::IGBackwardLinksSize() const
{
    return mIGBackwardLinkStack.size();
}


/** @brief returns the iterator on the forward stack
 *         at the specified index.
 *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
 *              if the index is invalid.
 *
 *  @param  index (in): the index into the stack.
 *         The index is from the stack top down to the bottom.
 *
 *  @return the iterator on the stack
 */
inline node_list_it_t Node::IGForwardLink(size_t index) const
{
    if (mIGForwardLinkStack.size()==0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    if (long(mIGForwardLinkStack.size()) -1L - long(index) < 0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    return mIGForwardLinkStack[mIGForwardLinkStack.size() - 1 - index];
}


inline Node& Node::IGForwardLinkRef(size_t index) const
{
    return *(*IGForwardLink(index));
}


/** @brief returns the iterator on the backward stack
 *         at the specified index.
 *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
 *              if the index is invalid.
 *
 *  @param  index (in): the index into the stack.
 *         The index is from the stack top down to the bottom.
 *
 *  @return the iterator on the stack
 */
inline node_list_it_t Node::IGBackwardLink(size_t index) const
{
    if (mIGBackwardLinkStack.size()==0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    if (long(mIGBackwardLinkStack.size()) -1L - long(index) < 0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    return mIGBackwardLinkStack[mIGBackwardLinkStack.size() - 1 - index];
}


inline Node& Node::IGBackwardLinkRef(size_t index) const
{
    return *(*IGBackwardLink(index));
}


/** @brief set an iterator on the forward link stack at the specified index
 *
 *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
 *              if the stack is empty
 *
 *  @param  it    (in): the iterator to be set.
 *
 *  @param  index (in): the index into the stack.
 *         The index is from the stack top down to the bottom.
 */
inline void Node::setIGForwardLink(node_list_it_t it, size_t index)
{
    if (mIGForwardLinkStack.size()==0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    if (long(mIGForwardLinkStack.size()) -1L - long(index) < 0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    mIGForwardLinkStack[mIGForwardLinkStack.size() - 1 - index] = it;
}


/** @brief set an iterator on the backward link stack at the specified
 *         index
 *
 *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
 *              if the stack is empty
 *
 *  @param  it    (in): the iterator to be set.
 *
 *  @param  index (in): the index into the stack.
 *         The index is from the stack top down to the bottom.
 */
inline void Node::setIGBackwardLink (node_list_it_t it, size_t index)
{
    if (mIGBackwardLinkStack.size()==0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    if (long(mIGBackwardLinkStack.size()) -1L - long(index) < 0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    mIGBackwardLinkStack[mIGBackwardLinkStack.size() - 1 - index] = it;
}


/** @brief pushes a utility value to the stack.
 *
 *  @param  val   (in): value to be pushed
 *
 *  @return index to the value pushed on the stack
 */
inline size_t Node::pushUtility(utility_t val)
{
    mUtilityStack.push_back(val);
    return mUtilityStack.size() - 1;
}


/** @brief pops the top of the utility stack.
 *
 *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
 *              if the stack is empty
 *
 *  @return the value on the top of the utility stack
 */
inline utility_t Node::popUtility()
{
    if (mUtilityStack.size()==0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    auto val = *(mUtilityStack.rbegin());
    mUtilityStack.pop_back();
    return val;
}


/** @brief returns the size of the utility stack
 */
inline size_t Node::utilitySize() const
{
    return mUtilityStack.size();
}


/** @brief returns the value on the utility stack at the specified index
 *
 *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
 *              if the stack is empty
 *
 *  @param  index (in): the index into the stack.
 *         The index is from the stack top down to the bottom.
 *
 *  @return the value on the stack
 */
inline utility_t Node::utility(size_t index) const
{
    if (mUtilityStack.size()==0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    if (long(mUtilityStack.size()) -1L - long(index) < 0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    return mUtilityStack[mUtilityStack.size() - 1 - index];

}


/** @brief set a value to the utility stack at the specified index
 *
 *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
 *              if the stack is empty
 *
 *  @param  index (in): the index into the stack.
 *         The index is from the stack top down to the bottom.
 *
 *  @param  val (in): new value to be set
 */
inline void Node::setUtility(utility_t val, size_t index)
{
    if (mUtilityStack.size()==0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    if (long(mUtilityStack.size()) -1L - long(index) < 0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    mUtilityStack[mUtilityStack.size() - 1 - index] = val;
}


/** @brief  default-constructs Edge to this.
 */
inline Edge::Edge() noexcept :
             mGraph(nullptr), mGeneration(0){};


/** @brief  destructs this object
 */
inline Edge::~Edge() noexcept {;};


/** @brief  returns the incident node 1
 *
 *  @throw  std::invalid_argument(Constants::kExceptionEdgeNotInGraph)
 *              if the edge does not belong to a graph.
 *
 *  @return reference to node 1
 */
inline Node& Edge::incidentNode1() const
{
    if (mGraph == nullptr) {
        throw std::invalid_argument(Constants::kExceptionEdgeNotInGraph);
    }

    return *(*mIncidentNode1);
}


/** @brief  returns the incident node 2
 *
 *  @throw  std::invalid_argument(Constants::kExceptionEdgeNotInGraph)
 *              if the edge does not belong to a graph.
 *
 *  @return reference to node 2
 */
inline Node& Edge::incidentNode2() const
{
    if (mGraph == nullptr) {
        throw std::invalid_argument(Constants::kExceptionEdgeNotInGraph);
    }

    return *(*mIncidentNode2);
}


/** @brief  returns the iterator in the incidence list of node 1
 *
 *  @throw  std::invalid_argument(Constants::kExceptionEdgeNotInGraph)
 *              if the edge does not belong to a graph.
 *
 *  @return iterator in the incidence list of node 1.
 */
inline node_incidence_it_t Edge::incidentBackItNode1() const noexcept
{
    return mBackItNode1;
}


/** @brief  returns the iterator in the incidence list of node 2
 *
 *  @throw  std::invalid_argument(Constants::kExceptionEdgeNotInGraph)
 *              if the edge does not belong to a graph.
 *
 *  @return iterator in the incidence list of node 2.
 */
inline node_incidence_it_t Edge::incidentBackItNode2() const noexcept
{
    return mBackItNode2;
}


/** @brief  returns the adjacent node of this edge opposed to the node 
 *          specified by the parameter.
 *          
 *  @param  v (in): opposing incident node of this edge.
 *
 *  @throw  std::invalid_argument(Constants::kExceptionEdgeNotInGraph)
 *              if the edge does not belong to a graph.
 *
 *  @return the other incident node of this edge.
 */
inline Node& Edge::adjacentNode(Node& v) const
{
    if (mGraph == nullptr) {
        throw std::invalid_argument(Constants::kExceptionEdgeNotInGraph);
    }

    if (&v == &(*(*mIncidentNode1))) {
        return *(*mIncidentNode2);
    }
    else {
        return *(*mIncidentNode1);
    }
}

/** @brief  resets the general purpose counter to zero.
 */
inline void Edge::resetGeneration() noexcept { mGeneration = 0; }


/** @brief returns the iteratror in mEdges list of the containing Graph.
 */
inline edge_list_it_t Edge::backIt() const noexcept {return mBackIt;}


/** @brief returns the containing graph.
 *
 *  @throw  std::invalid_argument(Constants::kExceptionEdgeNotInGraph)
 *              if the edge does not belong to a graph.
 *
 *  @return reference to the containing graph.
 */
inline Graph& Edge::graph(){

    if (mGraph == nullptr) {
        throw std::invalid_argument(Constants::kExceptionEdgeNotInGraph);
    }
    return *mGraph;

}


/** @brief returns if the containing graph is set.
 *
 *  @return true if the containing graph is set.
 */
inline bool Edge::isGraphValid() const noexcept {return mGraph != nullptr;};


/** @brief pushes a forward link to the stack.
 *
 *  @param  it   (in): iterator to an Edge.
 *
 *  @return index to the link pushed on the stack
 */
inline size_t Edge::pushIGForwardLink(edge_list_it_t it)
{
    mIGForwardLinkStack.push_back(it);
    return mIGForwardLinkStack.size() - 1;
}


/** @brief pushes a backward link to the stack.
 *
 *  @param  it   (in): iterator to an Edge.
 *
  *  @return index to the link pushed on the stack
  */
inline size_t Edge::pushIGBackwardLink(edge_list_it_t it)
{
    mIGBackwardLinkStack.push_back(it);
    return mIGBackwardLinkStack.size() - 1;
}


/** @brief pops the top of the forward link stack.
 *
 *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
 *              if the stack is empty
 *
 *  @return the iterator on the top of the forward link stack
 */
inline edge_list_it_t Edge::popIGForwardLink()
{
    if (mIGForwardLinkStack.size()==0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    auto it = *(mIGForwardLinkStack.rbegin());
    mIGForwardLinkStack.pop_back();
    return it;
}


/** @brief pops the top of the backward link stack.
 *
 *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
 *              if the stack is empty
 *
 *  @return the iterator on the top of the backward link stack
 */
inline edge_list_it_t Edge::popIGBackwardLink()
{
    if (mIGBackwardLinkStack.size()==0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    auto it = *(mIGBackwardLinkStack.rbegin());
    mIGBackwardLinkStack.pop_back();
    return it;
}


/** @brief returns the size of the forward link stack.
 */
inline size_t Edge::IGForwardLinksSize() const
{
    return mIGForwardLinkStack.size();
}


/** @brief returns the size of the backward link stack.
 */
inline size_t Edge::IGBackwardLinksSize() const
{
    return mIGBackwardLinkStack.size();
}


/** @brief returns the iterator on the forward stack
 *         at the specified index.
 *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
 *              if the index is invalid.
 *
 *  @param  index (in): the index into the stack.
 *         The index is from the stack top down to the bottom.
 *
 *  @return the iterator on the stack
 */
inline edge_list_it_t Edge::IGForwardLink(size_t index) const
{
    if (mIGForwardLinkStack.size()==0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    if (long(mIGForwardLinkStack.size()) -1L - long(index) < 0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    return mIGForwardLinkStack[mIGForwardLinkStack.size() - 1 - index];
}


inline Edge& Edge::IGForwardLinkRef(size_t index) const
{
    return *(*IGForwardLink(index));
}


/** @brief returns the iterator on the backward stack
 *         at the specified index.
 *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
 *              if the index is invalid.
 *
 *  @param  index (in): the index into the stack.
 *         The index is from the stack top down to the bottom.
 *
 *  @return the iterator on the stack
 */
inline edge_list_it_t Edge::IGBackwardLink(size_t index) const
{
    if (mIGBackwardLinkStack.size()==0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    if (long(mIGBackwardLinkStack.size()) -1L - long(index) < 0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    return mIGBackwardLinkStack[mIGBackwardLinkStack.size() - 1 - index];
}


inline Edge& Edge::IGBackwardLinkRef(size_t index) const
{
    return *(*IGBackwardLink(index));
}


/** @brief set an iterator on the forward link stack at the specified index
 *
 *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
 *              if the stack is empty
 *
 *  @param  it    (in): the iterator to be set.
 *
 *  @param  index (in): the index into the stack.
 *         The index is from the stack top down to the bottom.
 */
inline void Edge::setIGForwardLink(edge_list_it_t it, size_t index)
{
    if (mIGForwardLinkStack.size()==0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    if (long(mIGForwardLinkStack.size()) -1L - long(index) < 0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    mIGForwardLinkStack[mIGForwardLinkStack.size() - 1 - index] = it;
}


/** @brief set an iterator on the backward link stack at the specified
 *         index
 *
 *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
 *              if the stack is empty
 *
 *  @param  it    (in): the iterator to be set.
 *
 *  @param  index (in): the index into the stack.
 *         The index is from the stack top down to the bottom.
 */
inline void Edge::setIGBackwardLink (edge_list_it_t it, size_t index)
{
    if (mIGBackwardLinkStack.size()==0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    if (long(mIGBackwardLinkStack.size()) -1L - long(index) < 0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    mIGBackwardLinkStack[mIGBackwardLinkStack.size() - 1 - index] = it;
}


/** @brief pushes a utility value to the stack.
 *
 *  @param  val   (in): value to be pushed
 *
 *  @return index to the value pushed on the stack
 */
inline size_t Edge::pushUtility(utility_t val)
{
    mUtilityStack.push_back(val);
    return mUtilityStack.size() - 1;
}


/** @brief pops the top of the utility stack.
 *
 *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
 *              if the stack is empty
 *
 *  @return the value on the top of the utility stack
 */
inline utility_t Edge::popUtility()
{
    if (mUtilityStack.size()==0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    auto val = *(mUtilityStack.rbegin());
    mUtilityStack.pop_back();
    return val;
}


/** @brief returns the size of the utility stack
 */
inline size_t Edge::utilitySize() const
{
    return mUtilityStack.size();
}


/** @brief returns the value on the utility stack at the specified index
 *
 *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
 *              if the stack is empty
 *
 *  @param  index (in): the index into the stack.
 *         The index is from the stack top down to the bottom.
 *
 *  @return the value on the stack
 */
inline utility_t Edge::utility(size_t index) const
{
    if (mUtilityStack.size()==0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    if (long(mUtilityStack.size()) -1L - long(index) < 0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    return mUtilityStack[mUtilityStack.size() - 1 - index];

}


/** @brief set a value to the utility stack at the specified index
 *
 *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
 *              if the stack is empty
 *
 *  @param  index (in): the index into the stack.
 *         The index is from the stack top down to the bottom.
 *
 *  @param  val (in): new value to be set
 */
inline void Edge::setUtility(utility_t val, size_t index)
{
    if (mUtilityStack.size()==0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    if (long(mUtilityStack.size()) -1L - long(index) < 0) {
        throw std::invalid_argument(Constants::kExceptionStackIndex);
    }
    mUtilityStack[mUtilityStack.size() - 1 - index] = val;
}


/** @brief  default-constructs Graph to this.
 */
inline Graph::Graph() noexcept : mGeneration(0){};


/** @brief  destructs this object
 */
inline Graph::~Graph() noexcept {};


/** @brief  move-constructs Graph to this.
 *
 *  @param  rhs  (in): Graph whose contents are swapped with this.
 */
inline Graph::Graph(Graph&& rhs) noexcept
{
    mNodes = std::move(rhs.mNodes);
    mEdges = std::move(rhs.mEdges);
    mGeneration = rhs.mGeneration;
}


/** @brief  move-assigns Graph to this.
 *
 *  @param  rhs  (in): Graph whose contents are swapped with this.
 *
 *  @return reference to this.
 */
inline Graph& Graph::operator=(Graph&& rhs) noexcept
{
    mNodes = std::move(rhs.mNodes);
    mEdges = std::move(rhs.mEdges);
    mGeneration = rhs.mGeneration;
    return *this;
}


/** @brief  returns the list of nodes in the graph in a pair of begin and end
 *          iterators.
 *  @return a pair of begin and end iterators for the node list.
 */
inline std::pair<node_list_it_t,node_list_it_t> Graph::nodes() noexcept
{
    std::pair<node_list_it_t,node_list_it_t> p(mNodes.begin(), mNodes.end());
    return p;
}


/** @brief  returns the list of edges in the graph in a pair of begin and end
 *          iterators.
 *  @return a pair of begin and end iterators for the edge list.
 */
inline std::pair<edge_list_it_t,edge_list_it_t>Graph::edges() noexcept
{
    std::pair<edge_list_it_t,edge_list_it_t> p(mEdges.begin(), mEdges.end());
    return p;
}


/** @brief  returns the number of nodes in the graph.
 *  @return number of nodes.
 */
inline size_t Graph::numNodes() const noexcept
{
   return mNodes.size();
}


/** @brief  returns the number of edges in the graph.
 *  @return number of edges.
 */
inline size_t Graph::numEdges() const noexcept
{
   return mEdges.size();
}


/** @brief pops a link from the link stack of all the nodes.
 *
 *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
 *              if the stack is empty
 */
inline void Graph::popIGForwardLinkFromNodes()
{
    for (auto& nit : mNodes) {
        nit->popIGForwardLink();
    }
}

inline void Graph::popIGBackwardLinkFromNodes()
{
    for (auto& nit : mNodes) {
        nit->popIGBackwardLink();
    }
}


/** @brief pops a utility from the utility stack of all the nodes.
 *
 *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
 *              if the stack is empty
 */
inline void Graph::popUtilityFromNodes()
{
    for (auto& nit : mNodes) {
        nit->popUtility();
    }
}


/** @brief pops a link from the link stack of all the edges.
 *
 *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
 *              if the stack is empty
 */
inline void Graph::popIGForwardLinkFromEdges()
{
    for (auto& eit : mEdges) {
        eit->popIGForwardLink();
    }
}

inline void Graph::popIGBackwardLinkFromEdges()
{
    for (auto& eit : mEdges) {
        eit->popIGBackwardLink();
    }
}


/** @brief pops a utility from the utility stack of all the edges.
 *
 *  @throw  std::invalid_argument(Constants::kExceptionStackIndex)
 *              if the stack is empty
 */
inline void Graph::popUtilityFromEdges()
{
    for (auto& eit : mEdges) {
        eit->popUtility();
    }
}



inline void Graph::pushDefaultIGForwardLinkToNodes()
{
    for (auto& nit : mNodes) {
        nit->pushIGForwardLink(mNodes.end());
    }
}

inline void Graph::pushDefaultIGBackwardLinkToNodes()
{
    for (auto& nit : mNodes) {
        nit->pushIGBackwardLink(mNodes.end());
    }
}

inline void Graph::pushDefaultUtilityToNodes()
{
    for (auto& nit : mNodes) {
        nit->pushUtility(0);
    }
}

inline void Graph::pushDefaultIGForwardLinkToEdges()
{
    for (auto& eit : mEdges) {
        eit->pushIGForwardLink(mEdges.end());
    }
}

inline void Graph::pushDefaultIGBackwardLinkToEdges()
{
    for (auto& eit : mEdges) {
        eit->pushIGBackwardLink(mEdges.end());
    }
}

inline void Graph::pushDefaultUtilityToEdges()
{
    for (auto& eit : mEdges) {
        eit->pushUtility(0);
    }
}


#ifdef UNIT_TESTS
class NumNode : public Node {

public:
    NumNode(int n):mNum(n){;};
    int num() {return mNum;}
    void printAdjacency(ostream& os){
        os << "Node [" << mNum << "]: ";
        for(auto eIt=incidentEdges().first;eIt!=incidentEdges().second;eIt++){
            auto& E = dynamic_cast<Edge&>(*(*(*eIt)));
            auto& A = dynamic_cast<NumNode&>(E.adjacentNode(*this));
            os << A.mNum << " ";
        }
        os << "\n";
    }
private:
    int mNum;
};
#endif



}// namespace Undirected

}// namespace Wailea

#endif /*_WAILEA_UNDIRECTED_BASE_HPP_*/
