# Daemon Methods (same functions with node with extra parameter: starting node)

- [ ] join(node_id:unsigned)
- [ ] depart(node_id:unsigned)
- [ ] insert(key:string, value:string)
- [ ] query(key:string): value:string
- [ ] delete(key:string)
- [ ] hash(key:string): value:unsigned (same with id, node_id)
- [ ] status()
- [x] start()
- [x] terminate()
- [ ]   terminate all nodes on exit
- [x] init_node()
- [ ] terminate_node()
- [ ] list_nodes()
- [ ] list_commands()

# Node Methods

- [x] join(node_id:unsigned)
- [x] depart(node_id:unsigned)
- [x] send_keys(node_id:unsigned, dictionary{key:value})
- [x] insert(key:string, value:string)
- [x] query(key:string): value:string
- [x] answer(value:string, node_id:unsigned)
- [x] delete(key:string)
- [x] hash(key:string): value:unsigned (same with id, node_id)

# Inter-Node communication methods

- [x] listen():interruptible or event_driven
- [x] create_socket(node_id:unsigned=null): socket_id:socket

# Definitions

- message (command,sender,[args]): json
- node    (node_id, previous_node, next_node, listening_socket, dictionary{key:value})
- daemon  (node_table:{node_id:socket_id}, command_list)
