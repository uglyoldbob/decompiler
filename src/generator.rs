//! This module contains code for creating arbitrary graphs of code for testing the decompiler and other aids for a user.

use graphviz_rust::dot_generator::*;
use graphviz_rust::dot_structures::Graph;
use graphviz_rust::dot_structures::*;

/// An iterator that produces a sequence of graphs with a specified number of nodes.
pub struct GraphIterator {
    /// The generator containing the number of nodes to build.
    gg: GraphGenerator,
    /// The links between all of the nodes.
    links: Vec<(Option<u32>, Option<u32>)>,
    /// Indicates the iterator is done and no more graphs will be generated.
    done: bool,
}

impl GraphIterator {
    /// Advance the iterator to the next graph.
    fn advance(&mut self) {
        if self.done {
            return;
        }
        for (elem, elem2) in self.links.iter_mut() {
            *elem = match *elem {
                None => Some(0),
                Some(a) => {
                    if (a + 1) < self.gg.num_blocks {
                        Some(a + 1)
                    } else {
                        None
                    }
                }
            };
            if elem.is_none() {
                *elem2 = match *elem2 {
                    None => Some(0),
                    Some(a) => {
                        if (a + 1) < self.gg.num_blocks {
                            Some(a + 1)
                        } else {
                            None
                        }
                    }
                };
                if elem2.is_some() {
                    break;
                }
            } else {
                break;
            }
        }
    }

    /// Check to see if the current graph is valid.
    pub fn check(&self) -> bool {
        let mut valid = true;
        for (a, b) in self.links.iter() {
            if a == b && a.is_some() {
                valid = false;
            }
            if a.is_none() && b.is_some() {
                valid = false;
            }
            if let Some(a) = a {
                if let Some(b) = b {
                    if a > b {
                        valid = false;
                    }
                }
            }
        }

        let (a, b) = self.links[self.gg.num_blocks as usize - 1];
        if a.is_some() || b.is_some() {
            valid = false;
        }

        let mut inputs = vec![0; self.gg.num_blocks as usize];
        for (index, (elem, elem2)) in self.links.iter().enumerate() {
            if let Some(a) = *elem {
                if index != a as usize {
                    inputs[a as usize] += 1;
                }
            }
            if let Some(a) = *elem2 {
                if index != a as usize {
                    inputs[a as usize] += 1;
                }
            }
        }
        for i in inputs.iter().skip(1) {
            if *i == 0 {
                valid = false;
            }
        }

        let mut check = Vec::new();
        let mut traced = vec![false; self.gg.num_blocks as usize];
        check.push(0);
        while let Some(index) = check.pop() {
            traced[index] = true;
            let node = self.links[index];
            if let Some(a) = node.0 {
                if !traced[a as usize] {
                    check.push(a as usize);
                }
            }
            if let Some(b) = node.1 {
                if !traced[b as usize] {
                    check.push(b as usize);
                }
            }
        }
        if !traced.iter().fold(true, |a, b| a & b) {
            valid = false;
        }

        valid
    }

    /// Generate graphs by advancing until a valid graph is found or the iterator is done.
    pub fn checked_advance(&mut self) {
        loop {
            self.advance();
            if self.done() {
                self.done = true;
                break;
            }
            if self.check() {
                break;
            }
        }
    }

    /// Is the iterator done?
    fn done(&self) -> bool {
        let mut done = true;
        for (a, b) in self.links.iter() {
            if a.is_some() || b.is_some() {
                done = false;
                break;
            }
        }
        done
    }
}

impl Iterator for GraphIterator {
    type Item = Graph;

    fn next(&mut self) -> Option<Self::Item> {
        self.checked_advance();
        let mut g = graph!(di id!("generated"));
        for i in 0..self.gg.num_blocks {
            let s = format!("{}", i);
            let id = graphviz_rust::dot_structures::Id::Plain(s);
            let id = graphviz_rust::dot_structures::NodeId(id, None);
            let e = graphviz_rust::dot_structures::Node {
                id,
                attributes: vec![],
            };
            g.add_stmt(graphviz_rust::dot_structures::Stmt::Node(e));
        }
        for (i, (la, lb)) in self.links.iter().enumerate() {
            if let Some(link) = la {
                let s = format!("{}", i);
                let id = graphviz_rust::dot_structures::Id::Plain(s);
                let id1 = graphviz_rust::dot_structures::NodeId(id, None);

                let s = format!("{}", link);
                let id = graphviz_rust::dot_structures::Id::Plain(s);
                let id2 = graphviz_rust::dot_structures::NodeId(id, None);

                let va = graphviz_rust::dot_structures::Vertex::N(id1);
                let vb = graphviz_rust::dot_structures::Vertex::N(id2);
                let t = graphviz_rust::dot_structures::EdgeTy::Pair(va, vb);
                let e = graphviz_rust::dot_structures::Edge {
                    ty: t,
                    attributes: Vec::new(),
                };
                g.add_stmt(graphviz_rust::dot_structures::Stmt::Edge(e));
            }
            if let Some(link) = lb {
                let s = format!("{}", i);
                let id = graphviz_rust::dot_structures::Id::Plain(s);
                let id1 = graphviz_rust::dot_structures::NodeId(id, None);

                let s = format!("{}", link);
                let id = graphviz_rust::dot_structures::Id::Plain(s);
                let id2 = graphviz_rust::dot_structures::NodeId(id, None);

                let va = graphviz_rust::dot_structures::Vertex::N(id1);
                let vb = graphviz_rust::dot_structures::Vertex::N(id2);
                let t = graphviz_rust::dot_structures::EdgeTy::Pair(va, vb);
                let e = graphviz_rust::dot_structures::Edge {
                    ty: t,
                    attributes: Vec::new(),
                };
                g.add_stmt(graphviz_rust::dot_structures::Stmt::Edge(e));
            }
        }
        if self.done() {
            None
        } else {
            Some(g)
        }
    }
}

#[derive(Copy, Clone)]
/// A graph generator that will generate the specified number of nodes for every graph in its iterator.
pub struct GraphGenerator {
    /// The number of blocks to create in each graph.
    num_blocks: u32,
}

impl GraphGenerator {
    /// Return the number of nodes that will be generated.
    pub fn num_blocks(&self) -> u32 {
        self.num_blocks
    }

    /// Create a new self with the specified number of nodes.
    pub fn new(num_blocks: u32) -> Self {
        Self { num_blocks }
    }

    /// Create the iterator that does the actual work of generating graphs.
    pub fn create_iter(&self) -> GraphIterator {
        GraphIterator {
            gg: *self,
            links: vec![(None, None); self.num_blocks as usize],
            done: false,
        }
    }
}
