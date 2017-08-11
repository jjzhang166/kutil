#ifndef locker_h__
#define locker_h__

#pragma once

#include <QMutex>
#include "macro.h"

// ���ļ��еĴ�����Ҫ���ڷ����ʵ�����ݵĶ��̰߳�ȫ����
// �����̰߳�ȫ��������һ����ʹ�����Ƶġ�
// ��ע��ṹ/������ʹ��ע��
namespace threadsafe
{
    class QRecursiveLocker : public QMutex
    {
    public:
        QRecursiveLocker()
            :QMutex(QMutex::Recursive)
        {

        }
    };

    template<typename _C>
    class lock_ptr
    {
    public:
        typedef _C _Typ;

    public:
        explicit lock_ptr(QRecursiveLocker* m, _C* c)
            :m_(m), c_(c)
        {
            // static_assert(!std::is_pointer<_C>::value)
            if (nullptr != m_)
            {
                m_->lock();
            }
        }

        // ���ƹ����ת�ƿ���Ȩ
        lock_ptr(const lock_ptr& t)
            :m_(t.m_), c_(t.c_)
        {
            const_cast<lock_ptr*>(&t)->m_ = nullptr;
            const_cast<lock_ptr*>(&t)->c_ = nullptr;
        }

        ~lock_ptr()
        {
            if (nullptr != m_)
            {
                m_->unlock();
                m_ = nullptr;
            }
        }

        _C * operator->()const
        {
            return c_;
        }

    private:
        QRecursiveLocker* m_ = nullptr;
        _C * c_ = nullptr;
    };

    // Ҫ��֤�������Ĵ�����Ψһ��
    template <typename _C>
    class lock_obj
    {
    public:
        typedef _C _Typ;

    public:
        lock_obj()
        {
            static_assert(!std::is_pointer<_C>::value, "_C Must NOT be a pointer!");
        }

        lock_ptr<_C> lock()const
        {
            return lock_ptr<_C>(const_cast<QRecursiveLocker*>(&m_), const_cast<_C*>(&c_));
        }

        // ���������ʡ�¶�������ȴ��֤�����̰߳�ȫ
        lock_ptr<_C> operator->()const
        {
            return lock();
        }

    private:
        // �ɵݹ����������ǵݹ��������ʹ������
        QRecursiveLocker m_;
        
        // ���ݱ��棬������ָ������
        _C c_;
    };

// 
// ֻ�е�����ʹ�÷�������ȷ�ģ�
//  ��ʹ��lock()�����ʱ����Ȼ����ȥ����
//
// 1, ��ȫ��ͬ�ڵڶ���д����begin/end ������һ������һ�ν�����ѭ��������ȫ����������
//     for (const auto& t : obj)    
//     {
//         std::cout << t << "    ";
//     }
//
// 2, ��ȫͬ��
//     for (auto i = obj->begin(); i != obj->end(); ++i)
//     {
//         std::cout << *i << "    ";
//     }
//  
// 3, ��Ϊ��obj.lock() ��������ʱ���󲻻����������Բ��������ѭ�����ܵ����ı���
//     auto& v = obj.lock();
//     for (auto i = v->begin(); i != v->end(); ++i)
//     {
//         std::cout << *i << "    ";
//     }

    template <typename _C>
    class container_lock_obj : public lock_obj<_C>
    {
        typedef typename _C::iterator _Itr;
        typedef typename _C::const_iterator _ConsItr;

    public:
        typedef _C _Typ;

    public:
        _Itr begin()
        {
            return lock()->begin();
        }

        _Itr end()
        {
            return lock()->end();
        }

        _ConsItr begin() const
        {
            return lock()->cbegin();
        }

        _ConsItr end()const 
        {
            return lock()->cend();
        }
    };

#define threadsafe_for(itr, cont) \
    static_assert(std::is_base_of<threadsafe::container_lock_obj<decltype(cont)::_Typ>, decltype(cont)>::value, \
            #cont##" ----------> !!! is NOT a container_lock_obj instance");\
    auto& _CAT_LINE(_v_) = cont.lock(); for(auto itr = _CAT_LINE(_v_)->begin(); itr != _CAT_LINE(_v_)->end(); ++itr)

#define threadsafe_for_each(val, cont) \
    static_assert(std::is_base_of<threadsafe::container_lock_obj<decltype(cont)::_Typ>, decltype(cont)>::value, \
            #cont##" ----------> !!! is NOT a container_lock_obj instance");\
    auto& _CAT_LINE(_v_) = cont.lock(); for(auto & val : cont)

}

#endif // locker_h__
