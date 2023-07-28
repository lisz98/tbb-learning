#pragma once

#include <new>
#include <utility>

template<class T>
struct pod{
private:
    T m_t;
public:
    pod(){}

    pod(pod &&p): m_t(std::move(p.m_t)){}

    pod(pod const &p) : m_t(p.m_t){}

    pod &operator=(pod &&p){
        m_t = std::move(p.m_t);
        return *this;
    }

    pod &operator=(pod const &p){
        m_t =p.m_t;
        return *this;
    }

    pod &operator=(T &&p){
        m_t = std::move(p);
        return *this;
    }

    pod &operator=(T const &p){
        m_t = p;
        return *this;
    }

    operator T const &() const{
        return m_t;
    }

    operator T &(){
        return m_t;
    }

    T const & get()const {
        return m_t;
    }

    T &get(){
        return  m_t;
    }

    template<class ...Ts>
    pod &emplace(Ts && ...ts){
        ::new (&m_t) T(std::forward<Ts>(ts)...);
        return *this;
    }

    void destory(){
        m_t.~T();
    }
};
