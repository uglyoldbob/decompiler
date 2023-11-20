//! This module contains code and object definitions related to distinct blocks of instructions

pub enum InstructionDecoderError {
    UnsupportedArchitecture,
}

pub enum InstructionDecoder<'a> {
    X86(iced_x86::Decoder<'a>),
}

impl<'a> InstructionDecoder<'a> {
    pub fn new(arch: object::Architecture, address: u64, data: &'a [u8]) -> Result<Self, InstructionDecoderError> {
        match arch {
            object::Architecture::Unknown => Err(InstructionDecoderError::UnsupportedArchitecture),
            object::Architecture::I386 => {
                Ok(InstructionDecoder::X86(iced_x86::Decoder::with_ip(32, data, address, 0)))
            }
            object::Architecture::X86_64 => {
                Ok(InstructionDecoder::X86(iced_x86::Decoder::with_ip(64, data, address, 0)))
            }
            object::Architecture::X86_64_X32 => {
                Ok(InstructionDecoder::X86(iced_x86::Decoder::with_ip(32, data, address, 0)))
            }
            _ => Err(InstructionDecoderError::UnsupportedArchitecture),
        }
    }

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

    pub fn decode(&mut self) -> Option<Instruction> {
        match self {
            InstructionDecoder::X86(x) => {
                if x.can_decode() {
                    Some(Instruction::X86(x.decode()))
                }
                else {
                    None
                }
            }
        }
    }
}

#[derive(Debug)]
pub enum Instruction {
    X86(iced_x86::Instruction),
}

impl<'a> std::fmt::Display for Instruction {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Instruction::X86(xi) => {
                f.write_str(&format!("{:?}", xi.mnemonic()))
            }
        }
    }
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
