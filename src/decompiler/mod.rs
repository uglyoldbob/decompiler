//! Defines code that defines the main functionality of the decompiler. This is where the majority of work happens.

mod project;

use std::{
    collections::{HashMap, VecDeque},
    path::PathBuf,
    sync::Arc,
};

use object::{Object, ObjectSection, SectionKind};
use ouroboros::self_referencing;

#[self_referencing]
pub struct ProjectInputFile {
    pub name: String,
    f: std::fs::File,
    data: Vec<u8>,
    #[borrows(data)]
    #[covariant]
    pub obj: object::File<'this>,
}

impl ProjectInputFile {
    /// Build a new project input file. The `obj_builder` argument is a closure that builds the `object::File` object.
    pub fn builder<T>(
        name: String,
        f: std::fs::File,
        data: Vec<u8>,
        obj_builder: T,
    ) -> ProjectInputFile
    where
        T: FnOnce(&'_ Vec<u8>) -> object::File<'_>,
    {
        let builder = ProjectInputFileBuilder {
            name,
            f,
            data,
            obj_builder,
        };
        builder.build()
    }
}

/// A message to the `InternalDecompiler` object from the `Decompiler` object.
pub enum MessageToDecompiler {
    /// A request for processing the given file contents.
    ProcessFile(Arc<ProjectInputFile>),
    /// Request to have all outputs written to disk
    WriteOutputs,
}

/// A message from the `InternalDecompiler` object, sent back to the `Decompiler` object.
pub enum MessageFromDecompiler {
    /// The decompiler is done writing files
    DoneWriting,
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

    /// Write all decompilation outputs to disk
    pub fn write_outputs(&self) {
        self.sender.send(MessageToDecompiler::WriteOutputs);
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
    /// The files that have been fully processed so far
    results: Vec<FileResults>,
    /// The project culmination object
    project: project::Project,
    /// True when the results should eventually be written to disk
    write_outputs: bool,
}

impl InternalDecompiler {
    /// Build and run an `InternalDecompiler` in a separate thread.
    fn run(
        sender: std::sync::mpsc::Sender<MessageFromDecompiler>,
        receiver: std::sync::mpsc::Receiver<MessageToDecompiler>,
        pb: PathBuf,
    ) {
        let i = InternalDecompiler {
            sender,
            receiver,
            results: Vec::new(),
            file_processors: HashMap::new(),
            project: project::Project::new(project::autotools::BuildSystem::new().into(), pb),
            write_outputs: false,
        };
        std::thread::spawn(move || {
            let mut a = i;
            a.go();
        });
    }

    /// The main algorithm for the `InternalDecompiler`
    fn go(&mut self) {
        loop {
            if let Ok(m) = self.receiver.try_recv() {
                match m {
                    MessageToDecompiler::WriteOutputs => {
                        self.write_outputs = true;
                    }
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
                                results: None,
                            };
                            file_processor.run(s2, r);
                            file_processor
                        });
                    }
                }
            }
            let mut keys_remove = Vec::new();
            for (i, fp) in self.file_processors.iter_mut() {
                fp.process_messages();
                if let Some(fr) = fp.results.take() {
                    keys_remove.push(i.to_owned());
                    self.results.push(fr);
                    println!("Done processing a file, removing file processor");
                }
            }
            for k in keys_remove {
                self.file_processors.remove(&k);
            }
            if self.write_outputs && self.file_processors.len() == 0 {
                println!("Writing project outputs to disk now that all files have been processed");
                let e = self.project.write(&self.results);
                if let Err(e) = e {
                    println!("Error writing outputs: {:?}", e);
                }
                self.write_outputs = false;
                self.sender.send(MessageFromDecompiler::DoneWriting);
            }
            std::thread::sleep(std::time::Duration::from_millis(10));
        }
    }
}

impl Decompiler {
    /// Build a new decompiler
    pub fn new(pb: PathBuf) -> Self {
        let (s, r) = std::sync::mpsc::channel();
        let (s2, r2) = std::sync::mpsc::channel();
        InternalDecompiler::run(s2, r, pb);
        Self {
            sender: s,
            receiver: r2,
        }
    }

    /// Process any outstanding events from worker threads. Returns true if there is more work to do.
    pub fn process_events(&mut self) -> bool {
        let mut work = true;
        loop {
            if let Ok(m) = self.receiver.try_recv() {
                match m {
                    MessageFromDecompiler::DoneWriting => {
                        work = false;
                    }
                }
            } else {
                break;
            }
        }
        work
    }
}

/// A message sent from the `DecompilerFileProcessor` to the `InternalDecompilerFileProcessor`
enum MessageToFileProcessor {
    /// Placeholder message, will be removed
    Test,
}

/// A message sent from the `InternalDecompilerFileProcessor` to the `DecompilerFileProcessor`
enum MessageFromFileProcessor {
    /// The results of processing a file
    Done(FileResults),
}

/// The results of processing a single file
pub struct FileResults {
    /// The name of the binary
    name: String,
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
        let mut ids: Vec<crate::block::InstructionDecoderPlus> = object::Object::sections(obj)
            .filter_map(|a| {
                if a.kind() == SectionKind::Text {
                    if let Ok(d) = a.data() {
                        let id = crate::block::InstructionDecoderPlus::new(
                            obj.architecture(),
                            a.address(),
                            d,
                        );
                        if let Ok(id) = id {
                            Some(id)
                        } else {
                            None
                        }
                    } else {
                        None
                    }
                } else {
                    None
                }
            })
            .collect();
        let mut addresses: VecDeque<u64> = VecDeque::new();
        addresses.push_back(obj.entry());
        loop {
            if let Some(a) = addresses.pop_front() {
                let next = self.code.process_address(a, &mut ids);
                if let Some(n) = next {
                    match n {
                        crate::block::BlockEnd::None => {}
                        crate::block::BlockEnd::KnownAddress(a) => addresses.push_back(a),
                        crate::block::BlockEnd::UnknownAddress => {}
                        crate::block::BlockEnd::KnownBranch(a, b) => {
                            addresses.push_back(a);
                            addresses.push_back(b);
                        }
                        crate::block::BlockEnd::UnknownBranch(a) => addresses.push_back(a),
                    }
                }
            } else {
                break;
            }
        }

        if let Err(e) = self
            .code
            .write_to_dot(PathBuf::from("./output.dot"), "example")
        {
            println!("Failed to write dot file: {:?}", e);
        }

        let results: FileResults = FileResults {
            name: self.file.borrow_name().to_owned(),
        };
        let _ = self.sender.send(MessageFromFileProcessor::Done(results));
        println!("Done processing a file");
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
    /// The results
    results: Option<FileResults>,
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

    /// Process any messages received from the file processor
    pub fn process_messages(&mut self) {
        while let Ok(a) = self.receiver.try_recv() {
            match a {
                MessageFromFileProcessor::Done(r) => {
                    self.results = Some(r);
                }
            }
        }
    }
}
