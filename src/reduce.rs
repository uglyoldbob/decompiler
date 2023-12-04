#![deny(missing_docs)]
#![deny(clippy::missing_docs_in_private_items)]

//! This program runs through as many generated graphs in order to come up with a certain number of graphs that it cannot simplify.
//! It writes the graphs it cannot simplify to disk and exits when it hits the limit.

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
    /// The path where the graph outputs should go
    #[arg(short = 'o', long = "output")]
    output: std::path::PathBuf,
    /// The number of graphs to generate
    #[arg(short, long, default_value_t = 10)]
    max_graphs: u32,
}

fn main() {
    let args = CliArguments::parse();

    let pb = std::path::PathBuf::from(args.output);
    std::fs::remove_dir_all(&pb);
    std::fs::create_dir_all(&pb);

    let mut failures = 0;
    for i in 2.. {
        let gg = crate::generator::GraphGenerator::new(i);
        let gi = gg.create_iter();
        for (i, g) in gi.enumerate() {
            let mut gb = crate::block::Graph::<crate::block::Block>::from(g);
            let s = gb.simplify();
            let mut dot = Vec::new();
            gb.write_to_dot("asdf", &mut dot).unwrap();
            if s.is_err() {
                failures += 1;
                let mut path = pb.clone();
                path.push(format!("{}.dot", failures));
                std::fs::write(path, dot).unwrap();
                if failures >= args.max_graphs {
                    break;
                }
            }
        }
        if failures >= args.max_graphs {
            break;
        }
    }

    println!("Exiting graph simplify reducer now");
}
