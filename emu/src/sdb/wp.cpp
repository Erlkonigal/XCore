#include <common.hpp>
#include <sdb/wp.hpp>

WatchPointPool wpPool;

WatchPointPool::WatchPointPool() {
    for (int i = 0; i < NR_WATCHPOINTS; i++) {
        pool[i].No = i;
        if (i == NR_WATCHPOINTS - 1) {
            pool[i].next = nullptr;
        } else {
            pool[i].next = &pool[i + 1];
        }
    }
    phead.next = nullptr;
    pfree.next = pool;
}

WatchPoint *WatchPointPool::newWatchPoint() {
    if (pfree.next == nullptr) {
        Error("No free watchpoint");
        return nullptr;
    }
    WatchPoint *wp = pfree.next;
    pfree.next = wp->next;
    wp->next = phead.next;
    phead.next = wp;
    return wp;
}

WatchPoint *WatchPointPool::getHead() {
    return &phead;
}

void WatchPointPool::freeWatchPoint(int No) {
    WatchPoint *cur = &phead;
    while (cur->next != nullptr) {
        if (cur->next->No == No) break;
        cur = cur->next;
    }
    if (cur->next == nullptr) {
        Error("No such watchpoint");
        return;
    }
    WatchPoint *wp = cur->next;
    cur->next = wp->next;
    wp->next = pfree.next;
    pfree.next = wp;
}

void WatchPointPool::print() {
    WatchPoint *cur = phead.next;
    while (cur != nullptr) {
        Info("Watchpoint %d:\t%s", cur->No, cur->expr);
        cur = cur->next;
    }
}
