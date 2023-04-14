#include <server/engineServer.hpp>

#include <cstring>      // Unix  socket datagram bind
#include <fcntl.h>      // Unix socket datagram bind
#include <sys/socket.h> // Unix socket datagram bind
#include <sys/un.h>     // Unix socket datagram bind
#include <unistd.h>     // Unix socket datagram bind

#include <exception>

#include <logging/logging.hpp>

namespace
{ /**
   * @brief Change the size of the thread pool worker of libuv (UV_THREADPOOL_SIZE)
   *
   * @param newSize The new size of the thread pool worker
   * @throw std::runtime_error If the new size is invalid or if the new size could not be set.
   */
void changeUVTreadPoolWorkerSize(int newSize)
{
    // Check if the new size is valid [MAX_THREADPOOL_SIZE == 1024]
    if (newSize < 1 || newSize > 1024)
    {
        throw std::runtime_error("Invalid thread pool worker size.");
    }

    // Convertir el tamaño del grupo de hilos de trabajo a una cadena
    std::string newSizeStr = std::to_string(newSize);

    // Set the new size for the thread pool worker
    if (setenv("UV_THREADPOOL_SIZE", newSizeStr.c_str(), true) != 0)
    {
        throw std::runtime_error("Could not set the new thread pool worker size.");
    }

    LOG_DEBUG("Thread pool worker size set to {}", newSize);
}
} // namespace

namespace engineserver
{

EngineServer::EngineServer(int threadPoolSize)
{
    // Change the size of the thread pool worker
    changeUVTreadPoolWorkerSize(threadPoolSize);

    m_loop = uvw::Loop::getDefault();
    m_status = Status::STOPPED;

    m_stopHandle = m_loop->resource<uvw::AsyncHandle>();
    m_stopHandle->on<uvw::AsyncEvent>(
        [this](const uvw::AsyncEvent&, uvw::AsyncHandle&)
        {
            this->stop();
            this->m_status = Status::STOPPED;
        });

    m_endpoints = std::unordered_map<std::string, std::shared_ptr<Endpoint>>();

    m_loop->on<uvw::ErrorEvent>(
        [](const uvw::ErrorEvent& e, uvw::Loop&)
        {
            LOG_ERROR("Error: {} - {}", e.name(), e.what());
        });
}

EngineServer::~EngineServer()
{
    this->stop();
};

void EngineServer::start()
{
    LOG_INFO("Starting the server...");
    m_status = Status::RUNNING;
    m_loop->run<uvw::Loop::Mode::DEFAULT>();
    LOG_INFO("Server stopped");
}

void EngineServer::stop()
{
    LOG_INFO("Stopping the server");
    m_loop->walk([](auto& handle) { handle.close(); });
    m_loop->stop();
    // FIXME wait for workers to finish before closing the loop
    // This throws segfault when closing the loop
    this->m_loop->close();
    LOG_INFO("Server closed");
}

void EngineServer::request_stop()
{
    LOG_DEBUG("Requesting stop");
    m_stopHandle->send();
}

void EngineServer::addEndpoint(const std::string& name, std::shared_ptr<Endpoint> endpoint)
{
    LOG_DEBUG("Adding endpoint {}", name);
    // first check if the endpoint already exists
    if (m_endpoints.find(name) != m_endpoints.end())
    {
        throw std::runtime_error(fmt::format("Endpoint {} already exists", name));
    }
    // add the endpoint
    endpoint->bind(m_loop);
    m_endpoints[name] = endpoint;
}
} // namespace engineserver
