


#include "./../../common/src/pgr_assert.h"
#include "pgr_pickDeliver.h"
#include "order.h"


Order::Order(ID p_id,
        const Vehicle_node &p_pickup,
        const Vehicle_node &p_delivery,
        const Pgr_pickDeliver &p_problem) :
    m_id(p_id),
    pickup_id(p_pickup.id()),
    delivery_id(p_delivery.id()),
    problem(p_problem) { 
        pgassert(pickup().is_pickup());
        pgassert(delivery().is_delivery());
    }

std::ostream&
operator<<(std::ostream &log, const Order &order) {
    log << "\n\nOrder " << order.m_id << ":\n"
        << "\tPickup: " << order.pickup() << "\n"
        << "\tDelivery: " << order.delivery() << "\n\n";
    if (order.delivery().is_partially_compatible_IJ(order.pickup())) {
        log << "\tis_partially_compatible_IJ: ";
    } else if ( order.delivery().is_tight_compatible_IJ(order.pickup())) {
        log << "\tis_tight_compatible_IJ: "; 
    } else if ( order.delivery().is_waitTime_compatible_IJ(order.pickup())) {
        log << "\tis_waitTime_compatible_IJ: ";
    } else {
        assert(false);
    }
    log << "\n\nThere are ** " <<  order.m_compatibleJ.size() << " ** compatible orders: ";
    for (const auto o : order.m_compatibleJ) {
        log << o << ",";
    }


    return log;
}



const Vehicle_node&
Order::delivery() const {return problem.node(delivery_id);}


const Vehicle_node&
Order::pickup() const {return problem.node(pickup_id);}


bool
Order::is_valid() const {
    return 
        pickup().is_pickup()
        && delivery().is_delivery()
        /* P -> D  */ 
        && delivery().is_compatible_IJ(pickup());
}


/*
 * Initializing the set of nodes that can be placed
 * inmediately after \bthis node
 *
 * (*this) -> J
 *
 */

void
Order::setCompatibles() {
    for (const auto J : problem.orders()) {
        if (J.id() == id()) continue;
        if (J.isCompatibleIJ(*this)) {
            m_compatibleJ.insert(J.id());
        }
    }
}

/*
 * True when
 *
 * I -> (*this)
 *
 */

bool
Order::isCompatibleIJ(const Order &I) const {

    /* this is true in all cases */
    auto all_cases(
            pickup().is_compatible_IJ(I.pickup()) 
            && delivery().is_compatible_IJ(I.pickup())
            );
    /*
       && I.delivery().is_compatible_IJ(I.pickup()) 
       && delivery().is_compatible_IJ(pickup()));
       */


    /* case other(P) other(D) this(P) this(D) */
    auto case1( pickup().is_compatible_IJ(I.delivery()) 
            && delivery().is_compatible_IJ(I.delivery()));

    /* case other(P) this(P) other(D) this(D) */
    auto case2(I.delivery().is_compatible_IJ(pickup()) 
            && delivery().is_compatible_IJ(I.delivery()));

    /* case other(P) this(P) this(D) other(D) */
    auto case3(I.delivery().is_compatible_IJ(pickup()) 
            && I.delivery().is_compatible_IJ(delivery()));

    return all_cases && (case1 || case2 || case3);
}


bool
Order::isOrderCompatibleEnd(const Vehicle_node &node) const {
    return false;
}

bool
Order::isOrderCompatibleStart(const Vehicle_node &node) const {
    return false;
}
