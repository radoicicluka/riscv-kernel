//
// Created by os on 5/11/23.
//

#ifndef PROJECT_BASE_LIST_HPP
#define PROJECT_BASE_LIST_HPP

template<typename T>
class List{
private:
    struct Elem{
        T* data;
        Elem* next;

        Elem(T* data, Elem* next) : data(data), next(next){}
    };
    Elem *head, *tail;

public:
    List() : head(0), tail(0){}
    List(const List<T> &) = delete; // brisanje kopirajuceg konstruktora

    List<T> &operator=(const List<T> &) = delete;

    void addFirst(T* data){
        Elem* elem = new Elem(data, 0);
        elem->next = head;
        head = elem;
        if (!tail) {
            tail = head;
        }
    }

    void addLast(T* data){
        Elem* elem = new Elem(data, 0);
        if (tail){
            tail->next = elem;
            tail = elem;
        }
        else {
            head = tail = elem;
        }
    }

    T* removeFirst(){
        if (!head) return 0;

        Elem* elem = head;
        head = head->next;
        if (!head) tail = 0;
        T* ret = elem->data;
        delete elem;
        return ret;
    }

    T* peekFirst(){
        if (!head) return 0;
        return head->data;
    }

    T* removeLast(){
        if (!head) return 0;
        Elem* prev = 0;
        for (Elem* curr = head; curr && curr != tail; curr = curr->next) prev = curr;
        Elem* elem = tail;
        if (prev) prev->next = 0;
        else head = 0;
        tail = prev;

        T* ret = elem->data;
        delete elem;
        return ret;
    }

    T* peekLast(){
        if (!tail) return 0;
        return tail->data;
    }

    bool contains(T* data){
        if (data == 0) return false;
        Elem* cur = head;
        while (cur != 0){
            if (cur->data == data) return true;
            cur = cur->next;
        }
        return false;
    }

    int len(){
        int ret = 0;
        Elem* cur = head;
        while(cur != 0) {
            ret++;
            cur = cur->next;
        }
        return ret;
    }


};


#endif //PROJECT_BASE_LIST_HPP
