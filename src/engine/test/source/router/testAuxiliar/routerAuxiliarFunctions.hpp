#ifndef _ROUTER_AUX_FUNCTIONS_H
#define _ROUTER_AUX_FUNCTIONS_H

#include <parseEvent.hpp>
#include <queue/concurrentQueue.hpp>

#include "utils/stringUtils.hpp"
#include <builder.hpp>
#include <registry.hpp>

#include <mocks/fakeMetric.hpp>

namespace aux
{
std::shared_ptr<builder::Builder> getFakeBuilder();
std::shared_ptr<store::IStore> getFakeStore();

const std::vector<std::string> sampleEventsStr {
    R"(2:10.0.0.1:Test Event - deco_1 )", R"(4:10.0.0.1:Test Event - deco_2 )", R"(8:10.0.0.1:Test Event - deco_3 )"};

base::Event createFakeMessage(std::optional<std::string> msgOpt = std::nullopt);

struct testQueue
{
    std::shared_ptr<base::queue::ConcurrentQueue<base::Event>> m_eventQueue;

    std::shared_ptr<base::queue::ConcurrentQueue<base::Event>> getQueue()
    {
        if (m_eventQueue == nullptr)
        {
            m_eventQueue = std::make_shared<base::queue::ConcurrentQueue<base::Event>>(
                100, std::make_shared<FakeMetricScope>(), std::make_shared<FakeMetricScope>());
        }
        return m_eventQueue;
    }

    void pushEvent(const base::Event& event)
    {
        auto e = event;
        getQueue()->push(std::move(e));
    }
};

} // namespace aux

#endif // _ROUTER_AUX_FUNCTIONS_H
