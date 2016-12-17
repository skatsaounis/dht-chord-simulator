# Daemon Methods (same functions with node with extra parameter: starting node)

- [ ] join(node_id:unsigned)
- [ ] depart(node_id:unsigned)
- [ ] insert(key:string, value:string)
- [ ] query(key:string): value:string
- [ ] delete(key:string)
- [ ] hash(key:string): value:unsigned (same with id, node_id)
- [ ] status()
- [ ] start()
- [ ] terminate()
- [ ] list_nodes()
- [ ] list_commands()

# Node Methods

- [ ] join(node_id:unsigned)
- [ ] depart(node_id:unsigned)
- [ ] insert(key:string, value:string)
- [ ] query(key:string): value:string
- [ ] answer(value:string, node_id:unsigned)
- [ ] delete(key:string)
- [ ] hash(key:string): value:unsigned (same with id, node_id)

# Inter-Node communication methods

- [ ] listen():interruptible or event_driven
- [ ] create_socket(node_id:unsigned=null): socket_id:socket

# Definitions

- message (size,sender,command,[args]): json
- node    (node_id, previous_node, next_node, listening_socket, dictionary{key:value})
- daemon  (node_table:{node_id:socket_id}, command_list)