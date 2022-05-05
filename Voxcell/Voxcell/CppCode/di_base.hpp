#ifndef _WAILEA_DIRECTED_DI_BASE_HPP_
#define _WAILEA_DIRECTED_DI_BASE_HPP_

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <exception>

#include "base.hpp"

#ifdef UNIT_TESTS
#include "gtest/gtest_prod.h"
#endif

/**
 * @file undirected/di_base.hpp
 *
 * @brief Base classes that implement the basic concept of directed graph
 *        G(V,E) and manipulations.
 *        It is inherited from the classes defined in base.hpp
 *
 * @details
 *        G is represented by Digraph, v in V is by DiNode, and e in E is 
 *        by DiEdge.
 *        It consists of three main classes: DiGraph, DiNode, and DiEdge.
 *        A DiGraph contains DiNode and DiEdge objects that belong to the 
 *        directed graph.
 *        A DiGraph owns the DiNode and DiEdge objects belonging to it.
 *        A DiNode represents a directed graph node. 
 *        A DiNode has an ordered in- and out-neighbor list.
 *        At one time, a DiNode object can belong to at most one DiGraph.
 *        A DiEdge represents an directed graph edge.
 *        A DiEdge holds one source node and one destination node information.
 *
 *        Inherited manipulations are:
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
 *         DiNode and DiEdge objects are intended to be managed by DiGraph 
 *         objects and hence copy construction, move construction, 
 *         copy assignment, and move assignment are prohibited.
 *
 *         Only move construction and move assignment are permitted for
 *         DiGraph objects. Hence, DiGraph objects are contained in the 
 *         standard containers in limited conditions just like std::unique_ptr
 *         objects.
 * 
 *         DiGraph object is copied via a specific method of DiGraph called
 *         copySubgraph().
 *
 *         Instantiation of DiGraph, DiNode, and DIEdge objects are allowed
 *         only by the factory classes inherited  from BaseGraphFactory class.
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

using namespace std;

using namespace Wailea::Undirected;

class DiGraph;
class DiEdge;
class DiNode;

/** @class  DiNode
 *  @brief  This class represents the basic directed node concept.
 */
class DiNode : public Node {

  public:


    /** @brief  default-constructs Node to this.
     */
    inline DiNode() noexcept;


    /** @brief copy construction is not allowed.
     */
    DiNode(const Node& rhs)            = delete;


    /** @brief move construction is not allowed.
     */
    DiNode(Node&& rhs)                 = delete;


    /** @brief copy assignment operator is not allowed.
     */
    DiNode& operator=(const Node& rhs) = delete;


    /** @brief move assignment operator is not allowed.
     */
    DiNode& operator=(Node&& rhs)      = delete;


    /** @brief  destructs this object
     */
    inline virtual ~DiNode() noexcept;


    /** @brief  returns pair of begin and end iterators of the incident
     *          in-neighbor edges in a list.
     *
     *  @throw  std::invalid_argument(Constants::kExceptionNodeNotInGraph)
     *              if the node does not belong to a graph.
     *
     *  @return pair of begin and end iterators
     */
    inline std::pair<node_incidence_it_t,node_incidence_it_t> 
                                                             incidentEdgesIn();

    /** @brief  returns pair of begin and end iterators of the incident
     *          out-neighbor edges in a list.
     *
     *  @throw  std::invalid_argument(Constants::kExceptionNodeNotInGraph)
     *              if the node does not belong to a graph.
     *
     *  @return pair of begin and end iterators
     */
    inline std::pair<node_incidence_it_t,node_incidence_it_t>
                                                            incidentEdgesOut();

    /** @brief  reorders the incident in-neighbor list
     *
     *  @throw  std::invalid_argument(Constants::kExceptionNodeNotInGraph)
     *              if the node does not belong to a graph.
     *
     *  @param  reorderedEdgesIn (in): list of incident in-neighbor edges
     *                                 in the new order.
     */
    inline void reorderIncidenceIn(list<edge_list_it_t>&& reorderedEdgesIn);


    /** @brief  reorders the incident in-neighbor list
     *
     *  @throw  std::invalid_argument(Constants::kExceptionNodeNotInGraph)
     *              if the node does not belong to a graph.
     *
     *  @param  reorderedEdgesIn (in): list of incident in-neighbor edges
     *                                 in the new order.
     */
    inline void reorderIncidenceOut(list<edge_list_it_t>&& reorderedEdgesIn);


    /** @brief  returns the in-degree of this node
     *
     *  @throw  std::invalid_argument(Constants::kExceptionNodeNotInGraph)
     *              if the node does not belong to a graph.
     *
     *  @return in-degree
     */
    inline size_t degreeIn() const;


    /** @brief  returns the out-degree of this node
     *
     *  @throw  std::invalid_argument(Constants::kExceptionNodeNotInGraph)
     *              if the node does not belong to a graph.
     *
     *  @return out-degree
     */
    inline size_t degreeOut() const;


  protected:


    /** @brief incicence in-neighbor edge list that holds the iterators
     *         of mEdges list of the graph it belongs to.
     */
    node_incidence_t mIncidenceIn;

    /** @brief incicence out-neighbor edge list that holds the iterators
     *         of mEdges list of the graph it belongs to.
     */
    node_incidence_t mIncidenceOut;


  friend class DiGraph;
  friend class DiEdge;

#ifdef UNIT_TESTS
  friend class DiNodeTests;
  friend class DiGraphTests;
#endif

};

/** @class  DiEdge
 *  @brief  This class represents the basic directed edge concept.
 */
class DiEdge : public Edge {

  public:


    /** @brief  default-constructs DiEdge to this.
     */
    inline DiEdge() noexcept;


    /** @brief copy constructor is not allowed.
     */
    DiEdge(const Edge& rhs)            = delete;


    /** @brief move constructor is not allowed.
     */
    DiEdge(Edge&& rhs)                 = delete;


    /** @brief copy assignment operator is not allowed.
     */
    DiEdge& operator=(const Edge& rhs) = delete;


    /** @brief move assignment operator is not allowed.
     */
    DiEdge& operator=(Edge&& rhs)      = delete;


    /** @brief  destructs this object
     */
    inline virtual ~DiEdge() noexcept;


    /** @brief  returns the incident source node
     *
     *  @throw  std::invalid_argument(Constants::kExceptionEdgeNotInGraph)
     *              if the edge does not belong to a graph.
     *
     *  @return reference to the source node
     */
    inline DiNode& incidentNodeSrc() const;

    /** @brief  returns the incident destination node
     *
     *  @throw  std::invalid_argument(Constants::kExceptionEdgeNotInGraph)
     *              if the edge does not belong to a graph.
     *
     *  @return reference to the destination node
     */
    inline DiNode& incidentNodeDst() const;


    /** @brief  returns the iterator in the incidence out-neighbor list of
     *          the source node.
     *
     *  @throw  std::invalid_argument(Constants::kExceptionEdgeNotInGraph)
     *              if the edge does not belong to a graph.
     *
     *  @return iterator in the incidence out-neighbor list of
     *          the source node.
     */
    inline node_incidence_it_t incidentBackItNodeSrc() const noexcept;


    /** @brief  returns the iterator in the incidence in-neighbor list of
     *          the destination node.
     *
     *  @throw  std::invalid_argument(Constants::kExceptionEdgeNotInGraph)
     *              if the edge does not belong to a graph.
     *
     *  @return iterator in the incidence in-neighbor list of
     *          the destination node.
     */
    inline node_incidence_it_t incidentBackItNodeDst() const noexcept;


    /** @brief  reverse the orientation of the edge
     *  @remark NOTE: This will break the underlying embedding
     *          as it will change the incident edges list of incident nodes.
     */
    inline void flipOrientation() noexcept;
  protected:

    node_incidence_it_t mBackItNodeSrc;

    node_incidence_it_t mBackItNodeDst;

    bool                mIsNode1Src;


  friend class DiGraph;
  friend class DiNode;

#ifdef UNIT_TESTS
  friend class DiEdgeTests;
  friend class DiGraphTests;
#endif

};


/** @class  DiGraph
 *  @brief  This class represents the basic undirected graph concept.
 *          It contains zero or more nodes and edges.
 */
class DiGraph : public Graph {

  public:


    /** @brief  default-constructs DiGraph to this.
     */
    inline DiGraph() noexcept;


    /** @brief copy constructor is not allowed.
     */
    DiGraph(const DiGraph& rhs) = delete;


    /** @brief  move-constructs DiGraph to this.
     *
     *  @param  rhs  (in): DiGraph whose contents are swapped with this.
     */
    inline DiGraph(DiGraph&& rhs) noexcept;


    /** @brief copy assignment operator is not allowed.
     */
    DiGraph& operator=(const DiGraph& rhs) = delete;


    /** @brief  move-assigns DiGraph to this.
     *
     *  @param  rhs  (in): DiGraph whose contents are swapped with this.
     *
     *  @return reference to this.
     */
    inline DiGraph& operator=(DiGraph&& rhs) noexcept;


    /** @brief  destructs this object
     */
    inline virtual ~DiGraph() noexcept;


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
     *  @param  posInGraph (in): the position before which the edge is inserted
     *                           in the edge list of the graph.
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
                  const node_incidence_it_t& posInNode2,
                  const edge_list_it_t&      posInGraph )
    override;

    inline virtual Edge& addEdge(edge_ptr_t&& e, Node& n1, Node& n2,
                  const node_incidence_it_t& posInNode1,
                  const node_incidence_it_t& posInNode2 )
    override;

    inline virtual Edge& addEdge(edge_ptr_t&& e, Node& n1, Node& n2)
    override;


    /** @brief  adds the specified edge to the graph between two specified
     *          nodes, and transfers the ownership of the edge from the caller
     *          to the graph.
     *
     *  @param  e            (in): pointer to the edge to be added to the graph
     *  @param  nSrc         (in): node to become source node of e.
     *  @param  nDst         (in): node to become destination node of e.
     *  @param  posInNodeSrc (in): the position in the incidence list of node 1
     *                             before which e is inserted.
     *  @param  posInNodeDst (in): the position in the incidence list of node 2
     *                             before which e is inserted.
     *  @param  posInGraph   (in): the position before which the edge is 
     *                             inserted in the edge list of the graph
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
    DiEdge& addDiEdge(edge_ptr_t&& e, DiNode& nSrc, DiNode& nDst,
                  const node_incidence_it_t& posInNodeSrc,
                  const node_incidence_it_t& posInNodeDst,
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
    virtual edge_ptr_t removeEdge(Edge& e)
    override;

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
            const node_list_it_t& pos          )
    override;


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
         node_incidence_it_t toN1Pos, Node& toN2, node_incidence_it_t toN2Pos)
    override;

    inline virtual void moveEdge(Edge& e, Node& toN1, Node& toN2)
    override;


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
    virtual edge_ptr_t contractEdge(Edge&e, node_ptr_t&removedNode)
    override;

    inline virtual edge_ptr_t contractEdge(Edge&e)
    override;


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
                      Graph& g)
    override;

protected:
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
                                  vector<node_list_it_t>&nodes, Graph& g)
    override;

#ifdef UNIT_TESTS
  friend class DiGraphTests;
#endif

};


/*
 * The following are implementation of inline functions.
 */


/** @brief  default-constructs DiNode to this.
 */
inline DiNode::DiNode() noexcept {;}


/** @brief  destructs this object
 */
inline DiNode::~DiNode() noexcept {;}


/** @brief  returns pair of begin and end iterators of the incident
 *          in-neighbor edges in a list.
 *
 *  @throw  std::invalid_argument(Constants::kExceptionNodeNotInGraph)
 *              if the node does not belong to a graph.
 *
 *  @return pair of begin and end iterators
 */
inline std::pair<node_incidence_it_t,node_incidence_it_t>
DiNode::incidentEdgesIn()
{
    if (!isGraphValid()) {
        throw std::invalid_argument(Constants::kExceptionNodeNotInGraph);
    }

    std::pair<node_incidence_it_t,
              node_incidence_it_t> p(mIncidenceIn.begin(),mIncidenceIn.end());
    return p;
}


/** @brief  returns pair of begin and end iterators of the incident
 *          out-neighbor edges in a list.
 *
 *  @throw  std::invalid_argument(Constants::kExceptionNodeNotInGraph)
 *              if the node does not belong to a graph.
 *
 *  @return pair of begin and end iterators
 */
inline std::pair<node_incidence_it_t,node_incidence_it_t>
DiNode::incidentEdgesOut()
{
    if (!isGraphValid()) {
        throw std::invalid_argument(Constants::kExceptionNodeNotInGraph);
    }

    std::pair<node_incidence_it_t,
             node_incidence_it_t> p(mIncidenceOut.begin(),mIncidenceOut.end());
    return p;
}


/** @brief  reorders the incident in-neighbor list
 *
 *  @throw  std::invalid_argument(Constants::kExceptionNodeNotInGraph)
 *              if the node does not belong to a graph.
 *
 *  @param  reorderedEdgesIn (in): list of incident in-neighbor edges
 *                                 in the new order.
 */
inline void DiNode::reorderIncidenceIn(
    std::list<edge_list_it_t>&& reorderedEdgesIn
) {

    if (!isGraphValid()) {
        throw std::invalid_argument(Constants::kExceptionNodeNotInGraph);
    }

    mIncidenceIn = std::move(reorderedEdgesIn);

    for (auto mit = mIncidenceIn.begin(); mit != mIncidenceIn.end(); mit++) {

        auto& E = dynamic_cast<DiEdge&>(*(*(*mit)));
        E.mBackItNodeDst = mit;
    }

}


/** @brief  reorders the incident out-neighbor list
 *
 *  @throw  std::invalid_argument(Constants::kExceptionNodeNotInGraph)
 *              if the node does not belong to a graph.
 *
 *  @param  reorderedEdgesOut (in): list of incident out-neighbor edges
 *                                 in the new order.
 */
inline void DiNode::reorderIncidenceOut(
    std::list<edge_list_it_t>&& reorderedEdgesOut
) {

    if (!isGraphValid()) {
        throw std::invalid_argument(Constants::kExceptionNodeNotInGraph);
    }

    mIncidenceOut = std::move(reorderedEdgesOut);

    for (auto mit = mIncidenceOut.begin(); mit != mIncidenceOut.end(); mit++) {

        auto& E = dynamic_cast<DiEdge&>(*(*(*mit)));
        E.mBackItNodeSrc = mit;
    }

}


/** @brief  returns the in-degree of this node
 *
 *  @throw  std::invalid_argument(Constants::kExceptionNodeNotInGraph)
 *              if the node does not belong to a graph.
 *
 *  @return in-degree
 */
inline size_t DiNode::degreeIn() const
{
    if (!isGraphValid()) {
        throw std::invalid_argument(Constants::kExceptionNodeNotInGraph);
    }
    return mIncidenceIn.size();
}


/** @brief  returns the out-degree of this node
 *
 *  @throw  std::invalid_argument(Constants::kExceptionNodeNotInGraph)
 *              if the node does not belong to a graph.
 *
 *  @return in-degree
 */
inline size_t DiNode::degreeOut() const
{
    if (!isGraphValid()) {
        throw std::invalid_argument(Constants::kExceptionNodeNotInGraph);
    }
    return mIncidenceOut.size();
}


/** @brief  default-constructs DiEdge to this.
 */
inline DiEdge::DiEdge() noexcept : mIsNode1Src(true) {;}


/** @brief  destructs this object
 */
inline DiEdge::~DiEdge() noexcept {;};


/** @brief  returns the incident source node
 *
 *  @throw  std::invalid_argument(Constants::kExceptionEdgeNotInGraph)
 *              if the edge does not belong to a graph.
 *
 *  @return reference to the source node
 */
inline DiNode& DiEdge::incidentNodeSrc() const
{
    if (!isGraphValid()) {
        throw std::invalid_argument(Constants::kExceptionEdgeNotInGraph);
    }
    if (mIsNode1Src) {
        return dynamic_cast<DiNode&>(incidentNode1());
    }
    else {
        return dynamic_cast<DiNode&>(incidentNode2());
    }
}


/** @brief  returns the incident destination node
 *
 *  @throw  std::invalid_argument(Constants::kExceptionEdgeNotInGraph)
 *              if the edge does not belong to a graph.
 *
 *  @return reference to the destination node
 */
inline DiNode& DiEdge::incidentNodeDst() const
{
    if (!isGraphValid()) {
        throw std::invalid_argument(Constants::kExceptionEdgeNotInGraph);
    }
    if (mIsNode1Src) {
        return dynamic_cast<DiNode&>(incidentNode2());
    }
    else {
        return dynamic_cast<DiNode&>(incidentNode1());
    }
}


/** @brief  returns the iterator in the incidence out-neighbor list of
 *          the source node.
 *
 *  @throw  std::invalid_argument(Constants::kExceptionEdgeNotInGraph)
 *              if the edge does not belong to a graph.
 *
 *  @return iterator in the incidence out-neighbor list of
 *          the source node.
 */
inline node_incidence_it_t DiEdge::incidentBackItNodeSrc() const noexcept
{
    return mBackItNodeSrc;
}


/** @brief  returns the iterator in the incidence in-neighbor list of
 *          the destination node.
 *
 *  @throw  std::invalid_argument(Constants::kExceptionEdgeNotInGraph)
 *              if the edge does not belong to a graph.
 *
 *  @return iterator in the incidence in-neighbor list of
 *          the destination node.
 */
inline node_incidence_it_t DiEdge::incidentBackItNodeDst() const noexcept
{
    return mBackItNodeDst;
}


/** @brief  reverse the orientation of the edge
 */
inline void DiEdge::flipOrientation() noexcept
{
    
    node_incidence_it_t mBackItNodeSrc;

    node_incidence_it_t mBackItNodeDst;

    auto& N1 = dynamic_cast<DiNode&>(incidentNode1());
    auto& N2 = dynamic_cast<DiNode&>(incidentNode2());
    if (mIsNode1Src) {
        N1.mIncidenceOut.erase(mBackItNodeSrc);
        N2.mIncidenceIn.erase(mBackItNodeDst);
        mBackItNodeDst = N1.mIncidenceIn.insert(
                                              N1.mIncidenceIn.end(),backIt());
        mBackItNodeSrc = N2.mIncidenceOut.insert(
                                              N2.mIncidenceOut.end(),backIt());
        mIsNode1Src = false;
    }
    else {
        N2.mIncidenceOut.erase(mBackItNodeSrc);
        N1.mIncidenceIn.erase(mBackItNodeDst);
        mBackItNodeDst = N2.mIncidenceIn.insert(
                                              N2.mIncidenceIn.end(),backIt());
        mBackItNodeSrc = N1.mIncidenceOut.insert(
                                              N1.mIncidenceOut.end(),backIt());
        mIsNode1Src = true;
    }
}


DiGraph::DiGraph() noexcept {;}

DiGraph::~DiGraph() noexcept {;}


DiGraph::DiGraph(DiGraph&& rhs) noexcept : Graph(std::move(rhs)) {;}

DiGraph& DiGraph::operator=(DiGraph&& rhs) noexcept {
    Graph::operator=(std::move(rhs));
    return *this;
}

inline Edge& DiGraph::addEdge(edge_ptr_t&& e, Node& n1, Node& n2,
              const node_incidence_it_t& posInNode1,
              const node_incidence_it_t& posInNode2 ) {
    return addEdge(std::forward<edge_ptr_t>(e), 
                          n1, n2, posInNode1, posInNode2, edges().second);
}


inline Edge& DiGraph::addEdge(edge_ptr_t&& e, Node& n1, Node& n2) {
    return addEdge(std::forward<edge_ptr_t>(e), n1, n2, 
         n1.incidentEdges().second, n2.incidentEdges().second, edges().second);
}


inline void DiGraph::moveEdge(Edge& e, Node& toN1, Node& toN2) {
    moveEdge(e, toN1, toN1.incidentEdges().second, 
                toN2, toN2.incidentEdges().second);
}


inline edge_ptr_t DiGraph::contractEdge(Edge&e) {
    node_ptr_t removedNode;
    return contractEdge(e, removedNode);
}



#ifdef UNIT_TESTS
class NumDiNode : public DiNode {

public:
    NumDiNode(int n):mNum(n){;};
    int num() {return mNum;}
    void printAdjacency(ostream& os){
        os << "DiNode In:[" << mNum << "]: ";
        for(auto eIt=incidentEdges().first;eIt!=incidentEdges().second;eIt++){
            auto& E = dynamic_cast<DiEdge&>(*(*(*eIt)));
            auto& A = dynamic_cast<NumDiNode&>(E.adjacentNode(*this));
            os << A.mNum << " ";
        }
        os << "\n";
    }
private:
    int mNum;
};
#endif

}// namespace Directed

}// namespace Wailea

#endif /*_WAILEA_DIRECTED_DI_BASE_HPP_*/
