//! This module contains code and object definitions related to distinct blocks of instructions

pub enum Instruction {
    X86(iced_x86::Instruction),
}

pub struct Block {
    code: Vec<Instruction>,
}

pub struct Graph<T> {
    elements: crate::AutoHashMap<(T, BlockEnd)>,
}

pub enum BlockEnd {
    Jump(usize),
    DecisionPoint(usize),
    Nowhere,
    Unknown,
}
