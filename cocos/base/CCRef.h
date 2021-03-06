/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2014 Chukong Technologies

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __BASE_CCREF_H__
#define __BASE_CCREF_H__

#include "platform/CCPlatformMacros.h"
#include "base/ccConfig.h"

#include <memory> // unique_ptr

/**
 * @addtogroup base
 * @{
 */

namespace cocos2d {

/**
 * Ref is used for reference count management. If a class inherits from Ref,
 * then it is easy to be shared in different places.
 * @js NA
 */
class CC_DLL Ref
{
public:
    /**
     * Retains the ownership.
     *
     * This increases the Ref's reference count.
     *
     * @see release, autorelease
     * @js NA
     */
    void retain() const;

    /**
     * Releases the ownership immediately.
     *
     * This decrements the Ref's reference count.
     *
     * If the reference count reaches 0 after the decrement, this Ref is
     * destructed.
     *
     * @see retain, autorelease
     * @js NA
     */
    void release() const;

    /**
     * Releases the ownership sometime soon automatically.
     *
     * This decrements the Ref's reference count at the end of current
     * autorelease pool block.
     *
     * If the reference count reaches 0 after the decrement, this Ref is
     * destructed.
     *
     * @returns The Ref itself.
     *
     * @see AutoreleasePool, retain, release
     * @js NA
     * @lua NA
     */
    void autorelease();

    /**
     * Returns the Ref's current reference count.
     *
     * @returns The Ref's reference count.
     * @js NA
     */
    unsigned int getReferenceCount() const;

protected:
    /**
     * Constructor
     *
     * The Ref's reference count is 1 after construction.
     * @js NA
     */
    Ref();

public:
    /**
     * Destructor
     *
     * @js NA
     * @lua NA
     */
    virtual ~Ref();

protected:
    /// count of references
    mutable unsigned int _referenceCount;

    friend class AutoreleasePool;
};

class Node;
class Action;

template<typename T>
struct retaining_ptr_deleter {
public:
    void operator()(T * p) const
    {
        static_assert(std::is_base_of<Ref, T>::value,
                      "retaining_ptr is for Ref-based types only");
        static_assert(! std::is_base_of<Node, T>::value,
                      "retaining_ptr is not for Node-based types. Use node_ptr");
        static_assert(! std::is_base_of<Action, T>::value,
                      "retaining_ptr is not for Action-based types. Use action_ptr");
        p->release();
    }
};

template<typename T>
using retaining_ptr = std::unique_ptr<T, retaining_ptr_deleter<T>>;

template<typename T>
retaining_ptr<T> to_retaining_ptr(T * ptr)
{
    static_assert(std::is_base_of<Ref, T>::value,
                  "retaining_ptr is for Ref-based types only");
    static_assert(! std::is_base_of<Node, T>::value,
                  "retaining_ptr is not for Node-based types. Use node_ptr");
    static_assert(! std::is_base_of<Action, T>::value,
                  "retaining_ptr is not for Action-based types. Use action_ptr");
    
    if (ptr)
    {
        ptr->retain();
    }
    return retaining_ptr<T>(ptr);
}

template<typename T>
std::shared_ptr<T> to_retaining_shared_ptr(T * ptr)
{
    static_assert(std::is_base_of<Ref, T>::value,
                  "retaining_ptr is for Ref-based types only");
    static_assert(! std::is_base_of<Node, T>::value,
                  "retaining_ptr is not for Node-based types. Use node_ptr");
    static_assert(! std::is_base_of<Action, T>::value,
                  "retaining_ptr is not for Action-based types. Use action_ptr");
    
    if (ptr)
    {
        ptr->retain();
    }
    return std::shared_ptr<T>(ptr, retaining_ptr_deleter<T>{});
}

} // namespace cocos2d
// end of base group
/** @} */

#endif // __BASE_CCREF_H__
