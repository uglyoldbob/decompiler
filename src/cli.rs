#![deny(missing_docs)]
#![deny(clippy::missing_docs_in_private_items)]

//! This is the cli version of the decompiler.

mod block;
mod decompiler;
mod generator;
mod map;

use std::{io::Read, sync::Arc};

use clap::Parser;

use crate::decompiler::ProjectInputFile;

#[derive(Parser)]
/// The command line arguments to the program.
struct CliArguments {
    /// The path to the object to decompile
    #[arg(short = 'i', long = "input")]
    input: std::path::PathBuf,
    /// The path where the decompiled outputs should go
    #[arg(short = 'o', long = "output")]
    output: std::path::PathBuf,
}

fn main() {
    let args = CliArguments::parse();

    let mut decompiler = crate::decompiler::Decompiler::new(args.output);

    let f = std::fs::File::open(&args.input);
    if let Ok(mut file) = f {
        let mut v = Vec::new();
        file.read_to_end(&mut v).expect("Unable to read input file");
        let nf = ProjectInputFile::builder(
            args.input
                .file_name()
                .unwrap()
                .to_str()
                .unwrap()
                .to_string(),
            file,
            v,
            |a| object::File::parse(&a[..]).unwrap(),
        );
        decompiler.process_file(&Arc::new(nf));
        decompiler.write_outputs();
    } else {
        panic!("Unable to open input file {}", args.input.display());
    }

    while decompiler.process_events() {}

    println!("Exiting decompiler now");
}
