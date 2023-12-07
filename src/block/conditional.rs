//! Contains code for conditional code statements

use super::{Block, BlockEnd, BlockTrait, Register, SimplifiedBlock, SpawnError, Value};

#[derive(Clone, serde::Serialize, serde::Deserialize)]
/// An infinite loop with no escape
pub struct IfElse1Block {
    /// The if conditional for the loop
    block: Box<Block>,
    /// The portion executed if the condition is met
    met: Box<Block>,
    /// The portion executed if the condition is not met
    unmet: Box<Block>,
}

impl IfElse1Block {
    /// Try to create a Self block from the elements specified in simplified
    pub fn try_create(
        simplified: &Vec<SimplifiedBlock>,
        head: Option<&SimplifiedBlock>,
        g: &mut super::graph::Graph<Block>,
        notes: &mut Vec<String>,
    ) -> bool {
        if let Some(h) = head {
            let mut ablock = None;
            let mut bblock = None;
            if let BlockEnd::Branch(a, b) = h.end {
                if a.is_known() {
                    let a = a.to_u64().unwrap();
                    for e in simplified {
                        if e.start == a {
                            ablock = Some(e);
                        }
                    }
                }
                if b.is_known() {
                    let a = b.to_u64().unwrap();
                    for e in simplified {
                        if e.start == a {
                            bblock = Some(e);
                        }
                    }
                }
            }
            if let Some(a) = ablock {
                if let Some(b) = bblock {
                    if let BlockEnd::None = a.end {
                        if let BlockEnd::None = b.end {
                            let block_if = g.elements.take(h.index).unwrap();
                            let if_true = g.elements.take(b.index).unwrap();
                            let if_false = g.elements.take(a.index).unwrap();
                            notes.push("if else 1 block detected\n".to_string());
                            let nb = IfElse1Block {
                                block: Box::new(block_if),
                                met: Box::new(if_true),
                                unmet: Box::new(if_false),
                            };
                            let nb = Block::IfElseEnding(nb);
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

impl BlockTrait for IfElse1Block {
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
        self.block.contains(addr) || self.met.contains(addr) || self.unmet.contains(addr)
    }

    #[doc = " Spawn another block from the specified address in this block"]
    fn spawn(&mut self, addr: u64) -> Result<Block, SpawnError> {
        Err(SpawnError::CannotSpawn)
    }

    #[doc = " Print the source code for the block, with the specified level of indents"]
    fn write_source(&self, level: u8, w: &mut impl std::io::Write) -> Result<(), std::io::Error> {
        self.indent(level, w)?;
        w.write_all("if (?) {\n".as_bytes())?;
        self.met.write_source(level + 1, w)?;
        self.indent(level, w)?;
        w.write_all("}\n".as_bytes())?;
        self.indent(level, w)?;
        w.write_all("else {\n".as_bytes())?;
        self.unmet.write_source(level + 1, w)?;
        self.indent(level, w)?;
        w.write_all("}\n".as_bytes())?;
        Ok(())
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
        } else {
            todo!();
        }
    }

    fn is_function_head(&self) -> bool {
        self.block.is_function_head()
    }

    fn conditional(&self) -> Option<super::Conditional> {
        None
    }
}

#[derive(Clone, serde::Serialize, serde::Deserialize)]
/// Represents a simple if else chain. Each if statement in the chain has a single condition. All blocks "executed" point to the next block.
/// Executed in this instance refers to the code executed in an if or else block of code.
pub struct SimpleIfElseBlock {
    /// The if blocks of the block. Each successive entry is an else block of the previous one. The bool indicates reverse logic of the if conditional.
    blocks: Vec<(Block, Block, bool)>,
    /// The optional else block at the end of the if else chain
    els: Option<Box<Block>>,
}

impl SimpleIfElseBlock {
    /// this function checks for a simple if block
    pub fn try_create_if(
        simplified: &Vec<SimplifiedBlock>,
        head: Option<&SimplifiedBlock>,
        g: &mut super::graph::Graph<Block>,
        notes: &mut Vec<String>,
    ) -> bool {
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
                if let Some(a) = b.to_u64() {
                    for e in simplified {
                        if e.start == a {
                            bblock = Some(e);
                        }
                    }
                }
            }
            if let Some(a) = ablock {
                if let Some(b) = bblock {
                    if let BlockEnd::Single(aend) = a.end {
                        if let Some(aend) = aend.to_u64() {
                            if aend == b.start {
                                notes.push("Found an if block\n".to_string());
                                let mut blocks = Vec::new();
                                let b1 = g.elements.take(h.index).unwrap();
                                let b2 = g.elements.take(a.index).unwrap();
                                blocks.push((b1, b2, false));
                                let nb = SimpleIfElseBlock { blocks, els: None };
                                let nb = Block::SimpleIfElse(nb);
                                g.elements.insert(nb);
                                return true;
                            }
                        }
                    }
                    if let BlockEnd::Single(bend) = b.end {
                        if let Some(bend) = bend.to_u64() {
                            if bend == a.start {
                                notes.push("Found a reversed logic if block\n".to_string());
                                let mut blocks = Vec::new();
                                let b1 = g.elements.take(h.index).unwrap();
                                let b2 = g.elements.take(b.index).unwrap();
                                blocks.push((b1, b2, true));
                                let nb = SimpleIfElseBlock { blocks, els: None };
                                let nb = Block::SimpleIfElse(nb);
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

impl BlockTrait for SimpleIfElseBlock {
    fn write_source(&self, level: u8, w: &mut impl std::io::Write) -> Result<(), std::io::Error> {
        for (i, (b1, b2, reverse)) in self.blocks.iter().enumerate() {
            b1.write_source(level, w)?;
            self.indent(level, w)?;
            if i > 0 {
                w.write_all("else ".as_bytes())?;
            }
            w.write_all("if (?) {\n".as_bytes())?;
            b2.write_source(level + 1, w)?;
            self.indent(level, w)?;
            w.write_all("}\n".as_bytes())?;
        }
        if let Some(e) = &self.els {
            w.write_all("else {\n".as_bytes())?;
            e.write_source(level + 1, w)?;
            self.indent(level, w)?;
            w.write_all("}\n".as_bytes())?;
        }
        Ok(())
    }

    fn address(&self) -> u64 {
        self.blocks.first().unwrap().0.address()
    }

    fn calc_next(&self) -> BlockEnd {
        return self.blocks.first().unwrap().1.calc_next();
    }

    fn contains(&self, addr: u64) -> bool {
        for (el1, el2, _reverse) in &self.blocks {
            if el1.contains(addr) {
                return true;
            }
            if el2.contains(addr) {
                return true;
            }
        }
        if let Some(b) = &self.els {
            if b.contains(addr) {
                return true;
            }
        }
        false
    }

    fn spawn(&mut self, addr: u64) -> Result<Block, SpawnError> {
        Err(SpawnError::CannotSpawn)
    }

    fn trace_register_all(&self, reg: Register) -> Value {
        todo!();
    }

    fn branch_value(&self) -> Option<Value> {
        None
    }

    fn set_block_end(&mut self, be: BlockEnd) -> Result<(), ()> {
        Err(())
    }

    fn dot_add(&self, g: &mut graphviz_rust::dot_structures::Graph, s: bool) {}

    fn is_function_head(&self) -> bool {
        self.blocks.first().unwrap().0.is_function_head()
    }

    fn conditional(&self) -> Option<super::Conditional> {
        None
    }
}
