//! This module contains code and object definitions related to distinct blocks of instructions

use std::io::Write;

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

/// A basic instruction from disasssembly of the code being decompiled.
#[derive(Debug)]
pub enum Instruction {
    /// An x86 instruction, 16, 32, or 64 bits.
    X86(iced_x86::Instruction),
}

impl std::fmt::Display for Instruction {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Instruction::X86(xi) => f.write_str(&format!("{:?}", xi.mnemonic())),
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

    /// Calculates the set of addresses that follow this instruction.
    pub fn calc_next(&self) -> BlockEnd {
        match self {
            Instruction::X86(xi) => match xi.mnemonic() {
                iced_x86::Mnemonic::Jmp | iced_x86::Mnemonic::Jmpe => match xi.op0_kind() {
                    iced_x86::OpKind::Register => BlockEnd::UnknownAddress,
                    iced_x86::OpKind::NearBranch16 => {
                        BlockEnd::KnownAddress(xi.near_branch16() as u64)
                    }
                    iced_x86::OpKind::NearBranch32 => {
                        BlockEnd::KnownAddress(xi.near_branch32() as u64)
                    }
                    iced_x86::OpKind::NearBranch64 => BlockEnd::KnownAddress(xi.near_branch64()),
                    iced_x86::OpKind::FarBranch16 => {
                        BlockEnd::KnownAddress(xi.far_branch16() as u64)
                    }
                    iced_x86::OpKind::FarBranch32 => {
                        BlockEnd::KnownAddress(xi.far_branch32() as u64)
                    }
                    iced_x86::OpKind::Immediate8 => BlockEnd::KnownAddress(xi.immediate8() as u64),
                    iced_x86::OpKind::Immediate8_2nd => todo!(),
                    iced_x86::OpKind::Immediate16 => {
                        BlockEnd::KnownAddress(xi.immediate16() as u64)
                    }
                    iced_x86::OpKind::Immediate32 => {
                        BlockEnd::KnownAddress(xi.immediate32() as u64)
                    }
                    iced_x86::OpKind::Immediate64 => BlockEnd::KnownAddress(xi.immediate64()),
                    iced_x86::OpKind::Immediate8to16 => {
                        BlockEnd::KnownAddress(xi.immediate8to16() as u64)
                    }
                    iced_x86::OpKind::Immediate8to32 => {
                        BlockEnd::KnownAddress(xi.immediate8to32() as u64)
                    }
                    iced_x86::OpKind::Immediate8to64 => {
                        BlockEnd::KnownAddress(xi.immediate8to64() as u64)
                    }
                    iced_x86::OpKind::Immediate32to64 => {
                        BlockEnd::KnownAddress(xi.immediate32to64() as u64)
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
                    | iced_x86::OpKind::Memory => BlockEnd::UnknownAddress,
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
                    iced_x86::OpKind::Register => BlockEnd::UnknownBranch(xi.next_ip()),
                    iced_x86::OpKind::NearBranch16 => {
                        BlockEnd::KnownBranch(xi.near_branch16() as u64, xi.next_ip())
                    }
                    iced_x86::OpKind::NearBranch32 => {
                        BlockEnd::KnownBranch(xi.near_branch32() as u64, xi.next_ip())
                    }
                    iced_x86::OpKind::NearBranch64 => {
                        BlockEnd::KnownBranch(xi.near_branch64(), xi.next_ip())
                    }
                    iced_x86::OpKind::FarBranch16 => {
                        BlockEnd::KnownBranch(xi.far_branch16() as u64, xi.next_ip())
                    }
                    iced_x86::OpKind::FarBranch32 => {
                        BlockEnd::KnownBranch(xi.far_branch32() as u64, xi.next_ip())
                    }
                    iced_x86::OpKind::Immediate8 => {
                        BlockEnd::KnownBranch(xi.immediate8() as u64, xi.next_ip())
                    }
                    iced_x86::OpKind::Immediate8_2nd => todo!(),
                    iced_x86::OpKind::Immediate16 => {
                        BlockEnd::KnownBranch(xi.immediate16() as u64, xi.next_ip())
                    }
                    iced_x86::OpKind::Immediate32 => {
                        BlockEnd::KnownBranch(xi.immediate32() as u64, xi.next_ip())
                    }
                    iced_x86::OpKind::Immediate64 => {
                        BlockEnd::KnownBranch(xi.immediate64(), xi.next_ip())
                    }
                    iced_x86::OpKind::Immediate8to16 => {
                        BlockEnd::KnownBranch(xi.immediate8to16() as u64, xi.next_ip())
                    }
                    iced_x86::OpKind::Immediate8to32 => {
                        BlockEnd::KnownBranch(xi.immediate8to32() as u64, xi.next_ip())
                    }
                    iced_x86::OpKind::Immediate8to64 => {
                        BlockEnd::KnownBranch(xi.immediate8to64() as u64, xi.next_ip())
                    }
                    iced_x86::OpKind::Immediate32to64 => {
                        BlockEnd::KnownBranch(xi.immediate32to64() as u64, xi.next_ip())
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
                    | iced_x86::OpKind::Memory => BlockEnd::UnknownBranch(xi.next_ip()),
                },
                _ => BlockEnd::KnownAddress(xi.next_ip()),
            },
        }
    }
}

/// Errors that can occur when spawning a block
pub enum SpawnError {
    InvalidAddress,
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
}

/// A single unit of code.
#[enum_dispatch::enum_dispatch]
pub enum Block {
    /// A basic sequence of `Instruction`, that runs without any branching. Non-branching jumps may be present in the sequence, meaning the addresses of the instructions contained may be a bit jumbled.
    /// A block could contain a bunch of non-conditional jumps but still be a single block of instructions.
    Instruction(Vec<Instruction>),
}

impl BlockTrait for Vec<Instruction> {
    fn address(&self) -> u64 {
        self.first().unwrap().address()
    }

    fn calc_next(&self) -> BlockEnd {
        self.last().unwrap().calc_next()
    }

    /// Does this block contain an instruction that starts at the specified address?
    fn contains(&self, addr: u64) -> bool {
        for i in self {
            if i.address() == addr {
                return true;
            }
        }
        false
    }

    fn spawn(&mut self, addr: u64) -> Result<Block, SpawnError> {
        if self.contains(addr) {
            let mut index = 0;
            for (i, el) in self.iter().enumerate() {
                if el.address() == addr {
                    index = i;
                    break;
                }
            }
            let spawn = self.split_off(index);
            Ok(Block::Instruction(spawn))
        } else {
            Err(SpawnError::InvalidAddress)
        }
    }
}

impl Block {
    /// Construct an empty block of instructions.
    pub fn new_instructions() -> Self {
        Block::Instruction(Vec::new())
    }

    /// Add the specified instruction to the end of this block.
    pub fn add_instruction(&mut self, i: Instruction) {
        if let Block::Instruction(b) = self {
            b.push(i);
        } else {
            panic!("Attempt to add instructions to Block that does not accept instructions");
        }
    }
}

/// An arbitrary graph of some type of object, where each object points to zero or more other objects in the graph.
pub struct Graph<T> {
    /// The elements in the graph
    elements: crate::AutoHashMap<T>,
}

impl<T> Graph<T> {
    /// Construct a blank graph
    pub fn new() -> Self {
        Self {
            elements: crate::AutoHashMap::new(),
        }
    }

    /// Returns the number of blocks in the graph
    pub fn num_blocks(&self) -> usize {
        self.elements.len()
    }
}

impl Graph<Block> {
    /// Write the graph to a dot file
    pub fn write_to_dot(&self, pb: std::path::PathBuf, name: &str) -> Result<(), std::io::Error> {
        let mut unknown = 0;
        let mut f = std::fs::File::create(pb)?;
        f.write_all(format!("digraph {}{{\n", name).as_bytes())?;
        for (_i, b) in self.elements.iter() {
            let start = b.address();
            let next = b.calc_next();
            let s = match next {
                BlockEnd::KnownAddress(a) => {
                    format!("addr_{:X} -> addr_{:X}\n", start, a)
                }
                BlockEnd::UnknownAddress => {
                    unknown += 1;
                    format!("addr_{:X} -> addr_unknown{:X}\n", start, unknown)
                }
                BlockEnd::KnownBranch(a, b) => {
                    format!(
                        "addr_{:X} -> addr_{:X}\naddr_{0:X} -> addr_{2:X}\n",
                        start, a, b
                    )
                }
                BlockEnd::UnknownBranch(a) => {
                    unknown += 1;
                    format!(
                        "addr_{:X} -> addr_{:X}\naddr_{0:X} -> addr_unknown{2:X}\n",
                        start, a, unknown
                    )
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

    /// Add the specified instruction to the graph
    pub fn add_instruction(&mut self, i: Instruction) -> bool {
        for (_index, b) in self.elements.iter_mut() {
            if b.contains(i.address()) {
                return false;
            } else {
                let n = b.calc_next();
                if let BlockEnd::KnownAddress(a) = n {
                    if a == i.address() {
                        println!("Instruction at {:x} is {}", i.address(), i);
                        b.add_instruction(i);
                        return true;
                    }
                }
            }
        }

        let mut nb = Block::new_instructions();
        println!("Instruction at {:x} is {}", i.address(), i);
        nb.add_instruction(i);
        self.elements.insert(nb);
        true
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
                    let next = instru.calc_next();
                    if self.add_instruction(instru) {
                        return Some(next);
                    } else {
                        return None;
                    }
                }
            }
        }
        None
    }
}

/// The end link for a node of a graph
pub enum BlockEnd {
    /// The address of the next instruction to be executed after this one is a known constant address
    KnownAddress(u64),
    /// The address of the next instruction to be executed after this block is not known.
    UnknownAddress,
    /// The instruction has a conditional branch where it leads to one of two destinations. Both destinations are known addresses.
    KnownBranch(u64, u64),
    /// The instruction has a conditional branch where it leads to one of two destinations. Only one address is a known address.
    UnknownBranch(u64),
    /// The instruction does not have a next instruction (like a return instruction).
    None,
}
