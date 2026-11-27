#include "networkmonitor.h"

#include <ifaddrs.h>
#include <net/if.h>

#ifdef Q_OS_MACOS
#include <net/if_var.h>
#elif defined(Q_OS_LINUX)
#include <linux/if_link.h>
#endif

QPair<quint64, quint64> NetworkMonitor::readByteCounters()
{
    quint64 received = 0;
    quint64 transmitted = 0;
    ifaddrs *interfaces = nullptr;
    if (getifaddrs(&interfaces) != 0) {
        lastReadFailed_ = true;
        return {0, 0};
    }
    lastReadFailed_ = false;

    for (auto *entry = interfaces; entry; entry = entry->ifa_next) {
        if (!entry->ifa_data || !entry->ifa_name || (entry->ifa_flags & IFF_LOOPBACK))
            continue;
#ifdef Q_OS_MACOS
        if (!entry->ifa_addr || entry->ifa_addr->sa_family != AF_LINK)
            continue;
        const auto *data = static_cast<const if_data *>(entry->ifa_data);
        received += data->ifi_ibytes;
        transmitted += data->ifi_obytes;
#elif defined(Q_OS_LINUX)
        if (!entry->ifa_addr || entry->ifa_addr->sa_family != AF_PACKET)
            continue;
        const auto *data = static_cast<const rtnl_link_stats *>(entry->ifa_data);
        received += data->rx_bytes;
        transmitted += data->tx_bytes;
#endif
    }
    freeifaddrs(interfaces);
    return {received, transmitted};
}

bool NetworkMonitor::isAvailable() const
{
    return true;
}

QPair<double, double> NetworkMonitor::sampleMbps()
{
    const auto counters = readByteCounters();
    if (!initialized_) {
        previousRx_ = counters.first;
        previousTx_ = counters.second;
        elapsed_.start();
        initialized_ = true;
        return {0.0, 0.0};
    }

    const qint64 milliseconds = elapsed_.restart();
    if (milliseconds <= 0)
        return {0.0, 0.0};
    const double seconds = milliseconds / 1000.0;
    const double incoming = counters.first >= previousRx_
        ? (counters.first - previousRx_) * 8.0 / seconds / 1'000'000.0 : 0.0;
    const double outgoing = counters.second >= previousTx_
        ? (counters.second - previousTx_) * 8.0 / seconds / 1'000'000.0 : 0.0;
    previousRx_ = counters.first;
    previousTx_ = counters.second;
    return {incoming, outgoing};
}