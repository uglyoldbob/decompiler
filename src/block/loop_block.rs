//! Contains code for loops.

use super::{Block, BlockEnd, BlockTrait, Register, SimplifiedBlock, SpawnError, Value};

#[derive(Clone, serde::Serialize, serde::Deserialize)]
/// A simple while block with an executing body.
pub struct SimpleWhileBlock {
    /// The conditional for the loop
    block: Box<Block>,
    /// The meat of the while loop
    meat: Box<Block>,
    /// Indicates the condition of the block is reverse logic
    reverse: bool,
}

impl SimpleWhileBlock {
    /// Try to create a simple while block from the given group of blocks
    pub fn try_create(
        simplified: &Vec<SimplifiedBlock>,
        head: Option<&SimplifiedBlock>,
        g: &mut super::graph::Graph<Block>,
        notes: &mut Vec<String>,
    ) -> bool {
        if simplified.len() == 2 {
            if let Some(h) = head {
                let mut ablock = None;
                let mut bblock = None;
                if let BlockEnd::Branch(a, b) = h.end {
                    if let Some(a) = a.to_u64() {
                        for e in simplified {
                            if e.start == a {
                                ablock = Some(e);
                            }
                        }
                    }
                    if let Some(b) = b.to_u64() {
                        for e in simplified {
                            if e.start == b {
                                bblock = Some(e);
                            }
                        }
                    }
                }
                if let Some(ablock) = ablock {
                    notes.push("Checking block a for while loop\n".to_string());
                    if let BlockEnd::Single(a) = ablock.end {
                        notes.push("While loop maybe1\n".to_string());
                        if let Some(a) = a.to_u64() {
                            if a == h.start {
                                notes.push("Found while loop\n".to_string());
                                let nb = SimpleWhileBlock {
                                    block: Box::new(g.elements.take(h.index).unwrap()),
                                    meat: Box::new(g.elements.take(ablock.index).unwrap()),
                                    reverse: false,
                                };
                                let nb = Block::SimpleWhileLoop(nb);
                                g.elements.insert(nb);
                                return true;
                            }
                        }
                    }
                }
                if let Some(bblock) = bblock {
                    notes.push("Checking block b for while loop\n".to_string());
                    if let BlockEnd::Single(a) = bblock.end {
                        notes.push("While loop maybe2\n".to_string());
                        if let Some(a) = a.to_u64() {
                            if a == h.start {
                                notes.push("Found reverse logic while loop\n".to_string());
                                let nb = SimpleWhileBlock {
                                    block: Box::new(g.elements.take(h.index).unwrap()),
                                    meat: Box::new(g.elements.take(bblock.index).unwrap()),
                                    reverse: true,
                                };
                                let nb = Block::SimpleWhileLoop(nb);
                                g.elements.insert(nb);
                                return true;
                            }
                        }
                    }
                }
            }
        }
        false
    }
}

impl BlockTrait for SimpleWhileBlock {
    #[doc = " Return the address of the head of this block"]
    fn address(&self) -> u64 {
        self.block.address()
    }

    #[doc = " Calculate the next address or addresses for this block"]
    fn calc_next(&self) -> BlockEnd {
        if self.reverse {
            if let BlockEnd::Branch(a, b) = self.block.calc_next() {
                BlockEnd::Single(a)
            } else {
                panic!("Invalid while loop detected");
            }
        } else {
            if let BlockEnd::Branch(a, b) = self.block.calc_next() {
                BlockEnd::Single(b)
            } else {
                panic!("Invalid while loop detected");
            }
        }
    }

    #[doc = " Does this block contain an instruction that starts at the specified address?"]
    fn contains(&self, addr: u64) -> bool {
        self.block.contains(addr) || self.meat.contains(addr)
    }

    #[doc = " Spawn another block from the specified address in this block"]
    fn spawn(&mut self, addr: u64) -> Result<Block, SpawnError> {
        Err(SpawnError::CannotSpawn)
    }

    #[doc = " Print the source code for the block, with the specified level of indents"]
    fn write_source(&self, level: u8, w: &mut impl std::io::Write) -> Result<(), std::io::Error> {
        todo!()
    }

    #[doc = " Attempt to find the value of the given register, over all instructions of the block."]
    fn trace_register_all(&self, reg: Register) -> Value {
        todo!()
    }

    #[doc = " Returns the conditional branch of the block, if one exists"]
    fn branch_value(&self) -> Option<Value> {
        None
    }

    #[doc = " Attempt to set the blockend for the graph"]
    fn set_block_end(&mut self, be: BlockEnd) -> Result<(), ()> {
        Err(())
    }

    #[doc = " Add applicable statements to the dot graph for this block, s refers if the simplified or expanded dot is created."]
    fn dot_add(&self, g: &mut graphviz_rust::dot_structures::Graph, s: bool) {
        if s {
            super::dot_add_node(g, self.address());
            if let BlockEnd::Single(a) = self.calc_next() {
                super::dot_add_link(g, Value::Bits64(self.address()), a);
            }
        } else {
            self.block.dot_add(g, s);
        }
    }

    fn is_function_head(&self) -> bool {
        self.block.is_function_head()
    }
}

#[derive(Clone, serde::Serialize, serde::Deserialize)]
/// An infinite loop with no escape
pub struct InfiniteLoopBlock {
    /// The single block for the loop
    block: Box<Block>,
}

impl InfiniteLoopBlock {
    /// Try to create a Self block from the elements specified in simplified
    pub fn try_create(
        simplified: &Vec<SimplifiedBlock>,
        head: Option<&SimplifiedBlock>,
        g: &mut super::graph::Graph<Block>,
        notes: &mut Vec<String>,
    ) -> bool {
        if simplified.len() == 1 {
            if let Some(h) = head {
                if let BlockEnd::Single(a) = h.end {
                    if a.is_known() {
                        let a = a.to_u64().unwrap();
                        if h.start == a {
                            notes.push("Infinite loop detected\n".to_string());
                            let block = Box::new(g.elements.take(h.index).unwrap());
                            let nb = Block::InfiniteLoop(InfiniteLoopBlock { block });
                            g.elements.insert(nb);
                            return true;
                        }
                    }
                }
            }
        }
        false
    }
}

impl BlockTrait for InfiniteLoopBlock {
    #[doc = " Return the address of the head of this block"]
    fn address(&self) -> u64 {
        self.block.address()
    }

    #[doc = " Calculate the next address or addresses for this block"]
    fn calc_next(&self) -> BlockEnd {
        BlockEnd::None
    }

    #[doc = " Does this block contain an instruction that starts at the specified address?"]
    fn contains(&self, addr: u64) -> bool {
        self.block.contains(addr)
    }

    #[doc = " Spawn another block from the specified address in this block"]
    fn spawn(&mut self, addr: u64) -> Result<Block, SpawnError> {
        Err(SpawnError::CannotSpawn)
    }

    #[doc = " Print the source code for the block, with the specified level of indents"]
    fn write_source(&self, level: u8, w: &mut impl std::io::Write) -> Result<(), std::io::Error> {
        todo!();
    }

    #[doc = " Attempt to find the value of the given register, over all instructions of the block."]
    fn trace_register_all(&self, reg: Register) -> Value {
        todo!()
    }

    #[doc = " Returns the conditional branch of the block, if one exists"]
    fn branch_value(&self) -> Option<Value> {
        None
    }

    #[doc = " Attempt to set the blockend for the graph"]
    fn set_block_end(&mut self, be: BlockEnd) -> Result<(), ()> {
        Err(())
    }

    #[doc = " Add applicable statements to the dot graph for this block"]
    fn dot_add(&self, g: &mut graphviz_rust::dot_structures::Graph, s: bool) {
        if s {
            super::dot_add_node(g, self.address());
        } else {
            self.block.dot_add(g, s);
        }
    }

    fn is_function_head(&self) -> bool {
        self.block.is_function_head()
    }
}

#[derive(Clone, serde::Serialize, serde::Deserialize)]
/// A simple do while block with a single condition for repeating
pub struct SimpleDoWhileBlock {
    /// The single block for the loop, containing both the operational statements and the conditional
    block: Box<Block>,
    /// Indicates that the condition is reversed
    reversed: bool,
}

impl SimpleDoWhileBlock {
    /// Try to create a Self block from the elements specified in simplified
    pub fn try_create(
        simplified: &Vec<SimplifiedBlock>,
        head: Option<&SimplifiedBlock>,
        g: &mut super::graph::Graph<Block>,
        notes: &mut Vec<String>,
    ) -> bool {
        if simplified.len() == 1 {
            if let Some(h) = head {
                notes.push(format!("Head end is {:?}\n", h.end));
                if let BlockEnd::Branch(a, b) = h.end {
                    notes.push("Branch detected for potential do while loop\n".to_string());
                    if a.is_known() {
                        let a = a.to_u64().unwrap();
                        if a == h.start {
                            notes.push("Loop detected for do while loop\n".to_string());
                            let block = Box::new(g.elements.take(h.index).unwrap());
                            let nb = Block::SimpleDoWhile(SimpleDoWhileBlock {
                                block,
                                reversed: false,
                            });
                            g.elements.insert(nb);
                            return true;
                        }
                    }
                    if b.is_known() {
                        let a = b.to_u64().unwrap();
                        if a == h.start {
                            notes.push("Reversed Loop detected for do while loop\n".to_string());
                            let block = Box::new(g.elements.take(h.index).unwrap());
                            let nb = Block::SimpleDoWhile(SimpleDoWhileBlock {
                                block,
                                reversed: true,
                            });
                            g.elements.insert(nb);
                            return true;
                        }
                    }
                }
            } else {
                notes.push("No head detected?\n".to_string());
            }
        }
        false
    }
}

impl BlockTrait for SimpleDoWhileBlock {
    #[doc = " Return the address of the head of this block"]
    fn address(&self) -> u64 {
        self.block.address()
    }

    #[doc = " Calculate the next address or addresses for this block"]
    fn calc_next(&self) -> BlockEnd {
        match self.block.calc_next() {
            BlockEnd::None => panic!("Invalid do while loop detected"),
            BlockEnd::Single(_) => panic!("Invalid do while loop detected"),
            BlockEnd::Branch(a, b) => {
                if self.reversed {
                    BlockEnd::Single(a)
                } else {
                    BlockEnd::Single(b)
                }
            }
        }
    }

    #[doc = " Does this block contain an instruction that starts at the specified address?"]
    fn contains(&self, addr: u64) -> bool {
        self.block.contains(addr)
    }

    #[doc = " Spawn another block from the specified address in this block"]
    fn spawn(&mut self, addr: u64) -> Result<Block, SpawnError> {
        Err(SpawnError::CannotSpawn)
    }

    #[doc = " Print the source code for the block, with the specified level of indents"]
    fn write_source(&self, level: u8, w: &mut impl std::io::Write) -> Result<(), std::io::Error> {
        self.indent(level, w)?;
        w.write_all("do {\n".as_bytes())?;
        self.block.write_source(level + 1, w)?;
        self.indent(level, w)?;
        w.write_all("} while (?);\n".as_bytes())?;
        Ok(())
    }

    #[doc = " Attempt to find the value of the given register, over all instructions of the block."]
    fn trace_register_all(&self, _reg: Register) -> Value {
        //TODO: try to trace register values
        Value::Unknown
    }

    #[doc = " Returns the conditional branch of the block, if one exists"]
    fn branch_value(&self) -> Option<Value> {
        None
    }

    #[doc = " Attempt to set the blockend for the graph"]
    fn set_block_end(&mut self, _be: BlockEnd) -> Result<(), ()> {
        Err(())
    }

    fn dot_add(&self, g: &mut graphviz_rust::dot_structures::Graph, s: bool) {
        if s {
            super::dot_add_node(g, self.address());
            if let BlockEnd::Single(a) = self.calc_next() {
                super::dot_add_link(g, Value::Bits64(self.address()), a);
            }
        } else {
            self.block.dot_add(g, s);
        }
    }

    fn is_function_head(&self) -> bool {
        self.block.is_function_head()
    }
}
