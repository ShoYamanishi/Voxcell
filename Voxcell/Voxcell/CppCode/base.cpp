#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <exception>

#include "base.hpp"

/** @file  undirected/base.cpp
 *
 *  @brief implementation of non-inline methods of Grpah, Node, and Edge.
 */

namespace Wailea {

namespace Undirected { 


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
 *  @param    g          (in): Graph object to which an isomorphic subgraph
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
 *          nodes() of the copied graph returns has the same ordering of 
 *          nodePairs.
 */
void Graph::copySubgraph(
    vector < pair< node_list_it_t, node_ptr_t > >& nodePairs,
    vector < pair< edge_list_it_t, edge_ptr_t > >& edgePairs,
    Graph& g
)
{

    vector < node_list_it_t > newNodes;
    vector < edge_list_it_t > newEdges;

    node_list_it_t nodeBegin;
    edge_list_it_t edgeBegin;
    size_t nodesPushed = 0;
    size_t edgesPushed = 0;


    // Identify the edges in the subgraph.
    mGeneration++;

    for (auto& ep : edgePairs) {

        Edge& originalEdge        = *(*(ep.first));
        originalEdge.mGeneration  = mGeneration;

    }

    // Calculate the degrees of the nodes in the subgraph into utility.
    for (auto& np : nodePairs) {

        Node& originalNode    = *(*(np.first));
        auto incidence        = originalNode.incidentEdges();
        originalNode.pushUtility(0);

        for (auto neit = incidence.first ; neit != incidence.second; neit++) {

            Edge& originalEdge = *(*(*neit));
            if (originalEdge.mGeneration == mGeneration) {
                originalNode.setUtility(originalNode.utility()+1);
            }

        }

    }

    try {

        /**
         * Allocate elements to newNodes and newEdges.
         * In this loop, an exception can be thrown.
         */
        if (nodePairs.size() > 0) {

            nodeBegin = g.mNodes.insert(g.mNodes.end(),
                                             std::unique_ptr<Node>(nullptr));
            newNodes.push_back(g.mNodes.begin());// Pushing dummy value

            for (size_t i = 1; i < nodePairs.size(); i++) {

                g.mNodes.push_back(std::unique_ptr<Node>(nullptr));
                                      
                nodesPushed++;
                newNodes.push_back(g.mNodes.begin());// Pushing dummy value

            }
        }

        if (edgePairs.size() > 0) {

            edgeBegin = g.mEdges.insert(g.mEdges.end(),
                                            std::unique_ptr<Edge>(nullptr));
            newEdges.push_back(g.mEdges.begin());// Pushing dummy value

            for (size_t i = 1; i < edgePairs.size(); i++) {

                g.mEdges.push_back(std::unique_ptr<Edge>(nullptr));
                edgesPushed++;
                newEdges.push_back(g.mEdges.begin());// Pushing dummy value

            }
        }

        /** 
         * Allocate elements to the incidence lists of the new Nodes.
         * In this loop, an exception can be thrown.
         */
        for (auto& pp : nodePairs) {

            Node& originalNode   = *(*(pp.first));
            Node& newNode        = *(pp.second);
            size_t numIncidence  = originalNode.utility();

            for (size_t i = 0; i < numIncidence; i++) {
                newNode.mIncidence.push_back(g.mEdges.begin());
            }

        }

    }
    catch (exception& e) {

        while (nodesPushed-- > 0) {
            g.mNodes.pop_back();
        }

        while (edgesPushed-- > 0) {
            g.mEdges.pop_back();
        }

        for (auto& pp : nodePairs) {
            Node& newNode = *(pp.second);
            newNode.mIncidence.clear();
        }        

        throw;
    }

    /**
     * Put the new nodes and edges under g, and manage them by iterators,
     */
    utility_t index = 0;
    node_list_it_t nit = nodeBegin;

    for (auto& np : nodePairs) {

        Node& originalNode    = *(*(np.first));
        (*nit)                = std::move(np.second);
        Node& newNode         = *(*nit);
        newNode.mBackIt       = nit;
        newNode.mGraph        = &g;

        newNodes[index]       = newNode.mBackIt;
        originalNode.setUtility(index);
        newNode.pushUtility(index);

        index++;
        nit++;
    }

    index = 0;
    edge_list_it_t eit = edgeBegin;

    for (auto& ep : edgePairs) {

        Edge& originalEdge        = *(*(ep.first));
        const Node& originalNode1 = originalEdge.incidentNode1();
        const Node& originalNode2 = originalEdge.incidentNode2();
        (*eit)                    = std::move(ep.second);
        newEdges[index]           = eit;
        Edge& newEdge             = *(*eit);
        newEdge.mBackIt           = eit;

        newEdge.mIncidentNode1    = newNodes[originalNode1.utility()];
        newEdge.mIncidentNode2    = newNodes[originalNode2.utility()];
        originalEdge.pushUtility(index);
        newEdge.pushUtility(index);
        newEdge.mGraph            = &g;

        index++;
        eit++;
    }

    /** 
     * At this point the new nodes and edges do not have incidence information.
     */
    for (auto& np : nodePairs) {

        Node& originalNode = *(*(np.first));
        Node& newNode      = *(*newNodes[originalNode.utility()]);
        auto incidence     = originalNode.incidentEdges();

        node_incidence_it_t nnit = newNode.mIncidence.begin();
        for (auto neit = incidence.first ; neit != incidence.second; neit++) {

            Edge& originalEdge = *(*(*neit));
            if (originalEdge.mGeneration == mGeneration) {
                Edge& newEdge      = *(*newEdges[originalEdge.utility()]);
                (*nnit) = newEdge.mBackIt;
                if (newEdge.mIncidentNode1 == newNode.mBackIt) {
                    newEdge.mBackItNode1 = nnit;
                }
                else {
                    newEdge.mBackItNode2 = nnit;
                }

                nnit++;
            }
        }
    }

    for (auto& np : nodePairs) {
        (*(np.first))->popUtility();
    }
    for (auto& ep : edgePairs) {
        (*(ep.first))->popUtility();
    }
    for (auto nit : newNodes) {
       (*nit)->popUtility();
    }
    for (auto eit : newEdges) {
       (*eit)->popUtility();
    }

}


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
 *              If an exception is thrown, all the data structures in the graph
 *              and the parameters don't change.
 */
std::vector<node_list_it_t> Graph::edgeInducedNodes(
    std::vector<edge_list_it_t>& edges
)
{
    mGeneration++;
    std::vector<node_list_it_t> inducedNodes;

    for (auto eit : edges) {

        (*eit)->mGeneration = this->mGeneration;

        if ((*eit)->incidentNode1().mGeneration <  this->mGeneration) {

            (*eit)->incidentNode1().mGeneration = this->mGeneration;
            inducedNodes.push_back((*eit)->incidentNode1().mBackIt);

        }        
        if ((*eit)->incidentNode2().mGeneration <  this->mGeneration) {

            (*eit)->incidentNode2().mGeneration = this->mGeneration;
            inducedNodes.push_back((*eit)->incidentNode2().mBackIt);

        }        
    }

    return inducedNodes; // rvo
}


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
 *              If an exception is thrown, all the data structures in the graph
 *              and the parameters don't change.
 */
std::vector<edge_list_it_t> Graph::nodeInducedEdges(
    std::vector<node_list_it_t>& nodes
)
{
    std::vector<edge_list_it_t> inducedEdges;
    mGeneration++;

    for (auto nit : nodes) {
        (*nit)->mGeneration = mGeneration;
    }

    for (auto nit : nodes) {

        auto incidence = (*nit)->incidentEdges();

        for (auto neit = incidence.first ; neit != incidence.second; neit++) {

            Node& adjacentNode = (*(*neit))->adjacentNode(*(*nit));
            if (adjacentNode.mGeneration == mGeneration) {

                if ((*(*neit))->mGeneration <  mGeneration) {
                    (*(*neit))->mGeneration = mGeneration;
                    inducedEdges.push_back(*neit);
                }
            }
        }
    }        

    return inducedEdges; // rvo
}


/** @brief  identifies the edges lying between the specified subgraph  and
 *          its complement, and removes them from the graph with their 
 *          ownerships transferred to the caller.
 *  @param  edges  (in): edges in the subgraph. 
 *  @param  nodes  (in): nodes in the subgraph.
 *
 *  @return removed edges with their ownerships.
 *
 *  @throws bad_alloc() 
 *              if default allocator is used for list<>.
 *              if any insert of list<> has failed due to memory shortage.
 *
 *  @remark exception safety:
 *              If an exception is thrown, all the data structures in the graph
 *              and the parameters don't change.
 *
 */
std::vector<edge_ptr_t> Graph::removeCutSet(
    std::vector<edge_list_it_t>& edges, 
    std::vector<node_list_it_t>& nodes
)
{
    std::vector<edge_ptr_t>     removedEdgePtrs;
    std::vector<edge_list_it_t> edgesToBeRemoved;
    mGeneration++;

    for (auto eit : edges) {
        (*eit)->mGeneration = mGeneration;
    }

    /**
      * First allocate all the necessary elements to removedEdges
      * This loop can throw bad_alloc, and abort.
      * At this stage, no data structure is changed.
      */

    for (auto nit : nodes) {

        auto incidence = (*nit)->incidentEdges();

        for (auto neit = incidence.first ; neit != incidence.second; neit++){

            if( (*(*neit))->mGeneration < mGeneration ){

                (*(*neit))->mGeneration = mGeneration;
                edgesToBeRemoved.push_back(*neit);
                removedEdgePtrs.push_back(std::unique_ptr<Edge>(nullptr));

            }
        }
    }        

    /**
      * In this second loop, the edges are removed.
      * This loop is guaranteed to run without any exception.
      */
    size_t removedEdgePtrsIndex = 0;
    for (auto rit : edgesToBeRemoved) {
    
        removedEdgePtrs[removedEdgePtrsIndex++] = removeEdge(*(*rit));

    }        

    return removedEdgePtrs; // rvo
}


/** @brief  identifies the edges lying between the specified subgraph  and
 *          its complement.
 *          ownerships transferred to the caller.
 *  @param  edges  (in): edges in the subgraph. 
 *  @param  nodes  (in): nodes in the subgraph.
 *
 *  @return cutset (edge set)
 *
 *  @throws bad_alloc() 
 *              if default allocator is used for list<>.
 *              if any insert of list<> has failed due to memory shortage.
 *
 *  @remark exception safety:
 *              If an exception is thrown, all the data structures in the
 *              graph and the parameters don't change.
 */
std::vector<edge_list_it_t> Graph::findCutSet(
    std::vector<edge_list_it_t>& edges, 
    std::vector<node_list_it_t>& nodes
)
{
    std::vector<edge_list_it_t> cutset;
    mGeneration++;

    for (auto eit : edges) {
        (*eit)->mGeneration = mGeneration;
    }

    for (auto nit : nodes) {

        auto incidence = (*nit)->incidentEdges();

        for (auto neit = incidence.first ; neit != incidence.second; neit++){

            if( (*(*neit))->mGeneration < mGeneration ){

                (*(*neit))->mGeneration = mGeneration;
                cutset.push_back(*neit);

            }
        }
    }        

    return cutset; // rvo
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
void Graph::moveEdgesAndNodes(
    std::vector<edge_list_it_t>& edges,
    std::vector<node_list_it_t>& nodes,
    Graph& g
)
{

    // Assign temporary node numbers and edge numbers to utility
    // and make incidence structures using them.
    vector<pair<size_t,size_t> > incidentNodesInIndices(edges.size()); 
    vector<vector<size_t> >      incidentEdgesInIndices(nodes.size()); 

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
        auto itPair = (*nit)->incidentEdges();
        for (auto iit = itPair.first; iit != itPair.second; iit++){
            incidentEdgesInIndices[index].push_back((*(*iit))->utility());
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
    nodes.clear();
    edges.clear();
    // Recreate the incident edge list for each node.
    for (auto nit = nodeInsersionStart; nit != g.mNodes.end(); nit++) {

        vector<size_t>& edgeNumbers = incidentEdgesInIndices[(*nit)->utility()];
                                      
        (*nit)->mIncidence.clear();
        for (auto edgeNumber : edgeNumbers) {
            auto edgeIt = edgeMapping[edgeNumber];
            auto iit = (*nit)->mIncidence.insert(
                                             (*nit)->mIncidence.end(), edgeIt);
            if ((*edgeIt)->mIncidentNode1 == nit) {
                (*edgeIt)->mBackItNode1= iit;
            }
            else {
                (*edgeIt)->mBackItNode2= iit;
            }
        }
        nodes.push_back(nit);
        (*nit)->popUtility();
    }

    for (auto eit = edgeInsersionStart; eit != g.mEdges.end(); eit++) {
        edges.push_back(eit);
        (*eit)->popUtility();
    }
}


/** @brief  moves subgraph induced by the given edges to another graph
 *          specified by g. It also removes from this graph the edges
 *          that span the subgraph and its complement (cutset)
 *
 *  @param  edges  (in/out): edges to be moved together with their induced
 *                           nodes. After thsi operation the contents are
 *                           replaced with the new iterators in mEdges of g.
 *  @param  g      (in): the target graph to which the subgraph is moved.
 *
 *  @return removed cutset edges with their ownerships.
 *
 *  @throws bad_alloc() (if default allocator is used for list<>.)
 *              - if any insert of list<> has failed due to memory shortage.
 *
 *  @remark exception safety:
 *              If an exception is thrown, all the data structures in the graph
 *              and the parameters don't change.
 */
std::vector<edge_ptr_t> Graph::moveEdgeInducedSubgraph(
    std::vector<edge_list_it_t>& edges,
    Graph&                       g
)
{
    auto inducedNodes = edgeInducedNodes(edges);

    auto removedEdges = removeCutSet(edges,inducedNodes);

    moveEdgesAndNodes(edges, inducedNodes, g);

    return removedEdges; // rvo
}


/** @brief  moves subgraph induced by the given nodes to another graph
 *          specified by g. It also removes from this graph the edges
 *          that span the subgraph and its complement (cutset)
 *
 *  @param  nodes  (in/out): nodes to be moved together with their induced
 *                           edges. After thsi operation the contents are
 *                           replaced with the new iterators in mNodes of g.

 *  @param  g      (in): the target graph to which the subgraph is moved.
 *
 *  @return removed cutset edges with their ownerships.
 *
 *  @throws bad_alloc() (if default allocator is used for list<>.)
 *              - if any insert of list<> has failed due to memory shortage.
 *
 *  @remark exception safety:
 *              If an exception is thrown, all the data structures in the graph
 *              and the parameters don't change.
 */
std::vector<edge_ptr_t> Graph::moveNodeInducedSubgraph(
    std::vector<node_list_it_t>& nodes,
    Graph&                       g
)
{
    auto inducedEdges = nodeInducedEdges(nodes);

    auto removedEdges = removeCutSet(inducedEdges,nodes);

    moveEdgesAndNodes(inducedEdges, nodes, g);

    return removedEdges; // rvo
}


/** @brief moves the specified edge between new nodes specified
 *         by toN1 and toN2.
 *
 *  @param  e       (in): target edge to be moved
 *  @param  toN1    (in): the new incident node 1 of e.
 *  @param  toN1Pos (in): the position in the incidence lis of node 1 before
 *                        which e is inserted.
 *  @param  toN2    (in): the new incident node 2 of e.
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
void Graph::moveEdge(
    Edge&               e,
    Node&               toN1, 
    node_incidence_it_t toN1Pos,
    Node&               toN2,
    node_incidence_it_t toN2Pos
)
{
    Node& fromN1 = e.incidentNode1();
    Node& fromN2 = e.incidentNode2();

    edge_list_it_t placeholderIt;
    node_incidence_it_t toN1It = toN1.mIncidence.insert(toN1Pos,placeholderIt);
    node_incidence_it_t toN2It = toN2.mIncidence.insert(toN2Pos,placeholderIt);

    (*toN1It) = e.mBackIt;
    (*toN2It) = e.mBackIt;

    fromN1.mIncidence.erase(e.mBackItNode1);
    fromN2.mIncidence.erase(e.mBackItNode2);

    e.mBackItNode1   = toN1It;
    e.mBackItNode2   = toN2It;
    e.mIncidentNode1 = toN1.mBackIt;
    e.mIncidentNode2 = toN2.mBackIt;

    return;
}


/** @brief moves the specified edge between new nodes specified
 *         by toN1 and toN2.
 *
 *  @param  e       (in): target edge to be moved
 *  @param  toN1    (in): the new incident node 1 of e.
 *  @param  toN2    (in): the new incident node 2 of e.
 *
 *  @throws bad_alloc() (if default allocator is used for list<>.)
 *              - if any insert of list<> has failed due to memory shortage.
 *
 *  @details the edge e is inserted at the end of the incidence lists of toN1
 *           and toN2.       
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
void Graph::moveEdge(
    Edge&               e,
    Node&               toN1, 
    Node&               toN2
)
{
    if (toN1.backIt() == e.incidentNode1().backIt()) {
        moveEdge(e, toN1,  e.incidentBackItNode1(), 
                                               toN2, toN2.mIncidence.end());
    }
    else if (toN2.backIt() == e.incidentNode2().backIt()) {
        moveEdge(e, toN1, toN1.mIncidence.end(), 
                                     toN2, e.incidentBackItNode2());
    }
    else {
        moveEdge(e, toN1, toN1.mIncidence.end(), toN2, toN2.mIncidence.end());
    }
}



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
    Graph::splitEdge(
        Edge&                 e,
        edge_ptr_t&           removedEdge, 
        edge_ptr_t&&          e1, 
        edge_ptr_t&&          e2,
        node_ptr_t&&          n,
        const node_list_it_t& pos          )
{
    /*
     * Remember the places in the mIncidence list of each 
     * of the two incident nodes of e.
     */
    Node& n1 = e.incidentNode1();
    Node& n2 = e.incidentNode2();
    auto backItNode1 = e.mBackItNode1;
    auto backItNode2 = e.mBackItNode2;

    /*
     * Allocate all the elements in the relevant lists.
     * This may throw an exception.
     */
    auto nit  = mNodes.insert(pos, unique_ptr<Node>(nullptr));
    auto eit1 = mEdges.insert(e.mBackIt, unique_ptr<Edge>(nullptr));
    auto eit2 = mEdges.insert(e.mBackIt, unique_ptr<Edge>(nullptr));

    edge_list_it_t placeholderIt;
    auto backItNew1 = n->mIncidence.insert(n->mIncidence.end(),placeholderIt);
    auto backItNew2 = n->mIncidence.insert(n->mIncidence.end(),placeholderIt);

    /*
     *  Add the new node, and the two new edges.
     */
    (*nit)                  = std::forward<node_ptr_t>(n);
    (*nit)->mBackIt         = nit;
    (*eit1)                 = std::forward<edge_ptr_t>(e1);
    (*eit1)->mBackIt        = eit1;
    (*eit2)                 = std::forward<edge_ptr_t>(e2);
    (*eit2)->mBackIt        = eit2;

    /*
     *  Relink the new node, and the two new edges.
     */
    (*eit1)->mIncidentNode1 = n1.mBackIt;
    (*eit1)->mIncidentNode2 = (*nit)->mBackIt;
    (*eit1)->mBackItNode1   = backItNode1;
    (*eit1)->mBackItNode2   = backItNew1;

    (*eit2)->mIncidentNode1 = (*nit)->mBackIt;
    (*eit2)->mIncidentNode2 = n2.mBackIt;
    (*eit2)->mBackItNode1   = backItNew2;
    (*eit2)->mBackItNode2   = backItNode2;

    *backItNode1 = eit1;
    *backItNode2 = eit2;

    *backItNew1  = eit1;
    *backItNew2  = eit2;

    (*nit)->mGraph  = this;
    (*eit1)->mGraph = this;
    (*eit2)->mGraph = this;

    /*
     * Remove e.
     */
    std::unique_ptr<Edge> pt(e.mBackIt->release());
    mEdges.erase(e.mBackIt);
    pt->mGraph = nullptr;
    removedEdge = std::move(pt);

    return std::tie(*(*nit),*(*eit1),*(*eit2));
}



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
 *              If an exception is thrown, all the data structures in the graph
 *              and the parameters don't change.
 */
edge_ptr_t Graph::contractEdge(Edge&e, node_ptr_t&removedNode)
{

    // Remove the edge.        
    Node& n1 = e.incidentNode1(); // Kept.
    Node& n2 = e.incidentNode2(); // Discarded.


    /** Save the incidence of the node to be removed.
     *  This may throw an exception.
     */
    auto incidenceSaved = n2.mIncidence;

    /** Insert elements to the lists.
     *  This may throw an exception.
     */
    edge_list_it_t placeholderIt;
    size_t index = 0;
    node_incidence_it_t n1AddBegin;

    try { 
        for (auto eit : incidenceSaved) {

            if (&e != &(*(*eit)) ) {

                Node* np1 = &(*(*((*eit)->mIncidentNode1)));

                if (np1 == &n2) {
                    (*eit)->mBackItNode1 =
                       n1.mIncidence.insert(
                                       n1.mIncidence.end(), placeholderIt);
                    if (index == 0) {
                        n1AddBegin = (*eit)->mBackItNode1;
                    }
                }
                else {
                    (*eit)->mBackItNode2 =
                       n1.mIncidence.insert(
                                       n1.mIncidence.end(), placeholderIt);
                    if (index == 0) {
                        n1AddBegin = (*eit)->mBackItNode2;
                    }
                }
                index++;
            }
        }
    }
    catch (exception& e) {

        // Rollingback.
        while (index > 0) {

            n1.mIncidence.pop_back();
            index--;

        }

        // Rethrow
        throw;
    }

    // Relink the node in each edge in the indence in n2.
    for (auto eit : incidenceSaved) {

        if (&e != &(*(*eit)) ) {

            Node* np1 = &(*(*((*eit)->mIncidentNode1)));
            (*n1AddBegin) = (*eit)->mBackIt;
            if (np1 == &n2) {

                (*eit)->mBackItNode1   = n1AddBegin;
                (*eit)->mIncidentNode1 = n1.mBackIt;

            }
            else {

                (*eit)->mBackItNode2   = n1AddBegin;
                (*eit)->mIncidentNode2 = n1.mBackIt;

            }
            n1AddBegin++;

        }
    }

    // Removed the edge to be contracted.
    n1.mIncidence.erase(e.mBackItNode1);
    std::unique_ptr<Edge> removedEdge(e.mBackIt->release());
    mEdges.erase(e.mBackIt);
    removedEdge->mGraph = nullptr;

    // Erase the stale incidence in n2.
    n2.mIncidence.clear();

    // Remove n2.
    //removedNode = std::move(removeNode(n2));
    removedNode = removeNode(n2);

    return removedEdge; // rvo
}


/** @brief  contracts the specified edge. Of the two adjacent nodes, node2
 *          is removed, and all the incident edges to node 2 except for the 
 *          one conracted are relinked to node 1.
 *          The ownership of the specified edge is transferred to the caller.
 *          The Node object of node 1 is deleted.
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
 *              If an exception is thrown, all the data structures in the graph
 *              and the parameters don't change.
 */
edge_ptr_t Graph::contractEdge(Edge&e)
{
    node_ptr_t removedNode;
    return contractEdge(e, removedNode);
    // At this point removeNode is destructed together with the Node objects.
}


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
 *              If an exception is thrown, all the data structures in the graph
 *              and the parameters don't change.
 */
Node& Graph::addNode(node_ptr_t&& n)
{
    return addNode(std::forward<node_ptr_t>(n),mNodes.end());
}

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
 *              If an exception is thrown, all the data structures in the graph
 *              and the parameters don't change.
 *
 *  @remark undefined behavior:
 *              If an invalid value is given to pos,
 *              it causes undefined behavior because the
 *              underlying list<>::insert behaves that way.
 */
Node& Graph::addNode(node_ptr_t&& n, const node_list_it_t& pos)
{
    if (n->mGraph != nullptr) {
        throw std::invalid_argument(Constants::kExceptionNodeAlreadyInGrpah);
    }

    auto it  = mNodes.insert(pos,unique_ptr<Node>(nullptr));

    (*it)          = std::forward<node_ptr_t>(n);
    (*it)->mBackIt = it;
    (*it)->mGraph  = this;

    return *(*it);
}


/** @brief  removes the specified node from the graph together with all
 *          the incident edges. The ownerships of the node and the edges
 *          are transferred to the caller.
 *
 *  @param  n            (in):  node to be removed.
 *  @param  removedEdges (out): list where edges to be removed will be stored
 *                              together with their ownerships.
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
 *              If an exception is thrown, all the data structures in the graph
 *              and the parameters don't change.
 */
node_ptr_t Graph::removeNode(Node& n, std::vector<edge_ptr_t>& removedEdges)
{
    if (n.mGraph != this) {
        throw std::invalid_argument(Constants::kExceptionNodeNotInGraph);
    }
    // Allocate objects and elements to removedEdges.
    // This loop can throw bad_alloc.
    for (size_t i = 0; i < n.mIncidence.size(); i++) {
        removedEdges.push_back(std::unique_ptr<Edge>(nullptr));
    }
    // Remove all the incident edges.
    size_t index = 0;
    try {
        /** Need a copy of incidence as n.mIncidence is being destructed 
         *  during the loop.
         */
        auto mIncidenceCopy = n.mIncidence;
        for (auto e : mIncidenceCopy) {
            removedEdges[index++] = removeEdge(*(*e));
        }

    }
    catch (exception& e) {
        removedEdges.clear();
        throw;

    }
    n.mIncidence.clear();
    node_ptr_t pt(n.mBackIt->release());
    mNodes.erase(n.mBackIt);
    pt->mGraph = nullptr;
    return pt; // rvo
}


/** @brief  removes the specified node from the graph together with all
 *          the incident edges. The ownership of the node is transferred
 *          to the owner. The removed edges in Edge objects will be deleted.
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
 *              If an exception is thrown, all the data structures in the graph
 *              and the parameters don't change.
 */
node_ptr_t Graph::removeNode(Node& n)
{
    if (n.mGraph != this) {
        throw std::invalid_argument(Constants::kExceptionNodeNotInGraph);
    }
    /** Need a copy of incidence as n.mIncidence is being destructed 
     *  during the loop.
     */
    auto mIncidenceCopy = n.mIncidence;
    for (auto e : mIncidenceCopy) {
        auto ep = removeEdge(*(*e));
        // The Edge object pointed to by ep is released here.
    }
    n.mIncidence.clear();
    node_ptr_t pt(n.mBackIt->release());
    mNodes.erase(n.mBackIt);
    pt->mGraph = nullptr;
    return pt; // rvo
}


/** @brief  adds the specified edge to the graph between two specified nodes,
 *          and transfers the ownership of the edge from the caller to the
 *          graph.
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
Edge& Graph::addEdge(
    edge_ptr_t&&               e,
    Node&                      n1,
    Node&                      n2,
    const node_incidence_it_t& posInNode1,
    const node_incidence_it_t& posInNode2,
    const edge_list_it_t&      posInGraph
)
{
    if (e->mGraph != nullptr) {
        throw std::invalid_argument(Constants::kExceptionEdgeAlreadyInGrpah);
    }
    // Allocate elements to the three lists first
    // They can throw an exception.
    auto it   = mEdges.insert(posInGraph, std::unique_ptr<Edge>(nullptr));
    edge_list_it_t placeholderIt;
    auto nit1 = n1.mIncidence.insert(posInNode1, placeholderIt);
    auto nit2 = n2.mIncidence.insert(posInNode2, placeholderIt);
    *it = std::forward<edge_ptr_t>(e);
    (*it)->mBackIt        = it;
    (*it)->mIncidentNode1 = n1.mBackIt;
    (*it)->mIncidentNode2 = n2.mBackIt;
    (*it)->mGraph         = this;
    (*nit1)               = (*it)->mBackIt;
    (*nit2)               = (*it)->mBackIt;
    (*it)->mBackItNode1   = nit1;
    (*it)->mBackItNode2   = nit2;
    return *(*it);
}


/** @brief  adds the specified edge to the graph between two specified nodes,
 *          and transfers the ownership of the edge from the caller to the
 *          graph.
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
Edge& Graph::addEdge(
    edge_ptr_t&&               e,
    Node&                      n1,
    Node&                      n2,
    const node_incidence_it_t& posInNode1,
    const node_incidence_it_t& posInNode2
)
{
    return addEdge(std::forward<edge_ptr_t>(e), n1, n2,
                      posInNode1, posInNode2, mEdges.end());
}


/** @brief  adds the specified edge to the graph between two specified nodes,
 *          and transfers the ownership of the edge from the caller to the
 *          graph.
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
 *              if an insersion of list<> has failed due to memory shortage.
 *
 *  @details e is inserted at the end of mEdges edge list in the graph.
 *          e is inserted at the end of mIncidence list in each of the node.
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
Edge& Graph::addEdge(edge_ptr_t&& e, Node& n1, Node& n2)
{
    return addEdge(std::forward<edge_ptr_t>(e), n1, n2,
                     n1.mIncidence.end(), n2.mIncidence.end(), mEdges.end());
}


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
edge_ptr_t Graph::removeEdge(Edge& e)
{
    if (e.mGraph != this) {
        throw std::invalid_argument(Constants::kExceptionEdgeNotInGraph);
    }
    Node& n1 = *(*e.mIncidentNode1);
    n1.mIncidence.erase(e.mBackItNode1);
    Node& n2 = *(*e.mIncidentNode2);
    n2.mIncidence.erase(e.mBackItNode2);
    std::unique_ptr<Edge> pt(e.mBackIt->release());
    mEdges.erase(e.mBackIt);
    pt->mGraph = nullptr;

    return pt; // rvo
}

/** @brief  resets the general purpose counters of this graph and
 *          its owning Nodes and Edges to zero.
 */
void Graph::resetGeneration() noexcept
{
    mGeneration = 0;

    for (auto& npt : mNodes) {
        npt->mGeneration = 0;
    }

    for (auto& ept : mEdges) {
        ept->mGeneration = 0;
    }
}


/** @brief finds (multi)edges between two given nodes.
 *
 *  @param   n1      (in):incident node 1.
 *  @param   n2      (in):incident node 2.
 *
 *  @return  edges found.
 */
vector<edge_list_it_t> Graph::findMultiEdges(Node& n1, Node& n2)
{

    vector<edge_list_it_t> edgesFound;
    if (n1.degree() < n2.degree()) {
        for (auto eit : n1.mIncidence) {

            if (&(*eit)->adjacentNode(n1) == &n2) {

                edgesFound.push_back(eit);

            }
        }
    }
    else {
        for (auto eit : n2.mIncidence) {

            if (&(*eit)->adjacentNode(n2) == &n1) {

                edgesFound.push_back(eit);

            }

        }

    }
    return  edgesFound;
}



}// namespace Undirected

}// namespace Wailea
