import XCTest

class MultiLinksTests: XCTestCase {

    func testOwnerListEmpty() throws {
    
        let ol_01 = OwnerList()

        XCTAssertNil(ol_01.head, "OwnerList.head is wrong")
        XCTAssertNil(ol_01.tail, "OwnerList.tail is wrong")
    }

    func testOwnerListAddOneElement() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElem()

        ol_01.insert(le_01, before: nil)

        XCTAssertIdentical(ol_01.head, le_01, "OwnerList.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_01, "OwnerList.tail is wrong")
        XCTAssertIdentical(le_01.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertNil(le_01.ownerChainNext, "Element's next is wrong")
        XCTAssertNil(le_01.ownerChainPrev, "Element's prev is wrong")
        
    }

    func testOwnerListAddOneElementThenRemoveElement() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElem()

        ol_01.insert(le_01, before: nil)
        le_01.removeFromOwnerList()

        XCTAssertNil(ol_01.head, "OwnerList.head is wrong")
        XCTAssertNil(ol_01.tail, "OwnerList.tail is wrong")
        XCTAssertNil(le_01.ownerList, "Element's ownerList is wrong")
        XCTAssertNil(le_01.ownerChainNext, "Element's next is wrong")
        XCTAssertNil(le_01.ownerChainPrev, "Element's prev is wrong")
        
    }

    func testOwnerListAddTwoElementsCase1() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElem()
        let le_02 = ListElem()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)

        XCTAssertIdentical(ol_01.head, le_01, "OwnerList.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_02, "OwnerList.tail is wrong")

        XCTAssertIdentical(le_01.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertIdentical(le_01.ownerChainNext, le_02, "Element's next is wrong")
        XCTAssertNil(le_01.ownerChainPrev, "Element's prev is wrong")

        XCTAssertIdentical(le_02.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertNil(le_02.ownerChainNext, "Element's next is wrong")
        XCTAssertIdentical(le_02.ownerChainPrev, le_01, "Element's prev is wrong")
    }

    func testOwnerListAddTwoElementsCase2() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElem()
        let le_02 = ListElem()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: le_01)

        XCTAssertIdentical(ol_01.head, le_02, "OwnerList.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_01, "OwnerList.tail is wrong")

        XCTAssertIdentical(le_01.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertNil(le_01.ownerChainNext, "Element's next is wrong")
        XCTAssertIdentical(le_01.ownerChainPrev, le_02, "Element's prev is wrong")

        XCTAssertIdentical(le_02.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertIdentical(le_02.ownerChainNext, le_01, "Element's next is wrong")
        XCTAssertNil(le_02.ownerChainPrev, "Element's prev is wrong")
    }

    func testOwnerListWithTwoElementsRemoveFirst() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElem()
        let le_02 = ListElem()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        le_01.removeFromOwnerList()

        XCTAssertNil(le_01.ownerList, "Element's ownerList is wrong")
        XCTAssertNil(le_01.ownerChainNext, "Element's prev is wrong")
        XCTAssertNil(le_01.ownerChainPrev, "Element's prev is wrong")

        XCTAssertIdentical(le_02.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertNil(le_02.ownerChainNext, "Element's prev is wrong")
        XCTAssertNil(le_02.ownerChainPrev, "Element's prev is wrong")
    }

    func testOwnerListWithTwoElementsRemoveSecond() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElem()
        let le_02 = ListElem()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        le_02.removeFromOwnerList()

        XCTAssertIdentical(le_01.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertNil(le_01.ownerChainNext, "Element's prev is wrong")
        XCTAssertNil(le_01.ownerChainPrev, "Element's prev is wrong")

        XCTAssertNil(le_02.ownerList, "Element's ownerList is wrong")
        XCTAssertNil(le_02.ownerChainNext, "Element's prev is wrong")
        XCTAssertNil(le_02.ownerChainPrev, "Element's prev is wrong")
    }

    func testOwnerListAddThreeElementsCase1() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElem()
        let le_02 = ListElem()
        let le_03 = ListElem()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: nil)

        XCTAssertIdentical(ol_01.head, le_01, "OwnerList.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_03, "OwnerList.tail is wrong")

        XCTAssertIdentical(le_01.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertIdentical(le_01.ownerChainNext, le_02, "Element's next is wrong")
        XCTAssertNil(le_01.ownerChainPrev, "Element's prev is wrong")

        XCTAssertIdentical(le_02.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertIdentical(le_02.ownerChainNext, le_03, "Element's prev is wrong")
        XCTAssertIdentical(le_02.ownerChainPrev, le_01, "Element's prev is wrong")

        XCTAssertIdentical(le_03.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertNil(le_03.ownerChainNext, "Element's next is wrong")
        XCTAssertIdentical(le_03.ownerChainPrev, le_02, "Element's prev is wrong")
    }

    func testOwnerListAddThreeElementsCase2() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElem()
        let le_02 = ListElem()
        let le_03 = ListElem()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: le_01)
        ol_01.insert(le_03, before: le_02)

        XCTAssertIdentical(ol_01.head, le_03, "OwnerList.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_01, "OwnerList.tail is wrong")

        XCTAssertIdentical(le_01.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertNil(le_01.ownerChainNext, "Element's next is wrong")
        XCTAssertIdentical(le_01.ownerChainPrev, le_02, "Element's prev is wrong")

        XCTAssertIdentical(le_02.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertIdentical(le_02.ownerChainNext, le_01, "Element's prev is wrong")
        XCTAssertIdentical(le_02.ownerChainPrev, le_03, "Element's prev is wrong")

        XCTAssertIdentical(le_03.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertIdentical(le_03.ownerChainNext, le_02, "Element's next is wrong")
        XCTAssertNil(le_03.ownerChainPrev, "Element's prev is wrong")
    }
    
    func testOwnerListAddThreeElementsCase3() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElem()
        let le_02 = ListElem()
        let le_03 = ListElem()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: le_01)
        ol_01.insert(le_03, before: nil)

        XCTAssertIdentical(ol_01.head, le_02, "OwnerList.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_03, "OwnerList.tail is wrong")

        XCTAssertIdentical(le_02.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertIdentical(le_02.ownerChainNext, le_01, "Element's next is wrong")
        XCTAssertNil(le_02.ownerChainPrev, "Element's prev is wrong")

        XCTAssertIdentical(le_01.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertIdentical(le_01.ownerChainNext, le_03, "Element's prev is wrong")
        XCTAssertIdentical(le_01.ownerChainPrev, le_02, "Element's prev is wrong")

        XCTAssertIdentical(le_03.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertNil(le_03.ownerChainNext, "Element's next is wrong")
        XCTAssertIdentical(le_03.ownerChainPrev, le_01, "Element's prev is wrong")
    }

    func testOwnerListAddThreeElementsCase4() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElem()
        let le_02 = ListElem()
        let le_03 = ListElem()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: le_01)

        XCTAssertIdentical(ol_01.head, le_03, "OwnerList.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_02, "OwnerList.tail is wrong")

        XCTAssertIdentical(le_03.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertIdentical(le_03.ownerChainNext, le_01, "Element's next is wrong")
        XCTAssertNil(le_03.ownerChainPrev, "Element's prev is wrong")

        XCTAssertIdentical(le_01.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertIdentical(le_01.ownerChainNext, le_02, "Element's prev is wrong")
        XCTAssertIdentical(le_01.ownerChainPrev, le_03, "Element's prev is wrong")

        XCTAssertIdentical(le_02.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertNil(le_02.ownerChainNext, "Element's next is wrong")
        XCTAssertIdentical(le_02.ownerChainPrev, le_01, "Element's prev is wrong")
    }

    func testOwnerListAddThreeElementsCase5() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElem()
        let le_02 = ListElem()
        let le_03 = ListElem()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: le_02)

        XCTAssertIdentical(ol_01.head, le_01, "OwnerList.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_02, "OwnerList.tail is wrong")

        XCTAssertIdentical(le_01.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertIdentical(le_01.ownerChainNext, le_03, "Element's next is wrong")
        XCTAssertNil(le_01.ownerChainPrev, "Element's prev is wrong")

        XCTAssertIdentical(le_03.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertIdentical(le_03.ownerChainNext, le_02, "Element's prev is wrong")
        XCTAssertIdentical(le_03.ownerChainPrev, le_01, "Element's prev is wrong")

        XCTAssertIdentical(le_02.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertNil(le_02.ownerChainNext, "Element's next is wrong")
        XCTAssertIdentical(le_02.ownerChainPrev, le_03, "Element's prev is wrong")
    }

    func testOwnerListThreeElementsRemove1st() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElem()
        let le_02 = ListElem()
        let le_03 = ListElem()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: nil)
        le_01.removeFromOwnerList()

        XCTAssertIdentical(ol_01.head, le_02, "OwnerList.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_03, "OwnerList.tail is wrong")

        XCTAssertNil(le_01.ownerList, "Element's ownerList is wrong")
        XCTAssertNil(le_01.ownerChainNext, "Element's prev is wrong")
        XCTAssertNil(le_01.ownerChainPrev, "Element's prev is wrong")

        XCTAssertIdentical(le_02.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertIdentical(le_02.ownerChainNext, le_03, "Element's prev is wrong")
        XCTAssertNil(le_02.ownerChainPrev, "Element's prev is wrong")

        XCTAssertIdentical(le_03.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertNil(le_03.ownerChainNext, "Element's next is wrong")
        XCTAssertIdentical(le_03.ownerChainPrev, le_02, "Element's prev is wrong")
    }

    func testOwnerListThreeElementsRemove2nd() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElem()
        let le_02 = ListElem()
        let le_03 = ListElem()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: nil)
        le_02.removeFromOwnerList()

        XCTAssertIdentical(ol_01.head, le_01, "OwnerList.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_03, "OwnerList.tail is wrong")

        XCTAssertNil(le_02.ownerList, "Element's ownerList is wrong")
        XCTAssertNil(le_02.ownerChainNext, "Element's prev is wrong")
        XCTAssertNil(le_02.ownerChainPrev, "Element's prev is wrong")

        XCTAssertIdentical(le_01.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertIdentical(le_01.ownerChainNext, le_03, "Element's prev is wrong")
        XCTAssertNil(le_01.ownerChainPrev, "Element's prev is wrong")

        XCTAssertIdentical(le_03.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertNil(le_03.ownerChainNext, "Element's next is wrong")
        XCTAssertIdentical(le_03.ownerChainPrev, le_01, "Element's prev is wrong")
    }

    func testOwnerListThreeElementsRemove3rd() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElem()
        let le_02 = ListElem()
        let le_03 = ListElem()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: nil)
        le_03.removeFromOwnerList()

        XCTAssertIdentical(ol_01.head, le_01, "OwnerList.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_02, "OwnerList.tail is wrong")

        XCTAssertNil(le_03.ownerList, "Element's ownerList is wrong")
        XCTAssertNil(le_03.ownerChainNext, "Element's prev is wrong")
        XCTAssertNil(le_03.ownerChainPrev, "Element's prev is wrong")

        XCTAssertIdentical(le_01.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertIdentical(le_01.ownerChainNext, le_02, "Element's prev is wrong")
        XCTAssertNil(le_01.ownerChainPrev, "Element's prev is wrong")

        XCTAssertIdentical(le_02.ownerList, ol_01, "Element's ownerList is wrong")
        XCTAssertNil(le_02.ownerChainNext, "Element's next is wrong")
        XCTAssertIdentical(le_02.ownerChainPrev, le_01, "Element's prev is wrong")
    }

    func testOwnerListIteratorEmptyList() throws {
    
        let ol_01 = OwnerList()
        var it_01 = ol_01.makeIterator()
        
        XCTAssertNil(it_01.next(), "Iterator's next() is wrong")
    }

    func testOwnerListIteratorOneElement() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElem()
        ol_01.insert(le_01, before: nil)

        var it_01 = ol_01.makeIterator()
        
        XCTAssertIdentical(it_01.next(), le_01, "Iterator's next() is wrong")
        XCTAssertNil(it_01.next(), "Iterator's next() is wrong")
    }

    func testOwnerListIteratorTwoElements() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElem()
        let le_02 = ListElem()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)

        var it_01 = ol_01.makeIterator()
        
        XCTAssertIdentical(it_01.next(), le_01, "Iterator's next() is wrong")
        XCTAssertIdentical(it_01.next(), le_02, "Iterator's next() is wrong")
        XCTAssertNil(it_01.next(), "Iterator's next() is wrong")
    }

    func testOwnerListIteratorThreeElements() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElem()
        let le_02 = ListElem()
        let le_03 = ListElem()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: nil)

        var it_01 = ol_01.makeIterator()
        
        XCTAssertIdentical(it_01.next(), le_01, "Iterator's next() is wrong")
        XCTAssertIdentical(it_01.next(), le_02, "Iterator's next() is wrong")
        XCTAssertIdentical(it_01.next(), le_03, "Iterator's next() is wrong")
        XCTAssertNil(it_01.next(), "Iterator's next() is wrong")
    }

    func testOwnerListIteratorFourElements() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElem()
        let le_02 = ListElem()
        let le_03 = ListElem()
        let le_04 = ListElem()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: nil)
        ol_01.insert(le_04, before: nil)

        var it_01 = ol_01.makeIterator()
        
        XCTAssertIdentical(it_01.next(), le_01, "Iterator's next() is wrong")
        XCTAssertIdentical(it_01.next(), le_02, "Iterator's next() is wrong")
        XCTAssertIdentical(it_01.next(), le_03, "Iterator's next() is wrong")
        XCTAssertIdentical(it_01.next(), le_04, "Iterator's next() is wrong")
        XCTAssertNil(it_01.next(), "Iterator's next() is wrong")
    }


    func testWeakList1Empty() throws {
    
        let wl_01 = WeakList1()

        XCTAssertNil(wl_01.head, "WeakList1.head is wrong")
        XCTAssertNil(wl_01.tail, "WeakList1.tail is wrong")
    }

    func testWeakList1AddOneElement() throws {
    
        let wl_01 = WeakList1()
        let le_01 = ListElemWithWeakChain1()
        wl_01.insert(le_01, before: nil)

        XCTAssertIdentical(wl_01.head, le_01, "WeakList1.head is wrong")
        XCTAssertIdentical(wl_01.tail, le_01, "WeakList1.tail is wrong")
        XCTAssertIdentical(le_01.weakList1, wl_01, "Element's WeakList1 is wrong")
        XCTAssertNil(le_01.weakChainNext1, "Element's next is wrong")
        XCTAssertNil(le_01.weakChainPrev1, "Element's prev is wrong")
    }
    
    func testWeakList1AddOneElementThenRemoveElement() throws {
    
        let ol_01 = WeakList1()
        let le_01 = ListElemWithWeakChain1()

        ol_01.insert(le_01, before: nil)
        le_01.removeFromWeakList1()

        XCTAssertNil(ol_01.head, "WeakList1.head is wrong")
        XCTAssertNil(ol_01.tail, "WeakList1.tail is wrong")
        XCTAssertNil(le_01.weakList1, "Element's WeakList1 is wrong")
        XCTAssertNil(le_01.weakChainNext1, "Element's next is wrong")
        XCTAssertNil(le_01.weakChainPrev1, "Element's prev is wrong")
        
    }

    func testWeakList1AddTwoElementsCase1() throws {
    
        let ol_01 = WeakList1()
        let le_01 = ListElemWithWeakChain1()
        let le_02 = ListElemWithWeakChain1()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)

        XCTAssertIdentical(ol_01.head, le_01, "WeakList1.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_02, "WeakList1.tail is wrong")

        XCTAssertIdentical(le_01.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertIdentical(le_01.weakChainNext1, le_02, "Element's next is wrong")
        XCTAssertNil(le_01.weakChainPrev1, "Element's prev is wrong")

        XCTAssertIdentical(le_02.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertNil(le_02.weakChainNext1, "Element's next is wrong")
        XCTAssertIdentical(le_02.weakChainPrev1, le_01, "Element's prev is wrong")
    }

    func testWeakList1AddTwoElementsCase2() throws {
    
        let ol_01 = WeakList1()
        let le_01 = ListElemWithWeakChain1()
        let le_02 = ListElemWithWeakChain1()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: le_01)

        XCTAssertIdentical(ol_01.head, le_02, "WeakList1.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_01, "WeakList1.tail is wrong")

        XCTAssertIdentical(le_01.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertNil(le_01.weakChainNext1, "Element's next is wrong")
        XCTAssertIdentical(le_01.weakChainPrev1, le_02, "Element's prev is wrong")

        XCTAssertIdentical(le_02.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertIdentical(le_02.weakChainNext1, le_01, "Element's next is wrong")
        XCTAssertNil(le_02.weakChainPrev1, "Element's prev is wrong")
    }

    func testWeakList1WithTwoElementsRemoveFirst() throws {
    
        let ol_01 = WeakList1()
        let le_01 = ListElemWithWeakChain1()
        let le_02 = ListElemWithWeakChain1()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        le_01.removeFromWeakList1()

        XCTAssertNil(le_01.weakList1, "Element's WeakList1 is wrong")
        XCTAssertNil(le_01.weakChainNext1, "Element's prev is wrong")
        XCTAssertNil(le_01.weakChainPrev1, "Element's prev is wrong")

        XCTAssertIdentical(le_02.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertNil(le_02.weakChainNext1, "Element's prev is wrong")
        XCTAssertNil(le_02.weakChainPrev1, "Element's prev is wrong")
    }

    func testWeakList1WithTwoElementsRemoveSecond() throws {
    
        let ol_01 = WeakList1()
        let le_01 = ListElemWithWeakChain1()
        let le_02 = ListElemWithWeakChain1()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        le_02.removeFromWeakList1()

        XCTAssertIdentical(le_01.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertNil(le_01.weakChainNext1, "Element's prev is wrong")
        XCTAssertNil(le_01.weakChainPrev1, "Element's prev is wrong")

        XCTAssertNil(le_02.weakList1, "Element's WeakList1 is wrong")
        XCTAssertNil(le_02.weakChainNext1, "Element's prev is wrong")
        XCTAssertNil(le_02.weakChainPrev1, "Element's prev is wrong")
    }

    func testWeakList1AddThreeElementsCase1() throws {
    
        let ol_01 = WeakList1()
        let le_01 = ListElemWithWeakChain1()
        let le_02 = ListElemWithWeakChain1()
        let le_03 = ListElemWithWeakChain1()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: nil)

        XCTAssertIdentical(ol_01.head, le_01, "WeakList1.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_03, "WeakList1.tail is wrong")

        XCTAssertIdentical(le_01.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertIdentical(le_01.weakChainNext1, le_02, "Element's next is wrong")
        XCTAssertNil(le_01.weakChainPrev1, "Element's prev is wrong")

        XCTAssertIdentical(le_02.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertIdentical(le_02.weakChainNext1, le_03, "Element's prev is wrong")
        XCTAssertIdentical(le_02.weakChainPrev1, le_01, "Element's prev is wrong")

        XCTAssertIdentical(le_03.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertNil(le_03.weakChainNext1, "Element's next is wrong")
        XCTAssertIdentical(le_03.weakChainPrev1, le_02, "Element's prev is wrong")
    }

    func testWeakList1AddThreeElementsCase2() throws {
    
        let ol_01 = WeakList1()
        let le_01 = ListElemWithWeakChain1()
        let le_02 = ListElemWithWeakChain1()
        let le_03 = ListElemWithWeakChain1()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: le_01)
        ol_01.insert(le_03, before: le_02)

        XCTAssertIdentical(ol_01.head, le_03, "WeakList1.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_01, "WeakList1.tail is wrong")

        XCTAssertIdentical(le_01.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertNil(le_01.weakChainNext1, "Element's next is wrong")
        XCTAssertIdentical(le_01.weakChainPrev1, le_02, "Element's prev is wrong")

        XCTAssertIdentical(le_02.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertIdentical(le_02.weakChainNext1, le_01, "Element's prev is wrong")
        XCTAssertIdentical(le_02.weakChainPrev1, le_03, "Element's prev is wrong")

        XCTAssertIdentical(le_03.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertIdentical(le_03.weakChainNext1, le_02, "Element's next is wrong")
        XCTAssertNil(le_03.weakChainPrev1, "Element's prev is wrong")
    }
    
    func testWeakList1AddThreeElementsCase3() throws {
    
        let ol_01 = WeakList1()
        let le_01 = ListElemWithWeakChain1()
        let le_02 = ListElemWithWeakChain1()
        let le_03 = ListElemWithWeakChain1()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: le_01)
        ol_01.insert(le_03, before: nil)

        XCTAssertIdentical(ol_01.head, le_02, "WeakList1.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_03, "WeakList1.tail is wrong")

        XCTAssertIdentical(le_02.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertIdentical(le_02.weakChainNext1, le_01, "Element's next is wrong")
        XCTAssertNil(le_02.weakChainPrev1, "Element's prev is wrong")

        XCTAssertIdentical(le_01.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertIdentical(le_01.weakChainNext1, le_03, "Element's prev is wrong")
        XCTAssertIdentical(le_01.weakChainPrev1, le_02, "Element's prev is wrong")

        XCTAssertIdentical(le_03.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertNil(le_03.weakChainNext1, "Element's next is wrong")
        XCTAssertIdentical(le_03.weakChainPrev1, le_01, "Element's prev is wrong")
    }

    func testWeakList1AddThreeElementsCase4() throws {
    
        let ol_01 = WeakList1()
        let le_01 = ListElemWithWeakChain1()
        let le_02 = ListElemWithWeakChain1()
        let le_03 = ListElemWithWeakChain1()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: le_01)

        XCTAssertIdentical(ol_01.head, le_03, "WeakList1.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_02, "WeakList1.tail is wrong")

        XCTAssertIdentical(le_03.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertIdentical(le_03.weakChainNext1, le_01, "Element's next is wrong")
        XCTAssertNil(le_03.weakChainPrev1, "Element's prev is wrong")

        XCTAssertIdentical(le_01.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertIdentical(le_01.weakChainNext1, le_02, "Element's prev is wrong")
        XCTAssertIdentical(le_01.weakChainPrev1, le_03, "Element's prev is wrong")

        XCTAssertIdentical(le_02.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertNil(le_02.weakChainNext1, "Element's next is wrong")
        XCTAssertIdentical(le_02.weakChainPrev1, le_01, "Element's prev is wrong")
    }

    func testWeakList1AddThreeElementsCase5() throws {
    
        let ol_01 = WeakList1()
        let le_01 = ListElemWithWeakChain1()
        let le_02 = ListElemWithWeakChain1()
        let le_03 = ListElemWithWeakChain1()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: le_02)

        XCTAssertIdentical(ol_01.head, le_01, "WeakList1.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_02, "WeakList1.tail is wrong")

        XCTAssertIdentical(le_01.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertIdentical(le_01.weakChainNext1, le_03, "Element's next is wrong")
        XCTAssertNil(le_01.weakChainPrev1, "Element's prev is wrong")

        XCTAssertIdentical(le_03.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertIdentical(le_03.weakChainNext1, le_02, "Element's prev is wrong")
        XCTAssertIdentical(le_03.weakChainPrev1, le_01, "Element's prev is wrong")

        XCTAssertIdentical(le_02.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertNil(le_02.weakChainNext1, "Element's next is wrong")
        XCTAssertIdentical(le_02.weakChainPrev1, le_03, "Element's prev is wrong")
    }

    func testWeakList1ThreeElementsRemove1st() throws {
    
        let ol_01 = WeakList1()
        let le_01 = ListElemWithWeakChain1()
        let le_02 = ListElemWithWeakChain1()
        let le_03 = ListElemWithWeakChain1()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: nil)
        le_01.removeFromWeakList1()

        XCTAssertIdentical(ol_01.head, le_02, "WeakList1.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_03, "WeakList1.tail is wrong")

        XCTAssertNil(le_01.weakList1, "Element's WeakList1 is wrong")
        XCTAssertNil(le_01.weakChainNext1, "Element's prev is wrong")
        XCTAssertNil(le_01.weakChainPrev1, "Element's prev is wrong")

        XCTAssertIdentical(le_02.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertIdentical(le_02.weakChainNext1, le_03, "Element's prev is wrong")
        XCTAssertNil(le_02.weakChainPrev1, "Element's prev is wrong")

        XCTAssertIdentical(le_03.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertNil(le_03.weakChainNext1, "Element's next is wrong")
        XCTAssertIdentical(le_03.weakChainPrev1, le_02, "Element's prev is wrong")
    }

    func testWeakList1ThreeElementsRemove2nd() throws {
    
        let ol_01 = WeakList1()
        let le_01 = ListElemWithWeakChain1()
        let le_02 = ListElemWithWeakChain1()
        let le_03 = ListElemWithWeakChain1()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: nil)
        le_02.removeFromWeakList1()

        XCTAssertIdentical(ol_01.head, le_01, "WeakList1.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_03, "WeakList1.tail is wrong")

        XCTAssertNil(le_02.weakList1, "Element's WeakList1 is wrong")
        XCTAssertNil(le_02.weakChainNext1, "Element's prev is wrong")
        XCTAssertNil(le_02.weakChainPrev1, "Element's prev is wrong")

        XCTAssertIdentical(le_01.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertIdentical(le_01.weakChainNext1, le_03, "Element's prev is wrong")
        XCTAssertNil(le_01.weakChainPrev1, "Element's prev is wrong")

        XCTAssertIdentical(le_03.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertNil(le_03.weakChainNext1, "Element's next is wrong")
        XCTAssertIdentical(le_03.weakChainPrev1, le_01, "Element's prev is wrong")
    }

    func testWeakList1ThreeElementsRemove3rd() throws {
    
        let ol_01 = WeakList1()
        let le_01 = ListElemWithWeakChain1()
        let le_02 = ListElemWithWeakChain1()
        let le_03 = ListElemWithWeakChain1()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: nil)
        le_03.removeFromWeakList1()

        XCTAssertIdentical(ol_01.head, le_01, "WeakList1.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_02, "WeakList1.tail is wrong")

        XCTAssertNil(le_03.weakList1, "Element's WeakList1 is wrong")
        XCTAssertNil(le_03.weakChainNext1, "Element's prev is wrong")
        XCTAssertNil(le_03.weakChainPrev1, "Element's prev is wrong")

        XCTAssertIdentical(le_01.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertIdentical(le_01.weakChainNext1, le_02, "Element's prev is wrong")
        XCTAssertNil(le_01.weakChainPrev1, "Element's prev is wrong")

        XCTAssertIdentical(le_02.weakList1, ol_01, "Element's WeakList1 is wrong")
        XCTAssertNil(le_02.weakChainNext1, "Element's next is wrong")
        XCTAssertIdentical(le_02.weakChainPrev1, le_01, "Element's prev is wrong")
    }

    func testWeakList1IteratorEmptyList() throws {
    
        let ol_01 = WeakList1()
        var it_01 = ol_01.makeIterator()
        
        XCTAssertNil(it_01.next(), "Iterator's next() is wrong")
    }

    func testWeakList1IteratorOneElement() throws {
    
        let ol_01 = WeakList1()
        let le_01 = ListElemWithWeakChain1()
        ol_01.insert(le_01, before: nil)

        var it_01 = ol_01.makeIterator()
        
        XCTAssertIdentical(it_01.next(), le_01, "Iterator's next() is wrong")
        XCTAssertNil(it_01.next(), "Iterator's next() is wrong")
    }

    func testWeakList1IteratorTwoElements() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElemWithWeakChain1()
        let le_02 = ListElemWithWeakChain1()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)

        var it_01 = ol_01.makeIterator()
        
        XCTAssertIdentical(it_01.next(), le_01, "Iterator's next() is wrong")
        XCTAssertIdentical(it_01.next(), le_02, "Iterator's next() is wrong")
        XCTAssertNil(it_01.next(), "Iterator's next() is wrong")
    }

    func testWeakList1IteratorThreeElements() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElemWithWeakChain1()
        let le_02 = ListElemWithWeakChain1()
        let le_03 = ListElemWithWeakChain1()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: nil)

        var it_01 = ol_01.makeIterator()
        
        XCTAssertIdentical(it_01.next(), le_01, "Iterator's next() is wrong")
        XCTAssertIdentical(it_01.next(), le_02, "Iterator's next() is wrong")
        XCTAssertIdentical(it_01.next(), le_03, "Iterator's next() is wrong")
        XCTAssertNil(it_01.next(), "Iterator's next() is wrong")
    }

    func testWeakList1IteratorFourElements() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElemWithWeakChain1()
        let le_02 = ListElemWithWeakChain1()
        let le_03 = ListElemWithWeakChain1()
        let le_04 = ListElemWithWeakChain1()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: nil)
        ol_01.insert(le_04, before: nil)

        var it_01 = ol_01.makeIterator()
        
        XCTAssertIdentical(it_01.next(), le_01, "Iterator's next() is wrong")
        XCTAssertIdentical(it_01.next(), le_02, "Iterator's next() is wrong")
        XCTAssertIdentical(it_01.next(), le_03, "Iterator's next() is wrong")
        XCTAssertIdentical(it_01.next(), le_04, "Iterator's next() is wrong")
        XCTAssertNil(it_01.next(), "Iterator's next() is wrong")
    }

    func testWeakList2Empty() throws {
    
        let wl_01 = WeakList2()

        XCTAssertNil(wl_01.head, "WeakList2.head is wrong")
        XCTAssertNil(wl_01.tail, "WeakList2.tail is wrong")
    }

    func testWeakList2AddOneElement() throws {
    
        let wl_01 = WeakList2()
        let le_01 = ListElemWithWeakChain2()
        wl_01.insert(le_01, before: nil)

        XCTAssertIdentical(wl_01.head, le_01, "WeakList2.head is wrong")
        XCTAssertIdentical(wl_01.tail, le_01, "WeakList2.tail is wrong")
        XCTAssertIdentical(le_01.weakList2, wl_01, "Element's WeakList2 is wrong")
        XCTAssertNil(le_01.weakChainNext2, "Element's next is wrong")
        XCTAssertNil(le_01.weakChainPrev2, "Element's prev is wrong")
    }
    
    func testWeakList2AddOneElementThenRemoveElement() throws {
    
        let ol_01 = WeakList2()
        let le_01 = ListElemWithWeakChain2()

        ol_01.insert(le_01, before: nil)
        le_01.removeFromWeakList2()

        XCTAssertNil(ol_01.head, "WeakList2.head is wrong")
        XCTAssertNil(ol_01.tail, "WeakList2.tail is wrong")
        XCTAssertNil(le_01.weakList2, "Element's WeakList2 is wrong")
        XCTAssertNil(le_01.weakChainNext2, "Element's next is wrong")
        XCTAssertNil(le_01.weakChainPrev2, "Element's prev is wrong")
        
    }

    func testWeakList2AddTwoElementsCase1() throws {
    
        let ol_01 = WeakList2()
        let le_01 = ListElemWithWeakChain2()
        let le_02 = ListElemWithWeakChain2()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)

        XCTAssertIdentical(ol_01.head, le_01, "WeakList2.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_02, "WeakList2.tail is wrong")

        XCTAssertIdentical(le_01.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertIdentical(le_01.weakChainNext2, le_02, "Element's next is wrong")
        XCTAssertNil(le_01.weakChainPrev2, "Element's prev is wrong")

        XCTAssertIdentical(le_02.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertNil(le_02.weakChainNext2, "Element's next is wrong")
        XCTAssertIdentical(le_02.weakChainPrev2, le_01, "Element's prev is wrong")
    }

    func testWeakList2AddTwoElementsCase2() throws {
    
        let ol_01 = WeakList2()
        let le_01 = ListElemWithWeakChain2()
        let le_02 = ListElemWithWeakChain2()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: le_01)

        XCTAssertIdentical(ol_01.head, le_02, "WeakList2.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_01, "WeakList2.tail is wrong")

        XCTAssertIdentical(le_01.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertNil(le_01.weakChainNext2, "Element's next is wrong")
        XCTAssertIdentical(le_01.weakChainPrev2, le_02, "Element's prev is wrong")

        XCTAssertIdentical(le_02.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertIdentical(le_02.weakChainNext2, le_01, "Element's next is wrong")
        XCTAssertNil(le_02.weakChainPrev2, "Element's prev is wrong")
    }

    func testWeakList2WithTwoElementsRemoveFirst() throws {
    
        let ol_01 = WeakList2()
        let le_01 = ListElemWithWeakChain2()
        let le_02 = ListElemWithWeakChain2()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        le_01.removeFromWeakList2()

        XCTAssertNil(le_01.weakList2, "Element's WeakList2 is wrong")
        XCTAssertNil(le_01.weakChainNext2, "Element's prev is wrong")
        XCTAssertNil(le_01.weakChainPrev2, "Element's prev is wrong")

        XCTAssertIdentical(le_02.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertNil(le_02.weakChainNext2, "Element's prev is wrong")
        XCTAssertNil(le_02.weakChainPrev2, "Element's prev is wrong")
    }

    func testWeakList2WithTwoElementsRemoveSecond() throws {
    
        let ol_01 = WeakList2()
        let le_01 = ListElemWithWeakChain2()
        let le_02 = ListElemWithWeakChain2()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        le_02.removeFromWeakList2()

        XCTAssertIdentical(le_01.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertNil(le_01.weakChainNext2, "Element's prev is wrong")
        XCTAssertNil(le_01.weakChainPrev2, "Element's prev is wrong")

        XCTAssertNil(le_02.weakList2, "Element's WeakList2 is wrong")
        XCTAssertNil(le_02.weakChainNext2, "Element's prev is wrong")
        XCTAssertNil(le_02.weakChainPrev2, "Element's prev is wrong")
    }

    func testWeakList2AddThreeElementsCase1() throws {
    
        let ol_01 = WeakList2()
        let le_01 = ListElemWithWeakChain2()
        let le_02 = ListElemWithWeakChain2()
        let le_03 = ListElemWithWeakChain2()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: nil)

        XCTAssertIdentical(ol_01.head, le_01, "WeakList2.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_03, "WeakList2.tail is wrong")

        XCTAssertIdentical(le_01.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertIdentical(le_01.weakChainNext2, le_02, "Element's next is wrong")
        XCTAssertNil(le_01.weakChainPrev2, "Element's prev is wrong")

        XCTAssertIdentical(le_02.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertIdentical(le_02.weakChainNext2, le_03, "Element's prev is wrong")
        XCTAssertIdentical(le_02.weakChainPrev2, le_01, "Element's prev is wrong")

        XCTAssertIdentical(le_03.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertNil(le_03.weakChainNext2, "Element's next is wrong")
        XCTAssertIdentical(le_03.weakChainPrev2, le_02, "Element's prev is wrong")
    }

    func testWeakList2AddThreeElementsCase2() throws {
    
        let ol_01 = WeakList2()
        let le_01 = ListElemWithWeakChain2()
        let le_02 = ListElemWithWeakChain2()
        let le_03 = ListElemWithWeakChain2()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: le_01)
        ol_01.insert(le_03, before: le_02)

        XCTAssertIdentical(ol_01.head, le_03, "WeakList2.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_01, "WeakList2.tail is wrong")

        XCTAssertIdentical(le_01.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertNil(le_01.weakChainNext2, "Element's next is wrong")
        XCTAssertIdentical(le_01.weakChainPrev2, le_02, "Element's prev is wrong")

        XCTAssertIdentical(le_02.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertIdentical(le_02.weakChainNext2, le_01, "Element's prev is wrong")
        XCTAssertIdentical(le_02.weakChainPrev2, le_03, "Element's prev is wrong")

        XCTAssertIdentical(le_03.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertIdentical(le_03.weakChainNext2, le_02, "Element's next is wrong")
        XCTAssertNil(le_03.weakChainPrev2, "Element's prev is wrong")
    }
    
    func testWeakList2AddThreeElementsCase3() throws {
    
        let ol_01 = WeakList2()
        let le_01 = ListElemWithWeakChain2()
        let le_02 = ListElemWithWeakChain2()
        let le_03 = ListElemWithWeakChain2()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: le_01)
        ol_01.insert(le_03, before: nil)

        XCTAssertIdentical(ol_01.head, le_02, "WeakList2.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_03, "WeakList2.tail is wrong")

        XCTAssertIdentical(le_02.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertIdentical(le_02.weakChainNext2, le_01, "Element's next is wrong")
        XCTAssertNil(le_02.weakChainPrev2, "Element's prev is wrong")

        XCTAssertIdentical(le_01.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertIdentical(le_01.weakChainNext2, le_03, "Element's prev is wrong")
        XCTAssertIdentical(le_01.weakChainPrev2, le_02, "Element's prev is wrong")

        XCTAssertIdentical(le_03.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertNil(le_03.weakChainNext2, "Element's next is wrong")
        XCTAssertIdentical(le_03.weakChainPrev2, le_01, "Element's prev is wrong")
    }

    func testWeakList2AddThreeElementsCase4() throws {
    
        let ol_01 = WeakList2()
        let le_01 = ListElemWithWeakChain2()
        let le_02 = ListElemWithWeakChain2()
        let le_03 = ListElemWithWeakChain2()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: le_01)

        XCTAssertIdentical(ol_01.head, le_03, "WeakList2.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_02, "WeakList2.tail is wrong")

        XCTAssertIdentical(le_03.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertIdentical(le_03.weakChainNext2, le_01, "Element's next is wrong")
        XCTAssertNil(le_03.weakChainPrev2, "Element's prev is wrong")

        XCTAssertIdentical(le_01.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertIdentical(le_01.weakChainNext2, le_02, "Element's prev is wrong")
        XCTAssertIdentical(le_01.weakChainPrev2, le_03, "Element's prev is wrong")

        XCTAssertIdentical(le_02.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertNil(le_02.weakChainNext2, "Element's next is wrong")
        XCTAssertIdentical(le_02.weakChainPrev2, le_01, "Element's prev is wrong")
    }

    func testWeakList2AddThreeElementsCase5() throws {
    
        let ol_01 = WeakList2()
        let le_01 = ListElemWithWeakChain2()
        let le_02 = ListElemWithWeakChain2()
        let le_03 = ListElemWithWeakChain2()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: le_02)

        XCTAssertIdentical(ol_01.head, le_01, "WeakList2.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_02, "WeakList2.tail is wrong")

        XCTAssertIdentical(le_01.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertIdentical(le_01.weakChainNext2, le_03, "Element's next is wrong")
        XCTAssertNil(le_01.weakChainPrev2, "Element's prev is wrong")

        XCTAssertIdentical(le_03.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertIdentical(le_03.weakChainNext2, le_02, "Element's prev is wrong")
        XCTAssertIdentical(le_03.weakChainPrev2, le_01, "Element's prev is wrong")

        XCTAssertIdentical(le_02.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertNil(le_02.weakChainNext2, "Element's next is wrong")
        XCTAssertIdentical(le_02.weakChainPrev2, le_03, "Element's prev is wrong")
    }

    func testWeakList2ThreeElementsRemove1st() throws {
    
        let ol_01 = WeakList2()
        let le_01 = ListElemWithWeakChain2()
        let le_02 = ListElemWithWeakChain2()
        let le_03 = ListElemWithWeakChain2()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: nil)
        le_01.removeFromWeakList2()

        XCTAssertIdentical(ol_01.head, le_02, "WeakList2.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_03, "WeakList2.tail is wrong")

        XCTAssertNil(le_01.weakList2, "Element's WeakList2 is wrong")
        XCTAssertNil(le_01.weakChainNext2, "Element's prev is wrong")
        XCTAssertNil(le_01.weakChainPrev2, "Element's prev is wrong")

        XCTAssertIdentical(le_02.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertIdentical(le_02.weakChainNext2, le_03, "Element's prev is wrong")
        XCTAssertNil(le_02.weakChainPrev2, "Element's prev is wrong")

        XCTAssertIdentical(le_03.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertNil(le_03.weakChainNext2, "Element's next is wrong")
        XCTAssertIdentical(le_03.weakChainPrev2, le_02, "Element's prev is wrong")
    }

    func testWeakList2ThreeElementsRemove2nd() throws {
    
        let ol_01 = WeakList2()
        let le_01 = ListElemWithWeakChain2()
        let le_02 = ListElemWithWeakChain2()
        let le_03 = ListElemWithWeakChain2()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: nil)
        le_02.removeFromWeakList2()

        XCTAssertIdentical(ol_01.head, le_01, "WeakList2.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_03, "WeakList2.tail is wrong")

        XCTAssertNil(le_02.weakList2, "Element's WeakList2 is wrong")
        XCTAssertNil(le_02.weakChainNext2, "Element's prev is wrong")
        XCTAssertNil(le_02.weakChainPrev2, "Element's prev is wrong")

        XCTAssertIdentical(le_01.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertIdentical(le_01.weakChainNext2, le_03, "Element's prev is wrong")
        XCTAssertNil(le_01.weakChainPrev2, "Element's prev is wrong")

        XCTAssertIdentical(le_03.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertNil(le_03.weakChainNext2, "Element's next is wrong")
        XCTAssertIdentical(le_03.weakChainPrev2, le_01, "Element's prev is wrong")
    }

    func testWeakList2ThreeElementsRemove3rd() throws {
    
        let ol_01 = WeakList2()
        let le_01 = ListElemWithWeakChain2()
        let le_02 = ListElemWithWeakChain2()
        let le_03 = ListElemWithWeakChain2()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: nil)
        le_03.removeFromWeakList2()

        XCTAssertIdentical(ol_01.head, le_01, "WeakList2.head is wrong")
        XCTAssertIdentical(ol_01.tail, le_02, "WeakList2.tail is wrong")

        XCTAssertNil(le_03.weakList2, "Element's WeakList2 is wrong")
        XCTAssertNil(le_03.weakChainNext2, "Element's prev is wrong")
        XCTAssertNil(le_03.weakChainPrev2, "Element's prev is wrong")

        XCTAssertIdentical(le_01.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertIdentical(le_01.weakChainNext2, le_02, "Element's prev is wrong")
        XCTAssertNil(le_01.weakChainPrev2, "Element's prev is wrong")

        XCTAssertIdentical(le_02.weakList2, ol_01, "Element's WeakList2 is wrong")
        XCTAssertNil(le_02.weakChainNext2, "Element's next is wrong")
        XCTAssertIdentical(le_02.weakChainPrev2, le_01, "Element's prev is wrong")
    }

    func testWeakList2IteratorEmptyList() throws {
    
        let ol_01 = WeakList2()
        var it_01 = ol_01.makeIterator()
        
        XCTAssertNil(it_01.next(), "Iterator's next() is wrong")
    }

    func testWeakList2IteratorOneElement() throws {
    
        let ol_01 = WeakList2()
        let le_01 = ListElemWithWeakChain2()
        ol_01.insert(le_01, before: nil)

        var it_01 = ol_01.makeIterator()
        
        XCTAssertIdentical(it_01.next(), le_01, "Iterator's next() is wrong")
        XCTAssertNil(it_01.next(), "Iterator's next() is wrong")
    }

    func testWeakList2IteratorTwoElements() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElemWithWeakChain2()
        let le_02 = ListElemWithWeakChain2()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)

        var it_01 = ol_01.makeIterator()
        
        XCTAssertIdentical(it_01.next(), le_01, "Iterator's next() is wrong")
        XCTAssertIdentical(it_01.next(), le_02, "Iterator's next() is wrong")
        XCTAssertNil(it_01.next(), "Iterator's next() is wrong")
    }

    func testWeakList2IteratorThreeElements() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElemWithWeakChain2()
        let le_02 = ListElemWithWeakChain2()
        let le_03 = ListElemWithWeakChain2()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: nil)

        var it_01 = ol_01.makeIterator()
        
        XCTAssertIdentical(it_01.next(), le_01, "Iterator's next() is wrong")
        XCTAssertIdentical(it_01.next(), le_02, "Iterator's next() is wrong")
        XCTAssertIdentical(it_01.next(), le_03, "Iterator's next() is wrong")
        XCTAssertNil(it_01.next(), "Iterator's next() is wrong")
    }

    func testWeakList2IteratorFourElements() throws {
    
        let ol_01 = OwnerList()
        let le_01 = ListElemWithWeakChain2()
        let le_02 = ListElemWithWeakChain2()
        let le_03 = ListElemWithWeakChain2()
        let le_04 = ListElemWithWeakChain2()
        ol_01.insert(le_01, before: nil)
        ol_01.insert(le_02, before: nil)
        ol_01.insert(le_03, before: nil)
        ol_01.insert(le_04, before: nil)

        var it_01 = ol_01.makeIterator()
        
        XCTAssertIdentical(it_01.next(), le_01, "Iterator's next() is wrong")
        XCTAssertIdentical(it_01.next(), le_02, "Iterator's next() is wrong")
        XCTAssertIdentical(it_01.next(), le_03, "Iterator's next() is wrong")
        XCTAssertIdentical(it_01.next(), le_04, "Iterator's next() is wrong")
        XCTAssertNil(it_01.next(), "Iterator's next() is wrong")
    }
}


