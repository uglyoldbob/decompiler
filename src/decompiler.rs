//! Defines code that defines the main functionality of the decompiler. This is where the majority of work happens.

use std::{collections::HashMap, sync::Arc};

use object::{Object, ObjectSection};

use crate::ProjectInputFile;

pub enum MessageToDecompiler {
    ProcessFile(Arc<crate::ProjectInputFile>),
}

pub enum MessageFromDecompiler {
    Test,
}

pub struct Decompiler {
    sender: std::sync::mpsc::Sender<MessageToDecompiler>,
    receiver: std::sync::mpsc::Receiver<MessageFromDecompiler>,
}

impl Decompiler {
    pub fn process_file(&mut self, f: &Arc<ProjectInputFile>) {
        self.sender
            .send(MessageToDecompiler::ProcessFile(f.to_owned()));
    }
}

struct InternalDecompiler {
    sender: std::sync::mpsc::Sender<MessageFromDecompiler>,
    receiver: std::sync::mpsc::Receiver<MessageToDecompiler>,
    file_processors: HashMap<String, DecompilerFileProcessor>,
}

impl InternalDecompiler {
    fn run(
        sender: std::sync::mpsc::Sender<MessageFromDecompiler>,
        receiver: std::sync::mpsc::Receiver<MessageToDecompiler>,
    ) {
        let i = InternalDecompiler {
            sender,
            receiver,
            file_processors: HashMap::new(),
        };
        std::thread::spawn(move || {
            let mut a = i;
            a.go();
        });
    }

    fn go(&mut self) {
        loop {
            if let Ok(m) = self.receiver.recv() {
                match m {
                    MessageToDecompiler::ProcessFile(f) => {
                        let (s, r) = std::sync::mpsc::channel();
                        let (s2, r2) = std::sync::mpsc::channel();
                        let n = f.borrow_name().to_owned();
                        println!("Received request to process file {}", n);
                        if !self.file_processors.contains_key(&n) {
                            let file_processor = DecompilerFileProcessor {
                                sender: s,
                                receiver: r2,
                                file: f,
                            };
                            file_processor.run(s2, r);
                            self.file_processors.insert(n, file_processor);
                        }
                    }
                }
            }
        }
    }
}

impl Decompiler {
    pub fn new() -> Self {
        let (s, r) = std::sync::mpsc::channel();
        let (s2, r2) = std::sync::mpsc::channel();
        InternalDecompiler::run(s2, r);
        Self {
            sender: s,
            receiver: r2,
        }
    }

    pub fn process_events(&mut self) {
        loop {
            if let Ok(m) = self.receiver.try_recv() {
                match m {
                    MessageFromDecompiler::Test => {}
                }
            } else {
                break;
            }
        }
    }
}

enum MessageToFileProcessor {
    Test,
}

enum MessageFromFileProcessor {
    Test,
}

struct InternalDecompilerFileProcessor {
    file: Arc<ProjectInputFile>,
    sender: std::sync::mpsc::Sender<MessageFromFileProcessor>,
    receiver: std::sync::mpsc::Receiver<MessageToFileProcessor>,
}

impl InternalDecompilerFileProcessor {
    fn go(&mut self) {
        let obj = self.file.borrow_obj();
        println!("Entry point is {:X}", obj.entry());
        for i in object::Object::sections(obj) {
            let start = i.address();
            let end = i.address() + i.size();
            if (start..end).contains(&obj.entry()) {
                println!("FOUND ENTRY SECTION @ {}", i.name().unwrap());
                if let Ok(d) = i.data() {
                    println!("The section has {} bytes", d.len());
                    if let Ok(mut dis) = crate::block::InstructionDecoder::new(obj.architecture(), start, d) {
                        println!("Got a valid disassembler object");
                        //dis.goto(obj.entry());
                        let instru = dis.decode();
                        if let Some(instru) = instru {
                            println!("First instruction is {}", instru);
                        }
                    }
                }
            }
        }
        loop {
            println!("Processing file in file processor?");
            std::thread::sleep(std::time::Duration::from_secs(5));
        }
    }
}

struct DecompilerFileProcessor {
    file: Arc<ProjectInputFile>,
    sender: std::sync::mpsc::Sender<MessageToFileProcessor>,
    receiver: std::sync::mpsc::Receiver<MessageFromFileProcessor>,
}

impl DecompilerFileProcessor {
    fn run(
        &self,
        sender: std::sync::mpsc::Sender<MessageFromFileProcessor>,
        receiver: std::sync::mpsc::Receiver<MessageToFileProcessor>,
    ) {
        let fp = InternalDecompilerFileProcessor {
            file: self.file.clone(),
            sender,
            receiver,
        };
        std::thread::spawn(move || {
            let mut a = fp;
            a.go();
        });
    }
}
