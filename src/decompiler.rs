//! Defines code that defines the main functionality of the decompiler. This is where the majority of work happens.

use std::{collections::HashMap, sync::Arc};

use object::{Object, ObjectSection};

use crate::{ProjectInputFile};

/// A message to the `InternalDecompiler` object from the `Decompiler` object.
pub enum MessageToDecompiler {
    /// A request for processing the given file contents.
    ProcessFile(Arc<crate::ProjectInputFile>),
}

/// A message from the `InternalDecompiler` object, sent back to the `Decompiler` object.
pub enum MessageFromDecompiler {
    /// A placeholder message, will be removed.
    Test,
}

/// The gui side of the decompiler. Communicates with an `InternalDecompiler` object that runs on a separate thread.
pub struct Decompiler {
    /// How messages are sent to the decompiler processing thread.
    sender: std::sync::mpsc::Sender<MessageToDecompiler>,
    /// How messages are received from the decompiler processing thread.
    receiver: std::sync::mpsc::Receiver<MessageFromDecompiler>,
}

impl Decompiler {
    /// Send a message requesting for a file to be decompiled.
    pub fn process_file(&mut self, f: &Arc<ProjectInputFile>) {
        self.sender
            .send(MessageToDecompiler::ProcessFile(f.to_owned()));
    }
}

/// The internal object of the decompiler
struct InternalDecompiler {
    /// How messages are sent back to the gui of the decompiler
    sender: std::sync::mpsc::Sender<MessageFromDecompiler>,
    /// How messages are received from the gui of the decompiler
    receiver: std::sync::mpsc::Receiver<MessageToDecompiler>,
    /// The list of all files being processed, along with the processing object for each one.
    file_processors: HashMap<String, DecompilerFileProcessor>,
}

impl InternalDecompiler {
    /// Build and run an `InternalDecompiler` in a separate thread.
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

    /// The main algorithm for the `InternalDecompiler`
    fn go(&mut self) {
        loop {
            if let Ok(m) = self.receiver.recv() {
                match m {
                    MessageToDecompiler::ProcessFile(f) => {
                        let (s, r) = std::sync::mpsc::channel();
                        let (s2, r2) = std::sync::mpsc::channel();
                        let n = f.borrow_name().to_owned();
                        println!("Received request to process file {}", n);
                        self.file_processors.entry(n).or_insert_with(|| {
                            let file_processor = DecompilerFileProcessor {
                                sender: s,
                                receiver: r2,
                                file: f,
                            };
                            file_processor.run(s2, r);
                            file_processor
                        });
                    }
                }
            }
        }
    }
}

impl Decompiler {
    /// Build a new decompiler
    pub fn new() -> Self {
        let (s, r) = std::sync::mpsc::channel();
        let (s2, r2) = std::sync::mpsc::channel();
        InternalDecompiler::run(s2, r);
        Self {
            sender: s,
            receiver: r2,
        }
    }

    /// Process any outstanding events from worker threads.
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

/// A message sent from the `DecompilerFileProcessor` to the `InternalDecompilerFileProcessor`
enum MessageToFileProcessor {
    /// Placeholder message, will be removed
    Test,
}

/// A message sent from the `InternalDecompilerFileProcessor` to the `DecompilerFileProcessor`
enum MessageFromFileProcessor {
    /// Placeholder message, will be removed
    Test,
}

/// This object belongs to a separate thread, one for each file being processed for decompilation.
struct InternalDecompilerFileProcessor {
    /// The file being anaylzed
    file: Arc<ProjectInputFile>,
    /// How messages are passed out of this thread.
    sender: std::sync::mpsc::Sender<MessageFromFileProcessor>,
    /// How messages are received from the rest of the decompiler
    receiver: std::sync::mpsc::Receiver<MessageToFileProcessor>,
    /// The graph of analyzed code
    code: crate::block::Graph<crate::block::Block>,
}

impl InternalDecompilerFileProcessor {
    /// The main algorithm for anaylzing an input file.
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
                    if let Ok(mut dis) =
                        crate::block::InstructionDecoder::new(obj.architecture(), start, d)
                    {
                        println!("Got a valid disassembler object");
                        //dis.goto(obj.entry());
                        let instru = dis.decode();
                        if let Some(instru) = instru {
                            println!("First instruction is {}", instru);
                            self.code.add_instruction(instru);
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

/// The object for tracking the thread that contains `InternalDecompilerFileProcessor`
struct DecompilerFileProcessor {
    /// The input file being processed
    file: Arc<ProjectInputFile>,
    /// The object for sending messages to the `InternalDecompilerFileProcessor`
    sender: std::sync::mpsc::Sender<MessageToFileProcessor>,
    /// The object for receiving messages from the `InternalDecompilerFileProcessor` object.
    receiver: std::sync::mpsc::Receiver<MessageFromFileProcessor>,
}

impl DecompilerFileProcessor {
    /// Create the `InternalDecompilerFileProcessor` object and run it on a separate thread.
    /// The message sender and receiver are used to construct the `InternalDecompilerFileProcessor` instance.
    fn run(
        &self,
        sender: std::sync::mpsc::Sender<MessageFromFileProcessor>,
        receiver: std::sync::mpsc::Receiver<MessageToFileProcessor>,
    ) {
        let fp = InternalDecompilerFileProcessor {
            file: self.file.clone(),
            sender,
            receiver,
            code: crate::block::Graph::new(),
        };
        std::thread::spawn(move || {
            let mut a = fp;
            a.go();
        });
    }
}
