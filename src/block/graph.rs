//! This module defines a graph with arbitray links contained between the elements.

use std::io::Write;

use graphviz_rust::printer::{DotPrinter, PrinterContext};

use super::{Block, BlockEnd, BlockTrait, GeneratedBlock, Instruction, Value};
use crate::map::AutoHashMap;

#[derive(Clone, serde::Serialize, serde::Deserialize)]
/// An arbitrary graph of some type of object, where each object points to zero or more other objects in the graph.
pub struct Graph<T> {
    /// The elements in the graph
    pub elements: AutoHashMap<T>,
}

impl<T> Graph<T> {
    /// Construct a blank graph
    pub fn new() -> Self {
        Self {
            elements: AutoHashMap::new(),
        }
    }

    /// Returns the number of blocks in the graph
    pub fn num_blocks(&self) -> usize {
        self.elements.len()
    }

    /// Iterate over the elements in the graph
    pub fn iter(&self) -> std::collections::hash_map::Iter<'_, usize, T> {
        self.elements.iter()
    }
}

impl From<graphviz_rust::dot_structures::Graph> for Graph<Block> {
    fn from(value: graphviz_rust::dot_structures::Graph) -> Self {
        let mut elements: AutoHashMap<Block> = AutoHashMap::new();
        let check_add_node = |elements: &mut AutoHashMap<Block>, nid| {
            if let graphviz_rust::dot_structures::Vertex::N(id) = nid {
                let id = id.0;
                if let graphviz_rust::dot_structures::Id::Plain(sid) = id {
                    let addr: u64 = sid.parse().expect("Non numeric id found in graph");
                    let mut found = false;
                    for e in elements.values() {
                        if e.address() == addr {
                            found = true;
                            break;
                        }
                    }
                    if !found {
                        let new_ele = Block::Generated(GeneratedBlock {
                            address: addr,
                            end: BlockEnd::None,
                            head: addr == 0,
                        });
                        elements.insert(new_ele);
                    }
                }
            }
        };
        let add_link = |elements: &mut AutoHashMap<Block>, a, b| {
            let mut addr_a = None;
            if let graphviz_rust::dot_structures::Vertex::N(id) = a {
                let id = id.0;
                if let graphviz_rust::dot_structures::Id::Plain(sid) = id {
                    let addr: u64 = sid.parse().expect("Non numeric id found in graph");
                    addr_a = Some(addr);
                }
            }
            let mut addr_b = None;
            if let graphviz_rust::dot_structures::Vertex::N(id) = b {
                let id = id.0;
                if let graphviz_rust::dot_structures::Id::Plain(sid) = id {
                    let addr: u64 = sid.parse().expect("Non numeric id found in graph");
                    addr_b = Some(addr);
                }
            }
            if let Some(a) = addr_a {
                if let Some(b) = addr_b {
                    let mut elem = None;
                    for e in elements.values_mut() {
                        if e.address() == a {
                            elem = Some(e);
                            break;
                        }
                    }
                    if let Some(elem) = elem {
                        let be = match elem.calc_next() {
                            BlockEnd::None => BlockEnd::Single(Value::Bits64(b)),
                            BlockEnd::Single(a) => BlockEnd::Branch(a, Value::Bits64(b)),
                            BlockEnd::Branch(_a, b) => {
                                panic!("Cannot have three links from one node")
                            }
                        };
                        elem.set_block_end(be).unwrap();
                    }
                }
            }
        };
        if let graphviz_rust::dot_structures::Graph::DiGraph {
            id: _,
            strict: _,
            stmts,
        } = value
        {
            for s in stmts {
                match s {
                    graphviz_rust::dot_structures::Stmt::Node(n) => {
                        check_add_node(
                            &mut elements,
                            graphviz_rust::dot_structures::Vertex::N(n.id),
                        );
                    }
                    graphviz_rust::dot_structures::Stmt::Subgraph(_) => {}
                    graphviz_rust::dot_structures::Stmt::Attribute(_) => {}
                    graphviz_rust::dot_structures::Stmt::GAttribute(_) => {}
                    graphviz_rust::dot_structures::Stmt::Edge(e) => {
                        let graphviz_rust::dot_structures::Edge { ty, attributes: _ } = e;
                        if let graphviz_rust::dot_structures::EdgeTy::Pair(a, b) = ty {
                            check_add_node(&mut elements, a.clone());
                            check_add_node(&mut elements, b.clone());
                            add_link(&mut elements, a, b);
                        }
                    }
                }
            }
        } else {
            panic!("Invalid graph type for conversion");
        }
        Self { elements }
    }
}

impl Graph<Block> {
    /// Create a graphviz graph directly
    pub fn create_graph(&self, s: bool) -> graphviz_rust::dot_structures::Graph {
        let mut g = super::make_gv_graph("generated");
        for b in self.elements.values() {
            b.dot_add(&mut g, s);
        }
        g
    }

    /// Write the graph to a dot file
    pub fn write_to_dot(&self, name: &str, f: &mut impl Write) -> Result<(), std::io::Error> {
        let mut unknown = 0;
        f.write_all(format!("digraph {}{{\n", name).as_bytes())?;
        for b in self.elements.values() {
            let start = b.address();
            let next = b.calc_next();
            let s =
                match next {
                    BlockEnd::Single(a) => {
                        if a.is_known() {
                            format!("addr_{:X} -> addr_{:X}\n", start, a)
                        } else {
                            unknown += 1;
                            format!("addr_{:X} -> addr_unknown{:X}\n", start, unknown)
                        }
                    }
                    BlockEnd::Branch(a, b) => {
                        if a.is_known() {
                            if b.is_known() {
                                format!(
                                    "addr_{:X} -> addr_{:X}\naddr_{0:X} -> addr_{2:X}\n",
                                    start, a, b
                                )
                            } else {
                                unknown += 1;
                                format!(
                                    "addr_{:X} -> addr_{:X}\naddr_{0:X} -> addr_unknown{2:X}\n",
                                    start, a, unknown
                                )
                            }
                        } else {
                            if b.is_known() {
                                unknown += 1;
                                format!(
                                    "addr_{:X} -> addr_{:X}\naddr_{0:X} -> addr_unknown{2:X}\n",
                                    start, b, unknown
                                )
                            } else {
                                unknown += 2;
                                format!(
                                "addr_{:X} -> addr_{:X}\naddr_unknown{0:X} -> addr_unknown{2:X}\n",
                                start, unknown-1, unknown
                            )
                            }
                        }
                    }
                    BlockEnd::None => {
                        format!("addr_{:X};\n", start)
                    }
                };
            f.write_all(s.as_bytes())?;
        }
        f.write_all(format!("}}\n").as_bytes())?;
        f.flush()?;
        Ok(())
    }

    /// Track the address of a branch instruction
    pub fn trace_branch(&self, addr: u64) -> Value {
        let mut index_found = None;
        for (index, b) in self.elements.iter() {
            if b.contains(addr) {
                index_found = Some(*index);
            }
        }
        let mut val = Value::Unknown;
        if let Some(index) = index_found {
            let b = self.elements.get(&index).unwrap();
            if let Some(v) = b.branch_value() {
                val = v;
            }
            match val {
                Value::Register(r) => {
                    let val2 = b.trace_register_all(r);
                    if val2.is_known() {
                        return val2;
                    } else {
                        todo!();
                    }
                }
                Value::Unknown => {
                    return Value::Unknown;
                }
                a => {
                    return a;
                }
            }
        } else {
            Value::Unknown
        }
    }

    /// Add the specified instruction to the graph
    pub fn add_instruction(&mut self, i: Instruction) -> Option<BlockEnd> {
        for (_index, b) in self.elements.iter_mut() {
            if b.contains(i.address()) {
                return None;
            } else {
                let n = b.calc_next();
                if let BlockEnd::Single(a) = n {
                    if a.is_known() {
                        let a = a.to_u64().unwrap();
                        if a == i.address() {
                            println!("Instruction at {:x} is {}", i.address(), i);
                            b.add_instruction(i);
                            return Some(b.calc_next());
                        }
                    }
                }
            }
        }

        let mut nb = Block::new_instructions(false);
        println!("Instruction at {:x} is {}", i.address(), i);
        nb.add_instruction(i);
        let n = nb.calc_next();
        self.elements.insert(nb);
        return Some(n);
    }

    /// Process the given address, modifying the graph as required. Returns the next statement or statements.
    pub fn process_address(
        &mut self,
        addr: u64,
        ids: &mut Vec<crate::block::InstructionDecoderPlus>,
    ) -> Option<BlockEnd> {
        for id in ids {
            if id.contains(addr) {
                let decoder = id.decoder();
                decoder.goto(addr);
                if let Some(instru) = decoder.decode() {
                    return self.add_instruction(instru);
                }
            }
        }
        None
    }

    pub fn is_simplified(&self) -> Result<(), ()> {
        if self.elements.len() > 1 {
            Err(())
        } else {
            Ok(())
        }
    }

    /// Simplify the graph as much as possible
    pub fn simplify(&mut self, notes: &mut Vec<String>) -> Result<(), ()> {
        notes.push("SIMPLIFY START WITH:\n".to_string());
        loop {
            let mut work = false;
            for i in 1..=self.elements.len() {
                notes.push(format!(
                    "\nSimplifying {} of {} elements\n",
                    i,
                    self.elements.len()
                ));
                for e in self.elements.values() {
                    notes.push(format!("{:X} ", e.address()));
                }
                notes.push("\n".to_string());
                let combo = self.elements.combo_iter_num(i);
                for c in combo {
                    notes.push(format!("COMBO ({}): ", self.elements.len()));
                    for d in &c {
                        notes.push(format!("{:X} ", self.elements.get(d).unwrap().address()));
                    }
                    notes.push("\n".to_string());
                    let pregraph = self.create_graph(true);
                    if Block::try_create(self, c, notes) {
                        notes.push("\tSimplified: ".to_string());
                        let graph = self.create_graph(true);
                        notes.push(format!(
                            "PREDOT IS {}\n",
                            pregraph.print(&mut PrinterContext::default())
                        ));
                        notes.push(format!(
                            "DOT IS {}\n",
                            graph.print(&mut PrinterContext::default())
                        ));
                        work = true;
                        break;
                    }
                }
                if work {
                    break;
                }
            }
            if !work {
                break;
            }
        }
        self.is_simplified()
    }
}
