#pragma once

#include <QUrl>
#include <QHash>

struct CrawlItem{
    QUrl url;
    int depth;

    bool operator==(const CrawlItem&) const = default;
};

inline size_t qHash(const CrawlItem& item, size_t seed = 0) noexcept {
    return qHashMulti(seed, item.url, item.depth);
}
