import Foundation


// This is an attempt to implement a graph embedding in Swift.
// Swift is not an ideal language to implement such complex data structures
// (probabl worse than Python) where one element belongs to multiple linked lists.
// It also lacks a proper Mixin support.
//
// In a usual implementation for linked list, the list is implemented as a Generic for type T,
// where T is the type of the elements linked. We could extend this approach to implement multiple
// link structure for one element by for example Mixin such as List<List<T>>, but this approach makes
// it difficult to explore/traverse the graph embedding over multiple lists, as the organization is
// by composition, not inheritance. Also, proper Mixin is not supported on Swift yet.n

// Under such circumstances, it seems to be best to utilize inheritance. Here's the basic strategy.

// Single strong reference per element. Every other reference is weak.
// The strong references are:
// - `OwnerList.head` to represent the head element of the doubly linked list
// - `ListElem.ownwerChainNext` to represent the next element along the doubly linked list.

// To enable one Element to belong to multiple lists, ListElem forms the following inheritance hierarchy.
//     ListElem -> ListElemWithWeakChain1 -> ListElemWithWeakChain2 -> ...
// Each implements the next/prev and parent references.

// The corresponding Lists are implemented as follows. They are not related to each other.
//     OwnerList, WeakList1, WeakList2,  WeakList3, ...

public class ListElem {

    // For the owner list
    public      var ownerChainNext: ListElem?
    public weak var ownerChainPrev: ListElem?
    
    // Reference back to the owner list
    public weak var ownerList: OwnerList?

    public func removeFromOwnerList() {

        if ownerChainNext == nil {

            if ownerChainPrev == nil {

                // the only ListElem in the list
                ownerList?.head = nil
                ownerList?.tail = nil
            }
            else {
                // at the end of the list
                ownerChainPrev?.ownerChainNext = nil
                ownerList?.tail = ownerChainPrev
                ownerChainPrev  = nil
            }
        } else {
            if ownerChainPrev == nil {

                // at the beginning of the list
                ownerChainNext?.ownerChainPrev = nil
                ownerList?.head = ownerChainNext
                ownerChainNext = nil
            }
            else {
                // at the end of the list
                ownerChainPrev?.ownerChainNext = ownerChainNext
                ownerChainNext?.ownerChainPrev = ownerChainPrev
                ownerChainPrev = nil
                ownerChainNext = nil
            }
        }
        ownerList = nil
    }
}

public class OwnerList : Sequence {

    public      var head: ListElem?
    public weak var tail: ListElem?

    public func insert( _ e: ListElem, before elemNext: ListElem? ) {

        e.ownerList = self

        if let eNext = elemNext {

            if let ePrev = eNext.ownerChainPrev {

                ePrev.ownerChainNext = e
                e.ownerChainPrev     = ePrev
                e.ownerChainNext     = eNext
                eNext.ownerChainPrev = e

            } else {
                // eNext is at the head of the list
                head                 = e
                e.ownerChainNext     = eNext
                eNext.ownerChainPrev = e
            }

        } else { // elemNext == nil. Put e at the end of the list.

            let tailSaved             = tail
            tail                      = e
            e.ownerChainPrev          = tailSaved
            tailSaved?.ownerChainNext = e

            if head == nil {
                head = e
            }
        }
    }
    
    public func makeIterator() -> OwnerListIterator {
        return OwnerListIterator(self)
    }
}

public struct OwnerListIterator : IteratorProtocol {
 
    let list     : OwnerList
    var nextElem : ListElem?

    init( _ list: OwnerList ) {
        self.list     = list
        self.nextElem = list.head
    }

    public mutating func next() -> ListElem? {

        guard nextElem != nil else { return nil }
        let cur = nextElem
        nextElem = cur!.ownerChainNext
        return cur
    }
}


public class ListElemWithWeakChain1 : ListElem {

    // For the weak list
    public weak var weakChainNext1: ListElemWithWeakChain1?
    public weak var weakChainPrev1: ListElemWithWeakChain1?
    
    // Reference back to the weak list
    public weak var weakList1: WeakList1?

    public func removeFromWeakList1() {

        if weakChainNext1 == nil {

            if weakChainPrev1 == nil {

                // the only ListElemWithWeakChain in the list
                weakList1?.head = nil
                weakList1?.tail = nil
            }
            else {
                // at the end of the list
                weakChainPrev1?.weakChainNext1 = nil
                weakList1?.tail = weakChainPrev1
                weakChainPrev1  = nil
            }
        } else {
            if weakChainPrev1 == nil {

                // at the beginning of the list
                weakChainNext1?.weakChainPrev1 = nil
                weakList1?.head = weakChainNext1
                weakChainNext1 = nil
            }
            else {
                // at the end of the list
                weakChainPrev1?.weakChainNext1 = weakChainNext1
                weakChainNext1?.weakChainPrev1 = weakChainPrev1
                weakChainPrev1 = nil
                weakChainNext1 = nil
            }
        }
        weakList1 = nil
    }
}


public class WeakList1 : Sequence {

    public weak var head: ListElemWithWeakChain1?
    public weak var tail: ListElemWithWeakChain1?

    public func insert( _ e: ListElemWithWeakChain1, before elemNext: ListElemWithWeakChain1? ) {

        e.weakList1 = self

        if let eNext = elemNext {

            if let ePrev = eNext.weakChainPrev1 {

                ePrev.weakChainNext1 = e
                e.weakChainPrev1     = ePrev
                e.weakChainNext1     = eNext
                eNext.weakChainPrev1 = e

            } else {

                head                 = e
                e.weakChainNext1     = eNext
                eNext.weakChainPrev1 = e
            }

        } else { // elemNext == nil. Put e at the end of the list.

            let tailSaved             = tail
            tail                      = e
            e.weakChainPrev1          = tailSaved
            tailSaved?.weakChainNext1 = e
            
            if head == nil {
                head = e
            }
        }
    }
 
    public func makeIterator() -> WeakListIterator1 {
        return WeakListIterator1(self)
    }
 
}

public struct WeakListIterator1 : IteratorProtocol {
 
    let list     : WeakList1
    var nextElem : ListElemWithWeakChain1?

    init( _ list: WeakList1 ) {
        self.list     = list
        self.nextElem = list.head
    }

    public mutating func next() -> ListElemWithWeakChain1? {

        guard nextElem != nil else { return nil }
        let cur = nextElem
        nextElem = cur!.weakChainNext1
        return cur
    }
}


public class ListElemWithWeakChain2 : ListElemWithWeakChain1 {

    // For the weak list
    public weak var weakChainNext2: ListElemWithWeakChain2?
    public weak var weakChainPrev2: ListElemWithWeakChain2?
    
    // Reference back to the weak list
    public weak var weakList2: WeakList2?

    public func removeFromWeakList2() {

        if weakChainNext2 == nil {

            if weakChainPrev2 == nil {

                // the only ListElemWithWeakChain2 in the list
                weakList2?.head = nil
                weakList2?.tail = nil
            }
            else {
                // at the end of the list
                weakChainPrev2?.weakChainNext2 = nil
                weakList2?.tail = weakChainPrev2
                weakChainPrev2  = nil
            }
        } else {
            if weakChainPrev2 == nil {

                // at the beginning of the list
                weakChainNext2?.weakChainPrev2 = nil
                weakList2?.head = weakChainNext2
                weakChainNext2 = nil
            }
            else {
                // at the end of the list
                weakChainPrev2?.weakChainNext2 = weakChainNext2
                weakChainNext2?.weakChainPrev2 = weakChainPrev2
                weakChainPrev2 = nil
                weakChainNext2 = nil
            }
        }
        weakList2 = nil
    }
}


public class WeakList2 : Sequence {

    public weak var head: ListElemWithWeakChain2?
    public weak var tail: ListElemWithWeakChain2?

    public func insert( _ e: ListElemWithWeakChain2, before elemNext: ListElemWithWeakChain2? ) {

        e.weakList2 = self

        if let eNext = elemNext {

            if let ePrev = eNext.weakChainPrev2 {

                ePrev.weakChainNext2 = e
                e.weakChainPrev2     = ePrev
                e.weakChainNext2     = eNext
                eNext.weakChainPrev2 = e

            } else {

                head                 = e
                e.weakChainNext2     = eNext
                eNext.weakChainPrev2 = e
            }

        } else { // elemNext == nil. Put e at the end of the list.

            let tailSaved             = tail
            tail                      = e
            e.weakChainPrev2          = tailSaved
            tailSaved?.weakChainNext2 = e
            
            if head == nil {
                head = e
            }
        }
    }
 
    public func makeIterator() -> WeakListIterator2 {
        return WeakListIterator2(self)
    }
 
}

public struct WeakListIterator2 : IteratorProtocol {
 
    let list     : WeakList2
    var nextElem : ListElemWithWeakChain2?

    init( _ list: WeakList2 ) {
        self.list     = list
        self.nextElem = list.head
    }

    public mutating func next() -> ListElemWithWeakChain2? {

        guard nextElem != nil else { return nil }
        let cur = nextElem
        nextElem = cur!.weakChainNext2
        return cur
    }
}
