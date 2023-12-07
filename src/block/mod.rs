//! This module contains code and object definitions related to distinct blocks of instructions

use crate::map::AutoHashMap;
use std::{collections::VecDeque, io::Write};

use graphviz_rust::dot_generator::*;
use graphviz_rust::dot_structures::*;

pub mod graph;

/// The errors that can occur when building an instruction decoder
pub enum InstructionDecoderError {
    /// The architecture is currently not supported by the decompiler.
    UnsupportedArchitecture,
}

/// The object for decoding instructions plus some extra convenience information
pub struct InstructionDecoderPlus<'a> {
    /// The instruction decoder object
    d: InstructionDecoder<'a>,
    /// The start address of the block used for the instruction decoder.
    start: u64,
    /// The length of the block used by the instruction decoder.
    len: usize,
}

impl<'a> InstructionDecoderPlus<'a> {
    /// Attempt to create an instruction decoder for the specified architecture, at the specified starting address,
    /// with the chunk of what is presumably code.
    pub fn new(
        arch: object::Architecture,
        address: u64,
        data: &'a [u8],
    ) -> Result<Self, InstructionDecoderError> {
        let d = match arch {
            object::Architecture::Unknown => Err(InstructionDecoderError::UnsupportedArchitecture),
            object::Architecture::I386 => Ok(InstructionDecoder::X86(iced_x86::Decoder::with_ip(
                32, data, address, 0,
            ))),
            object::Architecture::X86_64 => Ok(InstructionDecoder::X86(
                iced_x86::Decoder::with_ip(64, data, address, 0),
            )),
            object::Architecture::X86_64_X32 => Ok(InstructionDecoder::X86(
                iced_x86::Decoder::with_ip(32, data, address, 0),
            )),
            _ => Err(InstructionDecoderError::UnsupportedArchitecture),
        }?;
        Ok(InstructionDecoderPlus {
            d,
            start: address,
            len: data.len(),
        })
    }

    /// Returns true when this decoder contains the specified address
    pub fn contains(&self, addr: u64) -> bool {
        let end = self.start + self.len as u64;
        (self.start..end).contains(&addr)
    }

    /// Return a mutable reference to the instruction decoder
    pub fn decoder(&mut self) -> &'_ mut InstructionDecoder<'a> {
        &mut self.d
    }
}

/// The object for decoding instructions from a chunk of memory. The chunk of memory is not necessarily owned by this object.
pub enum InstructionDecoder<'a> {
    /// Decode instructions for x86 16,32, and 64 bits.
    X86(iced_x86::Decoder<'a>),
}

impl<'a> InstructionDecoder<'a> {
    /// Attempt to create an instruction decoder for the specified architecture, at the specified starting address,
    /// with the chunk of what is presumably code.
    pub fn new(
        arch: object::Architecture,
        address: u64,
        data: &'a [u8],
    ) -> Result<Self, InstructionDecoderError> {
        match arch {
            object::Architecture::Unknown => Err(InstructionDecoderError::UnsupportedArchitecture),
            object::Architecture::I386 => Ok(InstructionDecoder::X86(iced_x86::Decoder::with_ip(
                32, data, address, 0,
            ))),
            object::Architecture::X86_64 => Ok(InstructionDecoder::X86(
                iced_x86::Decoder::with_ip(64, data, address, 0),
            )),
            object::Architecture::X86_64_X32 => Ok(InstructionDecoder::X86(
                iced_x86::Decoder::with_ip(32, data, address, 0),
            )),
            _ => Err(InstructionDecoderError::UnsupportedArchitecture),
        }
    }

    /// Move to the specified code address for future decode operations.
    pub fn goto(&mut self, address: u64) {
        match self {
            InstructionDecoder::X86(x) => {
                let ip = x.ip();
                if ip > address {
                    let error = ip - address;
                    let pos = x.position();
                    let newpos = pos - error as usize;
                    x.set_position(newpos);
                } else {
                    let error = address - ip;
                    let pos = x.position();
                    let newpos = pos + error as usize;
                    x.set_position(newpos);
                }
                x.set_ip(address);
            }
        }
    }

    /// Decode and return and instruction, if one is available.
    pub fn decode(&mut self) -> Option<Instruction> {
        match self {
            InstructionDecoder::X86(x) => {
                if x.can_decode() {
                    Some(Instruction::X86(x.decode()))
                } else {
                    None
                }
            }
        }
    }
}

#[derive(Copy, Clone, Debug, serde::Serialize, serde::Deserialize)]
/// The values that an item can have
pub enum Value {
    /// The element contains the contents of another register.
    Register(Register),
    /// The element contains a literal 8 bit value (sign is unknown)
    Bits8(u8),
    /// The element contains a literal 16 bit value (sign is unknown)
    Bits16(u16),
    /// The element contains a literal 32 bit value (sign is unknown)
    Bits32(u32),
    /// The element contains a literal 64 bit value (sign is unknown)
    Bits64(u64),
    /// The element contains a literal signed 16 bit value
    SignedBits16(i16),
    /// The element contains a literal signed 32 bit value
    SignedBits32(i32),
    /// The element contains a literal signed 64 bit value
    SignedBits64(i64),
    /// The value is unknown
    Unknown,
}

impl std::fmt::UpperHex for Value {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Value::Unknown | Value::Register(_) => f.write_str("UNKNOWN"),
            Value::Bits8(a) => f.write_fmt(format_args!("{:X}", a)),
            Value::Bits16(a) => f.write_fmt(format_args!("{:X}", a)),
            Value::Bits32(a) => f.write_fmt(format_args!("{:X}", a)),
            Value::Bits64(a) => f.write_fmt(format_args!("{:X}", a)),
            Value::SignedBits16(a) => f.write_fmt(format_args!("{:X}", a)),
            Value::SignedBits32(a) => f.write_fmt(format_args!("{:X}", a)),
            Value::SignedBits64(a) => f.write_fmt(format_args!("{:X}", a)),
        }
    }
}

impl Value {
    /// Returns true when the value is known
    pub fn is_known(&self) -> bool {
        match self {
            Value::Unknown | Value::Register(_) => false,
            Value::Bits8(_)
            | Value::Bits16(_)
            | Value::Bits32(_)
            | Value::Bits64(_)
            | Value::SignedBits16(_)
            | Value::SignedBits32(_)
            | Value::SignedBits64(_) => true,
        }
    }

    /// Try to get a u64 from the value
    pub fn to_u64(&self) -> Option<u64> {
        match self {
            Value::Unknown | Value::Register(_) => None,
            Value::Bits8(a) => Some(*a as u64),
            Value::Bits16(a) => Some(*a as u64),
            Value::Bits32(a) => Some(*a as u64),
            Value::Bits64(a) => Some(*a),
            Value::SignedBits16(a) => Some(*a as u64),
            Value::SignedBits32(a) => Some(*a as u64),
            Value::SignedBits64(a) => Some(*a as u64),
        }
    }
}

#[derive(Copy, Clone, Debug, serde::Serialize, serde::Deserialize)]
/// An x86 register
pub enum X86Register {
    /// An instruction addressable register
    RegularRegister(iced_x86::Register),
    /// The 32 bit EFLAGS register
    Flags32(u32),
    /// The 64 bit RFLAGS register
    Flags64(u64),
}

#[derive(Copy, Clone, Debug, serde::Serialize, serde::Deserialize)]
/// A register for the target architecture
pub enum Register {
    /// An x86 register
    X86(X86Register),
    /// Some other register set
    Other,
}

/// A basic instruction from disasssembly of the code being decompiled.
#[derive(Debug, Clone, serde::Serialize, serde::Deserialize)]
pub enum Instruction {
    /// An x86 instruction, 16, 32, or 64 bits.
    X86(iced_x86::Instruction),
}

impl std::fmt::Display for Instruction {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Instruction::X86(xi) => xi.fmt(f),
        }
    }
}

impl Instruction {
    /// Returns the address of the instruction
    pub fn address(&self) -> u64 {
        match self {
            Instruction::X86(xi) => xi.ip(),
        }
    }

    /// Trace the value of a register, finding the value for that register at the end of this instruction.
    pub fn trace_register(&self, reg: Register) -> Value {
        let mut val = Value::Register(reg);
        match self {
            Instruction::X86(xi) => {
                if let Register::X86(xr) = reg {
                    match xi.mnemonic() {
                        iced_x86::Mnemonic::Mov => {
                            if xi.op0_kind() == iced_x86::OpKind::Register {
                                if let X86Register::RegularRegister(r) = xr {
                                    if r == xi.op0_register() {
                                        match xi.op1_kind() {
                                            iced_x86::OpKind::Register => {
                                                val = Value::Register(Register::X86(
                                                    X86Register::RegularRegister(xi.op1_register()),
                                                ));
                                            }
                                            iced_x86::OpKind::NearBranch16 => todo!(),
                                            iced_x86::OpKind::NearBranch32 => todo!(),
                                            iced_x86::OpKind::NearBranch64 => todo!(),
                                            iced_x86::OpKind::FarBranch16 => todo!(),
                                            iced_x86::OpKind::FarBranch32 => todo!(),
                                            iced_x86::OpKind::Immediate8 => {
                                                val = Value::Bits8(xi.immediate8());
                                            }
                                            iced_x86::OpKind::Immediate8_2nd => todo!(),
                                            iced_x86::OpKind::Immediate16 => {
                                                val = Value::Bits16(xi.immediate16());
                                            }
                                            iced_x86::OpKind::Immediate32 => {
                                                val = Value::Bits32(xi.immediate32());
                                            }
                                            iced_x86::OpKind::Immediate64 => {
                                                val = Value::Bits64(xi.immediate64());
                                            }
                                            iced_x86::OpKind::Immediate8to16 => {
                                                val = Value::SignedBits16(xi.immediate8to16());
                                            }
                                            iced_x86::OpKind::Immediate8to32 => {
                                                val = Value::SignedBits32(xi.immediate8to32());
                                            }
                                            iced_x86::OpKind::Immediate8to64 => {
                                                val = Value::SignedBits64(xi.immediate8to64());
                                            }
                                            iced_x86::OpKind::Immediate32to64 => {
                                                val = Value::SignedBits64(xi.immediate32to64());
                                            }
                                            iced_x86::OpKind::MemorySegSI => todo!(),
                                            iced_x86::OpKind::MemorySegESI => todo!(),
                                            iced_x86::OpKind::MemorySegRSI => todo!(),
                                            iced_x86::OpKind::MemorySegDI => todo!(),
                                            iced_x86::OpKind::MemorySegEDI => todo!(),
                                            iced_x86::OpKind::MemorySegRDI => todo!(),
                                            iced_x86::OpKind::MemoryESDI => todo!(),
                                            iced_x86::OpKind::MemoryESEDI => todo!(),
                                            iced_x86::OpKind::MemoryESRDI => todo!(),
                                            iced_x86::OpKind::Memory => todo!(),
                                        }
                                    }
                                }
                            }
                        }
                        _ => {}
                    }
                }
            }
        }
        val
    }

    /// Calculates the set of addresses that follow this instruction.
    pub fn calc_next(&self) -> BlockEnd {
        match self {
            Instruction::X86(xi) => match xi.mnemonic() {
                iced_x86::Mnemonic::Jmp | iced_x86::Mnemonic::Jmpe => match xi.op0_kind() {
                    iced_x86::OpKind::Register => BlockEnd::Single(Value::Register(Register::X86(
                        X86Register::RegularRegister(xi.op0_register()),
                    ))),
                    iced_x86::OpKind::NearBranch16 => {
                        BlockEnd::Single(Value::Bits16(xi.near_branch16()))
                    }
                    iced_x86::OpKind::NearBranch32 => {
                        BlockEnd::Single(Value::Bits32(xi.near_branch32()))
                    }
                    iced_x86::OpKind::NearBranch64 => {
                        BlockEnd::Single(Value::Bits64(xi.near_branch64()))
                    }
                    iced_x86::OpKind::FarBranch16 => {
                        BlockEnd::Single(Value::Bits16(xi.far_branch16()))
                    }
                    iced_x86::OpKind::FarBranch32 => {
                        BlockEnd::Single(Value::Bits32(xi.far_branch32()))
                    }
                    iced_x86::OpKind::Immediate8 => BlockEnd::Single(Value::Bits8(xi.immediate8())),
                    iced_x86::OpKind::Immediate8_2nd => todo!(),
                    iced_x86::OpKind::Immediate16 => {
                        BlockEnd::Single(Value::Bits16(xi.immediate16()))
                    }
                    iced_x86::OpKind::Immediate32 => {
                        BlockEnd::Single(Value::Bits32(xi.immediate32()))
                    }
                    iced_x86::OpKind::Immediate64 => {
                        BlockEnd::Single(Value::Bits64(xi.immediate64()))
                    }
                    iced_x86::OpKind::Immediate8to16 => {
                        BlockEnd::Single(Value::SignedBits16(xi.immediate8to16()))
                    }
                    iced_x86::OpKind::Immediate8to32 => {
                        BlockEnd::Single(Value::SignedBits32(xi.immediate8to32()))
                    }
                    iced_x86::OpKind::Immediate8to64 => {
                        BlockEnd::Single(Value::SignedBits64(xi.immediate8to64()))
                    }
                    iced_x86::OpKind::Immediate32to64 => {
                        BlockEnd::Single(Value::SignedBits64(xi.immediate32to64()))
                    }
                    iced_x86::OpKind::MemorySegSI
                    | iced_x86::OpKind::MemorySegESI
                    | iced_x86::OpKind::MemorySegRSI
                    | iced_x86::OpKind::MemorySegDI
                    | iced_x86::OpKind::MemorySegEDI
                    | iced_x86::OpKind::MemorySegRDI
                    | iced_x86::OpKind::MemoryESDI
                    | iced_x86::OpKind::MemoryESEDI
                    | iced_x86::OpKind::MemoryESRDI
                    | iced_x86::OpKind::Memory => BlockEnd::Single(Value::Unknown),
                },
                iced_x86::Mnemonic::Ret | iced_x86::Mnemonic::Retf => BlockEnd::None,
                iced_x86::Mnemonic::Ja
                | iced_x86::Mnemonic::Jae
                | iced_x86::Mnemonic::Jb
                | iced_x86::Mnemonic::Jbe
                | iced_x86::Mnemonic::Jcxz
                | iced_x86::Mnemonic::Je
                | iced_x86::Mnemonic::Jecxz
                | iced_x86::Mnemonic::Jg
                | iced_x86::Mnemonic::Jge
                | iced_x86::Mnemonic::Jknzd
                | iced_x86::Mnemonic::Jkzd
                | iced_x86::Mnemonic::Jl
                | iced_x86::Mnemonic::Jle
                | iced_x86::Mnemonic::Jne
                | iced_x86::Mnemonic::Jno
                | iced_x86::Mnemonic::Jnp
                | iced_x86::Mnemonic::Jns
                | iced_x86::Mnemonic::Jo
                | iced_x86::Mnemonic::Jp
                | iced_x86::Mnemonic::Jrcxz
                | iced_x86::Mnemonic::Js
                | iced_x86::Mnemonic::Loop
                | iced_x86::Mnemonic::Loope
                | iced_x86::Mnemonic::Loopne => match xi.op0_kind() {
                    iced_x86::OpKind::Register => BlockEnd::Branch(
                        Value::Bits64(xi.next_ip()),
                        Value::Register(Register::X86(X86Register::RegularRegister(
                            xi.op0_register(),
                        ))),
                    ),
                    iced_x86::OpKind::NearBranch16 => BlockEnd::Branch(
                        Value::Bits64(xi.next_ip()),
                        Value::Bits16(xi.near_branch16()),
                    ),
                    iced_x86::OpKind::NearBranch32 => BlockEnd::Branch(
                        Value::Bits64(xi.next_ip()),
                        Value::Bits32(xi.near_branch32()),
                    ),
                    iced_x86::OpKind::NearBranch64 => BlockEnd::Branch(
                        Value::Bits64(xi.next_ip()),
                        Value::Bits64(xi.near_branch64()),
                    ),
                    iced_x86::OpKind::FarBranch16 => BlockEnd::Branch(
                        Value::Bits64(xi.next_ip()),
                        Value::Bits16(xi.far_branch16()),
                    ),
                    iced_x86::OpKind::FarBranch32 => BlockEnd::Branch(
                        Value::Bits64(xi.next_ip()),
                        Value::Bits32(xi.far_branch32()),
                    ),
                    iced_x86::OpKind::Immediate8 => {
                        BlockEnd::Branch(Value::Bits64(xi.next_ip()), Value::Bits8(xi.immediate8()))
                    }
                    iced_x86::OpKind::Immediate8_2nd => todo!(),
                    iced_x86::OpKind::Immediate16 => BlockEnd::Branch(
                        Value::Bits64(xi.next_ip()),
                        Value::Bits16(xi.immediate16()),
                    ),
                    iced_x86::OpKind::Immediate32 => BlockEnd::Branch(
                        Value::Bits64(xi.next_ip()),
                        Value::Bits32(xi.immediate32()),
                    ),
                    iced_x86::OpKind::Immediate64 => BlockEnd::Branch(
                        Value::Bits64(xi.next_ip()),
                        Value::Bits64(xi.immediate64()),
                    ),
                    iced_x86::OpKind::Immediate8to16 => BlockEnd::Branch(
                        Value::Bits64(xi.next_ip()),
                        Value::SignedBits16(xi.immediate8to16()),
                    ),
                    iced_x86::OpKind::Immediate8to32 => BlockEnd::Branch(
                        Value::Bits64(xi.next_ip()),
                        Value::SignedBits32(xi.immediate8to32()),
                    ),
                    iced_x86::OpKind::Immediate8to64 => BlockEnd::Branch(
                        Value::Bits64(xi.next_ip()),
                        Value::SignedBits64(xi.immediate8to64()),
                    ),
                    iced_x86::OpKind::Immediate32to64 => BlockEnd::Branch(
                        Value::Bits64(xi.next_ip()),
                        Value::SignedBits64(xi.immediate32to64()),
                    ),
                    iced_x86::OpKind::MemorySegSI
                    | iced_x86::OpKind::MemorySegESI
                    | iced_x86::OpKind::MemorySegRSI
                    | iced_x86::OpKind::MemorySegDI
                    | iced_x86::OpKind::MemorySegEDI
                    | iced_x86::OpKind::MemorySegRDI
                    | iced_x86::OpKind::MemoryESDI
                    | iced_x86::OpKind::MemoryESEDI
                    | iced_x86::OpKind::MemoryESRDI
                    | iced_x86::OpKind::Memory => {
                        BlockEnd::Branch(Value::Bits64(xi.next_ip()), Value::Unknown)
                    }
                },
                _ => BlockEnd::Single(Value::Bits64(xi.next_ip())),
            },
        }
    }
}

/// Errors that can occur when spawning a block
pub enum SpawnError {
    /// The target block trying to be split does not contain the specified address
    InvalidAddress,
    /// The target block cannot be split
    CannotSpawn,
}

#[derive(Clone, serde::Serialize, serde::Deserialize)]
/// An operation is a way to describe a portion of source code. `a = 5;` is an equality operation
pub enum Operation {}

#[derive(Clone, serde::Serialize, serde::Deserialize)]
/// The basic instruction that has been translated from assembly
pub struct Statement {
    /// The operation and address pairs for the statement
    ops: Vec<(u64, Operation)>,
    /// A comment printed at the end of the statement
    comment: String,
    /// The next address set
    next: BlockEnd,
}

impl Statement {
    /// Return the starting address of the statement
    pub fn address(&self) -> u64 {
        self.ops.first().unwrap().0
    }

    /// Return the blockend for this statement
    pub fn calc_next(&self) -> BlockEnd {
        self.next
    }

    /// Does this statement contain the specified address
    pub fn contains(&self, addr: u64) -> bool {
        for (a, _ops) in &self.ops {
            if *a == addr {
                return true;
            }
        }
        false
    }

    /// Spawn a new Statement starting at the specified address
    pub fn spawn(&mut self, addr: u64) -> Result<Statement, SpawnError> {
        todo!();
    }
}

/// The trait that all blocks of code must implement
#[enum_dispatch::enum_dispatch(Block)]
pub trait BlockTrait {
    /// Return the address of the head of this block
    fn address(&self) -> u64;
    /// Calculate the next address or addresses for this block
    fn calc_next(&self) -> BlockEnd;
    /// Does this block contain an instruction that starts at the specified address?
    fn contains(&self, addr: u64) -> bool;
    /// Spawn another block from the specified address in this block
    fn spawn(&mut self, addr: u64) -> Result<Block, SpawnError>;
    /// Print the source code for the block, with the specified level of indents
    fn write_source(&self, level: u8, w: &mut impl std::io::Write) -> Result<(), std::io::Error>;
    /// Indent the specified number of times (proviced)
    fn indent(&self, level: u8, w: &mut impl std::io::Write) -> Result<(), std::io::Error> {
        let o = vec![b'\t'; level as usize];
        w.write_all(&o)
    }
    /// Attempt to find the value of the given register, over all instructions of the block.
    fn trace_register_all(&self, reg: Register) -> Value;
    /// Returns the conditional branch of the block, if one exists
    fn branch_value(&self) -> Option<Value>;
    /// Attempt to set the blockend for the graph
    fn set_block_end(&mut self, be: BlockEnd) -> Result<(), ()>;
    /// Add applicable statements to the dot graph for this block, s refers if the simplified or expanded dot is created.
    fn dot_add(&self, g: &mut graphviz_rust::dot_structures::Graph, s: bool);
    /// Returns true when this block is the head of a function
    fn is_function_head(&self) -> bool;
}

/// This represents a simplified block, used for simplifying a collection of blocks.
pub struct SimplifiedBlock {
    /// The start address of the simplified block
    start: u64,
    /// The end point of the simplified block
    end: BlockEnd,
    /// The nodes outside of the collection, that point to this block
    remote_inputs: Vec<u64>,
    /// The nodes inside the collection, that point to this block
    local_inputs: Vec<u64>,
    /// The index of the block in the graph that contains it
    index: usize,
    /// True when the block is a function head
    head: bool,
}

/// A single unit of code. Each variety here can be assumed to run in sequence as a unit. Non-branching jumps may be present in a sequence, meaning the addresses of the instructions contained may be a bit jumbled.
/// A block could contain a bunch of non-conditional jumps but still be a single block of instructions.
#[derive(Clone, serde::Serialize, serde::Deserialize)]
#[enum_dispatch::enum_dispatch]
pub enum Block {
    /// A basic sequence of `Instruction`.
    Instruction(InstructionBlock),
    /// A linear sequence of one or more blocks.
    Sequence(SequenceBlock),
    /// A basic sequence of statements.
    Statements(StatementBlock),
    /// A simple if else chain
    SimpleIfElse(SimpleIfElseBlock),
    /// A do while loop with a simple condition
    SimpleDoWhile(SimpleDoWhileBlock),
    /// A simple while loop with a single condition
    SimpleWhileLoop(SimpleWhileBlock),
    /// An infinite loop
    InfiniteLoop(InfiniteLoopBlock),
    /// An if else block where both cases have no next block
    IfElseEnding(IfElse1Block),
    /// A generated block of code. For testing purposes only
    Generated(GeneratedBlock),
}

impl graph::GraphIdTrait for Block {
    fn id(&self) -> u64 {
        self.address()
    }

    fn linked_to(&self) -> Vec<u64> {
        let mut v = Vec::new();
        match self.calc_next() {
            BlockEnd::None => {}
            BlockEnd::Single(a) => {
                if let Some(a) = a.to_u64() {
                    v.push(a);
                }
            }
            BlockEnd::Branch(a, b) => {
                if let Some(a) = a.to_u64() {
                    v.push(a);
                }
                if let Some(b) = b.to_u64() {
                    v.push(b);
                }
            }
        }
        v
    }
}

impl Block {
    /// Try to create a block with the given group of blocks by index
    pub fn try_create(
        g: &mut graph::Graph<Block>,
        indexes: Vec<usize>,
        notes: &mut Vec<String>,
    ) -> bool {
        let mut simplified: Vec<SimplifiedBlock> = indexes
            .iter()
            .map(|i| {
                let b = g.elements.get(i).unwrap();
                SimplifiedBlock {
                    start: b.address(),
                    end: b.calc_next(),
                    remote_inputs: Vec::new(),
                    local_inputs: Vec::new(),
                    index: *i,
                    head: b.is_function_head(),
                }
            })
            .collect();
        for (i, block) in g.elements.iter() {
            let local = indexes.contains(i);
            if local {
                match block.calc_next() {
                    BlockEnd::None => {}
                    BlockEnd::Single(a) => {
                        if a.is_known() {
                            let a = a.to_u64().unwrap();
                            for le in &mut simplified {
                                if le.start == a && block.address() != le.start {
                                    le.local_inputs.push(block.address());
                                }
                            }
                        }
                    }
                    BlockEnd::Branch(a, b) => {
                        if a.is_known() {
                            let a = a.to_u64().unwrap();
                            for le in &mut simplified {
                                if le.start == a && block.address() != le.start {
                                    le.local_inputs.push(block.address());
                                }
                            }
                        }
                        if b.is_known() {
                            let a = b.to_u64().unwrap();
                            for le in &mut simplified {
                                if le.start == a && block.address() != le.start {
                                    le.local_inputs.push(block.address());
                                }
                            }
                        }
                    }
                }
            } else {
                match block.calc_next() {
                    BlockEnd::None => {}
                    BlockEnd::Single(a) => {
                        if a.is_known() {
                            let a = a.to_u64().unwrap();
                            for le in &mut simplified {
                                if le.start == a {
                                    le.remote_inputs.push(block.address());
                                }
                            }
                        }
                    }
                    BlockEnd::Branch(a, b) => {
                        if a.is_known() {
                            let a = a.to_u64().unwrap();
                            for le in &mut simplified {
                                if le.start == a {
                                    le.remote_inputs.push(block.address());
                                }
                            }
                        }
                        if b.is_known() {
                            let a = b.to_u64().unwrap();
                            for le in &mut simplified {
                                if le.start == a {
                                    le.remote_inputs.push(block.address());
                                }
                            }
                        }
                    }
                }
            }
        }
        let mut head = None;
        let mut function_head = None;
        // Cannot reasonably simplify a collection if not all blocks are used.

        for el in &simplified {
            notes.push(format!(
                "Block {:x} has {}+{}: {:?}\n",
                el.start,
                el.local_inputs.len(),
                el.remote_inputs.len(),
                el.end
            ));
        }

        for el in &simplified {
            if el.head {
                if function_head.is_none() {
                    // One block is allowed to be unused, and it would be considered the head of a function
                    notes.push(format!("Function head detected {:x}\n", el.start));
                    function_head = Some(el);
                } else {
                    // More than one unused block is not allowed.
                    notes.push("More than one function head detected\n".to_string());
                    return false;
                }
            }
            // Do not simplify if more than one block has remote inputs
            if el.remote_inputs.len() > 0 {
                if head.is_some() {
                    notes.push("More than one block with remote inputs detected\n".to_string());
                    return false;
                } else {
                    notes.push(format!("Regular head detected {:x}\n", el.start));
                    head = Some(el);
                }
            }
        }
        let head = function_head.or(head);

        SequenceBlock::try_create(&simplified, head, g, notes)
            || SimpleDoWhileBlock::try_create(&simplified, head, g, notes)
            || InfiniteLoopBlock::try_create(&simplified, head, g, notes)
            || IfElse1Block::try_create(&simplified, head, g, notes)
            || SimpleIfElseBlock::try_create_if(&simplified, head, g, notes)
            || SimpleWhileBlock::try_create(&simplified, head, g, notes)
    }
}

/// Add an address node to the specified graphviz graph
pub fn dot_add_node(g: &mut graphviz_rust::dot_structures::Graph, addr: u64) {
    let s = format!("addr_{:x}", addr);
    let id = graphviz_rust::dot_structures::Id::Plain(s);
    let id = graphviz_rust::dot_structures::NodeId(id, None);
    let e = graphviz_rust::dot_structures::Node {
        id,
        attributes: vec![],
    };
    g.add_stmt(graphviz_rust::dot_structures::Stmt::Node(e));
}

/// Add a link from the first address to the second address in the specified graph
pub fn dot_add_link(g: &mut graphviz_rust::dot_structures::Graph, a: Value, b: Value) {
    if a.is_known() {
        let a = a.to_u64().unwrap();
        if b.is_known() {
            let b = b.to_u64().unwrap();

            let s = format!("addr_{:x}", a);
            let id = graphviz_rust::dot_structures::Id::Plain(s);
            let id1 = graphviz_rust::dot_structures::NodeId(id, None);

            let s = format!("addr_{:x}", b);
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
}

mod loop_block;
use loop_block::InfiniteLoopBlock;
use loop_block::SimpleDoWhileBlock;
use loop_block::SimpleWhileBlock;

mod conditional;
use conditional::IfElse1Block;
use conditional::SimpleIfElseBlock;

#[derive(Clone, serde::Serialize, serde::Deserialize)]
/// Represents a generated dummy block of code
pub struct GeneratedBlock {
    /// The start address of the generated block
    address: u64,
    /// The end of the generated block
    end: BlockEnd,
    /// Is the block a function head
    head: bool,
}

impl BlockTrait for GeneratedBlock {
    #[doc = " Return the address of the head of this block"]
    fn address(&self) -> u64 {
        self.address
    }

    #[doc = " Calculate the next address or addresses for this block"]
    fn calc_next(&self) -> BlockEnd {
        self.end
    }

    #[doc = " Does this block contain an instruction that starts at the specified address?"]
    fn contains(&self, addr: u64) -> bool {
        addr == self.address
    }

    #[doc = " Spawn another block from the specified address in this block"]
    fn spawn(&mut self, _addr: u64) -> Result<Block, SpawnError> {
        Err(SpawnError::CannotSpawn)
    }

    #[doc = " Print the source code for the block, with the specified level of indents"]
    fn write_source(&self, level: u8, w: &mut impl std::io::Write) -> Result<(), std::io::Error> {
        self.indent(level, w)?;
        w.write_all("//Dummy code\n".as_bytes())?;
        Ok(())
    }

    #[doc = " Attempt to find the value of the given register, over all instructions of the block."]
    fn trace_register_all(&self, reg: Register) -> Value {
        Value::Register(reg)
    }

    #[doc = " Returns the conditional branch of the block, if one exists"]
    fn branch_value(&self) -> Option<Value> {
        self.end.branch_value()
    }

    fn set_block_end(&mut self, be: BlockEnd) -> Result<(), ()> {
        self.end = be;
        Ok(())
    }

    fn dot_add(&self, g: &mut graphviz_rust::dot_structures::Graph, _s: bool) {
        let sa = self.address();
        dot_add_node(g, sa);

        match self.calc_next() {
            BlockEnd::None => {}
            BlockEnd::Single(a) => {
                dot_add_link(g, Value::Bits64(sa), a);
            }
            BlockEnd::Branch(a, b) => {
                dot_add_link(g, Value::Bits64(sa), a);
                dot_add_link(g, Value::Bits64(sa), b);
            }
        }
    }

    fn is_function_head(&self) -> bool {
        self.head
    }
}

#[derive(Clone, serde::Serialize, serde::Deserialize)]
/// A block of sequential statements
pub struct StatementBlock {
    /// The statements in the block
    statements: Vec<Statement>,
    /// True when this is a function head block
    head: bool,
}

impl BlockTrait for StatementBlock {
    fn write_source(&self, level: u8, w: &mut impl std::io::Write) -> Result<(), std::io::Error> {
        self.indent(level, w)?;
        w.write_all("#error series of statements\n".as_bytes())?;
        Ok(())
    }

    fn address(&self) -> u64 {
        self.statements.first().unwrap().address()
    }

    fn calc_next(&self) -> BlockEnd {
        self.statements.last().unwrap().calc_next()
    }

    fn contains(&self, addr: u64) -> bool {
        for el in &self.statements {
            if el.contains(addr) {
                return true;
            }
        }
        false
    }

    fn spawn(&mut self, addr: u64) -> Result<Block, SpawnError> {
        if self.contains(addr) {
            let mut index = 0;
            for (i, el) in self.statements.iter().enumerate() {
                if el.address() == addr {
                    index = i;
                    break;
                }
            }
            let mut spawn = VecDeque::from(self.statements.split_off(index));
            let mut splitme = spawn.pop_front().unwrap();
            let newblock = splitme.spawn(addr)?;
            spawn.push_front(newblock);
            self.statements.push(splitme);
            let s: Vec<Statement> = spawn.into_iter().collect();
            Ok(Block::Statements(Self {
                statements: s,
                head: false,
            }))
        } else {
            Err(SpawnError::InvalidAddress)
        }
    }

    fn trace_register_all(&self, reg: Register) -> Value {
        todo!();
    }

    fn branch_value(&self) -> Option<Value> {
        todo!();
    }

    fn set_block_end(&mut self, be: BlockEnd) -> Result<(), ()> {
        Err(())
    }

    fn dot_add(&self, g: &mut graphviz_rust::dot_structures::Graph, s: bool) {}

    fn is_function_head(&self) -> bool {
        self.head
    }
}

#[derive(Clone, serde::Serialize, serde::Deserialize)]
/// A sequence of blocks
pub struct SequenceBlock {
    /// The blocks in the sequence
    blocks: Vec<Block>,
}

impl SequenceBlock {
    /// This is used to simplify a sequence that contains a sequence
    fn simplify(&mut self) {
        let mut found_sequence = false;
        for b in &self.blocks {
            if let Block::Sequence(_s) = b {
                found_sequence = true;
            }
        }
        if found_sequence {
            let mut blocks = Vec::new();
            for b in self.blocks.clone().into_iter() {
                if let Block::Sequence(mut s) = b {
                    blocks.append(&mut s.blocks);
                } else {
                    blocks.push(b);
                }
            }
            self.blocks = blocks;
        }
    }

    /// Try to create a Self block from the elements specified in simplified
    fn try_create(
        simplified: &Vec<SimplifiedBlock>,
        head: Option<&SimplifiedBlock>,
        g: &mut graph::Graph<Block>,
        notes: &mut Vec<String>,
    ) -> bool {
        if simplified.len() > 1 {
            let mut nsi = Vec::new();

            let mut element = head;
            let mut nodes_visited = Vec::new();
            loop {
                if let Some(n) = element.take() {
                    if nodes_visited.contains(&n.start) {
                        break;
                    }
                    nodes_visited.push(n.start);
                    match n.end {
                        BlockEnd::None => {
                            notes.push(format!("{:X}", n.start));
                            nsi.push(n.index);
                            break;
                        }
                        BlockEnd::Single(a) => {
                            if let Some(a) = a.to_u64() {
                                notes.push(format!("{:X}->", n.start));
                                nsi.push(n.index);
                                for e in simplified {
                                    if e.start == a {
                                        element = Some(e);
                                    }
                                }
                            }
                        }
                        BlockEnd::Branch(_a, _b) => {
                            nsi.push(n.index);
                            notes.push("Branch\n".to_string());
                            break;
                        }
                    }
                } else {
                    break;
                }
            }

            if nsi.len() > 1 {
                notes.push("Creating sequence\n".to_string());
                let ns: Vec<Block> = nsi.iter().map(|i| g.elements.take(*i).unwrap()).collect();
                let mut sb = SequenceBlock { blocks: ns };
                sb.simplify();
                let nb = Block::Sequence(sb);
                g.elements.insert(nb);
                return true;
            }
        }
        false
    }
}

impl BlockTrait for SequenceBlock {
    fn write_source(&self, level: u8, w: &mut impl std::io::Write) -> Result<(), std::io::Error> {
        for b in &self.blocks {
            b.write_source(level, w)?;
        }
        Ok(())
    }

    fn address(&self) -> u64 {
        self.blocks.first().unwrap().address()
    }

    fn calc_next(&self) -> BlockEnd {
        self.blocks.last().unwrap().calc_next()
    }

    fn contains(&self, addr: u64) -> bool {
        for el in &self.blocks {
            if el.contains(addr) {
                return true;
            }
        }
        false
    }

    fn spawn(&mut self, addr: u64) -> Result<Block, SpawnError> {
        if self.contains(addr) {
            let mut index = 0;
            for (i, el) in self.blocks.iter().enumerate() {
                if el.address() == addr {
                    index = i;
                    break;
                }
            }
            let mut spawn = VecDeque::from(self.blocks.split_off(index));
            let mut splitme = spawn.pop_front().unwrap();
            let newblock = splitme.spawn(addr)?;
            spawn.push_front(newblock);
            self.blocks.push(splitme);
            let s: Vec<Block> = spawn.into_iter().collect();
            Ok(Block::Sequence(Self { blocks: s }))
        } else {
            Err(SpawnError::InvalidAddress)
        }
    }

    fn trace_register_all(&self, reg: Register) -> Value {
        todo!();
    }

    fn branch_value(&self) -> Option<Value> {
        self.blocks.last().unwrap().branch_value()
    }

    fn set_block_end(&mut self, be: BlockEnd) -> Result<(), ()> {
        Err(())
    }

    fn dot_add(&self, g: &mut graphviz_rust::dot_structures::Graph, s: bool) {
        if s {
            let fa = self.blocks.first().unwrap().address();
            dot_add_node(g, fa);
            match self.blocks.last().unwrap().calc_next() {
                BlockEnd::None => {}
                BlockEnd::Single(a) => {
                    dot_add_link(g, Value::Bits64(fa), a);
                }
                BlockEnd::Branch(a, b) => {
                    dot_add_link(g, Value::Bits64(fa), a);
                    dot_add_link(g, Value::Bits64(fa), b);
                }
            }
        } else {
            for e in &self.blocks {
                e.dot_add(g, s);
            }
        }
    }

    fn is_function_head(&self) -> bool {
        self.blocks.first().unwrap().is_function_head()
    }
}

#[derive(Clone, serde::Serialize, serde::Deserialize)]
/// The block created dynamically by the disassembly process
pub struct InstructionBlock {
    /// The instructions of the block
    instructions: Vec<Instruction>,
    /// The state of the block being a head block
    head: bool,
}

impl BlockTrait for InstructionBlock {
    fn write_source(&self, level: u8, w: &mut impl std::io::Write) -> Result<(), std::io::Error> {
        self.indent(level, w)?;
        w.write_all("#error instruction block ".as_bytes())?;
        w.write_all(format!("{:X}\n", self.address()).as_bytes())?;
        for i in &self.instructions {
            self.indent(level, w)?;
            w.write_all(format!("{}\n", i).as_bytes())?;
        }
        Ok(())
    }

    fn address(&self) -> u64 {
        self.instructions.first().unwrap().address()
    }

    fn calc_next(&self) -> BlockEnd {
        self.instructions.last().unwrap().calc_next()
    }

    /// Does this block contain an instruction that starts at the specified address?
    fn contains(&self, addr: u64) -> bool {
        for i in &self.instructions {
            if i.address() == addr {
                return true;
            }
        }
        false
    }

    fn spawn(&mut self, addr: u64) -> Result<Block, SpawnError> {
        if self.contains(addr) {
            let mut index = 0;
            for (i, el) in self.instructions.iter().enumerate() {
                if el.address() == addr {
                    index = i;
                    break;
                }
            }
            let spawn = Self {
                instructions: self.instructions.split_off(index),
                head: false,
            };
            Ok(Block::Instruction(spawn))
        } else {
            Err(SpawnError::InvalidAddress)
        }
    }

    /// Trace the value of a register until a known value is found, or to the beginning of the block.
    fn trace_register_all(&self, reg: Register) -> Value {
        let mut val = Value::Unknown;
        for instr in self.instructions.iter().rev() {
            val = instr.trace_register(reg);
            if val.is_known() {
                break;
            }
        }
        val
    }

    fn branch_value(&self) -> Option<Value> {
        match self.instructions.last().unwrap().calc_next() {
            BlockEnd::Single(_a) => None,
            BlockEnd::Branch(_a, b) => Some(b),
            BlockEnd::None => None,
        }
    }

    fn set_block_end(&mut self, _be: BlockEnd) -> Result<(), ()> {
        Err(())
    }

    fn dot_add(&self, g: &mut graphviz_rust::dot_structures::Graph, s: bool) {
        let sa = self.address();
        dot_add_node(g, sa);

        match self.calc_next() {
            BlockEnd::None => {}
            BlockEnd::Single(a) => {
                dot_add_link(g, Value::Bits64(sa), a);
            }
            BlockEnd::Branch(a, b) => {
                dot_add_link(g, Value::Bits64(sa), a);
                dot_add_link(g, Value::Bits64(sa), b);
            }
        }
    }

    fn is_function_head(&self) -> bool {
        self.head
    }
}

impl Block {
    /// Construct an empty block of instructions.
    pub fn new_instructions(head: bool) -> Self {
        Block::Instruction(InstructionBlock {
            instructions: Vec::new(),
            head,
        })
    }

    /// Add the specified instruction to the end of this block.
    pub fn add_instruction(&mut self, i: Instruction) {
        if let Block::Instruction(b) = self {
            b.instructions.push(i);
        } else {
            panic!("Attempt to add instructions to Block that does not accept instructions");
        }
    }
}

/// Helper function for out Graph struct to be able to generate a graphviz_rust Graph
pub fn make_gv_graph(n: &str) -> graphviz_rust::dot_structures::Graph {
    graph!(di id!(n))
}

#[derive(Copy, Clone, Debug, serde::Serialize, serde::Deserialize)]
/// The end link for a node of a graph
pub enum BlockEnd {
    /// The instruction does not have a next instruction (like a return instruction).
    None,
    /// The block has a single block that executes next
    Single(Value),
    /// A regular branch with a non-matching block, and a matching block.
    Branch(Value, Value),
}

impl BlockEnd {
    /// Return the conditional branch, if it exists.
    fn branch_value(&self) -> Option<Value> {
        match self {
            BlockEnd::None => None,
            BlockEnd::Single(_) => None,
            BlockEnd::Branch(_a, b) => Some(*b),
        }
    }
}
