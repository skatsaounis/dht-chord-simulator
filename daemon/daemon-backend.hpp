#ifndef _DSEMU_DAEMON_BACKEND_HPP_
#define _DSEMU_DAEMON_BACKEND_HPP_

#include "commands.hpp"
#include <vector>
#include <queue>
#include <string>
#include <unordered_map>

class MessageQueue {
    bool _m_is_open = false;
    int sfd = 0;
    std::string _m_socket_path;
    std::deque<std::string> message_queue;

    std::string _get_message() const;

public:

    class iterator
    {
    public:
        typedef unsigned difference_type;
        typedef std::string value_type;
        typedef std::string* pointer;
        typedef std::string& reference;
        typedef std::input_iterator_tag iterator_category;

        typedef std::deque<value_type> container;

    private:
        MessageQueue& parent;
        container& mqueue;
        container recycled;
        container::iterator current;
        std::string _m_message;
        bool _m_has_message;
        bool _m_is_endit = false;

        bool is_end() const;
        reference message();

    public:
        static iterator createEnd(MessageQueue& parent_queue, container& msg_queue);

        iterator(MessageQueue& parent_queue, container& msg_queue);
        ~iterator();

        bool operator==(const iterator&);
        bool operator!=(const iterator&);

        iterator& operator++();
        reference operator*();
        pointer operator->();

        void recycle_message();
    };

    ~MessageQueue();

    bool is_open() const;

    void open(std::string socket_path);
    void close();

    iterator begin();
    iterator end();
};


class Daemon {
    unsigned _m_replica_factor = 0;
    std::unordered_map<std::string, unsigned> node_ids;

    void _send_message(const std::string& node_id, const std::string& msg) const;
    void _send_terminate(const std::string& node_id) const;
    void _wait_for_notify(Commands) const;
    std::string _pick_random_node();
    std::string _pick_random_node(const std::string& excluded_node_id);

public:
    mutable MessageQueue message_queue;

    Daemon();

    unsigned get_replica_factor() const;
    void set_replica_factor(unsigned);

    void terminate();
    void init_node(const std::string& node_id, const std::string& replica_factor,
        const std::string& consistency, unsigned verbosity);
    void terminate_node(const std::string& node_id);
    void list_nodes();
    void list_ring(const std::string& node_id);
    void list_all_nodes();
    void join(const std::string& node_id);
    void depart(const std::string& node_id);
    void query(const std::string& key);
    void insert(const std::string& key, const std::string& value);
    void remove(const std::string& key);
};

#endif  // _DSEMU_DAEMON_BACKEND_HPP_
