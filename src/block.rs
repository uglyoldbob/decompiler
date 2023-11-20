//! This module contains code and object definitions related to distinct blocks of instructions

/// The errors that can occur when building an instruction decoder
pub enum InstructionDecoderError {
    /// The architecture is currently not supported by the decompiler.
    UnsupportedArchitecture,
}

/// The object for decoding instructions plus some extra convenience information
pub struct InstructionDecoderPlus<'a> {
    d: InstructionDecoder<'a>,
    start: u64,
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
                x.set_position(0).unwrap();
                let ip = x.ip();
                let a = (address - ip) as usize;
                if a <= x.max_position() {
                    x.set_position(a);
                }
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

impl<'a> std::fmt::Display for Instruction {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Instruction::X86(xi) => f.write_str(&format!("{:?}", xi.mnemonic())),
        }
    }
}

impl Instruction {
    /// Calculates the set of addresses that follow this instruction.
    pub fn calc_next(&self) -> BlockEnd {
        match self {
            Instruction::X86(xi) => match xi.mnemonic() {
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
                | iced_x86::Mnemonic::Js => match xi.op0_kind() {
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
                    iced_x86::OpKind::MemorySegSI => BlockEnd::UnknownBranch(xi.next_ip()),
                    iced_x86::OpKind::MemorySegESI => BlockEnd::UnknownBranch(xi.next_ip()),
                    iced_x86::OpKind::MemorySegRSI => BlockEnd::UnknownBranch(xi.next_ip()),
                    iced_x86::OpKind::MemorySegDI => BlockEnd::UnknownBranch(xi.next_ip()),
                    iced_x86::OpKind::MemorySegEDI => BlockEnd::UnknownBranch(xi.next_ip()),
                    iced_x86::OpKind::MemorySegRDI => BlockEnd::UnknownBranch(xi.next_ip()),
                    iced_x86::OpKind::MemoryESDI => BlockEnd::UnknownBranch(xi.next_ip()),
                    iced_x86::OpKind::MemoryESEDI => BlockEnd::UnknownBranch(xi.next_ip()),
                    iced_x86::OpKind::MemoryESRDI => BlockEnd::UnknownBranch(xi.next_ip()),
                    iced_x86::OpKind::Memory => BlockEnd::UnknownBranch(xi.next_ip()),
                },
                _ => BlockEnd::KnownAddress(xi.next_ip()),
            },
        }
    }
}

/// A basic sequence of `Instruction`, that runs without any branching. Non-branching jumps may be present in the sequence, meaning the addresses of the instructions contained may be a bit jumbled.
/// A block could contain a bunch of non-conditional jumps but still be a single block of instructions.
pub struct Block {
    /// The instructions for the block
    code: Vec<Instruction>,
}

impl Block {
    /// Construct an empty block of instructions.
    pub fn new() -> Self {
        Self { code: Vec::new() }
    }

    /// Add the specified instruction to the end of this block.
    pub fn add_instruction(&mut self, i: Instruction) {
        self.code.push(i);
    }
}

/// An arbitrary graph of some type of object, where each object points to zero or more other objects in the graph.
pub struct Graph<T> {
    /// The elements in the graph
    elements: crate::AutoHashMap<(T, BlockEnd)>,
}

impl<T> Graph<T> {
    /// Construct a blank graph
    pub fn new() -> Self {
        Self {
            elements: crate::AutoHashMap::new(),
        }
    }
}

impl Graph<Block> {
    /// Add the specified instruction to the graph
    pub fn add_instruction(&mut self, i: Instruction) {
        let mut new_block = self.elements.len() == 0;

        if new_block {
            let mut nb = Block::new();
            let next = i.calc_next();
            nb.add_instruction(i);
            self.elements.insert((nb, next));
        }
    }

    /// Process the given address, modifying the graph as required
    pub fn process_address(
        &mut self,
        addr: u64,
        ids: &mut Vec<crate::block::InstructionDecoderPlus>,
    ) {
        for id in ids {
            if id.contains(addr) {
                let decoder = id.decoder();
                decoder.goto(addr);
                if let Some(instru) = decoder.decode() {
                    println!("Instruction at {:x} is {}", addr, instru);
                    self.add_instruction(instru);
                }
            }
        }
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
}
