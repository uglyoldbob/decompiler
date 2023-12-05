#![deny(missing_docs)]
#![deny(clippy::missing_docs_in_private_items)]

//! This program runs through as many generated graphs in order to come up with a certain number of graphs that it cannot simplify.
//! It writes the graphs it cannot simplify to disk and exits when it hits the limit.

mod block;
mod decompiler;
mod generator;
mod map;

use std::{
    io::{Read, Write},
    sync::Arc,
};

use clap::Parser;
use graphviz_rust::printer::{DotPrinter, PrinterContext};

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

    let mut pb2 = pb.clone();
    pb2.push("fail");
    std::fs::create_dir_all(&pb2);

    let mut pb2 = pb.clone();
    pb2.push("success");
    std::fs::create_dir_all(&pb2);

    let mut failures = 0;
    let mut successes = 0;
    for i in 2.. {
        let gg = crate::generator::GraphGenerator::new(i);
        let gi = gg.create_iter();
        for (i, g) in gi.enumerate() {
            let mut notes = Vec::new();
            notes.push("Graph NOTES\n".to_string());
            let mut gb = crate::block::graph::Graph::<crate::block::Block>::from(g);
            let dotgraph = gb.create_graph();
            let s = gb.simplify(&mut notes);

            if s.is_err() {
                failures += 1;
                println!("Fail to reduce {}", failures);
                let graph_png = graphviz_rust::exec(
                    dotgraph.clone(),
                    &mut PrinterContext::default(),
                    vec![graphviz_rust::cmd::Format::Png.into()],
                )
                .unwrap();
                let mut path = pb.clone();
                path.push("fail");
                path.push(format!("{}.png", failures));
                std::fs::write(path, graph_png).unwrap();

                let mut path = pb.clone();
                path.push("fail");
                path.push(format!("{}.dot", failures));
                std::fs::write(path, dotgraph.print(&mut PrinterContext::default())).unwrap();

                let mut path = pb.clone();
                let dotgraph = gb.create_graph();
                path.push("fail");
                path.push(format!("{}-simplify-attempt.dot", failures));
                std::fs::write(path, dotgraph.print(&mut PrinterContext::default())).unwrap();

                let graph_png = graphviz_rust::exec(
                    dotgraph.clone(),
                    &mut PrinterContext::default(),
                    vec![graphviz_rust::cmd::Format::Png.into()],
                )
                .unwrap();
                let mut path = pb.clone();
                path.push("fail");
                path.push(format!("{}-simplify-attempt.png", failures));
                std::fs::write(path, graph_png).unwrap();

                let mut path = pb.clone();
                path.push("fail");
                path.push(format!("{}-notes.txt", failures));
                let mut f = std::fs::File::create(path).unwrap();
                for s in notes {
                    f.write_all(s.as_bytes());
                }
                f.flush();

                if failures >= args.max_graphs {
                    break;
                }
            } else {
                successes += 1;
                let graph_png = graphviz_rust::exec(
                    dotgraph.clone(),
                    &mut PrinterContext::default(),
                    vec![graphviz_rust::cmd::Format::Png.into()],
                )
                .unwrap();
                let mut path = pb.clone();
                path.push("success");
                path.push(format!("{}.dot", successes));
                std::fs::write(path, dotgraph.print(&mut PrinterContext::default())).unwrap();

                let mut path = pb.clone();
                path.push("success");
                path.push(format!("{}.png", successes));
                std::fs::write(path, graph_png).unwrap();
            }
        }
        if failures >= args.max_graphs {
            break;
        }
    }

    println!("Exiting graph simplify reducer now");
}
