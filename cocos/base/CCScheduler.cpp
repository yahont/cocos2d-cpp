/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017      Iakov Sergeev <yahont@github>

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

#include "base/CCScheduler.h"

#include "base/ccCArray.h"
#include "base/CCDirector.h"
#include "base/utlist.h"
#include "base/ccMacros.h"

namespace cocos2d {

// data structures

// implementation Timer

Timer::Timer()
: _scheduler(nullptr)
, _elapsed(-1)
, _runForever(false)
, _useDelay(false)
, _timesExecuted(0)
, _repeat(0)
, _delay(0.0f)
, _interval(0.0f)
{
}

void Timer::setupTimerWithInterval(float seconds, unsigned int repeat, float delay)
{
	_elapsed = -1;
	_interval = seconds;
	_delay = delay;
	_useDelay = (_delay > 0.0f) ? true : false;
	_repeat = repeat;
	_runForever = (_repeat == CC_REPEAT_FOREVER) ? true : false;
}

void Timer::update(float dt)
{
    if (_elapsed == -1)
    {
        _elapsed = 0;
        _timesExecuted = 0;
        return;
    }

    // accumulate elapsed time
    _elapsed += dt;
    
    // deal with delay
    if (_useDelay)
    {
        if (_elapsed < _delay)
        {
            return;
        }
        trigger(_delay);
        _elapsed = _elapsed - _delay;
        _timesExecuted += 1;
        _useDelay = false;
        // after delay, the rest time should compare with interval
        if (!_runForever && _timesExecuted > _repeat)
        {    //unschedule timer
            cancel();
            return;
        }
    }
    
    // if _interval == 0, should trigger once every frame
    float interval = (_interval > 0) ? _interval : _elapsed;
    while (_elapsed >= interval)
    {
        trigger(interval);
        _elapsed -= interval;
        _timesExecuted += 1;

        if (!_runForever && _timesExecuted > _repeat)
        {
            cancel();
            break;
        }

        if (_elapsed <= 0.f)
        {
            break;
        }
    }
}

// TimerTargetSelector

TimerTargetSelector::TimerTargetSelector()
: _target(nullptr)
, _selector(nullptr)
{
}

bool TimerTargetSelector::initWithSelector(Scheduler* scheduler, SEL_SCHEDULE selector, Ref* target, float seconds, unsigned int repeat, float delay)
{
    _scheduler = scheduler;
    _target = target;
    _selector = selector;
    setupTimerWithInterval(seconds, repeat, delay);
    return true;
}

void TimerTargetSelector::trigger(float dt)
{
    if (_target && _selector)
    {
        (_target->*_selector)(dt);
    }
}

void TimerTargetSelector::cancel()
{
    _scheduler->unschedule(_selector, _target);
}

// TimerTargetCallback

TimerTargetCallback::TimerTargetCallback()
: _target(nullptr)
, _callback(nullptr)
{
}

bool TimerTargetCallback::initWithCallback(Scheduler* scheduler, const ccSchedulerFunc& callback, void *target, const std::string& key, float seconds, unsigned int repeat, float delay)
{
    _scheduler = scheduler;
    _target = target;
    _callback = callback;
    _key = key;
    setupTimerWithInterval(seconds, repeat, delay);
    return true;
}

void TimerTargetCallback::trigger(float dt)
{
    if (_callback)
    {
        _callback(dt);
    }
}

void TimerTargetCallback::cancel()
{
    _scheduler->unschedule(_key, _target);
}

// implementation of Scheduler

// Priority level reserved for system services.
const int Scheduler::PRIORITY_SYSTEM = INT_MIN;

// Minimum priority level for user scheduling.
const int Scheduler::PRIORITY_NON_SYSTEM_MIN = PRIORITY_SYSTEM + 1;

Scheduler::Scheduler(void)
: _timeScale(1.0f)
, _updatesNegList(nullptr)
, _updates0List(nullptr)
, _updatesPosList(nullptr)
, _hashForUpdates(nullptr)
, _hashForTimers()
, _currentTarget(nullptr)
, _currentTargetSalvaged(false)
, _updateHashLocked(false)
{
    // I don't expect to have more than 30 functions to all per frame
    _functionsToPerform.reserve(30);
}

Scheduler::~Scheduler(void)
{
    unscheduleAll();
}

void Scheduler::schedule(const ccSchedulerFunc& callback, void *target, float interval, bool paused, const std::string& key)
{
    this->schedule(callback, target, interval, CC_REPEAT_FOREVER, 0.0f, paused, key);
}

void Scheduler::schedule(const ccSchedulerFunc& callback, void *target, float interval, unsigned int repeat, float delay, bool paused, const std::string& key)
{
    CCASSERT(target, "Argument target must be non-nullptr");
    CCASSERT(!key.empty(), "key should not be empty!");

    auto & element = _hashForTimers[target];

    if (!element)
    {
        element.reset(new tHashTimerEntry);
        element->paused = paused;
    }
    else
    {
        CCASSERT(element->paused == paused, "element's paused should be paused!");
    }

    for (auto & t : element->timers)
    {
        TimerTargetCallback *timer = dynamic_cast<TimerTargetCallback*>(t.get());

        if (timer && key == timer->getKey())
        {
            CCLOG("CCScheduler#scheduleSelector. Selector already scheduled. Updating interval from: %.4f to %.4f", timer->getInterval(), interval);
            timer->setInterval(interval);
            return;
        }        
    }

    auto timer = to_retaining_ptr(new (std::nothrow) TimerTargetCallback());
    timer->initWithCallback(this, callback, target, key, interval, repeat, delay);
    element->timers.push_back( retaining_ptr<Timer>( timer.release()));
}

template<typename F>
void Scheduler::unschedule(void *target, F compareTimers)
{
    auto element_it = _hashForTimers.find(target);

    if (_hashForTimers.end() == element_it)
    {
        return;
    }

    auto & element = *element_it->second;
    auto & timers = element.timers;

    auto timer_it = std::find_if( timers.begin(), timers.end(), compareTimers );

    if (timer_it == timers.end())
    {
        return;
    }

    if ((! element.currentTimerSalvaged)
        && timer_it->get() == element.currentTimer.get())
    {
        element.currentTimerSalvaged = true;
    }

    // update timerIndex in case we are in tick:, looping over the actions
    if (element.timerIndex >= std::distance(timers.begin(), timer_it))
    {
        element.timerIndex--;
    }

    timers.erase(timer_it);

    if (timers.empty())
    {
        if (_currentTarget == target)
        {
            _currentTargetSalvaged = true;
        }
        else
        {
            CC_ASSERT(element_it == _hashForTimers.find(target));
            _hashForTimers.erase(element_it);
        }
    }
}

void Scheduler::unschedule(const std::string &key, void *target)
{
    // explicit handle nil arguments when removing an object
    if (target == nullptr || key.empty())
    {
        return;
    }
    unschedule(target,
               [&key](const retaining_ptr<Timer> & p) {
                   auto timer = dynamic_cast<const TimerTargetCallback*>(p.get());
                   return timer && key == timer->getKey();
               });
}

void Scheduler::unschedule(SEL_SCHEDULE selector, Ref *target)
{
    if (target == nullptr || selector == nullptr)
    {
        return;
    }

    unschedule(target,
               [&selector](const retaining_ptr<Timer> & p) {
                   auto timer = dynamic_cast<const TimerTargetSelector*>(p.get());
                   return timer && selector == timer->getSelector();
               });
}

void Scheduler::priorityIn(tListEntry **list, const ccSchedulerFunc& callback, void *target, int priority, bool paused)
{
    tListEntry *listElement = new (std::nothrow) tListEntry();

    listElement->callback = callback;
    listElement->target = target;
    listElement->priority = priority;
    listElement->paused = paused;
    listElement->next = listElement->prev = nullptr;
    listElement->markedForDeletion = false;

    // empty list ?
    if (! *list)
    {
        DL_APPEND(*list, listElement);
    }
    else
    {
        bool added = false;

        for (tListEntry *element = *list; element; element = element->next)
        {
            if (priority < element->priority)
            {
                if (element == *list)
                {
                    DL_PREPEND(*list, listElement);
                }
                else
                {
                    listElement->next = element;
                    listElement->prev = element->prev;

                    element->prev->next = listElement;
                    element->prev = listElement;
                }

                added = true;
                break;
            }
        }

        // Not added? priority has the higher value. Append it.
        if (! added)
        {
            DL_APPEND(*list, listElement);
        }
    }

    // update hash entry for quick access
    tHashUpdateEntry *hashElement = (tHashUpdateEntry *)calloc(sizeof(*hashElement), 1);
    hashElement->target = target;
    hashElement->list = list;
    hashElement->entry = listElement;
    HASH_ADD_PTR(_hashForUpdates, target, hashElement);
}

void Scheduler::appendIn(tListEntry **list, const ccSchedulerFunc& callback, void *target, bool paused)
{
    tListEntry *listElement = new (std::nothrow) tListEntry();

    listElement->callback = callback;
    listElement->target = target;
    listElement->paused = paused;
    listElement->priority = 0;
    listElement->markedForDeletion = false;

    DL_APPEND(*list, listElement);

    // update hash entry for quicker access
    tHashUpdateEntry *hashElement = (tHashUpdateEntry *)calloc(sizeof(*hashElement), 1);
    hashElement->target = target;
    hashElement->list = list;
    hashElement->entry = listElement;
    HASH_ADD_PTR(_hashForUpdates, target, hashElement);
}

void Scheduler::schedulePerFrame(const ccSchedulerFunc& callback, void *target, int priority, bool paused)
{
    tHashUpdateEntry *hashElement = nullptr;
    HASH_FIND_PTR(_hashForUpdates, &target, hashElement);
    if (hashElement)
    {
        // check if priority has changed
        if ((*hashElement->list)->priority != priority)
        {
            if (_updateHashLocked)
            {
                CCLOG("warning: you CANNOT change update priority in scheduled function");
                hashElement->entry->markedForDeletion = false;
                hashElement->entry->paused = paused;
                return;
            }
            else
            {
            	// will be added again outside if (hashElement).
                unscheduleUpdate(target);
            }
        }
        else
        {
            hashElement->entry->markedForDeletion = false;
            hashElement->entry->paused = paused;
            return;
        }
    }

    // most of the updates are going to be 0, that's way there
    // is an special list for updates with priority 0
    if (priority == 0)
    {
        appendIn(&_updates0List, callback, target, paused);
    }
    else if (priority < 0)
    {
        priorityIn(&_updatesNegList, callback, target, priority, paused);
    }
    else
    {
        // priority > 0
        priorityIn(&_updatesPosList, callback, target, priority, paused);
    }
}

bool Scheduler::isScheduled(const std::string& key, void *target)
{
    CCASSERT(!key.empty(), "Argument key must not be empty");
    CCASSERT(target, "Argument target must be non-nullptr");
    
    auto element_it = _hashForTimers.find(target);

    if (_hashForTimers.end() != element_it)
    {
        auto & timers = element_it->second->timers;

        for (auto it = timers.begin(), end = timers.end(); it != end; it++)
        {
            TimerTargetCallback *timer = dynamic_cast<TimerTargetCallback*>(it->get());

            if (timer && key == timer->getKey())
            {
                return true;
            }
        }
    }

    return false;
}

void Scheduler::removeUpdateFromHash(tListEntry *entry)
{
    tHashUpdateEntry *element = nullptr;

    HASH_FIND_PTR(_hashForUpdates, &entry->target, element);
    if (element)
    {
        // list entry
        DL_DELETE(*element->list, element->entry);
        CC_SAFE_DELETE(element->entry);

        // hash entry
        HASH_DEL(_hashForUpdates, element);
        free(element);
    }
}

void Scheduler::unscheduleUpdate(void *target)
{
    if (target == nullptr)
    {
        return;
    }

    tHashUpdateEntry *element = nullptr;
    HASH_FIND_PTR(_hashForUpdates, &target, element);
    if (element)
    {
        if (_updateHashLocked)
        {
            element->entry->markedForDeletion = true;
        }
        else
        {
            this->removeUpdateFromHash(element->entry);
        }
    }
}

void Scheduler::unscheduleAll(void)
{
    unscheduleAllWithMinPriority(PRIORITY_SYSTEM);
}

void Scheduler::unscheduleAllWithMinPriority(int minPriority)
{
    // Custom Selectors

    auto it = _hashForTimers.find(_currentTarget);

    if (it != _hashForTimers.end())
    {
        std::unique_ptr<tHashTimerEntry> tmp = std::move(it->second);
        _hashForTimers.erase(it);
        _hashForTimers.clear();
        _hashForTimers[_currentTarget] = std::move(tmp);
        unscheduleAllForTarget(_currentTarget);
    }
    else
    {
        _hashForTimers.clear();
    }

    // Updates selectors
    tListEntry *entry, *tmp;
    if(minPriority < 0)
    {
        DL_FOREACH_SAFE(_updatesNegList, entry, tmp)
        {
            if(entry->priority >= minPriority)
            {
                unscheduleUpdate(entry->target);
            }
        }
    }

    if(minPriority <= 0)
    {
        DL_FOREACH_SAFE(_updates0List, entry, tmp)
        {
            unscheduleUpdate(entry->target);
        }
    }

    DL_FOREACH_SAFE(_updatesPosList, entry, tmp)
    {
        if(entry->priority >= minPriority)
        {
            unscheduleUpdate(entry->target);
        }
    }
}

void Scheduler::unscheduleAllForTarget(void *target)
{
    // explicit nullptr handling
    if (target == nullptr)
    {
        return;
    }

    auto it = _hashForTimers.find(target);
    if (_hashForTimers.end() == it)
    {
        return;
    }

    // Custom Selectors
    if (_currentTarget != target)
    {
        _hashForTimers.erase(it);
    }
    else
    {
        auto & timers = it->second->timers;

        if (! it->second->currentTimerSalvaged)
        {
            auto currentTimer_it = std::find(timers.begin(), timers.end(), it->second->currentTimer);
            if (currentTimer_it !=  timers.end())
            {
                it->second->currentTimerSalvaged = true;
            }
        }

        timers.clear();

        _currentTargetSalvaged = true;
    }

    // update selector
    unscheduleUpdate(target);
}

void Scheduler::resumeTarget(void *target)
{
    CCASSERT(target != nullptr, "target can't be nullptr!");

    // custom selectors
    auto it = _hashForTimers.find(target);
    if (_hashForTimers.end() != it)
    {
        it->second->paused = false;
    }

    // update selector
    tHashUpdateEntry *elementUpdate = nullptr;
    HASH_FIND_PTR(_hashForUpdates, &target, elementUpdate);
    if (elementUpdate)
    {
        CCASSERT(elementUpdate->entry != nullptr, "elementUpdate's entry can't be nullptr!");
        elementUpdate->entry->paused = false;
    }
}

void Scheduler::pauseTarget(void *target)
{
    CCASSERT(target != nullptr, "target can't be nullptr!");

    // custom selectors
    auto it = _hashForTimers.find(target);
    if (_hashForTimers.end() != it)
    {
        it->second->paused = true;
    }

    // update selector
    tHashUpdateEntry *elementUpdate = nullptr;
    HASH_FIND_PTR(_hashForUpdates, &target, elementUpdate);
    if (elementUpdate)
    {
        CCASSERT(elementUpdate->entry != nullptr, "elementUpdate's entry can't be nullptr!");
        elementUpdate->entry->paused = true;
    }
}

bool Scheduler::isTargetPaused(void *target)
{
    CCASSERT( target != nullptr, "target must be non nil" );

    // Custom selectors
    auto it = _hashForTimers.find(target);
    if (_hashForTimers.end() != it)
    {
        return it->second->paused;
    }
    
    // We should check update selectors if target does not have custom selectors
	tHashUpdateEntry *elementUpdate = nullptr;
	HASH_FIND_PTR(_hashForUpdates, &target, elementUpdate);
	if ( elementUpdate )
    {
		return elementUpdate->entry->paused;
    }
    
    return false;  // should never get here
}

std::set<void*> Scheduler::pauseAllTargets()
{
    return pauseAllTargetsWithMinPriority(PRIORITY_SYSTEM);
}

std::set<void*> Scheduler::pauseAllTargetsWithMinPriority(int minPriority)
{
    std::set<void*> idsWithSelectors;

    // Custom Selectors
    for (auto & pair : _hashForTimers)
    {
        pair.second->paused = true;
        idsWithSelectors.insert(pair.first);
    }

    // Updates selectors
    tListEntry *entry, *tmp;
    if(minPriority < 0)
    {
        DL_FOREACH_SAFE( _updatesNegList, entry, tmp ) 
        {
            if(entry->priority >= minPriority)
            {
                entry->paused = true;
                idsWithSelectors.insert(entry->target);
            }
        }
    }

    if(minPriority <= 0)
    {
        DL_FOREACH_SAFE( _updates0List, entry, tmp )
        {
            entry->paused = true;
            idsWithSelectors.insert(entry->target);
        }
    }

    DL_FOREACH_SAFE( _updatesPosList, entry, tmp ) 
    {
        if(entry->priority >= minPriority) 
        {
            entry->paused = true;
            idsWithSelectors.insert(entry->target);
        }
    }

    return idsWithSelectors;
}

void Scheduler::resumeTargets(const std::set<void*>& targetsToResume)
{
    for(const auto &obj : targetsToResume) {
        this->resumeTarget(obj);
    }
}

void Scheduler::performFunctionInCocosThread(const std::function<void ()> &function)
{
    _performMutex.lock();

    _functionsToPerform.push_back(function);

    _performMutex.unlock();
}

// main loop
void Scheduler::update(float dt)
{
    _updateHashLocked = true;

    if (_timeScale != 1.0f)
    {
        dt *= _timeScale;
    }

    //
    // Selector callbacks
    //

    // Iterate over all the Updates' selectors
    tListEntry *entry, *tmp;

    // updates with priority < 0
    DL_FOREACH_SAFE(_updatesNegList, entry, tmp)
    {
        if ((! entry->paused) && (! entry->markedForDeletion))
        {
            entry->callback(dt);
        }
    }

    // updates with priority == 0
    DL_FOREACH_SAFE(_updates0List, entry, tmp)
    {
        if ((! entry->paused) && (! entry->markedForDeletion))
        {
            entry->callback(dt);
        }
    }

    // updates with priority > 0
    DL_FOREACH_SAFE(_updatesPosList, entry, tmp)
    {
        if ((! entry->paused) && (! entry->markedForDeletion))
        {
            entry->callback(dt);
        }
    }

    // Iterate over all the custom selectors
    for (auto it = _hashForTimers.begin(), end = _hashForTimers.end(); it != end; )
    {
        auto target = it->first;
        _currentTarget = it->first;
        _currentTargetSalvaged = false;

        auto elt = it->second.get();

        if (! elt->paused)
        {
            // The 'timers' array may change while inside this loop
            for (elt->timerIndex = 0; elt->timerIndex < static_cast<int>(elt->timers.size()); elt->timerIndex++)
            {
                elt->currentTimer = to_retaining_ptr<Timer>(elt->timers[elt->timerIndex].get());
                elt->currentTimerSalvaged = false;
                elt->currentTimer->update(dt);
                elt->currentTimer.reset();
            }
        }

        it = _hashForTimers.find(_currentTarget);

        CC_ASSERT(it != _hashForTimers.end());

        // only delete currentTarget if no actions were scheduled during the cycle (issue #481)
        if (_currentTargetSalvaged && elt->timers.empty())
        {
            CC_ASSERT(it == _hashForTimers.find(target));
            it = _hashForTimers.erase(it);
        }
        else
        {
            it++;
        }
    }

    // delete all updates that are marked for deletion
    // updates with priority < 0
    DL_FOREACH_SAFE(_updatesNegList, entry, tmp)
    {
        if (entry->markedForDeletion)
        {
            this->removeUpdateFromHash(entry);
        }
    }

    // updates with priority == 0
    DL_FOREACH_SAFE(_updates0List, entry, tmp)
    {
        if (entry->markedForDeletion)
        {
            this->removeUpdateFromHash(entry);
        }
    }

    // updates with priority > 0
    DL_FOREACH_SAFE(_updatesPosList, entry, tmp)
    {
        if (entry->markedForDeletion)
        {
            this->removeUpdateFromHash(entry);
        }
    }

    _updateHashLocked = false;
    _currentTarget = nullptr;

    //
    // Functions allocated from another thread
    //

    // Testing size is faster than locking / unlocking.
    // And almost never there will be functions scheduled to be called.
    if( !_functionsToPerform.empty() ) {
        _performMutex.lock();
        // fixed #4123: Save the callback functions, they must be invoked after '_performMutex.unlock()', otherwise if new functions are added in callback, it will cause thread deadlock.
        auto temp = _functionsToPerform;
        _functionsToPerform.clear();
        _performMutex.unlock();
        for( const auto &function : temp ) {
            function();
        }
        
    }
}

void Scheduler::schedule(SEL_SCHEDULE selector, Ref *target, float interval, unsigned int repeat, float delay, bool paused)
{
    CCASSERT(target, "Argument target must be non-nullptr");
    
    auto & element = _hashForTimers[target];

    if (!element)
    {
        element.reset(new tHashTimerEntry);
        element->paused = paused;
    }
    else
    {
        CCASSERT(element->paused == paused, "element's paused should be paused!");
    }

    for (auto & t : element->timers)
    {
        TimerTargetSelector *timer = dynamic_cast<TimerTargetSelector*>(t.get());

        if (timer && selector == timer->getSelector())
        {
            CCLOG("CCScheduler#scheduleSelector. Selector already scheduled. Updating interval from: %.4f to %.4f", timer->getInterval(), interval);
            timer->setInterval(interval);
            return;
        }        
    }

    auto timer = to_retaining_ptr(new (std::nothrow) TimerTargetSelector());
    timer->initWithSelector(this, selector, target, interval, repeat, delay);
    element->timers.push_back( retaining_ptr<Timer>( timer.release()));
}

void Scheduler::schedule(SEL_SCHEDULE selector, Ref *target, float interval, bool paused)
{
    this->schedule(selector, target, interval, CC_REPEAT_FOREVER, 0.0f, paused);
}

bool Scheduler::isScheduled(SEL_SCHEDULE selector, Ref *target)
{
    CCASSERT(selector, "Argument selector must be non-nullptr");
    CCASSERT(target, "Argument target must be non-nullptr");
    
    auto element_it = _hashForTimers.find(target);

    if (_hashForTimers.end() != element_it)
    {
        auto & timers = element_it->second->timers;

        for (auto it = timers.begin(), end = timers.end(); it != end; it++)
        {
            TimerTargetSelector *timer = dynamic_cast<TimerTargetSelector*>(it->get());

            if (timer && selector == timer->getSelector())
            {
                return true;
            }
        }
    }

    return false;
}

} // namespace cocos2d
