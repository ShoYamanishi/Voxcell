#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <exception>

#include "di_base.hpp"

/** @file  directed/di_base.cpp
 *
 *  @brief implementation of non-inline methods of DiGrpah, DiNode, and DiEdge.
 */

namespace Wailea {

namespace Directed { 


/**
 *  The following functions are not needed as they would be the same as
 *  Graph's version except for the virtual removeEdge() called in them.
 *
 *      node_ptr_t DiGraph::removeNode(
 *                           Node& n, std::vector<edge_ptr_t>& removedEdges);
 *
 *      node_ptr_t DiGraph::removeNode(Node& n);
 *
 *      std::vector<edge_ptr_t> DiGraph::removeCutSet(
 *                                  std::vector<edge_list_it_t>& edges, 
 *                                  std::vector<node_list_it_t>& nodes);
 *
 *  The following functions are not needed as they would be the same as
 *  Graph's version except for the virtual moveEdgesAndNodes() called in them.
 *
 *   vector<edge_ptr_t> DiGraph::moveEdgeInducedSubgraph(
 *                                   vector<edge_list_it_t>&edges, Graph& g);
 *
 *   vector<edge_ptr_t> DiGraph::moveNodeInducedSubgraph(
 *                                   vector<node_list_it_t>&nodes, Graph& g);
 */


/** @brief  adds the specified edge to the graph between two specified nodes,
 *          and transfers the ownership of the edge from the caller to the
 *          graph.
 *
 *  @param  ePtr       (in): pointer to the edge to be added to the graph.
 *  @param  n1         (in): source node to become incident node 1 of e.
 *  @param  n2         (in): destination node to become incident node 2 of e.
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
 *              if an insersion of list<> has failed due to memory shortage.
 *
 *  @remark exception safety:
 *              If an exception is thrown, all the data structures in the graph
 *              and the parameters don't change.
 *
 *  @remark undefined behavior:
 *              If an invalid value is given to posInGraph, posInNode1, or
 *              posInNode2y, it causes undefined behavior because the
 *              underlying list<>::insert behaves that way.
 */
Edge& DiGraph::addEdge(
    edge_ptr_t&&               ePtr,
    Node&                      n1,
    Node&                      n2,
    const node_incidence_it_t& posInNode1,
    const node_incidence_it_t& posInNode2,
    const edge_list_it_t&      posInGraph
)
{
    auto& E = dynamic_cast<DiEdge&>(Graph::addEdge(
                                    std::forward<edge_ptr_t>(ePtr),
                                    n1,
                                    n2,
                                    posInNode1, 
                                    posInNode2,
                                    posInGraph
                                   ));

    auto& nSrc = dynamic_cast<DiNode&>(n1);
    auto& nDst = dynamic_cast<DiNode&>(n2);

    E.mBackItNodeSrc = nSrc.mIncidenceOut.insert(
                                         nSrc.mIncidenceOut.end(), E.backIt());

    E.mBackItNodeDst = nDst.mIncidenceIn.insert(
                                          nDst.mIncidenceIn.end(), E.backIt());
    return E;
}


/** @brief  adds the specified edge to the graph between two specified
 *          nodes, and transfers the ownership of the edge from the caller
 *          to the graph.
 *
 *  @param  ePtr            (in): pointer to the edge to be added to the graph
 *  @param  nSrc         (in): node to become source node of e.
 *  @param  nDst         (in): node to become destination node of e.
 *  @param  posInNodeSrc (in): the position in the incidence list of node 1
 *                             before which e is inserted.
 *  @param  posInNodeDst   (in): the position in the incidence list of node 2
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
DiEdge& DiGraph::addDiEdge(edge_ptr_t&& ePtr, DiNode& nSrc, DiNode& nDst,
                  const node_incidence_it_t& posInNodeSrc,
                  const node_incidence_it_t& posInNodeDst,
                  const edge_list_it_t&      posInGraph
) {
    auto& E = dynamic_cast<DiEdge&>(Graph::addEdge(
                                        std::forward<edge_ptr_t>(ePtr),
                                        nSrc,
                                        nDst,
                                        nSrc.incidentEdges().second,
                                        nDst.incidentEdges().second,
                                        posInGraph
                                   ));

    E.mIsNode1Src = true;
    E.mBackItNodeSrc = nSrc.mIncidenceOut.insert(posInNodeSrc,E.backIt());
    E.mBackItNodeDst = nDst.mIncidenceIn.insert(posInNodeDst, E.backIt());
                     
    return E;
}





/** @brief   removes the specified edge from the digraph, and transfers the
 *           ownership to the caller.
 *
 *  @param   be (in): the edge to be removed.
 *
 *  @return  removed edge wrapped in a unique pointer.
 *
 *  @throws  invalid_argument(Constants::kExceptionEdgeNotInGraph)
 *               if e is not part of this graph.}
 *  @remark  if an exception is thrown, no data structure in this graph
 *           is changed.
 */
edge_ptr_t DiGraph::removeEdge(Edge& be)
{
//   cerr << "DiGraph::removeEdge()\n";
    if (!be.isGraphValid()) {
        throw std::invalid_argument(Constants::kExceptionEdgeNotInGraph);
    }

    auto& E = dynamic_cast<DiEdge&>(be);

    auto& nSrc = dynamic_cast<DiNode&>(
                          (E.mIsNode1Src)?E.incidentNode1():E.incidentNode2());
    auto& nDst = dynamic_cast<DiNode&>(
                          (E.mIsNode1Src)?E.incidentNode2():E.incidentNode1());
    nSrc.mIncidenceOut.erase(E.mBackItNodeSrc);
    nDst.mIncidenceIn.erase(E.mBackItNodeDst);

    return Graph::removeEdge(be); //rvo
}



/** @brief  splits the specified edge into two. The specified edge e
 *          is removed, the new node n is inserted, and then two new
 *          edges e1 and e2 are inserted.
 *          Before: n1 - (e) - n2
 *          After:  n1 - (e1) - n - (e2) - n2
 *
 *  @param  be           (in):  edge to be split (removed)
 *  @param  removedEdge (out): edge removed with ownership transferred to
 *                             the caller.
 *  @param  e1          (in):  new edge to be inserted between n1 and n.
 *  @param  e2          (in):  new edge to be inserted between n and n2.
 *  @param  n           (in):  new node inserted between e1 and e2.
 *  @param  pos         (in):  the position in mNodes before which n is
 *                             inserted.
 *
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
 *              If an exception is thrown, all the data structures in the graph
 *              and the parameters don't change.
 *
 *  @remark undefined behavior:
 *              If an invalid value is given to pos
 *              it causes undefined behavior because the
 *              underlying list<>::insert behaves that way.
 */
std::tuple<Node&, Edge&, Edge&>
    DiGraph::splitEdge(
        Edge&                 be,
        edge_ptr_t&           removedEdge,
        edge_ptr_t&&          e1,
        edge_ptr_t&&          e2,
        node_ptr_t&&          n,
        const node_list_it_t& pos          )
{
    auto& E = dynamic_cast<DiEdge&>(be);
    bool  IsNode1Src = E.mIsNode1Src;

    auto backItNodeSrc = E.mBackItNodeSrc;
    auto backItNodeDst = E.mBackItNodeDst;


    std::tuple<Node&, Edge&, Edge&> T = Graph::splitEdge
                                           (
                                              be,
                                              removedEdge,
                                              std::forward<edge_ptr_t>(e1),
                                              std::forward<edge_ptr_t>(e2),
                                              std::forward<node_ptr_t>(n),
                                              pos
                                           );

    auto& Nnew  = dynamic_cast<DiNode&>(get<0>(T));
    auto& Enew1 = dynamic_cast<DiEdge&>(get<1>(T));
    auto& Enew2 = dynamic_cast<DiEdge&>(get<2>(T));

    if (IsNode1Src) {
        // N1 --Enew1-> Nnew --Enew2-> N2
        (*backItNodeSrc) = Enew1.backIt();
        Enew1.mBackItNodeSrc = backItNodeSrc;
        Enew1.mBackItNodeDst = Nnew.mIncidenceIn.insert(
                                      Nnew.mIncidenceIn.end(), Enew1.backIt());
        Enew2.mBackItNodeSrc = Nnew.mIncidenceOut.insert(
                                     Nnew.mIncidenceOut.end(), Enew2.backIt());
        (*backItNodeDst) = Enew2.backIt();
        Enew2.mBackItNodeDst = backItNodeDst;
    }
    else {

        // N1 <-Enew1-- Nnew <-Enew2-- N2
        (*backItNodeDst) = Enew1.backIt();
        Enew1.mBackItNodeSrc = Nnew.mIncidenceOut.insert(
                                    Nnew.mIncidenceOut.end(), Enew1.backIt());
        Enew2.mBackItNodeDst = Nnew.mIncidenceIn.insert(
                                     Nnew.mIncidenceIn.end(), Enew2.backIt());
        (*backItNodeSrc) = Enew2.backIt();

    }

    return T;
}


/** @brief moves the specified edge between new nodes specified
 *         by toN1 and toN2.
 *
 *  @param  be       (in): target edge to be moved
 *  @param  toN1    (in): the new incident node 1 of e.
 *                        This will be the source node.
 *  @param  toN1Pos (in): the position in the incidence lis of node 1 before
 *                        which e is inserted.
 *  @param  toN2    (in): the new incident node 2 of e.
 *                        This will be the destination node.
 *  @param  toN2Pos (in): the position in the incidence lis of node 2 before
 *                        which e is inserted.
 *
 *  @throws bad_alloc() (if default allocator is used for list<>.)
 *              - if any insert of list<> has failed due to memory shortage.
 *
 *  @remark exception safety:
 *              If an exception is thrown, all the data structures in the graph
 *              and the parameters don't change.
 *
 *  @remark undefined behavior:
 *              If an invalid value is given to pos
 *              it causes undefined behavior because the
 *              underlying list<>::insert behaves that way.
 */
void DiGraph::moveEdge(
    Edge&               be,
    Node&               toN1,
    node_incidence_it_t toN1Pos,
    Node&               toN2,
    node_incidence_it_t toN2Pos
)
{
    auto& E = dynamic_cast<DiEdge&>(be);
    auto& N1 = dynamic_cast<DiNode&>(E.incidentNode1());
    auto& N2 = dynamic_cast<DiNode&>(E.incidentNode2());

    bool  IsNode1Src = E.mIsNode1Src;

    if (IsNode1Src) {
        N1.mIncidenceOut.erase(E.mBackItNodeSrc);
        N2.mIncidenceIn.erase(E.mBackItNodeDst);
    }
    else {
        N1.mIncidenceIn.erase(E.mBackItNodeDst);
        N2.mIncidenceOut.erase(E.mBackItNodeSrc);
    }

    Graph::moveEdge(be, toN1, toN1Pos, toN2, toN2Pos);

    auto& N1new = dynamic_cast<DiNode&>(toN1);
    auto& N2new = dynamic_cast<DiNode&>(toN2);

    if (IsNode1Src) {
        E.mBackItNodeSrc = N1new.mIncidenceOut.insert(
                                       N1new.mIncidenceOut.end(), E.backIt());
        E.mBackItNodeDst = N2new.mIncidenceIn.insert(
                                        N2new.mIncidenceIn.end(), E.backIt());
    }
    else {
        E.mBackItNodeSrc = N2new.mIncidenceOut.insert(
                                       N2new.mIncidenceOut.end(), E.backIt());
        E.mBackItNodeDst = N1new.mIncidenceIn.insert(
                                        N1new.mIncidenceIn.end(), E.backIt());
    }

    return;
}


/** @brief  contracts the specified edge. Of the two adjacent nodes, node2
 *          is removed, and all the incident edges to node 2 except for the
 *          one conracted are relinked to node 1.
 *          The ownerships of the specified edge and node 1 are transferred
 *          to the caller.
 *
 *  @param  be            (in):  the edge to be contracted.
 *  @param  removedNode  (out): the node 2 removed with its ownership.
 *
 *  @return the edge contracted with its ownership.
 *
 *  @throws bad_alloc()
 *              if default allocator is used for list<>.
 *              if any insert of list<> has failed due to memory shortage.
 *
 *  @remark exception safety:
 *              If an exception is thrown, all the data structures in the graph
 *              and the parameters don't change.
 */
edge_ptr_t DiGraph::contractEdge(Edge&be, node_ptr_t&removedNode)
{
    auto& Ec = dynamic_cast<DiEdge&>(be);
    auto& EcN1 = dynamic_cast<DiNode&>(Ec.incidentNode1());
    auto& EcN2 = dynamic_cast<DiNode&>(Ec.incidentNode2());

    bool  IsNode1Src = Ec.mIsNode1Src;

    // Unlink src & dst between Ec and EcN1
    if (IsNode1Src) {
        EcN1.mIncidenceOut.erase(Ec.mBackItNodeSrc);
    }
    else {
        EcN1.mIncidenceIn.erase(Ec.mBackItNodeDst);
    }

    // Unlink src & dst between all the edges incident to EcN2

    // The incident edges of EcN2 will be moved to N1.
    vector<edge_list_it_t> edgesOnEcN2;

    // On the edges of EcN2, which side of N1 or N2 is EcN2.
    vector<bool>           EcN2_On_N1;

    for( auto eIt = EcN2.incidentEdges().first;
                                 eIt != EcN2.incidentEdges().second; eIt++) {
        auto& E = dynamic_cast<DiEdge&>(*(*(*eIt)));
        if (*eIt != Ec.backIt()) {

            edgesOnEcN2.push_back(*eIt);

            if (E.mIncidentNode1 == EcN2.backIt()){
                EcN2_On_N1.push_back(true);
            }
            else {
                EcN2_On_N1.push_back(false);
            }
        }
    }

    EcN2.mIncidenceIn.clear();
    EcN2.mIncidenceOut.clear();

    edge_ptr_t ePtr = Graph::contractEdge(be, removedNode);

    // Link dst / src to the edges that were incident to EcN2.
    auto bIt = EcN2_On_N1.begin();
    for( auto eIt : edgesOnEcN2) {
        auto& E = dynamic_cast<DiEdge&>(*(*eIt));
        if ( (*bIt  && E.mIsNode1Src)  ||  (!(*bIt) && !(E.mIsNode1Src)) ) {
            // EcN2 was on N1 side and it was src, or
            // EcN2 was on N2 side and it was src.
            E.mBackItNodeSrc = EcN1.mIncidenceOut.insert(
                                         EcN1.mIncidenceOut.end(), E.backIt());
        }
        else {
            // EcN2 was on N1 side and it was dst, or
            // EcN2 was on N2 side and it was dst.
            E.mBackItNodeDst = EcN1.mIncidenceIn.insert(
                                          EcN1.mIncidenceIn.end(), E.backIt());
        }

        bIt++;
    }

    return ePtr;
}


/** @brief  moves the specified nodes and the edges to another graph specified
 *          by g.
 *
 *  @param   edges (in/out): edges to be moved to g.
 *                           after this operation, the contents are replaced
 *                           with the iterators in mEdges of g.
 *  @param   nodes (in/out): nodes to be moved to g.
 *                           after this operation, the contents are replaced
 *                           with the iterators in mNodes of g.
 *  @param   g     (in):     graph to which edges and nodes are moved.
 *
 *  @throws bad_alloc()
 *              if default allocator is used for list<>.
 *              if any insert of list<> has failed due to memory shortage.
 *
 *  @remark exception safety:
 *              If an exception is thrown, all the data structures in the graph
 *              and the parameters don't change.
 */
void DiGraph::moveEdgesAndNodes(
    std::vector<edge_list_it_t>& edges,
    std::vector<node_list_it_t>& nodes,
    Graph& g
)
{

    // Assign temporary node numbers and edge numbers to utility
    // and make incidence structures using them.
    vector<pair<size_t,size_t> > incidentNodesInIndices(edges.size());
    vector<vector<size_t> >      incidentEdgesInIndices(nodes.size());
    vector<vector<size_t> >      incidentEdgesInInIndices(nodes.size());
    vector<vector<size_t> >      incidentEdgesOutInIndices(nodes.size());

    size_t index = 0;
    for (auto& nit : nodes) {
        (*nit)->pushUtility(index++);
    }

    index = 0;
    for (auto& eit : edges) {

        (*eit)->pushUtility(index);
        auto& n1 = (*eit)->incidentNode1();
        auto& n2 = (*eit)->incidentNode2();

        incidentNodesInIndices[index]=make_pair(n1.utility(), n2.utility());
        index++;
    }

    index = 0;
    for (auto& nit : nodes) {
        auto& N = dynamic_cast<DiNode&>(*(*nit));

        auto itPair = N.incidentEdges();
        for (auto iit = itPair.first; iit != itPair.second; iit++){
            incidentEdgesInIndices[index].push_back((*(*iit))->utility());
        }

        itPair = N.incidentEdgesIn();
        for (auto iit = itPair.first; iit != itPair.second; iit++){
            incidentEdgesInInIndices[index].push_back((*(*iit))->utility());
        }

        itPair = N.incidentEdgesOut();
        for (auto iit = itPair.first; iit != itPair.second; iit++){
            incidentEdgesOutInIndices[index].push_back((*(*iit))->utility());
        }

        index++;
    }


    // Allocate place holders to the new graphs' edge and node lists.
    size_t numNodesInserted = 0;
    size_t numEdgesInserted = 0;

    node_list_it_t nodeInsersionStart;
    edge_list_it_t edgeInsersionStart;

    try {
        if (nodes.size() > 0) {

            nodeInsersionStart = g.mNodes.insert(g.mNodes.end(),
                                              std::unique_ptr<Node>(nullptr));
            numNodesInserted++;

            for (;numNodesInserted < nodes.size();numNodesInserted++) {

                g.mNodes.insert(g.mNodes.end(),std::unique_ptr<Node>(nullptr));

            }

        }

        if (edges.size() > 0) {

            edgeInsersionStart = g.mEdges.insert(g.mEdges.end(),
                                              std::unique_ptr<Edge>(nullptr));
            numEdgesInserted++;

            for (;numEdgesInserted < edges.size();numEdgesInserted++) {

                g.mEdges.insert(g.mEdges.end(),std::unique_ptr<Edge>(nullptr));

            }

        }

    }
    catch (exception& e) {

        while (numEdgesInserted-- > 0) {
            g.mEdges.pop_back();
        }

        while (numNodesInserted-- > 0) {
            g.mNodes.pop_back();
        }

        throw;

    }

    // Move the nodes and edges to the new graph.
    vector<node_list_it_t> nodeMapping(numNodesInserted);
    vector<edge_list_it_t> edgeMapping(numEdgesInserted);

    node_list_it_t gn(nodeInsersionStart);
    for (auto& nit : nodes) {

        std::unique_ptr<Node> pt(nit->release());
        pt->mGraph = &g;
        mNodes.erase(nit);
        (*gn) = std::move(pt);
        nodeMapping[(*gn)->utility()] = gn;
        (*gn)->mBackIt = gn;

        gn++;
    }
    nodes.clear();

    edge_list_it_t ge(edgeInsersionStart);
    for (auto& eit : edges) {

        std::unique_ptr<Edge> pt(eit->release());
        pt->mGraph = &g;
        mEdges.erase(eit);
        (*ge) = std::move(pt);
        edgeMapping[(*ge)->utility()] = ge;
        (*ge)->mBackIt = ge;

        // Set the new iterators of the incident nodes.

        auto& nodeNumbers = incidentNodesInIndices[(*ge)->utility()];
        (*ge)->mIncidentNode1 = nodeMapping[nodeNumbers.first];
        (*ge)->mIncidentNode2 = nodeMapping[nodeNumbers.second];
        ge++;

    }
    edges.clear();

    // Recreate the incident edge list for each node.
    for (auto nit = nodeInsersionStart; nit != g.mNodes.end(); nit++) {

        auto& N = dynamic_cast<DiNode&>(*(*nit));

        vector<size_t>& edgeNumbers = incidentEdgesInIndices[N.utility()];
        N.mIncidence.clear();
        for (auto edgeNumber : edgeNumbers) {
            auto edgeIt = edgeMapping[edgeNumber];
            auto& E = dynamic_cast<DiEdge&>(*(*edgeIt));
            auto iit = N.mIncidence.insert(N.mIncidence.end(), edgeIt);

            if (E.mIncidentNode1 == nit) {
                E.mBackItNode1= iit;
            }
            else {
                E.mBackItNode2= iit;
            }
        }

        vector<size_t>& edgeNumbersIn = incidentEdgesInInIndices[N.utility()];
        N.mIncidenceIn.clear();
        for (auto edgeNumber : edgeNumbersIn) {
            auto edgeIt = edgeMapping[edgeNumber];
            auto& E = dynamic_cast<DiEdge&>(*(*edgeIt));
            E.mBackItNodeDst =
                           N.mIncidenceIn.insert(N.mIncidenceIn.end(), edgeIt);
        }

        vector<size_t>& edgeNumbersOut = incidentEdgesOutInIndices[N.utility()];
        N.mIncidenceOut.clear();
        for (auto edgeNumber : edgeNumbersOut) {
            auto edgeIt = edgeMapping[edgeNumber];
            auto& E = dynamic_cast<DiEdge&>(*(*edgeIt));
            E.mBackItNodeSrc =
                         N.mIncidenceOut.insert(N.mIncidenceOut.end(), edgeIt);
        }
        nodes.push_back(nit);
        (*nit)->popUtility();
    }
       
    for (auto eit = edgeInsersionStart; eit != g.mEdges.end(); eit++) {
        edges.push_back(eit);
        (*eit)->popUtility();
    }
}


/** @brief  creates a subgraph in another Graph object specified by g,
 *          that are isomorphic to the subgraph in this Graph specified by
 *          the nodes and edges given in the parameters using new Nodes and
 *          Edges objects given in the parameters.
 *
 *  @param    nodePairs  (in): list of pairs
 *                first:  node whose copy is created in g.
 *                second: Node object with which first's copy is created in g.
 *                        The ownership is transferred from this list to g.
 *
 *  @param    edgePairs  (in): list of pairs
 *                first:  edge whose copy is created in g.
 *                second: Edge Object with which first's copy is created in g.
 *                        The ownership is transferred from this list to g.
 *
 *  @param    g          (in): DiGraph object to which an isomorphic subgraph
 *                             is created.
 *
 *  @throws bad_alloc()
 *              if default allocator is used for list<>.
 *              if any insert of list<> has failed due to memory shortage.
 *
 *  @remark exception safety:
 *              If an exception is thrown, all the data structures in the graph
 *              and the parameters don't change.
 *
 *  @remark node ordering in the copied graph:
 *              The ordering of the copied nodes in g.nodes() is aligned
 *              with the ordering in nodePairs.
 *              The ordering of the copied nodes in g.edges() is aligned
 *              with the ordering in edgePairs.
 *              The ordering of incidentEdges() of each node in g is
 *              aligned with the appropriate subsequence of incidentEdges()
 *              of the original node in this graph.
 */
void DiGraph::copySubgraph(
    vector < pair< node_list_it_t, node_ptr_t > >& nodePairs,
    vector < pair< edge_list_it_t, edge_ptr_t > >& edgePairs,
    Graph& g
)
{

    // Make a subgraph of Graph version.
    size_t numOriginalNodes = g.numNodes();
    size_t numOriginalEdges = g.numEdges();

    Graph::copySubgraph(nodePairs, edgePairs, g);

    mGeneration++;

    // Allocate elements to incidence lists
    try {

        vector<edge_list_it_t> newEdgesArray;

        // Make edge's src and dst, and nodes in and out structures.
        size_t eIndex = 0;
        for (auto newEit = g.edges().first;
                                        newEit != g.edges().second; newEit++) {
            if (numOriginalEdges > 0) {
                numOriginalEdges--;
                continue;
            }

            auto& newE = dynamic_cast<DiEdge&>(*(*newEit));

            auto& orgE = dynamic_cast<DiEdge&>(*(*(edgePairs[eIndex].first)));

            newE.mIsNode1Src = orgE.mIsNode1Src;

            orgE.pushUtility(eIndex);
            orgE.mGeneration = mGeneration;

            newEdgesArray.push_back(newE.backIt());

            eIndex++;

        }

        size_t nIndex = 0;
        for (auto newNit = g.nodes().first;
                                        newNit != g.nodes().second; newNit++) {

            if (numOriginalNodes > 0) {
                numOriginalNodes--;
                continue;
            }

            auto& newN = dynamic_cast<DiNode&>(*(*newNit));

            auto& orgN = dynamic_cast<DiNode&>(*(*(nodePairs[nIndex].first)));

            for (auto orgEit = orgN.mIncidenceIn.begin();
                                 orgEit != orgN.mIncidenceIn.end(); orgEit++) {

                auto& orgE = dynamic_cast<DiEdge&>(*(*(*orgEit)));
                if (orgE.mGeneration == mGeneration) {

                    auto& newE = dynamic_cast<DiEdge&>(
                                          *(*(newEdgesArray[orgE.utility()])));
 
                    newE.mBackItNodeDst = newN.mIncidenceIn.insert(
                                       newN.mIncidenceIn.end(), newE.backIt());

                }
            }

            for (auto orgEit = orgN.mIncidenceOut.begin();
                               orgEit != orgN.mIncidenceOut.end(); orgEit++) {

                auto& orgE = dynamic_cast<DiEdge&>(*(*(*orgEit)));
                if (orgE.mGeneration == mGeneration) {
                    auto& newE = dynamic_cast<DiEdge&>(
                                          *(*(newEdgesArray[orgE.utility()])));

                    newE.mBackItNodeSrc = newN.mIncidenceOut.insert(
                                      newN.mIncidenceOut.end(), newE.backIt());
                }
            }

            nIndex++;

        }

        for (auto& ep : edgePairs) {
            auto eit = ep.first;
            (*eit)->popUtility();
        }

    }
    catch(exception& e) {

        // Allocation failure. Rolling back.
        for (auto newNit = g.nodes().first; newNit != g.nodes().second; ) {

            auto& N = dynamic_cast<DiNode&>(*(*newNit));

            newNit++;

            g.removeNode(N);

        }

        throw;

    }

}

}// namespace Directed

}// namespace Wailea



